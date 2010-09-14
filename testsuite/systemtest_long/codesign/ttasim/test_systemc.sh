#!/bin/sh
cd systemc
make -s
./simulator 2>&1 | grep recv
