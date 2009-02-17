#!/bin/sh

# generate lastresults
../../../../tce/scheduler/testbench/scheduler_tester.py -vx -t -g "-O3 -k_Output" -a ti64x_subset.adf -a huge.adf -a RF_constrained.adf -a FU_constrained.adf -a minimal_with_io.adf

FILES=`find -name lastresults.csv`

for i in $FILES; do
    TOPRESULTS=`dirname $i`/topresults.csv
    echo "Updating: $TOPRESULTS"
    cp $i $TOPRESULTS
done