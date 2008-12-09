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

# build & install llvm-frontend from scratch (from rnc.sh)
function install_llvm-frontend {

    SOURCE_DIR=${BRANCH_DIR}/llvm-frontend
    BUILD_DIR=${BRANCH_DIR}/llvm-frontend/build

    export MAKEFLAGS=-j1
    export CXX="g++${ALTGCC}"
    export CXXFLAGS="-O2"
    export CFLAGS="-O2"
    export CC="gcc${ALTGCC}"


    cd ${SOURCE_DIR} || return 1
    autoreconf || return 1 $> /dev/null

    # remove build dir
    rm -rf ${BUILD_DIR}
    mkdir -p ${BUILD_DIR}
    cd ${BUILD_DIR} || return 1
    ${SOURCE_DIR}/configure --prefix=${LLVM_FRONTEND_DIR} $> /dev/null || return 1

    make -s $> /dev/null || return 1
    rm -rf ${LLVM_FRONTEND_DIR}
    make install $> /dev/null || return 1
}

function start_compiletest {
    cd "${BRANCH_DIR}/tce"

    export ERROR_MAIL=yes
    export ERROR_MAIL_ADDRESS=otto.esko@tut.fi
    export CXX="ccache g++${ALTGCC}"
    export CC="ccache gcc${ALTGCC}"
    export CXXFLAGS="-O3 -Wall -pedantic -Wno-long-long -g -Wno-variadic-macros -Wno-deprecated"
    export CPPFLAGS="-O3 -Wall -pedantic -Wno-long-long -g -Wno-variadic-macros -Wno-deprecated"
    export TCE_CONFIGURE_SWITCHES="--disable-python"

    autoreconf $> /dev/null

    tools/scripts/compiletest.sh $@ $> test.log

    ${BRANCH_DIR}/tce/src/bintools/Compiler/tcecc --clear-plugin-cache
}

# supercomplex main program
export PATH=$LLVM_DIR/bin:$LLVM_FRONTEND_DIR/bin:$PATH
install_llvm-frontend
start_compiletest

if [ -s compiletest.error.log ]
then
   exit 1
fi
exit 0

