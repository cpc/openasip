#!/bin/bash
export TCE_ROOT="../../../../../tce"

function addToVarEnd {
    export $1="${!1}${2}"
}

function addToPath {
    if ! $(\grep -qE ":?${1}(:|$)" <(echo "${PATH}")); then
        addToVarEnd PATH ":$1" 
    fi  
}

# program paths
addToPath "${TCE_ROOT}/src/codesign/Explorer"
addToPath "${TCE_ROOT}/src/bintools/Compiler"
addToPath "${TCE_ROOT}/src/bintools/BEMGenerator"
addToPath "${TCE_ROOT}/src/procgen/ProGe"
addToPath "${TCE_ROOT}/src/bintools/PIG"

${TCE_ROOT}/scripts/c2vhdl data/application1/complex_multiply.c || { echo "c2vhdl script failed."; exit 1; }

# check that image files were created
if [ ! -e "complex_multiply_data.img" ]; then
    echo "Error: No data image generated."
fi

if [ ! -e "complex_multiply.img" ]; then
    echo "Error: No image generated."
fi
