#!/bin/bash
function simulate {
    ../../../src/codesign/ttasim/ttasim $* \
        -a data/address_clipping.adf \
        -p data/address_clipping.tpef \
        --no-debugmode
}

simulate

# Compiled simulator segfaults with this test, enabled when fixed.
# simulate -q 
