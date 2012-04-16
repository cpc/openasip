#!/bin/bash

tce_build_root=$PWD/../../../../tce/src
. ../../../../testsuite/tce_src_paths.sh

tcecc --clear-plugin-cache
tcecc -loclhost-sa -a data/2-way-clustered.adf --vectorize --vectorize-mem-only -DVERIFICATION\
    --vector-backend --swfp -O3 -o prog OpenCL/example1/src/{kernel.cl,main.cpp}
ttasim -a data/2-way-clustered.adf -p prog --no-debugmode

