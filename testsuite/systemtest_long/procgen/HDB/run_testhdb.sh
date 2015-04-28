#!/bin/bash

HDBTESTER=../../../../tce/src/procgen/HDBEditor/testhdb
DEFAULT_HDB=../../../../tce/hdb/asic_130nm_1.5V.hdb

$HDBTESTER $DEFAULT_HDB 2>&1

exit 0
