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
LLVM_BUILD_MODE="-DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_ASSERTIONS=ON"
build_dir=llvm-build-Release
fi

echo "### LLVM build mode: "$LLVM_BUILD_MODE

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
patch_dir=$script_dir/../patches
llvm_co_dir=release_70

mkdir -p $build_dir
cd $build_dir

function eexit {
   echo "$1"
   exit 1
}

function fetch_llvm {
    REV_TO_FETCH=$1
    if [ -n "$REV_TO_FETCH" ]; then
        REV_TO_FETCH="-r$REV_TO_FETCH"
    fi

    if ! test -d $llvm_co_dir;
    then
        svn -q $REV_TO_FETCH co http://llvm.org/svn/llvm-project/llvm/branches/release_70 $llvm_co_dir \
            || eexit "SVN co $REV_TO_FETCH from LLVM failed"
    else
        svn up $REV_TO_FETCH $llvm_co_dir \
            || eexit "SVN update $REV_TO_FETCH of LLVM failed."
        # Wipe out possible previously applied patches.
        svn revert -R $llvm_co_dir
    fi
}

function fetch_clang {
    cd $llvm_co_dir/tools
    if ! test -d clang;
    then
        svn -q co http://llvm.org/svn/llvm-project/cfe/branches/release_70 clang \
            || eexit "SVN co from Clang failed"
    else
        svn up clang || eexit "SVN update of Clang failed."
        svn revert -R clang
    fi
    cd ../..
}

function try_patch {
    echo "### Using patch file: $1"
    patch -Np0 < $1 \
        || { echo "patching with $(basename $1) failed"; $ON_PATCH_FAIL; }
}

function apply_patches {
    cd $llvm_co_dir
    try_patch $patch_dir/llvm-8-fix-load-lowering.patch
    try_patch $patch_dir/llvm-7.0-custom-vector-extension.patch
    try_patch $patch_dir/llvm-7.0-vect-datalayout.patch
    try_patch $patch_dir/llvm-6.0-SPIR-address-space-numbers.patch
    cd ..
}

fetch_llvm
fetch_clang
apply_patches

cd $llvm_co_dir
mkdir -p build
cd build

cmake -G "Unix Makefiles" \
    $LLVM_BUILD_MODE\
    -DCMAKE_INSTALL_PREFIX=$TARGET_DIR \
    -DLLVM_LINK_LLVM_DYLIB=TRUE \
    -DLLVM_ENABLE_RTTI=TRUE \
    .. \
    || eexit "Configuring LLVM/Clang failed."
make -j8 CXXFLAGS="-std=c++11" REQUIRES_RTTI=1 \
    || eexit "Building LLVM/Clang failed."
make install || eexit "Installation of LLVM/Clang failed."

