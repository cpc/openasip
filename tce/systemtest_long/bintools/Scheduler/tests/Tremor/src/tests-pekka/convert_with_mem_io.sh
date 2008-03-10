#!/bin/sh
dst_file=output.raw
pushd . > /dev/null
cd ..
make clean
CFLAGS="-DMEMORY_IO -DDUMP_MEMORY_BUFFER_TO_STDOUT" make example
popd > /dev/null
echo "Converting OGG (initial_data.h) to PCM RAW (${dst_file})..."
../ivorbisfile_example > $dst_file
