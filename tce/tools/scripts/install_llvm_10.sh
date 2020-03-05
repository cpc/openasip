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
llvm_co_dir=release_10

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
        git clone --branch release/10.x https://github.com/llvm/llvm-project.git $llvm_co_dir\
	    || eexit "Git clone $REV_TOFETCH from llvm failed"
    else
	      # Discard all differences with release/9.x branch
	      cd $llvm_co_dir;
        git checkout release/10.x ||	eexit "checking out git branch failed"
	      git reset --hard HEAD || eexit "resetting --hard HEAD failed"
	      git pull || eexit "error doing a git pull"
	      cd ..;
    fi
}

function try_patch {
    echo "### Using patch file: $1"
    patch -Np0 < $1 \
        || { echo "patching with $(basename $1) failed"; $ON_PATCH_FAIL; }
}

function apply_patches {
    cd $llvm_co_dir
    try_patch $patch_dir/llvm-9-fix-load-lowering.patch
    try_patch $patch_dir/llvm-10-custom-vector-extension.patch
    try_patch $patch_dir/llvm-9-vect-datalayout.patch
    try_patch $patch_dir/llvm-9-SPIR-address-space-numbers.patch
    try_patch $patch_dir/llvm-10-SWP.patch
    cd ..
}
fetch_llvm
apply_patches

cd $llvm_co_dir
mkdir -p build
cd build

# -DLLVM_ENABLE_Z3_SOLVER=OFF due to the issue described in
# https://reviews.llvm.org/D54978#1390652
# You might also need to delete libz3-dev.
# This appears at least with Ubuntu 18.04.
cmake ../llvm/ -DLLVM_ENABLE_PROJECTS=clang \
    -G "Unix Makefiles" \
    $LLVM_BUILD_MODE\
    -DCMAKE_INSTALL_PREFIX=$TARGET_DIR \
    -DLLVM_LINK_LLVM_DYLIB=TRUE \
    -DLLVM_ENABLE_RTTI=TRUE \
    -DLLVM_ENABLE_Z3_SOLVER=OFF \
    || eexit "Configuring LLVM/Clang failed."
make -j8 CXXFLAGS="-std=c++11" REQUIRES_RTTI=1 \
    || eexit "Building LLVM/Clang failed."
make install || eexit "Installation of LLVM/Clang failed."

