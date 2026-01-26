#!/bin/bash

TARGET_DIR=${1:?"Missing installation directory argument"}

GNU_TOOLCHAIN_OPTS=""

if test "x$2" == "x--parallel-build"
then
GNU_TOOLCHAIN_OPTS=-j$(nproc)
fi

build_dir="riscv-tools-build"

rm -rf $build_dir
mkdir $build_dir
cd $build_dir

function eexit {
   echo "$1"
   exit 1
}

function install_gnu_toolchain {
    version=2026.01.23
    repository=https://github.com/riscv-collab/riscv-gnu-toolchain
    name=riscv-gnu-toolchain
    git clone $repository || eexit "git clone $name failed"
    cd $name
    git checkout $version || eexit "git checkout $name $version failed"
    ./configure --prefix=$TARGET_DIR --with-arch=rv32im \
        || eexit "configure $name failed"
    make $GNU_TOOLCHAIN_OPTS || eexit "make $name failed"
    cd ..
}

function install_elf2hex {
    version=v20.08.00.00
    repository=https://github.com/sifive/elf2hex.git
    name=elf2hex
    git clone $repository || eexit "git clone $name failed"
    cd $name
    git checkout $version || eexit "git checkout $name $version failed"
    ./configure --prefix=$TARGET_DIR --target=riscv32-unknown-elf \
        || eexit "configure $name failed"
    make || eexit "make $name failed"
    make install || eexit "make install $name failed"
    cd ..
}

install_gnu_toolchain
install_elf2hex
