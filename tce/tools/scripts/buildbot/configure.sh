#!/bin/bash

cd tce
make clean $> /dev/null
autoreconf $> reconf.log
./configure 1>> reconf.log 2>> reconf.log
exit $?
