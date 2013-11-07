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

if ! test -d llvm-3.3svn;
then
  svn -q co http://llvm.org/svn/llvm-project/llvm/branches/release_33 llvm-3.3svn \
  || eexit "SVN co from LLVM failed"
  cd llvm-3.3svn
  patch -p0 < $script_dir/../patches/dag-comparison-fix-3.3.patch \
  || eexit "Patching failed"
  cd tools
  svn -q co http://llvm.org/svn/llvm-project/cfe/branches/release_33 clang \
  || eexit "SVN co from LLVM failed" 
  cd ../..
fi
cd llvm-3.3svn

export CFLAGS=-O3
export CPPFLAGS=-O3
export CXXFLAGS=-O3
./configure --enable-optimized --enable-shared --prefix=$TARGET_DIR || eexit "Configure of LLVM failed."
make -j2 REQUIRES_RTTI=1 || eexit "Build of LLVM failed."
make install || eexit "Installed of LLVM failed."
