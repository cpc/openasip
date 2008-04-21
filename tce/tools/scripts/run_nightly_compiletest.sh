#!/bin/bash
#
# Crontab entry should for executing this script be something like:
#
# 0 4,13 * * 1,2,3,4,5 ~/svnroot/trunk/tce/tools/scripts/run_nightly_compiletest.sh > /dev/null 
#
# You should have a TCE and LLVM/TCE installation at $HOME/tce-installation before
# executing this script the first time, otherwise systemtests that test 'tcecc'
# will fail. The script reinstalls TCE and LLVM/TCE only after the compiletest 
# to avoid extra recompilations.

# global variables
export REPO_DIR="${HOME}/repo"
export BRANCH_DIR="${REPO_DIR}/trunk"
export LLVM_BIN_DIR=${HOME}/llvm/bin
export LLVM_FRONTEND_INSTALL_DIR=${HOME}/llvm-frontend
#export INSTALLATION_PATH=$HOME/tce-installation

export PATH="${LLVM_BIN_DIR}:${LLVM_FRONTEND_INSTALL_DIR}/bin:/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin"
#export LD_LIBRARY_PATH=$INSTALLATION_PATH/lib

function eexit {
    echo $1 >&2
    exit 1
}

# $1: full, do full rebuild
function install_llvm-frontend {
    SOURCE_DIR=${BRANCH_DIR}/llvm-frontend
    BUILD_DIR=${BRANCH_DIR}/llvm-frontend/build

    export MAKEFLAGS=-j1
    export CXX=g++
    export CXXFLAGS="-O2"
    export CFLAGS="-O2"
    export CC=gcc

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
    autoreconf > /dev/null 2>&1

    export ERROR_MAIL=yes
    export ERROR_MAIL_ADDRESS=tce-logs@cs.tut.fi
    export CXX="ccache g++"
    export CC="ccache gcc"
    export CXXFLAGS="-O3 -Wall -pedantic -Wno-long-long -g -Wno-variadic-macros"
    #export TCE_CONFIGURE_SWITCHES="--prefix=$INSTALLATION_PATH --disable-python"
    export TCE_CONFIGURE_SWITCHES="--disable-python"

    tools/scripts/compiletest.sh $@

    tcecc --clear-plugin-cache
}

function update_repo {
    cd ${BRANCH_DIR}
    bzr up || return 1
}

##############################################################

if [ ! -e "${LLVM_BIN_DIR}" ]; then
    eexit "LLVM bin directory: ${LLVM_BIN_DIR}, was not found."
fi

# first update repository
update_repo &>/dev/null || eexit "Updating bzr repository failed."

# now compile and install tce llvm-frontend from trunk branch
# llvm is expected to be intalled
install_llvm-frontend &>/dev/null || eexit "Compiling/installing llvm-frontend failed."

# call compiletest.sh
start_compiletest $@
