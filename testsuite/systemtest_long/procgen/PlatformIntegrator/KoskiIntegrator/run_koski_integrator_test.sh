#!/bin/bash
PROGE=../../../../../tce/src/procgen/ProGe/generateprocessor
ADF=data/koski_hibi_pe_dma.adf
IDF=$(echo $ADF | sed 's/.adf/.idf/g')
TPEF=prog.tpef
PO_DIR=proge-out-koski
ENT=koskenlaskija
IPXACT=spirit_comp_def_${ENT}_toplevel.xml
INTEG=KoskiIntegrator
LOG=koski_integrator.runlog

QMEGAWIZ=$(which qmegawiz 2> /dev/null)
XVFB=$(which xvfb-run 2> /dev/null)
EMULATE_QMEGAWIZ=yes
## is real qmegawiz available?
if [ "x$QMEGAWIZ" != "x" ];then
  if [ "x$DISPLAY" != "x" ];then
    # qmegawiz is in PATH and X connection available
    EMULATE_QMEGAWIZ=no
  elif [ "x$XVFB" != "x" ];then
    # can emulate X connection with xvfb-run
    EMULATE_QMEGAWIZ=no
    PROGE="$XVFB -a $PROGE"
  fi
fi

if [ "x$EMULATE_QMEGAWIZ" == "xyes" ]
then
  # Emulate qmegawiz with a script
  export PATH=$PWD/../data:$PATH
fi

# run integrator
rm -f $IPXACT
rm -rf $PO_DIR
$PROGE -i $IDF -f vhdl_array -d onchip -e $ENT -o $PO_DIR -p $TPEF \
-g $INTEG $ADF || exit 1

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

