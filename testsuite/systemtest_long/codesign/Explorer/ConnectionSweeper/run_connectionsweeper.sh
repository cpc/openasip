#!/bin/bash
EXPLORE_BIN="../../../../../openasip/src/codesign/Explorer/explore"
COMPILER_BIN="../../../../../openasip/src/bintools/Compiler/tcecc"
TTASIM_BIN="../../../../../openasip/src/codesign/ttasim/ttasim"

ADF_PATH="./data/minimal2bus.adf"
"${EXPLORE_BIN}" -a ${ADF_PATH} testi.dsdb 1>/dev/null
"${EXPLORE_BIN}" -d ./data/ testi.dsdb 1>/dev/null
${EXPLORE_BIN} $1 -e ConnectionSweeper -s 1 -u all_or_nothing_mode=false -u cc_worsening_threshold=5 testi.dsdb 1>/dev/null
ROWS="$(${EXPLORE_BIN} --pareto_set C testi.dsdb | wc -l)"

# With LLVM 2.8 one gets 7
# with LLVM 2.9 and --old-builder one gets 5 rows
# with LLVM 2.9 and --new-builder one gets 4 rows
MIN_ROWS=3

if [ "${ROWS}" -lt "${MIN_ROWS}" ]; then
   echo "Not enough rows in the pareto set result! Got ${ROWS}."
else
   echo "Looking fine. Got some archs!"
fi



