#!/bin/bash

tce_build_root=$PWD/../../../../tce/src
. ../../../../testsuite/tce_src_paths.sh

for way in 2 4 8;
do

./create_clustered_tta.sh $way

echo "Cluster nodes: $way "
prog=`mktemp`
tcecc -loclhost-sa -a data/$way-way-clustered.adf --vectorize --vectorize-mem-only -DVERIFICATION\
    --vector-backend --swfp -O3 -o $prog OpenCL/example1/src/{kernel.cl,main.cpp}
ttasim -a data/$way-way-clustered.adf -p $prog --no-debugmode
rm $prog

done
