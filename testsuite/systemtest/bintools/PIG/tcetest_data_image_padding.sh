#!/bin/bash
### TCE TESTCASE
### title: Tests padding of data images, when the data
### image size is not divisible by data memory width.

#
# This is a test added after finding a mismatch in ttasim and RTL
# bustraces in tce tour due to a padding bug with LE machines.
#

OPTIND=1
while getopts "d" OPTION
do
    case $OPTION in
        d)
            leavedirty=true
            ;;
        ?)
            exit 1
            ;;
    esac
done
shift "$((OPTIND-1))"

SRC=./data/data_image_padding.tceasm
ADF=./data/data_mem_padding_test.adf
IDF=./data/data_mem_padding_test.idf
TPEF=./data/data_image_padding.tpef
PROCDIR=./data/data_padding_proc

clear_test_data() {
    rm -rf ${PROCDIR}
    rm -f ${TPEF}
    rm -f *.img
    rm -f *.tpef.*
    rm -f data/*.bustrace*
}

clear_test_data



tceasm -o ${TPEF} ${ADF} ${SRC}  >& /dev/null \
    || { echo "Error from tceasm"; exit 1; }
ttasim -e "setting bus_trace 1;mach ${ADF}; prog ${TPEF};run;quit" \
    >& /dev/null || { echo "Error from ttasim"; exit 1; }
generateprocessor -t -i ${IDF} -o ${PROCDIR} ${ADF} >& /dev/null \
    || { echo "Error from generateprocessor"; exit 1; }
generatebits -d -w 4 -p ${TPEF} -x ${PROCDIR} ${ADF} >& /dev/null \
    || { echo "Error from generatebits"; exit 1; }
cd ${PROCDIR}
./ghdl_compile.sh >& /dev/null \
    || { echo "Error from ghdl_compile.sh"; exit 1; }
ghdl -r --workdir=work --ieee=synopsys testbench --assert-level=none \
     --stop-time=1000ns >& /dev/null \
    || { echo "Error from simulation"; exit 1; }
cd ../..
diff ${TPEF}.bustrace \
     <(head -n $(wc -l < ${TPEF}.bustrace) < ${PROCDIR}/execbus.dump) \
     || { echo "ttasim vs RTL simulation bustrace mismatch"; exit 1; }


if [ "${leavedirty}" != "true" ]; then
    clear_test_data
fi
