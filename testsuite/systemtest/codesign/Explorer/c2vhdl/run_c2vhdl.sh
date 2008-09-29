#!/bin/bash
TCE_ROOT="../../../../../tce"

EXPLORE="${TCE_ROOT}/src/codesign/Explorer/explore"
TCECC="${TCE_ROOT}/src/bintools/Compiler/tcecc"
CREATEBEM="${TCE_ROOT}/src/bintools/BEMGenerator/createbem"
GENERATEPROCESSOR="${TCE_ROOT}/src/procgen/ProGe/generateprocessor"
GENERATEBITS="${TCE_ROOT}/src/bintools/PIG/generatebits"

ADF="${TCE_ROOT}/data/mach/minimal.adf"

OUTPUT="/dev/null"
__DEBUG="false"
if [ "${__DEBUG}x" == "truex" ]; then
    OUTPUT="debug.log"
fi

{
$TCECC data/application1/complex_multiply.c -O2 -o data/program.bc
echo "running: $EXPLORE -a $ADF -d data/application1 ExpRes.dsdb" 
$EXPLORE -a $ADF -d data ExpRes.dsdb 

NEW_CONF="$($EXPLORE -e SimpleICOptimizer -s 1 ExpRes.dsdb \
| tail -n1 | grep -x '[[:space:]][0-9][0-9]*' | tr -d '[[:space:]]')"
echo "New config created: ${NEW_CONF}" 

NEW_CONF="$($EXPLORE -e ImplementationSelector -s $NEW_CONF ExpRes.dsdb \
| tail -n1 | grep -x '[[:space:]][0-9][0-9]*' | tr -d '[[:space:]]')"
echo "New config created: ${NEW_CONF}" 

$EXPLORE -w ${NEW_CONF} ExpRes.dsdb
$TCECC -a ${NEW_CONF}.adf -o complex_multiply.scheduled.tpef data/program.bc 
$CREATEBEM ${NEW_CONF}.adf 
$GENERATEPROCESSOR -b ${NEW_CONF}.bem -i ${NEW_CONF}.idf ${NEW_CONF}.adf 
$GENERATEBITS -b ${NEW_CONF}.bem -d -p complex_multiply.scheduled.tpef ${NEW_CONF}.adf 
} 1>${OUTPUT} 2>&1

# check that image files were created
if [ ! -e "complex_multiply.scheduled_data.img" ]; then
    echo "Error: No data image generated."
fi

if [ ! -e "complex_multiply.scheduled.img" ]; then
    echo "Error: No image generated."
fi
