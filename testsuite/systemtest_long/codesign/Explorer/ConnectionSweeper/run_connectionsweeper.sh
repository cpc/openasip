#!/bin/bash
EXPLORE_BIN="../../../../../tce/src/codesign/Explorer/explore"
COMPILER_BIN="../../../../../tce/src/bintools/Compiler/tcecc"
TTASIM_BIN="../../../../../tce/src/codesign/ttasim/ttasim"

ADF_PATH="./data/minimal2bus.adf"
"${EXPLORE_BIN}" -a ${ADF_PATH} testi.dsdb 1>/dev/null
"${EXPLORE_BIN}" -d ./data/ testi.dsdb 1>/dev/null
${EXPLORE_BIN} -e ConnectionSweeper -s 1 -u cc_worsening_threshold=10 testi.dsdb 1>/dev/null
ROWS="$(${EXPLORE_BIN} --pareto_set C testi.dsdb | wc -l)"

# With LLVM 2.8 one gets 7, with LLVM 2.9 one gets 11 rows.
MIN_ROWS=6

if [ "${ROWS}" -lt "${MIN_ROWS}" ]; then
   echo "Not enough rows in the pareto set result!"
else
   echo "Looking fine. Got some archs!"
fi



