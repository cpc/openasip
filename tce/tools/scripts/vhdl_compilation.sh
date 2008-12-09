#!/bin/bash
# Copyright (c) 2002-2009 Tampere University of Technology.
#
# This file is part of TTA-Based Codesign Environment (TCE).
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
###
# modelsim and ghdl compilation script
# Esa Määttä <esa.maatta@tut.fi>
# TODO: this script doesn't work if there are whitespace chars in file or path names
# TODO: add vmake work > Makefile
###
# "testbench" is used as testbench name for now.
# "./work" is used as work dir.
# 
##

printusage() {
    echo "Usage: $0 [vcom|ghdl] RelativeProgeOutPutDir RelativeDataDir [echo]"
    echo -e "\t RelativeProgeOutPutDir is a directory relative to your current working dir."
    echo -e "\t RelativeDataDir is a relative directory that contains additional data needed for compilation & simulation."
    echo
    echo -e "\t To get dependencies between vhdl files right, a hdb file is needed in project root dir."
}

# check cmdline params
case "x-$1" in
    x-ghdl)
        PROGEX="ghdl -i --workdir=work";;
    x-vcom)
        PROGEX="vcom";;
    x-h) # help
        printusage
        exit 1;;
    x-) # this is default
        printusage
        exit 1;;
    *)
        printusage
        exit 1;;
esac

ROOTDIR="."
DATADIR="."
WORKDIR="${ROOTDIR}/work"
PROGEDIR="${ROOTDIR}/$(basename ${2})"
VHDLDIR="${PROGEDIR}/vhdl"
GCUICDIR="${PROGEDIR}/gcu_ic"

# parameters given to ghdl
GHDLPARAMS="--ieee=synopsys -fexplicit"
TESTBENCH="testbench"

# where to find hdb file
HDBFILE='*.hdb' # block source files parsed from this hdb file

if [ ! "${2}" ]; then
    printusage
    exit 1 
fi

if [ ! "${3}" -o "${3}" == "echo" ]; then
    DATADIR="${ROOTDIR}"
else
    DATADIR="${ROOTDIR}/${3}"
fi

if [[ ! -d "${PROGEDIR}" && ! -d "${VHDLDIR}" && ! -d "${GCUICDIR}"  && ! -d "${DATADIR}" ]]; then
    echo "Proge output file directory didn't exist or didn't contain directories:"
    echo "Toplevel directory: ${VHDLDIR}"
    echo "Plugin directory: ${GCUICDIR}"
    echo "Data directory: ${DATADIR}"
    exit 1
fi

RUNCMD=""
PARAMS=($@)
LASTPARAMNUM=$#-1
if [ "${PARAMS[LASTPARAMNUM]}" == "echo" ]; then
    RUNCMD="echo" #DEBUG, dosn't run commands just echos them to screen
fi

# doesn't work if space chars in file or path names
# $2 prefix for files (ex. dir)
# $1 = files to go through
runcmdfor() {
    ARR=(${1})
    for arvar in "${ARR[@]}"; do
        if [[ "${2}" && "$(basename ${arvar})" == "${arvar}" ]]; then
            if [ -f "${2}/${arvar}" ]; then
                ${RUNCMD} ${PROGEX} ${2}/${arvar} || ISFAILED="TRUE"
            fi
        else
            if [ -f "${arvar}" ]; then
                ${RUNCMD} ${PROGEX} ${arvar} || ISFAILED="TRUE"
            fi
        fi
    done
}

checkecmd() {
    if [[ "no" == "$(which "${1}" | awk '{print $2}')" ]]; then
        echo "${1} program was not in path."
        exit 1
    fi
}


# ===========================================================================

# global parameters for toplevel
GPTOP=("globals_pkg.vhdl" "toplevel_params_pkg.vhdl")

# decoder, decompressor and ifetch
DDIF=("opcodes_pkg.vhdl" "decoder.vhdl" \
    "decompressor.vhdl" "ifetch.vhdl" "highest_pkg.vhdl" )

# TODO: is -depth same as max+mindepth
# IC sockets
INPUTSOC=($(find "${GCUICDIR}" -maxdepth 1 -mindepth 1 -type f -name \
"input_socket_*.vhdl" | xargs))
OUTPUTSOC=($(find "${GCUICDIR}" -maxdepth 1 -mindepth 1 -type f -name \
"output_socket_*.vhdl" | sort | xargs))

