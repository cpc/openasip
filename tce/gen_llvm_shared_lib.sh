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

# Script used to build and install shared versions of LLVM libraries.
#
# Requires LLVM to be installed and llvm-config found in PATH.
#
# Background:
#
# TCE requires shared versions of LLVM libraries. However, shared libraries 
# are currently not installed by LLVM. This is a known bug that is being
# resolved by LLVM (see: http://llvm.org/bugs/show_bug.cgi?id=3201). Hopefully,
# in the future versions of LLVM the shared libraries are built and 
# installed properly and this script can be removed. For now, use it to
# generate the requires shared library.
#
# Linking TCE against static LLVM libraries is tricky due to the on-the-fly
# generation of TTA backends. The backend plugins require certain LLVM
# symbols and the TCE library that loads the plugin requires them also.
# Linking the static libraries both to the loader and the plugin does not
# work because it causes the LLVM analysis passes registered twice to 
# LLVM framework, thus causing an assert. In addition, new libtool versions
# do not seem to allow proper use of the -whole-archive switch which would
# allow us to link all LLVM symbols to the loader library, not requiring
# any in the loaded plugins. 

LLVM_VERSION=$(llvm-config --version)

function error_exit() {
    echo $2
    exit $1
}

which llvm-config > /dev/null || error_exit 1 "llvm-config not found in PATH" 

LLVM_LIBFILES=$(llvm-config --libfiles)
LLVM_LIBDIR=$(llvm-config --libdir)
LLVM_LIBFILE=${LLVM_LIBDIR}/libllvm${LLVM_VERSION}.so

if test -e $LLVM_LIBFILE;
then
    error_exit 3 "$LLVM_LIBFILE already exists. Remove it to regenerate."
fi

CXX=g++

echo "Generating ${LLVM_LIBFILE}..."
$CXX -Wl,--whole-archive $LLVM_LIBFILES -Wl,--no-whole-archive -shared -o $LLVM_LIBFILE || \
    error_exit 2 "Failed. Do you have write access to $LLVM_LIBDIR? Are you root?"

if test "$(llvm-config --build-mode)" = "Debug";
then
    echo "Do not strip library compiled in Debug mode." 
else
    echo "Stripping debugging symbols to save space..."
    strip --strip-debug $LLVM_LIBFILE
fi


