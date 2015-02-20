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

echo "CXX: $CXX"
echo "CC: $CC"

function start_compiletest {
    cd "${BRANCH_DIR}/tce"

    # remove the zOMG error mail when testing is finished, buildbot will handle the mails
    export ERROR_MAIL=no
    export ERROR_MAIL_ADDRESS=tce-logs@cs.tut.fi
    export CXXFLAGS="-O3 -Wall -pedantic -Wno-long-long -g -Wno-variadic-macros -Wno-deprecated -Wextra"
    export CPPFLAGS="-O3 -Wall -pedantic -Wno-long-long -g -Wno-variadic-macros -Wno-deprecated -Wextra"
    export LDFLAGS="-L/usr/local/lib"

    ./autogen.sh >& bb_autogen.log

    if [ -x gen_llvm_shared_lib.sh ]
    then
        ./gen_llvm_shared_lib.sh >& /dev/null
    fi
    cd test; make clean -s; cd ..
    tools/scripts/compiletest.sh -t $@ >& test.log

    ${BRANCH_DIR}/tce/src/bintools/Compiler/tcecc --clear-plugin-cache
}

# supercomplex main program
export PATH=$LLVM_DIR/bin:$PATH
export LD_LIBRARY_PATH=$LLVM_DIR/lib:$LD_LIBRARY_PATH

start_compiletest

# ugly way to determine whether compiletest succeeded or not :)
if [ -s compiletest.error.log ]
then
   exit 1
fi
exit 0

