#!/bin/sh
### TCE TESTCASE
### title: Test BlocksTranslator tool
### xstdout: 51, 20, 8

blocks_xml=./data/generic_blocks_vliw_arch.xml

blocks_translator $blocks_xml out

BUSES="$(grep '<bus name.*>' out.adf | wc -l)"
FUS="$(grep '<function-unit name.*>' out.adf | wc -l)"
RFS="$(grep '<register-file name.*>' out.adf | wc -l)"
echo "$BUSES, $FUS, $RFS"

rm -f out.adf
