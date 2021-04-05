#!/bin/sh
### TCE TESTCASE 
### title: Create BlocksConnectIC ADF via plugin
### xstdout: 8, 4, 1

mach=data/blocks_base.adf

explore -e BlocksConnectIC -a $mach -s 1 testdb.dsdb 1>/dev/null
explore -w 2 testdb.dsdb 1>/dev/null

BUSES="$(grep '<bus name.*>' 2.adf | wc -l)"
FUS="$(grep '<function-unit name.*>' 2.adf | wc -l)"
RFS="$(grep '<register-file name.*>' 2.adf | wc -l)"
echo "$BUSES, $FUS, $RFS"

rm -f testdb.dsdb 2.adf
