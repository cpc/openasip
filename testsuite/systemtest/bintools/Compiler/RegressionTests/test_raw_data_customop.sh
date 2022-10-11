#!/bin/bash

tcecc=../../../../../openasip/src/bintools/Compiler/tcecc
ttasim=../../../../../openasip/src/codesign/ttasim/ttasim

tpef=`mktemp tmpXXXXX`

$tcecc -a data/ldw2stw2.adf data/raw_data_customop.c -o $tpef -O3
$ttasim -a data/ldw2stw2.adf -p $tpef --no-debugmode

rm -f $tpef
