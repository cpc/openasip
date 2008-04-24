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


# global variables
export REPO_DIR="${HOME}/repo"
export BRANCH_DIR="${REPO_DIR}/trunk"
export LLVM_BIN_DIR=${HOME}/llvm/bin
export LLVM_FRONTEND_INSTALL_DIR=${HOME}/llvm-frontend
#export INSTALLATION_PATH=$HOME/tce-installation

export PATH="${LLVM_BIN_DIR}:${LLVM_FRONTEND_INSTALL_DIR}/bin:/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin"
#export LD_LIBRARY_PATH=$INSTALLATION_PATH/lib

# default output for make and autotools
OUTPUT="/dev/null"
DEBUG_LOG="$HOME/rnc-early.debug"

# alternative GCC version used with parameter altgcc
ALT_GCC_VERSION="4.3"

function eexit {
    echo $1 >&2
    exit 1
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
    autoreconf >${OUTPUT} 2>&1

    export ERROR_MAIL=yes
    export ERROR_MAIL_ADDRESS=tce-logs@cs.tut.fi
    export CXX="ccache g++${ALTGCC}"
    export CC="ccache gcc${ALTGCC}"
    export CXXFLAGS="-O3 -Wall -pedantic -Wno-long-long -g -Wno-variadic-macros"
    #export TCE_CONFIGURE_SWITCHES="--prefix=$INSTALLATION_PATH --disable-python"
    export TCE_CONFIGURE_SWITCHES="--disable-python"

    tools/scripts/compiletest.sh $@

    ${BRANCH_DIR}/tce/src/bintools/Compiler/tcecc --clear-plugin-cache
}

function update_repo {
    cd ${BRANCH_DIR}
    bzr up || return 1
}

##############################################################

if [ ! -e "${LLVM_BIN_DIR}" ]; then
    eexit "LLVM bin directory: ${LLVM_BIN_DIR}, was not found."
fi

# output to a debug log
[ "${1}x" == "debugx" ] && { shift; OUTPUT="${DEBUG_LOG}"; }

# use alternative gcc version
[ "${1}x" == "altgccx" ] && { shift; ALTGCC="-${ALT_GCC_VERSION}"; } || ALTGCC=""

##############################################################

# first update repository
update_repo &>${OUTPUT} || eexit "Updating bzr repository failed."

# now compile and install tce llvm-frontend from trunk branch
# llvm is expected to be intalled
install_llvm-frontend "${1}" &>${OUTPUT} || eexit "Compiling/installing llvm-frontend failed."

# shift parameters belonging to install_llvm-frontend
[ "${1}x" == "fullx" ] && shift

# call compiletest.sh
start_compiletest $@
