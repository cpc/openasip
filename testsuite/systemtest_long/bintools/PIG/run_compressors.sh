#!/bin/bash
# Purpose of this test is to verify that the compressor plugins work. If ghdl
# is not installed, this test will only check that the plugins can create
# compressed images without crashing. When ghdl is present this test will run
# three vhdl simulations: without compression, with SimpleDictionary
# compressor and with MoveSlotDictionary compression. Finally the bus dumps
# are compared for verification.
# The reason why this test starts from C compilation is simple, this way we
# can assure that the compressors will work with up-to-date tce tools.
# There is no point compressing pre-generated, age old tpefs.

CC=../../../../tce/src/bintools/Compiler/tcecc
PROGE=../../../../tce/src/procgen/ProGe/generateprocessor
PIG=../../../../tce/src/bintools/PIG/generatebits
CBEM=../../../../tce/src/bintools/BEMGenerator/createbem

SRC="data/crc.c data/main.c"
INC="-Idata/"
ORIG=../../../../tce/data/mach/minimal.adf
ADF=minimal.adf
IDF=data/minimal.idf
BEM=minimal.bem
TPEF=app.tpef

DIR0=proge-output-no-compression
DIR1=proge-output-InstructionDictionary
DIR2=proge-output-MoveSlotDictionary

COMP1=../../../../tce/compressors/InstructionDictionary.so
COMP2=../../../../tce/compressors/MoveSlotDictionary.so

IMEM=app.img
DMEM=app_data.img

eexit() {
	echo "$1"
	exit 1
}

# We need to decrease the address spaces in minimal.adf. Otherwise ghdl will
# initiate a massive swap festival (this will fail if the default values are
# changed in minimal.adf)
cat $ORIG | sed 's|<max-address>16777215</max-address>|<max-address>8191</max-address>|g' \
| sed 's|<max-address>1048576</max-address>|<max-address>8191</max-address>|g' > $ADF

# compile from c code
$CC -O2 -a $ADF -o $TPEF $INC $SRC || eexit "Failed to compile app"

# generate bem
$CBEM $ADF || eexit "Createbem failed"

# generate processor without compression (0)
$PROGE -b $BEM -i $IDF -o $DIR0 $ADF >& proge.out || eexit "ProGe failed! Check that data/minimal.idf is up to date!"
cat proge.out | grep -v "Warning: Opcode defined in HDB for operation"
rm -f proge.out

# generate processor for simple dictionary (1)
$PROGE -b $BEM -i $IDF -o $DIR1 $ADF >& proge.out || eexit "ProGe failed! Check that data/minimal.idf is up to date!"
cat proge.out | grep -v "Warning: Opcode defined in HDB for operation"
rm -f proge.out

# generate processor for move slot dictionary (2)
$PROGE -b $BEM -i $IDF -o $DIR2 $ADF >& proge.out || eexit "ProGe failed! Check that data/minimal.idf is up to date!"
cat proge.out | grep -v "Warning: Opcode defined in HDB for operation"
rm -f proge.out

# generate images for processor 0
$PIG -b $BEM -d -w 4 -p $TPEF -x $DIR0 $ADF || eexit "PIG failed (without compression)!"
mv $IMEM $DIR0/tb/imem_init.img || eexit "Imem image is missing"
mv $DMEM $DIR0/tb/dmem_init.img || eexit "Dmem image is missing"

# generate images for processor 1
$PIG -b $BEM -d -w 4 -p $TPEF -c $COMP1 -g -x $DIR1 $ADF || eexit "PIG failed with InstructionDictionary"
mv $IMEM $DIR1/tb/imem_init.img || eexit "Imem image is missing"
mv $DMEM $DIR1/tb/dmem_init.img || eexit "Dmem image is missing"

# generate images for processor 2
$PIG -b $BEM -d -w 4 -p $TPEF -c $COMP2 -g -x $DIR2 $ADF || eexit "PIG failed with InstructionDictionary"
mv $IMEM $DIR2/tb/imem_init.img || exit "Imem image is missing"
mv $DMEM $DIR2/tb/dmem_init.img || exit "Dmem image is missing"

GHDL=$(which ghdl)

if [ x$GHDL = "x" ]
then
	# ghdl is not installed, no point going further
	exit 0
fi

COMPILE=./ghdl_compile.sh
SIMULATE=./ghdl_simulate.sh


cd $DIR0
$COMPILE >& compile.log || eexit "Failed to compile testbench without compression. See $DIR0/compile.log"
$SIMULATE >& sim.log || eexit "Failed to simulate testbench without compression. See $DIR0/sim.log"
cd ..

cd $DIR1
$COMPILE >& compile.log || eexit "Failed to compile testbench with InstructionDictionary. See $DIR1/compile.log"
$SIMULATE >& sim.log || eexit "Failed to simulate testbench with InstructionDictionary. See $DIR1/sim.log"
cd ..

cd $DIR2
$COMPILE >& compile.log || eexit "Failed to compile testbench with MoveSlotDictionary. See $DIR2/compile.log"
$SIMULATE >& sim.log || eexit "Failed to simulate testbench with MoveSlotDictionary. See $DIR2/sim.log"
cd ..

DIFF1=simple_diff.txt
diff $DIR0/bus.dump $DIR1/bus.dump >& $DIFF1 || echo "Simple dictionary compressor is broken! Difference in $DIFF1"

DIFF2=move_slot_diff.txt
diff $DIR0/bus.dump $DIR2/bus.dump >& $DIFF2|| echo "Move slot dictionary compressor is broken! Difference in $DIFF2"


