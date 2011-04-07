#!/bin/bash
TCECC=../../../../../tce/src/bintools/Compiler/tcecc
PROGE=../../../../../tce/src/procgen/ProGe/generateprocessor
PIG=../../../../../tce/src/bintools/PIG/generatebits
ADF=data/avalon_lsu_sfu.adf
IDF=$(echo $ADF | sed 's/.adf/.idf/g')
TPEF=prog.tpef
PO_DIR=proge-out-avalon-lsu-sfu
ENT=tta_avalon_lsu_sfu
INTEG=AvalonIntegrator
LOG=run_avalon_lsu_sfu.log

# Extra option for generating real HW with functional program images
# Require real qmegawiz found from PATH
HW=no
QMEGAWIZ=$(which qmegawiz 2> /dev/null)
if [ "x$QMEGAWIZ" == "x" ]
then
  # Emulate qmegawiz with a script
  export PATH=$PWD/../data:$PATH
else
  HW=yes
fi

# run integrator
rm -f ${ENT}_toplevel_hw.tcl $LOG
rm -rf $PO_DIR
$PROGE -i $IDF -f onchip -d none -e $ENT -o $PO_DIR -p $TPEF \
-g $INTEG $ADF >& $LOG || exit 1

# check sopc builder component file
echo "Component file contents:"
echo "module properties:"
cat ${ENT}_toplevel_hw.tcl | grep "set_module_property" | wc -l | tr -d "\ "
echo "design files:"
cat ${ENT}_toplevel_hw.tcl | grep "add_file" | wc -l | tr -d "\ "
echo "interfaces:"
cat ${ENT}_toplevel_hw.tcl | grep "add_interface "

# Check if new toplevel is correct. Signals might be in different order
# depending on the language setting so grep them away
cat $PO_DIR/platform/${ENT}_toplevel.vhdl | grep -v " signal "

if [ "$HW" == "yes" ]
then
    # compile code
    $TCECC -O3 -a $ADF -o $TPEF data/src/main_dual.c || exit 1
    $PIG -d -w 4 -o mif -f mif -p $TPEF -x $PO_DIR $ADF || exit 1
fi

