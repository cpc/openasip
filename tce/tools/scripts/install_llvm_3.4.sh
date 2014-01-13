#!/bin/bash

TARGET_DIR=$1

if test "x$2" == "x--debug-build";
then
LLVM_BUILD_MODE=--enable-debug
export CFLAGS=-O0
export CPPFLAGS=-O0
export CXXFLAGS=-O0
else
export CFLAGS=-O3
export CPPFLAGS=-O3
export CXXFLAGS=-O3
LLVM_BUILD_MODE=--enable-optimized
fi

echo "### LLVM build mode: "$LLVM_BUILD_MODE

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
patch_dir=$script_dir/../patches

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

patch -Np0 < $patch_dir/clang-3.4-no-forced-64bit-doubles.patch
patch -Np0 < $patch_dir/llvm-3.4-fix-SROA-creating-invalid-bitcasts-beween-address-spaces.patch

./configure $LLVM_BUILD_MODE --enable-shared --prefix=$TARGET_DIR || eexit "Configuring LLVM/Clang failed."
make -j2 REQUIRES_RTTI=1 || eexit "Building LLVM/Clang failed."
make install || eexit "Installation of LLVM/Clang failed."
