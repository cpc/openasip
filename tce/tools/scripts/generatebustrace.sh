#!/bin/bash

usage() {
    echo "Usage: ./generatebustrace [options] ADFFILE TPEFFILE"
    echo "Produces bus trace from program running on machine."
    echo "Options:"
    echo "  -o       Output bus trace file. Default: TPEFFILE.bustrace"
    echo "  -i       Include path to custom OSAL file (.opp)"
    echo "  -h       This help text."
}

clean_n_exit() {
    [ -n "$tmp_tpef" ] && rm $tmp_tpef
    exit $1
}

osal_includes=

OPTIND=1
while getopts "ho:i:" OPTION
do
    case $OPTION in
        o)
            bustracefile=$OPTARG
            ;;
        i)
            osal_includes="${osal_includes} $OPTARG"
            ;;
        h)
            usage
            exit 0
            ;;
        ?)
            usage
            exit 1
            ;;
    esac
done
shift "$((OPTIND-1))"

adf=$1
tpef=$2

# By default bustrace is generated at tpef's directory.
tmp_tpef=$(mktemp tmpXXXXXX.tpef) || clean_n_exit 1
cp $tpef $tmp_tpef

modulenames=
# Custom osal files are needed where tta simulator is invoked.
if [ -n "$osal_includes" ]; then
    cp $osal_includes .
    for i in $osal_includes; do  
        modulefile=$(basename $i)
        modulename=$(basename $modulefile .opp)
        modulenames="${modulenames} $modulename"
        osal_src=$(dirname $i)
        buildopset -s $osal_src $modulename || clean_n_exit 1
    done
fi

ttasim --no-debugmode -e "setting bus_trace 1; mach $adf; prog $tmp_tpef; run; quit;" >& /dev/null || clean_n_exit 1

if [ -n "$bustracefile" ]; then
    mv ${tmp_tpef}.bustrace $bustracefile || clean_n_exit 1
else
    mv ${tmp_tpef}.bustrace $(basename ${tpef}).bustrace || clean_n_exit 1
fi

clean_n_exit 0
