#!/bin/bash

TARGET_DIR=${1:?"Missing installation directory argument"}
REVS=${2:?"Missing llvm-3.8 branch revision(s)"}
#HOST_GCC=${HOST_GCC:-/usr}
ON_PATCH_FAIL="exit 1"

if test "x$3" == "x--debug-build";
then
LLVM_BUILD_MODE=-DCMAKE_BUILD_TYPE=Debug
export CFLAGS=-O0
export CPPFLAGS=-O0
export CXXFLAGS=-O0
else
export CFLAGS=-O3
export CPPFLAGS=-O3
export CXXFLAGS=-O3
LLVM_BUILD_MODE=-DCMAKE_BUILD_TYPE=Release
fi

echo "### LLVM build mode: "$LLVM_BUILD_MODE

script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
patch_dir=$script_dir/../patches
llvm_co_dir=llvm-3.8-patch-trial
PATCH_FAIL_LOG=$script_dir/llvm-3.8-patch-fail.log

temp_dir=llvm-build-temp
mkdir -p $temp_dir
cd $temp_dir

function log {
    echo "[$(date +%H:%M:%S)]: $1"
    echo "[$(date +%H:%M:%S)]: $1" >> $script_dir/${llvm_co_dir}.log
}

function eexit {
   log "$1"
   exit 1
}

function fetch_llvm {
    REV_TO_FETCH=$1
    if [ -n "$REV_TO_FETCH" ]; then
        REV_TO_FETCH="-r$REV_TO_FETCH"
    fi

    if ! test -d $llvm_co_dir;
    then
        svn -q $REV_TO_FETCH co http://llvm.org/svn/llvm-project/llvm/branches/release_38 $llvm_co_dir \
            || eexit "SVN co $REV_TO_FETCH from LLVM failed"
    else
        svn up $REV_TO_FETCH $llvm_co_dir \
            || eexit "SVN update $REV_TO_FETCH of LLVM failed."
        # Wipe out possible previously applied patches.
        svn revert -R $llvm_co_dir
    fi
}

function fetch_clang {
    pushd $llvm_co_dir/tools
    if ! test -d clang;
    then
        svn -q co http://llvm.org/svn/llvm-project/cfe/branches/release_38 clang \
            || eexit "SVN co from Clang failed" 
    else
        svn up clang || eexit "SVN update of Clang failed."
        svn revert -R clang
    fi
    popd
}

function try_patch {
    patch -Np0 < $1 >& $PATCH_FAIL_LOG \
	|| { log "patching with $(basename $1) failed"; $ON_PATCH_FAIL; } 
}

function apply_patches {
    log "Trying patching with rev $rev"
    pushd $llvm_co_dir
    try_patch $patch_dir/llvm-3.8-custom-vector-extension.patch 
    try_patch $patch_dir/llvm-3.8-tce-and-tcele.patch
    try_patch $patch_dir/llvm-3.8-memcpyoptimizer-only-on-default-as.patch
    try_patch $patch_dir/llvm-3.8-loopidiomrecognize-only-on-default-as.patch
    popd
    log "completed patching (rev $rev)"
}

rev_to_trial_build=
for rev in $REVS; do
    log "Fetching llvm rev $rev"

    fetch_llvm $rev
    fetch_clang
    apply_patches
    rev_to_trial_build=$rev
done

log "Fetching rev $rev_to_trial_build for trial build."
fetch_llvm $rev_to_trial_build
fetch_clang $rev_to_trial_build
apply_patches

test -n "$rev_to_trial_build" || eexit "No build to try. Terminating."

cd $llvm_co_dir
mkdir -p build
cd build

log "trial building rev $rev_to_trial_build."

# using CMake since autoconf is deprecated in LLVM 3.8 and removed in LLVM 3.9
cmake -G "Unix Makefiles" \
    $LLVM_BUILD_MODE\
    -DCMAKE_INSTALL_PREFIX=$TARGET_DIR \
    -DLLVM_BUILD_LLVM_DYLIB=TRUE \
    -DLLVM_ENABLE_RTTI=TRUE \
    .. \
    || eexit "Configuring LLVM/Clang failed."
#    -DGCC_INSTALL_PREFIX=${HOST_GCC} \
#    -DBUILD_SHARED_LIBS=true \
make -j4 CXXFLAGS="-std=c++11" REQUIRES_RTTI=1 \
    || eexit "Building LLVM/Clang failed."
make install || eexit "Installation of LLVM/Clang failed."

log "patching and building successed with rev $rev_to_trial_build."
