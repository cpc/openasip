#!/bin/bash

TARGET_DIR=${1:?"Missing installation directory argument"}
ON_PATCH_FAIL="exit 1"

if test "x$2" == "x--debug-build";
then
LLVM_BUILD_MODE="-DCMAKE_BUILD_TYPE=Debug -DLLVM_ENABLE_ASSERTIONS=ON"
export CFLAGS=-O0
export CPPFLAGS=-O0
export CXXFLAGS=-O0
build_dir=llvm-build-Debug
else
export CFLAGS=-O3
export CPPFLAGS=-O3
export CXXFLAGS=-O3
build_dir=llvm-build-Release
LLVM_BUILD_MODE="-DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_ASSERTIONS=ON"
fi

echo "### LLVM build mode: "$LLVM_BUILD_MODE

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
patch_dir=$script_dir/../patches
llvm_co_dir=llvm-trunk

mkdir -p $build_dir
cd $build_dir

function eexit {
   echo $1
   exit 1
}

function apply_patches {
    cd $llvm_co_dir
    try_patch $patch_dir/llvm-4.0-custom-vector-extension.patch
# It's likely these patches are not needed anymore as address space aware
# builtin calls are generated nowadays. If still having issues, check.
#    try_patch $patch_dir/llvm-3.8-memcpyoptimizer-only-on-default-as.patch
#    try_patch $patch_dir/llvm-3.8-loopidiomrecognize-only-on-default-as.patch
    cd ..
}

# Fetch LLVM from SVN.
if ! test -d $llvm_co_dir;
then
  svn -q co http://llvm.org/svn/llvm-project/llvm/trunk $llvm_co_dir \
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
svn -q co http://llvm.org/svn/llvm-project/cfe/trunk clang \
  || eexit "SVN co from Clang failed"
else
  svn up clang || eexit "SVN update of Clang failed."
  svn revert -R clang
fi

cd ../../$llvm_co_dir

apply_patches

./configure $LLVM_BUILD_MODE --enable-shared --prefix=$TARGET_DIR || eexit "Configuring LLVM/Clang failed."
make -j2 CXXFLAGS="-std=c++11" REQUIRES_RTTI=1 || eexit "Building LLVM/Clang failed."
make install || eexit "Installation of LLVM/Clang failed."
