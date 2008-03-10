#!/bin/sh
#                      build script for llvm-gcc-4.0   
# 
# This file was developed by Reid Spencer and is distributed under the
# University of Illinois Open Source License. See LICENSE.TXT for details.
# 
#===------------------------------------------------------------------------===#
#

# Get the llvm-top library
. ../library.sh

# Process the arguments
process_arguments "$@"

# First, see if the build directory is there. If not, create it.
build_dir="$LLVM_TOP/build.llvm-gcc-4.0"
if test ! -d "$build_dir" ; then
  mkdir -p "$build_dir"
fi

# Next, see if we have previously been configured by sensing the presense
# of the config.status scripts
config_status="$build_dir/config.status"
if test ! -d "$config_status" -o "$config_status" -ot "$0" ; then
  # We must configure so build a list of configure options
  config_options="--prefix=$PREFIX --enable-llvm=$LLVM_TOP/llvm "
  config_options="$config_options --program-prefix=llvm-"
  config_options="$config_options --enable-languages=c,c++"
  if test "$OPTIMIZED" -eq 0 ; then
    config_options="$config_options --enable-checking --enable-libstdcxx-debug"
  fi
  host=`./config.guess`
  if test ! -z `echo "$host" | grep 'linux'` ; then
    config_options="$config_options --disable-shared"
  fi
  config_options="$config_options $config_opts"
  src_dir=`pwd`
  cd "$build_dir"
  echo $src_dir/configure $config_options
  $src_dir/configure $config_options
fi
make
