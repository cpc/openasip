#!/bin/bash

HDBTESTER=../../../../tce/src/procgen/HDBEditor/testhdb
DEFAULT_HDB=../../../../tce/hdb/asic_130nm_1.5V.hdb

$HDBTESTER $DEFAULT_HDB 2>&1 | grep -v "Simulation of FUs with external ports is not supported" | grep -v "RF does not have a read port"

exit 0
