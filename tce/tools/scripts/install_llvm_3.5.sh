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
llvm_co_dir=llvm-3.5svn

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
  svn -q co http://llvm.org/svn/llvm-project/llvm/branches/release_35 llvm-3.5svn \
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
svn -q co http://llvm.org/svn/llvm-project/cfe/branches/release_35 clang \
  || eexit "SVN co from Clang failed" 
else
  svn up clang || eexit "SVN update of Clang failed."
  svn revert -R clang
fi

cd ../../$llvm_co_dir

# apply patches
patch -Np0 < $patch_dir/clang-3.5-64bit-doubles-not-forced-to-single.patch

./configure $LLVM_BUILD_MODE --enable-shared --prefix=$TARGET_DIR || eexit "Configuring LLVM/Clang failed."
make -j2 CXXFLAGS="-std=c++11" REQUIRES_RTTI=1 || eexit "Building LLVM/Clang failed."
if -w $TARGET_DIR 
then
    make install || eexit "Installation of LLVM/Clang failed."
else
    echo "Installation directory not writable, enter 'sudo' password or cancel and install manually."
    sudo make install || eexit "Installation of LLVM/Clang failed."
fi
