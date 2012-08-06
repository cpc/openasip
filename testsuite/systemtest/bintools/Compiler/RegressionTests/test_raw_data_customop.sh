#!/bin/bash

tcecc=../../../../../tce/src/bintools/Compiler/tcecc
ttasim=../../../../../tce/src/codesign/ttasim/ttasim

tpef=`mktemp tmpXXXXX`.tpef

$tcecc -a data/ldw2stw2.adf data/raw_data_customop.c -o $tpef -O3
$ttasim -a data/ldw2stw2.adf -p $tpef --no-debugmode
