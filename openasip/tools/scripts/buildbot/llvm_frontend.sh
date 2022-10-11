#!/bin/bash
# Copyright (c) 2002-2009 Tampere University.
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
cd tce-llvm-gcc

if [ ! -e "${LLVM_GCC_SOURCES}" ]; then
        echo "LLVM-GCC source directory: ${LLVM_GCC_SOURCES}, was not found and no tce-llvm-gcc could not be compiled. Unpack correct sources of your llvm version e.g. http://llvm.org/releases/2.5/llvm-gcc-4.2-2.5.source.tar.gz for llvm-2.5." >&2
        exit 1
fi

if [ ! -e build_dir ]
then
	mkdir build_dir
else
	rm -rf build_dir
        mkdir build_dir
fi

export CXXFLAGS="-O2"
export CFLAGS="-O2"
export PATH=$LLVM_DIR/bin:$PATH

autoreconf >& /dev/null
cd build_dir
../configure --prefix=$LLVM_FRONTEND_DIR --with-llvm-gcc-sources=${LLVM_GCC_SOURCES} >& compile.log
make 1>> compile.log 2>> compile.log || exit 1
make install 1>> compile.log 2>> compile.log || exit 1
exit $?
