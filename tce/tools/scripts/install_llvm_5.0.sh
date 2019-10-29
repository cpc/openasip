#!/bin/bash

TARGET_DIR=${1:?"Missing installation directory argument"}
ON_PATCH_FAIL="exit 1"

if test "x$2" == "x--debug-build";
then
LLVM_BUILD_MODE="-DCMAKE_BUILD_TYPE=Debug -DLLVM_ENABLE_ASSERTIONS=ON"
export CFLAGS=-O0
export CPPFLAGS=-O0
export CXXFLAGS=-O0
BUILD_DIR=build-debug
else
export CFLAGS=-O3
export CPPFLAGS=-O3
export CXXFLAGS=-O3
LLVM_BUILD_MODE="-DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_ASSERTIONS=ON"
BUILD_DIR=build-release
fi

echo "### LLVM build mode: "$LLVM_BUILD_MODE

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
patch_dir=$script_dir/../patches
llvm_co_dir=llvm-5.0

temp_dir=llvm-build-temp
mkdir -p $temp_dir
cd $temp_dir

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
        svn -q $REV_TO_FETCH co http://llvm.org/svn/llvm-project/llvm/branches/release_50 $llvm_co_dir \
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
        svn -q co http://llvm.org/svn/llvm-project/cfe/branches/release_50 clang \
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
    try_patch $patch_dir/llvm-5.0-custom-vector-extension.patch
    try_patch $patch_dir/llvm-5.0-vect-datalayout.patch
    try_patch $patch_dir/llvm-5.0-SPIR-address-space-numbers.patch
    cd ..
}

fetch_llvm
fetch_clang
apply_patches

cd $llvm_co_dir
mkdir -p $BUILD_DIR
cd $BUILD_DIR

cmake -G "Unix Makefiles" \
    $LLVM_BUILD_MODE\
    -DCMAKE_INSTALL_PREFIX=$TARGET_DIR \
    -DLLVM_LINK_LLVM_DYLIB=TRUE \
    -DLLVM_ENABLE_RTTI=TRUE \
    .. \
    || eexit "Configuring LLVM/Clang failed."
make -j4 CXXFLAGS="-std=c++11" REQUIRES_RTTI=1 \
    || eexit "Building LLVM/Clang failed."
make install || eexit "Installation of LLVM/Clang failed."

echo "LLVM installation is finished, add following lines to the search path:"
echo
echo "  export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:"$TARGET_DIR"/lib"
echo "  export PATH=\$PATH:"$TARGET_DIR"/bin"
echo "  export LDFLAGS=-L"$TARGET_DIR"/lib"
echo
