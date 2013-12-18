#!/bin/bash

TARGET_DIR=$1

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

temp_dir=llvm-build-temp
mkdir -p $temp_dir
cd $temp_dir

function eexit {
   echo $1
   exit 1
}

# Fetch LLVM from SVN.
if ! test -d llvm-3.4svn;
then
  svn -q co http://llvm.org/svn/llvm-project/llvm/branches/release_34 llvm-3.4svn \
  || eexit "SVN co from LLVM failed"
else
  svn up llvm-3.4svn || eexit "SVN update of LLVM failed."
fi

cd llvm-3.4svn/tools

# Fetch Clang from SVN.
if ! test -d clang;
then
svn -q co http://llvm.org/svn/llvm-project/cfe/branches/release_34 clang \
  || eexit "SVN co from Clang failed" 
else
svn up clang || eexit "SVN update of Clang failed."
fi

cd ../../llvm-3.4svn

patch -Np0 < $script_dir/../patches/clang-3.4-no-forced-64bit-doubles.patch

export CFLAGS=-O3
export CPPFLAGS=-O3
export CXXFLAGS=-O3
./configure --enable-optimized --enable-shared --prefix=$TARGET_DIR || eexit "Configure of LLVM failed."
make -j2 REQUIRES_RTTI=1 || eexit "Build of LLVM failed."
make install || eexit "Installed of LLVM failed."
