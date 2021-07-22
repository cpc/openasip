#!/bin/sh
### TCE TESTCASE
### title: Test BlocksTranslator tool
### xstdout: 51, 20, 8

blocks_xml=./data/generic_blocks_vliw_arch.xml

outfile=$(mktemp)

blocks_translator $blocks_xml $outfile

BUSES="$(grep '<bus name.*>' $outfile | wc -l)"
FUS="$(grep '<function-unit name.*>' $outfile | wc -l)"
RFS="$(grep '<register-file name.*>' $outfile | wc -l)"
echo "$BUSES, $FUS, $RFS"

rm -f $outfile
