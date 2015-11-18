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
llvm_co_dir=llvm-3.6svn

temp_dir=llvm-build-temp
mkdir -p $temp_dir
cd $temp_dir

function eexit {
   echo $1
   exit 1
}

# Fetch LLVM from SVN.
if ! test -d $llvm_co_dir;
then
  svn -q co http://llvm.org/svn/llvm-project/llvm/branches/release_36 $llvm_co_dir \
  || eexit "SVN co from LLVM failed"
else
  svn up $llvm_co_dir || eexit "SVN update of LLVM failed."
  # Wipe out possible previously applied patches.
  svn revert -R $llvm_co_dir 
fi

cd $llvm_co_dir/tools

# Fetch Clang from SVN.
if ! test -d clang;
then
svn -q co http://llvm.org/svn/llvm-project/cfe/branches/release_36 clang \
  || eexit "SVN co from Clang failed" 
else
  svn up clang || eexit "SVN update of Clang failed."
  svn revert -R clang
fi

cd ../../$llvm_co_dir

##### Add patches here.
#####

./configure $LLVM_BUILD_MODE --enable-shared --prefix=$TARGET_DIR || eexit "Configuring LLVM/Clang failed."
make -j6 CXXFLAGS="-std=c++11" REQUIRES_RTTI=1 || eexit "Building LLVM/Clang failed."
make install || eexit "Installation of LLVM/Clang failed."
