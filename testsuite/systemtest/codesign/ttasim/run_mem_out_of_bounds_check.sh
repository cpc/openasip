#!/bin/bash
function simulate {
    ../../../../tce/src/codesign/ttasim/ttasim $* \
        -a data/mem_out_of_bounds.adf \
        -p data/mem_out_of_bounds.tpef \
        --no-debugmode
}

# Should throw out of memory bounds error.
simulate

