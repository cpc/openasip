#!/bin/sh
export PYTHONPATH=.:${PYTHONPATH}
export LD_LIBRARY_PATH=../../../src/base/.libs:../../../src/tools/.libs:../../../src/applibs/.libs:../../../src/.libs
./run_python_file hello.py > hello.output
if diff hello.output hello.expected
then
  echo "Embedded hello test passed"
else
  echo "Embedded hello test failed"
fi
python hello.py > hello.output
if diff hello.output hello.expected
then
  echo "Standalone hello test passed"
else
  echo "Standalone hello test failed"
fi
./run_python_file disass_test.py > disass.output
if diff disass.output disass.expected
then
  echo "Embedded disass test passed"
else
  echo "Embedded disass test failed"
fi
python disass_test.py > disass.output
if diff disass.output disass.expected
then
  echo "Standalone disass test passed"
else
  echo "Standalone disass test failed"
fi
