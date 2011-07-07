#!/bin/bash
PROGE=../../../../../tce/src/procgen/ProGe/generateprocessor
TCECC=../../../../../tce/src/bintools/Compiler/tcecc
PIG=../../../../../tce/src/bintools/PIG/generatebits
ADF=data/koski_hibi_pe_dma.adf
IDF=$(echo $ADF | sed 's/.adf/.idf/g')
SRC=data/main.c
TPEF=prog.tpef
PO_DIR=proge-out-koski
ENT=koskenlaskija
IPXACT=spirit_comp_def_${ENT}_toplevel.xml
INTEG=KoskiIntegrator

$TCECC -a $ADF -o $TPEF $SRC >& /dev/null

# run integrator
rm -f $IPXACT
rm -rf $PO_DIR
$PROGE -t -i $IDF -f onchip -d onchip -e $ENT -o $PO_DIR -p $TPEF \
-g $INTEG $ADF || exit 1

$PIG -d -w4 -f mif -o mif -p $TPEF -x PO_DIR -e $ENT -x $PO_DIR $ADF >& /dev/null

# reset env params to make sure sort is deterministic
export LANG=C
export LC_LL=C
# check that hibi bus interface is found and mapped to component ports
grep "component " $IPXACT
grep "busType" $IPXACT
grep "abstractionType" $IPXACT
echo "Component signal mappings"
grep "physicalPort" $IPXACT | wc -l | tr -d "\ "
echo "Bus signal mappings"
grep "logicalPort" $IPXACT | wc -l | tr -d "\ "

# Check if new toplevel is correct. Signals might be in different order
# depending on the language setting so grep them away
cat $PO_DIR/platform/${ENT}_toplevel.vhdl | grep -v " signal "
exit 0

