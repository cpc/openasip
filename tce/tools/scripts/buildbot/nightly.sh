#!/bin/bash
# Copyright (c) 2002-2009 Tampere University of Technology.
#
# This file is part of TTA-Based Codesign Environment (TCE).
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

BRANCH_DIR=$PWD

function check_llvm_gcc_sources {
    if [ ! -e "${LLVM_GCC_SOURCES}" ]; then
        echo "LLVM-GCC source directory: ${LLVM_GCC_SOURCES}, was not found and no tce-llvm-gcc could not be compiled. Unpack correct sources of your llvm version e.g. http://llvm.org/releases/2.5/llvm-gcc-4.2-2.5.source.tar.gz for llvm-2.5." >&2
        exit 1
    fi
}

# build & install llvm-frontend from scratch (from rnc.sh)
function install_llvm-frontend {

    SOURCE_DIR=${BRANCH_DIR}/tce-llvm-gcc
    BUILD_DIR=${BRANCH_DIR}/tce-llvm-gcc/build_dir

    export MAKEFLAGS=-j1
    export CXX="g++${ALTGCC}"
    export CXXFLAGS="-O2"
    export CFLAGS="-O2"
    export CC="gcc${ALTGCC}"


    cd ${SOURCE_DIR} || return 1
    autoreconf >& compile.log || return 1

    # remove build dir
    rm -rf ${BUILD_DIR}
    mkdir -p ${BUILD_DIR}
    cd ${BUILD_DIR} || return 1
    ${SOURCE_DIR}/configure --prefix=${LLVM_FRONTEND_DIR} --with-llvm-gcc-sources=${LLVM_GCC_SOURCES} >& compile.log || return 1

    make -s >>& compile.log || return 1
    rm -rf ${LLVM_FRONTEND_DIR}
    make install >& compile.log || return 1
}

# TODO: support for altgcc
function start_compiletest {
    cd "${BRANCH_DIR}/tce"

    # remove the zOMG error mail when testing is finished, buildbot will handle the mails
    export ERROR_MAIL=no
    export ERROR_MAIL_ADDRESS=tce-logs@cs.tut.fi
    export CXX="ccache g++${ALTGCC}"
    export CC="ccache gcc${ALTGCC}"
    export CXXFLAGS="-O3 -Wall -pedantic -Wno-long-long -g -Wno-variadic-macros -Wno-deprecated"
    export CPPFLAGS="-O3 -Wall -pedantic -Wno-long-long -g -Wno-variadic-macros -Wno-deprecated"

    ./gen_config.sh >& /dev/null

    if [ -x gen_llvm_shared_lib.sh ]
    then
        ./gen_llvm_shared_lib.sh >& /dev/null
    fi

    tools/scripts/compiletest.sh -t $@ >& test.log

    ${BRANCH_DIR}/tce/src/bintools/Compiler/tcecc --clear-plugin-cache
}

# supercomplex main program
export PATH=$LLVM_DIR/bin:$LLVM_FRONTEND_DIR/bin:$PATH
check_llvm_gcc_sources
install_llvm-frontend
start_compiletest

# ugly way to determine whether compiletest succeeded or not :)
if [ -s compiletest.error.log ]
then
   exit 1
fi
exit 0

