#!/bin/bash
#
# Compiles the simple dictionary compressor.
pushd . > /dev/null
cd ../../../compressors
../src/bintools/PIG/buildcompressor SimpleDictionary.so simple_dictionary.cc > /dev/null
popd . > /dev/null
