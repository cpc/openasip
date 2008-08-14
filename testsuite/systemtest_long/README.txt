This directory is reserved for long-running system tests. There are
basically no run time limit for tests in this set, so it's often not feasible
to run the tests before every commit, but automatically nightly in the 
compile test farm machines. Use 'compiletest.sh -q' to skip the long running
tests.

Software Requirements

1. GHDL

System tests for Processor Generator needs the GHDL (VHDL compiler &
simulator) to be found from PATH. The tests are disabled if 'ghdl'
binary is not found.
