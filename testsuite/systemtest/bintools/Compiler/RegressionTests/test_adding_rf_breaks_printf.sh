#!/bin/bash
#
# Reproduces a bug where adding an RF produces zeros outputted
# from printf().

tcecc=../../../../../tce/src/bintools/Compiler/tcecc
ttasim=../../../../../tce/src/codesign/ttasim/ttasim
tpef=/tmp/fooo234sdwe342klwemwet.124.tpef

function test_machine {
  $tcecc -O0 -a $1 -o $tpef -k main data/printf_broken.c
  $ttasim -a $1 -p $tpef -e "run; puts [x /u b temp1]; puts [x /u b temp2]; quit;"
  rm -f $tpef
}

test_machine data/printf_machine_works.adf
test_machine data/printf_machine_broken.adf
