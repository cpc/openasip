#!/bin/bash
pushd . > /dev/null
cd ../../../../tce/src/tools
make > /dev/null
cd ../base
make > /dev/null
cd ../applibs
make > /dev/null
cd ../procgen/ProGe
rm -f generateprocessor-static
make generateprocessor-static > /dev/null
popd . > /dev/null