# FU's and RF's from .hdb file
VHDLFILES=()
HDBTMP=($HDBFILE)
if [ -e "${HDBTMP[0]}" ]; then
    checkecmd "sqlite3"
    VHDLFILES=($(echo ".dump block_source_file" | sqlite3 -batch -line \
    ${HDBTMP[0]} | grep -Po "'[[:print:]]*'" | tr -d "'" | xargs))
fi
# testbed constants and memory models from root
#MEMOD=("testbench_constants_pkg.vhdl" "synch_dualport_sram.vhdl" \
#    "synch_sram.vhdl" "imem_arbiter.vhdl" "mem_arbiter.vhdl" "proc_ent.vhdl" \
#    "proc_arch.vhdl" "clkgen.vhdl" "testbench.vhdl" "testbench_cfg.vhdl")

############################
# files remaining in VHDLDIR
TEMP="$(\ls -1 "${VHDLDIR}"/*.{pkg,vhd{l,}} 2>/dev/null)"

VHDLALL=("${GPTOP[@]}" "${VHDLFILES[@]}")

for file in "${VHDLALL[@]}"; do
    TEMP=$(echo -n "${TEMP}" | \grep -Pv "${file}")
done

VHDLFILES_REM=$(echo $TEMP | xargs)
#echo "REM_VHDL: ${VHDLFILES_REM}"

#############################
# files remaining in GCUICDIR
TEMP="$(\ls -1 "${GCUICDIR}"/*.{pkg,vhd{l,}} 2>/dev/null)"

GCUICALL=("${DDIF[@]}" "${INPUTSOC[@]}" "${OUTPUTSOC[@]}")

for file in "${GCUICALL[@]}"; do
    TEMP=$(echo -n "${TEMP}" | \grep -Pv "${file}")
done

GCUICFILES_REM=$(echo $TEMP | xargs)
#echo "REM_GCUIC: ${GCUICFILES_REM}"

#############################
# files remaining in DATADIR
TEMP="$(\find "${DATADIR}"/ -iname '*.pkg' -o -iname '*.vhd' -o -iname '*.vhdl' 2>/dev/null)"

TEMP=$(echo -n "${TEMP}" | \grep -Pv "${VHDLDIR}|${GCUICDIR}")

DATAFILES_REM=$(echo $TEMP | xargs)
#echo "REM_ROOT: ${DATAFILES_REM}"

# ===========================================================================

# remove old work dir
${RUNCMD} rm -rf $(basename ${WORKDIR})

# if workdir doesn't exist, create it, vcom creates it automaticly
if [[ ! "${PROGEX}" == "vcom" ]]; then
    checkecmd "ghdl"
    ${RUNCMD} rm -rf bus.dump
    ${RUNCMD} rm -rf testbench
    ${RUNCMD} mkdir -p "${WORKDIR}"
fi

if [[ "${PROGEX}" == "vcom" ]]; then
    checkecmd "vcom"
    ${RUNCMD} vlib $(basename ${WORKDIR})
    ${RUNCMD} vmap ${WORKDIR}
fi


#echo "${GPTOP[*]}"
runcmdfor "${GPTOP[*]}" "${VHDLDIR}"
#echo "${DDIF[*]}"
runcmdfor "${DDIF[*]}" "${GCUICDIR}"
#echo "${INPUTSOC[*]}"
runcmdfor "${INPUTSOC[*]}" "${GCUICDIR}"
#echo "${OUTPUTSOC[*]}"
runcmdfor "${OUTPUTSOC[*]}" "${GCUICDIR}"
#echo "${GCUICFILES_REM[*]}"
runcmdfor "${GCUICFILES_REM[*]}"

#echo "${VHDLFILES[*]}"
runcmdfor "${VHDLFILES[*]}" "${VHDLDIR}" 
#echo "${VHDLFILES_REM[*]}"
runcmdfor "${VHDLFILES_REM[*]}" "${VHDLDIR}"

# compiling testbench and memories
#echo "${DATAFILES_REM[*]}"
runcmdfor "${DATAFILES_REM[*]}"

if [[ ! "${PROGEX}" == "vcom" ]]; then
    #echo "ghdl -m --workdir=work --ieee=synopsys -fexplicit testbench >& \
    #/dev/null"
    echo "Starting compiling..."
    ${RUNCMD} ghdl -m --workdir=$(basename ${WORKDIR}) ${GHDLPARAMS} ${TESTBENCH} || ISFAILED="TRUE"
    echo
    echo "Now if testbench creation was succesfull simulation can be run for example by command:"
    echo "./testbench --assert-level=none --stop-time=52390ns"
fi

if [ "${ISFAILED}" ]; then
    echo "WARNING: Something went wrong.."
fi

# TODO:
# modelsim simulation
#vsim -c 

# EOF
