#!/bin/bash
EXPLORE_BIN="../../../../../tce/src/codesign/Explorer/explore"
COMPILER_BIN="../../../../../tce/src/bintools/Compiler/tcecc"
TTASIM_BIN="../../../../../tce/src/codesign/ttasim/ttasim"

ADF_PATH="./data/minimal2bus.adf"
"${EXPLORE_BIN}" -a ${ADF_PATH} testi.dsdb 1>/dev/null
"${EXPLORE_BIN}" -d ./data/ testi.dsdb 1>/dev/null
${EXPLORE_BIN} -e ConnectionSweeper -s 1 -u cc_worsening_threshold=10 testi.dsdb | grep -v "No fitting"
${EXPLORE_BIN} --pareto_set C testi.dsdb | wc -l

