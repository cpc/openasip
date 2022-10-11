#!/bin/bash

LICENSE_FILE_SCRIPT="license_script"
LICENSE_FILE_CPP="license_cpp"
LICENSE_FILE_TEX="license_tex"
LICENSE_FILE_VHDL="license_vhdl"

[ ! -e "${LICENSE_FILE_SCRIPT}" ] && echo "File ${LICENSE_FILE_SCRIPT} missing." && exit 1
[ ! -e "${LICENSE_FILE_CPP}" ] && echo "File ${LICENSE_FILE_CPP} missing." && exit 1
[ ! -e "${LICENSE_FILE_TEX}" ] && echo "File ${LICENSE_FILE_TEX} missing." && exit 1
[ ! -e "${LICENSE_FILE_VHDL}" ] && echo "File ${LICENSE_FILE_VHDL} missing." && exit 1

# default license file is cpp
LICENSE_FILE="${LICENSE_FILE_CPP}"

# function to test a file to determine the how and what license is prepended to it
# $1 file to be tested
function testFile {
    LINE_TO_INSERT=1
    # checking if some copyright or license already in the file
    head -40 "${1}" | grep -m 1 -iqE '[[:space:]](Copyright|License)([[:space:]]|\.)' && return 1
    # checking if the file is a script file
    head -1 "${1}" | grep -Eq '#!/.*/php($|[[:space:]])' && LINE_TO_INSERT=3 && return 0
    head -1 "${1}" | grep -Fq '#!/' && LICENSE_FILE="${LICENSE_FILE_SCRIPT}" && LINE_TO_INSERT=2 && return 0
    grep -qiE '.*\.(py|sh)$' <(echo $1) && LICENSE_FILE="${LICENSE_FILE_SCRIPT}" && return 0
    grep -qiE '.*\.tex$' <(echo $1) && LICENSE_FILE="${LICENSE_FILE_TEX}" && return 0
    grep -qiE '.*\.vhd[l]?$' <(echo $1) && LICENSE_FILE="${LICENSE_FILE_VHDL}" && return 0
    return 0
}

# insert $LICENSE_FILE to a another file given as a parameter
# $1 file where $LICENSE_FILE is inserted
# $LINE_TO_INSERT line in $1 where $LICENSE_FILE 
function insertFileToFile {
    local lineA="$LINE_TO_INSERT"
    local lineB=""
    let lineB=lineA+1
    # file where insert is done must have at least $LINE_TO_INSERT+1 lines
    [ "$(cat ${1} | wc -l)" -lt "${lineB}" ] && return 1
    echo "Adding license to file: $1 $LICENSE_FILE, to the line $lineA"

    sed -i -e "$lineA{ h
    r $LICENSE_FILE
    D
    }
    $lineB{ x
    G
    }" "$1"
}

# export function and variables needed by them
export -f insertFileToFile
export -f testFile
export LICENSE_FILE_SCRIPT
export LICENSE_FILE_TEX
export LICENSE_FILE_VHDL
export LICENSE_FILE

PREPEND_REGEX='.*\.(h|hh|cc|icc|sh|py|php|vhd[l]?|tex)$'

find . -regextype posix-extended -type f -regex "$PREPEND_REGEX" -exec bash -c "\
testFile {} && insertFileToFile {}" \;
