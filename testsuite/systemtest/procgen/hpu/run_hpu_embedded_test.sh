#!/bin/bash
TCECC=../../../../openasip/src/bintools/Compiler/tcecc
DISASM=../../../../openasip/src/bintools/Disassembler/tcedisasm
PROGE=../../../../openasip/src/procgen/ProGe/generateprocessor
PIG=../../../../openasip/src/bintools/PIG/generatebits

PDIR=proge-output
ADF=data/test_fpu_half.adf
#ADF=data/test2.adf
IDF=$(echo $ADF | sed 's/.adf/.idf/')
TPEF=test_code.tpef

RUNCYCLES=3000
RUNTIME=${RUNCYCLES}0

echo "Testing FPUs..."

# Compile program
$TCECC -O1 -a $ADF -o $TPEF -k result data/test_code.cpp --bottom-up-scheduler || exit 1

# Check that there are no emulation functions
$DISASM -o temp.txt $ADF $TPEF || exit 1
grep emulate temp.txt
rm temp.txt

# Generate processor
$PROGE -t -i $IDF -o $PDIR $ADF
# Generate program image
$PIG -d -w 4 -p $TPEF -x $PDIR $ADF >& /dev/null

# Simulate the processor
cd $PDIR || exit 1

# change the simulation time
./ghdl_compile.sh >& /dev/null ||  exit 1
if [ -e testbench ]; then
    eval "./testbench --assert-level=none --vcd=wave.vcd --stop-time=${RUNTIME}ns >& /dev/null" || exit 1
else
    ghdl -r --workdir=work --ieee=synopsys testbench \
         --vcd=wave.vcd --assert-level=none --stop-time=${RUNTIME}ns \
         >& /dev/null || exit 1
fi

# Print simulation output
cat printchar_output.txt

echo "Testing FMA and minf/maxf..."

# Do the same with MAC test
ADF=data/test2.adf
IDF=$(echo $ADF | sed 's/.adf/.idf/')
cd ..
./finalize
$TCECC -O1 -a $ADF -o $TPEF -k result data/test_code_2.cpp --bottom-up-scheduler || exit 1
$DISASM -o temp.txt $ADF $TPEF || exit 1
grep emulate temp.txt
rm temp.txt
$PROGE -t -i $IDF -o $PDIR $ADF
$PIG -d -w 4 -p $TPEF -x $PDIR $ADF >& /dev/null
cd $PDIR || exit 1
./ghdl_compile.sh >& /dev/null ||  exit 1
if [ -e testbench ]; then
    eval "./testbench --assert-level=none --vcd=wave.vcd --stop-time=${RUNTIME}ns >& /dev/null" || exit 1
else
    ghdl -r --workdir=work --ieee=synopsys testbench \
         --vcd=wave.vcd --assert-level=none --stop-time=${RUNTIME}ns \
         >& /dev/null || exit 1
fi
cat printchar_output.txt

echo "Testing SIMD2 half operations..."

# Do the same with MAC test
ADF=data/complex.adf
IDF=$(echo $ADF | sed 's/.adf/.idf/')
cd ..
./finalize
$TCECC -O1 -a $ADF -o $TPEF -k result data/complex.cpp --bottom-up-scheduler || exit 1
$DISASM -o temp.txt $ADF $TPEF || exit 1
grep emulate temp.txt
rm temp.txt
$PROGE -t -i $IDF -o $PDIR $ADF
$PIG -d -w 4 -p $TPEF -x $PDIR $ADF >& /dev/null
cd $PDIR || exit 1
./ghdl_compile.sh >& /dev/null ||  exit 1
if [ -e testbench ]; then
    eval "./testbench --assert-level=none --vcd=wave.vcd --stop-time=${RUNTIME}ns >& /dev/null" || exit 1
else
    ghdl -r --workdir=work --ieee=synopsys testbench \
         --vcd=wave.vcd --assert-level=none --stop-time=${RUNTIME}ns \
         >& /dev/null || exit 1
fi

cat printchar_output.txt
