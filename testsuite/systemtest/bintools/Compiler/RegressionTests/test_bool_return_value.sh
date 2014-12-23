#!/bin/bash
tcecc=../../../../../tce/src/bintools/Compiler/tcecc
ttasim=../../../../../tce/src/codesign/ttasim/ttasim

tpef=bool_return_value.tpef
src=data/bool_return_value.cpp
adf=../../../../../tce/data/mach/minimal.adf

$tcecc -O0 --swfp -a $adf -o $tpef $src

$ttasim -p $tpef -a $adf -e "run; puts [x /n 1 /u w eq]; puts [x /n 1 /u w gt]; puts [x /n 1 /u w nef]; puts [x /n 1 /u w gtf]; quit;"

rm -f $tpef
