#!/bin/bash

usage() {
    echo "Usage: ./generatebustrace [options] OPPFILES ADFFILE TPEFFILE"
    echo "Produces bus trace from program running on machine."
    echo "Options:"
    echo "  -o       Output bus trace file. Default: TPEFFILE.bustrace"
    echo "  -i       Include path to custom OSAL file (.opp)"
    echo "  -p       Print trace to stdout."
    echo "  -l       Limit simulation to given amount instruction stepping."
    echo "           Default simulation limit is unlimited."
    echo "  -h       This help text."
}

clean_n_exit() {
    [ -n "$2" ] && printf "$2 \n"
    [ -n "$tmp_tpef" ] && rm -f $tmp_tpef*
    [ -n "$ttasimlog" ] && rm $ttasimlog
    exit $1
}

osal_includes=
adf=
tpef=
modulenames=
printstdout=
run_stmt="run"

OPTIND=1
while getopts "ho:i:pl:" OPTION
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
        l)
            run_stmt="stepi ${OPTARG}"
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

ttasimlog=$tpef.ttasim.log

[ -z "$adf" ] && clean_n_exit 1 "Missing ADF."
[ -z "$tpef" ] && clean_n_exit 1 "Missing tpef."

# By default bustrace is generated at tpef's directory.
tmp_tpef=$(mktemp tmpXXXXXX.tpef) || { tmp_tpef=; clean_n_exit 1; }
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

ttasim --no-debugmode -e "setting bus_trace 1; mach $adf; prog $tmp_tpef; ${run_stmt}; quit;" > $ttasimlog || { cat $ttasimlog; clean_n_exit 1; }

generated_bustrace=
if [ -f "${tmp_tpef}.bustrace" ]; then
    generated_bustrace=${tmp_tpef}.bustrace
elif [ -f "${tmp_tpef}.core0.bustrace" ]; then
    generated_bustrace=${tmp_tpef}.core0.bustrace
else
    cat $ttasimlog;
    clean_n_exit 1
fi

[ -n "$printstdout" ] && cat $generated_bustrace

if [ -n "$bustracefile" ]; then
    cp $generated_bustrace $bustracefile || clean_n_exit 1
else
    cp $generated_bustrace $(basename ${tpef}).bustrace || clean_n_exit 1
fi


clean_n_exit 0
