#!/bin/sh

# delete old results 
rm `find -name topresults.csv`

# and generate new ones
../../../../tce/scheduler/testbench/scheduler_tester.py -vx -t -g "-O3 -k_Output" -a ti64x_subset.adf -a huge.adf -a RF_constrained.adf -a FU_constrained.adf -a minimal_with_io.adf
