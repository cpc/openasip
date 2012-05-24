#!/bin/bash
EXPLORE_BIN="../../../../../tce/src/codesign/Explorer/explore"

MINIMAL_ADF_PATH="../../../../../tce/data/mach/minimal.adf"
ADF_PATH="${MINIMAL_ADF_PATH}"

# Create 6 nodes of minimal.adf
NODE_COUNT=6
${EXPLORE_BIN} -e ADFCombiner -u node=${MINIMAL_ADF_PATH} -u extra=${MINIMAL_ADF_PATH} -u node_count=${NODE_COUNT} -u build_idf=true  test.dsdb 1>/dev/null

"${EXPLORE_BIN}" -w 2 test.dsdb 1>/dev/null

CONNECTING_BUSES="$(grep '<bus name.*>' 2.adf |grep "connect"| wc -l)"
# NODE_COUNT buses for each node internal connections
# NODE_COUNT + 1 buses for each register file and and neighbour register files, including extra
((REQUIRED_BUSES=(NODE_COUNT+1)*2 + NODE_COUNT))
if [ ! "${CONNECTING_BUSES}" -eq "${REQUIRED_BUSES}" ]; then
    echo "Added wrong number of connecting buses (${CONNECTING_BUSES} != ${REQUIRED_BUES})."
fi
CONNECTING_SOCKET_R="$(grep '<socket name.*>' 2.adf |grep "connect_r"| wc -l)"
CONNECTING_SOCKET_W="$(grep '<socket name.*>' 2.adf |grep "connect_w"| wc -l)"
if [ ! "${CONNECTING_SOCKET_R}" -eq "${CONNECTING_SOCKET_W}" ]; then
    echo "Added wrong number of connecting sockets between register files (${CONNECTING_SOCKET_R} != ${CONNECTING_SOCKET_W})."
fi
IDF=`ls *.idf`
if [ ! "${IDF}" = "2.idf" ]; then
    echo "Creation of IDF file failed. ${IDF}"
fi

# Try to create two vector load store units (reuse instructions address space :)
${EXPLORE_BIN} -e ADFCombiner -u node=${MINIMAL_ADF_PATH} -u extra=${MINIMAL_ADF_PATH} -u node_count=${NODE_COUNT} -u vector_lsu=true -u address_spaces="data;instructions"  test.dsdb 1>/dev/null

"${EXPLORE_BIN}" -w 4 test.dsdb 1>/dev/null

DATA_LSU="$(grep 'function-unit name=\"VectorLSU_' 4.adf|wc -l)"
if [ ! "$DATA_LSU" -eq 2 ]; then
    echo "Incorrect number of vector load store units created."
fi

rm 2.adf 2.idf
rm 4.adf
