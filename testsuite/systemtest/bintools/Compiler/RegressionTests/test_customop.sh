#!/bin/bash

tcecc=../../../../../tce/src/bintools/Compiler/tcecc
tcedisasm=../../../../../tce/src/bintools/Disassembler/tcedisasm
tceopgen=../../../../../tce/src/bintools/Compiler/tceopgen/tceopgen

tpef=/tmp/test_customop.1234werqu802.tpef
adf=../../../../../tce/scheduler/testbench/ADF/huge.adf
program=data/customop.c

$tceopgen 2>&1 | grep -o "_TCE_ADD(i1, i2, o1)"
$tceopgen 2>&1 | grep -o "_TCEFU_ADD(FU, i1, i2, o1)"

$tcecc -O1 -a $adf -o $tpef $program 2>/dev/null
$tcedisasm -s $adf $tpef | grep -o "ALU8.S2.add"
$tcedisasm -s $adf $tpef | grep -o "ALU6.S2.sub"

