#!/bin/bash
EXPLORE_BIN="../../../../../tce/src/codesign/Explorer/explore"

TEMPLATE_NAME="newTemplate"
WIDTH=99

"${EXPLORE_BIN}" -a ./data/imm_test_3_bus.adf test3bne.dsdb &>/dev/null

"${EXPLORE_BIN}" -e ImmediateGenerator -s 1 -u add_it_name="${TEMPLATE_NAME}" -u width=$WIDTH -u split='true' test3bne.dsdb 2>&1 | grep 99 | grep -v 3
