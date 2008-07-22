#!/bin/bash
#
# Crontab entry should for executing this script be something like:
#
# 0 4,13 * * 1,2,3,4,5 ~/repo/trunk/tce/tools/scripts/run_nightly_compiletest.sh &> /dev/null 
#
# Before running this script LLVM should be installed and its bin directory
# should be found from LLVM_BIN_DIR specified below under global variables.

# parameters for this script:
#   first three paremeters, if they are given as below, are ignored when passing
#   parameters for compiletest.sh
#
# 1:     "debug"    for printing build output to a file specified with DEBUG_LOG
#                   variable
# 1|2:   "altgcc"   use alternative gcc version as specified in parameter ALTGCC
# 1|2|3: "full"     rebuild and install llvm-frontend from scratch
# 1..n:  *          parameters to be passed for compiletests.sh

# parameters taken as variables set before calling:
#                                                                Defaults:
#  REPO_DIR                     Repository root directory        $HOME/repo
#  BRANCH_NAME                  Branch name                      trunk
#  LLVM_BIN_DIR                 llvm binary directory            $HOME/llvm/bin
#  LLVM_FRONTEND_INSTALL_DIR    llvm-frontend install directory  $HOME/llvm-frontend


function eexit {
    echo $1 >&2
    exit 1
}

# adds the $1 param to the end of PATH variable
function addToPath {
    local re='[:\n]{1}'
    if [ -z "$(echo "$PATH" | grep -E "${1}${re}")" ]; then
        export PATH="${PATH}:${1}"
    fi  
}

# $1: full, do full rebuild
function install_llvm-frontend {
    SOURCE_DIR=${BRANCH_DIR}/llvm-frontend
    BUILD_DIR=${BRANCH_DIR}/llvm-frontend/build

    export MAKEFLAGS=-j1
    export CXX="g++${ALTGCC}"
    export CXXFLAGS="-O2"
    export CFLAGS="-O2"
    export CC="gcc${ALTGCC}"

    if [ "${1}x" == "fullx" ]; then
        cd ${SOURCE_DIR} || return 1
        autoreconf || return 1

        # remove build dir
        rm -rf ${BUILD_DIR}
        mkdir -p ${BUILD_DIR}
        cd ${BUILD_DIR} || return 1
        ${SOURCE_DIR}/configure --prefix=${LLVM_FRONTEND_INSTALL_DIR} || return 1
    else
        cd ${BUILD_DIR} || return 1
    fi

    make -s || return 1
    rm -rf ${LLVM_FRONTEND_INSTALL_DIR}
    make install || return 1
}

function start_compiletest {
    cd "${BRANCH_DIR}/tce"
    autoreconf >>${OUTPUT} 2>&1

    export ERROR_MAIL=yes
    export ERROR_MAIL_ADDRESS=tce-logs@cs.tut.fi
    export CXX="ccache g++${ALTGCC}"
    export CC="ccache gcc${ALTGCC}"
    export CXXFLAGS="-O3 -Wall -pedantic -Wno-long-long -g -Wno-variadic-macros"
    export CPPFLAGS="-O3 -Wall -pedantic -Wno-long-long -g -Wno-variadic-macros"
    export TCE_CONFIGURE_SWITCHES="--disable-python"

    tools/scripts/compiletest.sh $@

    ${BRANCH_DIR}/tce/src/bintools/Compiler/tcecc --clear-plugin-cache
}

function update_repo {
    cd ${BRANCH_DIR}
    bzr up || return 1
}

##############################################################

# global variables
export REPO_DIR=${REPO_DIR:-$HOME/repo}
export BRANCH_NAME=${BRANCH_NAME:-trunk}
export BRANCH_DIR="${REPO_DIR}/${BRANCH_NAME}"
export LLVM_BIN_DIR=${LLVM_BIN_DIR:-$HOME/llvm/bin}
export LLVM_FRONTEND_INSTALL_DIR=${LLVM_FRONTEND_INSTALL_DIR:-$HOME/llvm-frontend}

addToPath "${LLVM_BIN_DIR}"
addToPath "${LLVM_FRONTEND_INSTALL_DIR}/bin"

# default output for make and autotools
OUTPUT="/dev/null"
DEBUG_LOG="$HOME/rnc-early.debug"

# alternative GCC version used with parameter altgcc
ALT_GCC_VERSION="4.3"

##############################################################

if [ ! -e "${LLVM_BIN_DIR}" ]; then
    if [ "$(whereis llvm-config 2>/dev/null | awk '{print $2}')x" == "x" ]; then 
        eexit "LLVM bin directory: ${LLVM_BIN_DIR}, was not found and no llvm was installed system wide."
    fi
fi

# output to a debug log, also empty the log file first.
[ "${1}x" == "debugx" ] && { shift; OUTPUT="${DEBUG_LOG}"; :>"${OUTPUT}"; }

# use alternative gcc version
[ "${1}x" == "altgccx" ] && { shift; ALTGCC="-${ALT_GCC_VERSION}"; } || ALTGCC=""

##############################################################

# first update repository
update_repo >>${OUTPUT} 2>&1 || eexit "Updating bzr repository failed."

# now compile and install tce llvm-frontend from trunk branch
# llvm is expected to be intalled
install_llvm-frontend "${1}" >>${OUTPUT} 2>&1 || eexit "Compiling/installing llvm-frontend failed."

# shift parameters belonging to install_llvm-frontend
[ "${1}x" == "fullx" ] && shift

# call compiletest.sh
start_compiletest $@
