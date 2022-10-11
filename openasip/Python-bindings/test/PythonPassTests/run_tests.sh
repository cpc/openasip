#!/bin/sh
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
export LD_LIBRARY_PATH=../../../src/base/.libs:../../../src/tools/.libs:../../../src/applibs/.libs:../../../src/.libs
../../../src/bintools/Scheduler/schedule -c NoOpPass.config -a ../../../data/mach/minimal.adf test.tpef | sed 's/0x[0-9a-f]*/0x......../g' > NoOpPass.output
if diff NoOpPass.output NoOpPass.expected
then
  echo "NoOpPass test succeeded"
else
  echo "NoOpPass test failed"
fi
../../../src/bintools/Scheduler/schedule -c NoOpPassTwice.config -a ../../../data/mach/minimal.adf test.tpef | sed 's/0x[0-9a-f]*/0x......../g' > NoOpPassTwice.output
if diff NoOpPassTwice.output NoOpPassTwice.expected
then
  echo "NoOpPassTwice test succeeded"
else
  echo "NoOpPassTwice test failed"
fi


