#!/bin/bash
tcecc=../../../../tce/src/bintools/Compiler/tcecc
ttasim=../../../../tce/src/codesign/ttasim/ttasim
mach_dir=../../../../tce/scheduler/testbench/ADF
mach=clustered_mul4-1mul_gal-1SS.adf
mach_mc=clustered_mul4-1mul_gal-1SS-mc.adf
#mach_dir=.
#mach=debug2.adf
#verbose=-v
# Use qttasim if set to -q, otherwise empty
quick=-q
tester="../../../../tce/scheduler/testbench/scheduler_tester.py"
tester_args="-e OpenCL -vrx -w 0.0 -a $mach -a $mach_mc" #  crashes with AES, disabled for now
parallel_wi=2

function compile_opencl {
    output=$1
    shift
    $tcecc --swfp -a $mach_dir/$mach --opencl-max-parallel-wi-count=$parallel_wi \
        -loclhost-sa $* -o $output $verbose
}

function simulate {
    $ttasim $quick -a $mach_dir/$mach -p $program --no-debugmode
}

# First run verify the result correctness.
program=$(mktemp tmpXXXXXX)

# clAESGladman with two custom ops
compile_opencl $program -DVERIFICATION -O3 -IOpenCL/oclAESGladman/src/ \
OpenCL/oclAESGladman/src/*.{cpp,cl} && \
simulate $program && \
compile_opencl OpenCL/oclAESGladman/program.bc -O3 -IOpenCL/oclAESGladman/src/ \
OpenCL/oclAESGladman/src/*.{cpp,cl} --emit-llvm

# OpenCL specs: example1
compile_opencl $program -DVERIFICATION -O3 OpenCL/example1/src/*.{cpp,cl} && \
simulate $program && \
compile_opencl OpenCL/example1/program.bc -O3 OpenCL/example1/src/*.{cpp,cl} --emit-llvm


# Run the benchmarks with cycle counts to avoid performance regressions.
# We generated the program.bc in the previous steps.

#temporarily disabled because --swfp cannot be given to scheduler tester
#when not compiling from sources but from .bc file instead.
#$tester $tester_args
#echo $program
rm -f $program
rm -f OpenCL/example1/program.bc 
rm -f OpenCL/clAESGladman/program.bc
