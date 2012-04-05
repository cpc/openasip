#!/bin/bash
EXPLORE_BIN="../../../../tce/src/codesign/Explorer/explore"

NODE_ADF="data/node.adf"
EXTRAS_ADF="data/extras.adf"
NODE_COUNT=$1
ADF_PATH="${MINIMAL_ADF_PATH}"

# Create 6 nodes of minimal.adf
${EXPLORE_BIN} -e ADFCombiner -u node=${NODE_ADF} -u extra=${EXTRAS_ADF} -u node_count=${NODE_COUNT} -u build_idf=false -u vector_lsu=true test.dsdb 1>/dev/null
"${EXPLORE_BIN}" -w 3 test.dsdb 1>/dev/null

mv 3.adf data/$1-way-clustered.adf
rm test.dsdb
