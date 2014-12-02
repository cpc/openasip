#!/bin/bash

usage() {
    echo "Usage: ./generatebustrace [options] OPPFILES ADFFILE TPEFFILE"
    echo "Produces bus trace from program running on machine."
    echo "Options:"
    echo "  -o       Output bus trace file. Default: TPEFFILE.bustrace"
    echo "  -i       Include path to custom OSAL file (.opp)"
    echo "  -p       Print trace to stdout."
    echo "  -h       This help text."
}

clean_n_exit() {
    [ -n "$tmp_tpef" ] && rm $tmp_tpef
    [ -n "$ttasimlog" ] && rm $ttasimlog
    [ -n "$2" ] && printf "$2 \n"
    exit $1
}

osal_includes=
ttasimlog=sim.log
adf=
tpef=
modulenames=
printstdout=

OPTIND=1
while getopts "ho:i:p" OPTION
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
        p)
            printstdout=true
            ;;
        ?)
            usage
            exit 1
            ;;
    esac
done
shift "$((OPTIND-1))"

for i in "$@"; do
    extension="${i##*.}"
    case $extension in
        adf)
            adf="$i"
            ;;
        tpef)
            tpef="$i"
            ;;
        opp)
            osal_includes="${osal_includes} $i"
            ;;
    esac
done

[ -z "$adf" ] && clean_n_exit 1 "Missing ADF."
[ -z "$tpef" ] && clean_n_exit 1 "Missing tpef."

# By default bustrace is generated at tpef's directory.
tmp_tpef=$(mktemp tmpXXXXXX.tpef) || clean_n_exit 1
cp $tpef $tmp_tpef


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

ttasim --no-debugmode -e "setting bus_trace 1; mach $adf; prog $tmp_tpef; run; quit;" > $ttasimlog || clean_n_exit 1 "ttasimlog"

[ -n "$printstdout" ] && cat "${tmp_tpef}.bustrace"

if [ -n "$bustracefile" ]; then
    mv ${tmp_tpef}.bustrace $bustracefile || clean_n_exit 1
else
    mv ${tmp_tpef}.bustrace $(basename ${tpef}).bustrace || clean_n_exit 1
fi


clean_n_exit 0
