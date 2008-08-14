#!/bin/sh
src_file=The_Clash_-_The_Guns_of_Brixton.ogg
dst_file=output.raw
pushd . > /dev/null
cd ..
make clean
#CFLAGS="-DMEMORY_IO -DDUMP_MEMORY_BUFFER_TO_STDOUT" make example
CFLAGS="-DSTANDARD_IO" make example
popd > /dev/null
echo "Converting OGG (${src_file}) to PCM RAW (${dst_file})..."
../ivorbisfile_example < $src_file > $dst_file

