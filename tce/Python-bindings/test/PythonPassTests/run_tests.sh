#!/bin/sh
export LD_LIBRARY_PATH=../../../src/base/.libs:../../../src/tools/.libs:../../../src/applibs/.libs:../../../src/.libs
#export PYTHONPATH=../../src
../../../src/bintools/Scheduler/schedule -c NoOpPass.config -a test.adf test.tpef | sed 's/0x[0-9a-f]*/0x......../g' > NoOpPass.output
if diff NoOpPass.output NoOpPass.expected
then
  echo "NoOpPass test succeeded"
else
  echo "NoOpPass test failed"
fi
../../../src/bintools/Scheduler/schedule -c NoOpPassTwice.config -a test.adf test.tpef | sed 's/0x[0-9a-f]*/0x......../g' > NoOpPassTwice.output
if diff NoOpPassTwice.output NoOpPassTwice.expected
then
  echo "NoOpPassTwice test succeeded"
else
  echo "NoOpPassTwice test failed"
fi


