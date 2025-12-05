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
llvm_co_dir=release_21
llvm_branch=release/21.x

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
        git clone --single-branch --depth=1 --branch $llvm_branch https://github.com/cpc/llvmtce.git $llvm_co_dir\
            || eexit "Git clone $REV_TOFETCH from llvm failed"
    else
        cd $llvm_co_dir;
        git checkout $llvm_branch ||	eexit "checking out git branch failed"
        git fetch
        git reset --hard $llvm_branch || eexit "resetting --hard HEAD failed"
        cd ..;
    fi
}

fetch_llvm

cd $llvm_co_dir
mkdir -p build
cd build

# -DLLVM_ENABLE_Z3_SOLVER=OFF due to the issue described in
# https://reviews.llvm.org/D54978#1390652
# You might also need to delete libz3-dev.
# This appears at least with Ubuntu 18.04.
cmake ../llvm/ -DLLVM_ENABLE_PROJECTS="clang;lld" \
    -G "Unix Makefiles" \
    $LLVM_BUILD_MODE\
    -DCMAKE_INSTALL_PREFIX=$TARGET_DIR \
    -DLLVM_TARGETS_TO_BUILD="X86" \
    -DLLVM_LINK_LLVM_DYLIB=TRUE \
    -DLLVM_ENABLE_RTTI=TRUE \
    -DLLVM_ENABLE_Z3_SOLVER=OFF \
    || eexit "Configuring LLVM/Clang failed."
make -j$(nproc) || eexit "Building LLVM/Clang failed."
make install || eexit "Installation of LLVM/Clang failed."
