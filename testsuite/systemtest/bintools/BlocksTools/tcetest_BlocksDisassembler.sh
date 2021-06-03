#!/bin/sh
### TCE TESTCASE
### title: Test BlocksTranslator tool
### xstdout: 37

blocks_adf=./data/blocks_risc.adf
tpef=./data/binarization.tpef
outfile=$(mktemp)

tpef2pasm -o $outfile $blocks_adf $tpef
cat $outfile | wc -l

rm -f $outfile
