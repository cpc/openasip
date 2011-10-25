#!/bin/sh
# The example is shown in the Appendix of the user manual.
# Moved it there as the user manual should not have dependencies
# to the test suite, but otherwise around it's fine.
cd ../../../../tce/doc/man/TCE/systemc_example
make -s
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(llvm-config --libdir)
./simulator 2>&1 | grep recv
