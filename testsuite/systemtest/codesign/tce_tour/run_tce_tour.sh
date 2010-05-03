#!/bin/bash
# Script to run tce tour test

BUILDOPSET=../../../../tce/src/codesign/osal/OSALBuilder/buildopset
TTASIM=../../../../tce/src/codesign/ttasim/ttasim
SIMCMD=data/simulator_commands
CREATEBEM=../../../../tce/src/bintools/BEMGenerator/createbem
PROGE=../../../../tce/src/procgen/ProGe/generateprocessor
PIG=../../../../tce/src/bintools/PIG/generatebits
TCECC=../../../../tce/src/bintools/Compiler/tcecc

MINIMAL=../../../../tce/data/mach/minimal.adf
MACH1=start.adf
MACH2=data/custom.adf
IDF=data/custom.idf
PROG1=crc.tpef
PROG2=custom.tpef
BEM=custom.bem

SYMBOLS=main,result
PROGE_OUT=proge-output
DMEM_IMG=custom_data.img
IMEM_IMG=custom.img

test_success() {
if [ $? -ne 0 ]
then
	echo $1
  exit 1
fi
}

$BUILDOPSET data/tutorial

# copy the minimal.adf
cp $MINIMAL $MACH1

if [ ! -e $MACH1 ]
then
  echo "Failed to copy minimal.adf from $MINIMAL"
  exit 1
fi

# compile the original
$TCECC -O0 -a $MACH1 -o $PROG1 -k $SYMBOLS data/crc.c data/main.c
test_success "Failed to compile original version of crc"

# simulate the original and verify result
$TTASIM -e "mach $MACH1; prog $PROG1" < $SIMCMD
test_success "Simulation of original program failed"

# compile the custom op version
$TCECC -O0 -a $MACH2 -o $PROG2 -k $SYMBOLS data/crc_with_custom_op.c data/main.c
test_success "Failed to compile custom op version of crc"

# simulate and verify results
$TTASIM -e "mach $MACH2; prog $PROG2" < $SIMCMD
test_success "Simulation of custom op program failed"

# create bem
$CREATEBEM $MACH2
test_success "Failed to create bem"

# use processor generator
rm -rf $PROGE_OUT
$PROGE -i $IDF -b $BEM -o $PROGE_OUT $MACH2
test_success "Failed to generate processor"

# generate bit images
$PIG -b $BEM -d -w 4 -p $PROG2 -x $PROGE_OUT $MACH2
test_success "Failed to generate bit images"

# If GHDL is found from PATH, compile and simulate
GHDL=$(which ghdl 2> /dev/null)
if [ "x${GHDL}" != "x" ]
then
  cd $PROGE_OUT
  ./ghdl_compile.sh >& /dev/null
  test_success "Failed to compile testbench"
  ./ghdl_simulate.sh >& /dev/null
  test_success "Failed to simulate testbench"
fi

exit 0
