#!/bin/bash
PROGE=../../../../../tce/src/procgen/ProGe/generateprocessor
ADF=data/koski_hibi_pe_dma.adf
IDF=$(echo $ADF | sed 's/.adf/.idf/g')
TPEF=prog.tpef
PO_DIR=proge-out-koski
ENT=koskenlaskija
IPXACT=spirit_comp_def_${ENT}.xml
INTEG=KoskiIntegrator
LOG=koski_integrator.runlog

# set qmegawiz script
export PATH=$PWD/data:$PATH

# run integrator
rm -f $IPXACT
rm -rf $PO_DIR
$PROGE -i $IDF -f vhdl_array -d onchip -e $ENT -o $PO_DIR -p $TPEF \
-g $INTEG $ADF || exit 1

# reset env params to make sure sort is deterministic
export LANG=C
export LC_LL=C
# check that hibi bus interface is found and mapped to component ports
grep "bus" $IPXACT | sort
grep "component" $IPXACT | sort

# Check if new toplevel is correct. Signals might be in different order
# depending on the language setting so grep them away
cat $PO_DIR/platform/$ENT.vhdl | grep -v " signal "
# Then check signals by sorting them
cat $PO_DIR/platform/$ENT.vhdl | grep " signal " | sort

