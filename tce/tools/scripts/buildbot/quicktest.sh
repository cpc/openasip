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

# remove old difference.txt files (buildbot logs them anyway)
rm -f testsuite/systemtest/difference.txt
rm -f testsuite/systemtest_long/difference.txt
rm -f testsuite/systemtest_longlong/difference.txt

# touch the new ones (just in case)
touch testsuite/systemtest/difference.txt
touch testsuite/systemtest_long/difference.txt
touch testsuite/systemtest_longlong/difference.txt

# export PATH to include llvm and llvm-frontend
# (those ENVs are defined in buildbot master.cfg)
export PATH=$LLVM_DIR/bin:$LLVM_FRONTEND_DIR/bin:$PATH
export LD_LIBRARY_PATH=$LLVM_DIR/lib:$LD_LIBRARY_PATH

export CXXFLAGS="-O3 -Wall -pedantic -Wno-long-long -g -Wno-variadic-macros -Wno-deprecated"
export CPPFLAGS="-O3 -Wall -pedantic -Wno-long-long -g -Wno-variadic-macros -Wno-deprecated"

# run the tests
cd tce
tools/scripts/compiletest.sh -q >& test.log

# ugly way to check if there were errors (return value tells
# buildbot whether this step was succesfull)
if [ -s compiletest.error.log ]
then
	exit 1
fi

exit 0
