#!/bin/bash
# Script to run checking verilog against vhdl on tce tour test

BUILDOPSET=../../../../tce/src/codesign/osal/OSALBuilder/buildopset
CREATEBEM=../../../../tce/src/bintools/BEMGenerator/createbem
PROGE=../../../../tce/src/procgen/ProGe/generateprocessor
PIG=../../../../tce/src/bintools/PIG/generatebits
TCECC=../../../../tce/src/bintools/Compiler/tcecc

MACH1=data/start.adf
MACH2=data/custom.adf
IDF1=data/start.idf
IDF2=data/custom.idf
PROG1=start.tpef
PROG2=custom.tpef
BEM1=start.bem
BEM2=custom.bem
VHDL_OUT1=vhdl_out1
VHDL_OUT2=vhdl_out2
VERILOG_OUT1=ver_out1
VERILOG_OUT2=ver_out2

SYMBOLS=main,result

test_success() {
if [ $? -ne 0 ]
then
	echo $1
  exit 1
fi
}

# GHDL and IVerilog must be used together for check verilog against vhdl netlist
# If GHDL and Iverilog is found from PATH, compile and simulate, otherwise error
GHDL=$(which ghdl 2> /dev/null)
IVER=$(which iverilog 2> /dev/null)
if [ "x${GHDL}" != "x" ] 
then
if [ "x${IVER}" != "x" ]
then

$BUILDOPSET data/tutorial

# compile the original
$TCECC -O0 -a $MACH1 -o $PROG1 -k $SYMBOLS data/crc.c data/main.c
test_success "Failed to compile original version of crc"
# compile the custom op version
$TCECC -O3 -a $MACH2 -o $PROG2 -k $SYMBOLS data/crc_with_custom_op.c data/main.c
test_success "Failed to compile custom version of crc"

# create bem
$CREATEBEM $MACH1
test_success "Failed to create bem of original crc version"
# create bem
$CREATEBEM $MACH2
test_success "Failed to create custom bem"

# use processor generator for vhdl netlist generate
rm -rf $VHDL_OUT1
$PROGE -t -i $IDF1 -b $BEM1 -o $VHDL_OUT1 $MACH1
test_success "Failed to generate vhdl processor on original crc"
# use processor generator for vhdl netlist generate
rm -rf $VHDL_OUT2
$PROGE -t -i $IDF2 -b $BEM2 -o $VHDL_OUT2 $MACH2
test_success "Failed to generate custom  vhdl processor"

# use processor generator for verilog netlist generate
rm -rf $VERILOG_OUT1
$PROGE -t  -l verilog -i $IDF1 -b $BEM1 -o $VERILOG_OUT1 $MACH1
test_success "Failed to generate verilog processor on original crc"
# use processor generator for verilog netlist generate
rm -rf $VERILOG_OUT2
$PROGE -t  -l verilog -i $IDF2 -b $BEM2 -o $VERILOG_OUT2 $MACH2
test_success "Failed to generate custom verilog processor"

# generate bit images
$PIG -b $BEM1 -d -w 4 -p $PROG1 -x $VHDL_OUT1 $MACH1
test_success "Failed to generate vhdl bit image of original crc version"
$PIG -b $BEM1 -d -w 4 -p $PROG1 -x $VERILOG_OUT1 $MACH1
test_success "Failed to generate verilog bit image of original crc version"

# generate bit images
$PIG -b $BEM2 -d -w 4 -p $PROG2 -x $VHDL_OUT2 $MACH2
test_success "Failed to generate custom vhdl bit images"
$PIG -b $BEM2 -d -w 4 -p $PROG2 -x $VERILOG_OUT2 $MACH2
test_success "Failed to generate custom verilog bit images"


cd $VHDL_OUT1
./ghdl_compile.sh >& /dev/null
test_success "Failed to compile original vhdl testbench"
./ghdl_simulate.sh >& /dev/null
test_success "Failed to simulate original vhdl testbench"
cd ..
cd $VERILOG_OUT1
./iverilog_compile.sh >& /dev/null
test_success "Failed to compile original verilog testbench"
./iverilog_simulate.sh >& /dev/null
test_success "Failed to simulate original verilog testbench"
cd ..
diff $VERILOG_OUT1/bus.dump $VHDL_OUT1/bus.dump >& /dev/null
test_success "Original version crc: Bus acivities verilog and vhdl are different"

cd $VHDL_OUT2
./ghdl_compile.sh >& /dev/null
test_success "Failed to compile custom vhdl testbench"
./ghdl_simulate.sh >& /dev/null
test_success "Failed to simulate custom vhdl testbench"
cd ..
cd $VERILOG_OUT2
./iverilog_compile.sh >& /dev/null
test_success "Failed to compile custom verilog testbench"
./iverilog_simulate.sh >& /dev/null
test_success "Failed to simulate custom verilog testbench"
cd ..
diff $VERILOG_OUT2/bus.dump $VHDL_OUT2/bus.dump >& /dev/null
test_success "Custom version crc: Bus acivities verilog and vhdl are different"

exit 0

fi
fi
exit 1