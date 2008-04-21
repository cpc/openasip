#!/bin/bash
function simulate {
    ../../../src/codesign/ttasim/ttasim $* \
        -a data/address_clipping.adf \
        -p data/address_clipping.tpef \
        --no-debugmode
}

simulate
simulate -q
