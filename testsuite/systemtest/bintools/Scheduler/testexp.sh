#!/bin/sh
$(dirname $0)/../../../../tce/scheduler/testbench/scheduler_tester.py \
$(dirname $0)/../../../../tce/scheduler/passes/experimental_scheduler.conf \
-v -a 3_bus_short_immediate_fields_and_reduced_connectivity.adf $*
