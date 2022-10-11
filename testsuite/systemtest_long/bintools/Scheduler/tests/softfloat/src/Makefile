# if SOURCE_FILES variable is not defined, all .c files in directory
# are selected to this list
#SOURCE_FILES=

# if OBJECT_FILES variable is not defined, list of object files are 
# automatically generated from SOURCE_FILES variable
# OBJECT_FILES=

include $(SCHEDULER_BENCHMARK_TEST_MAKEFILE_DEFS)

LLVM_PRESERVE_SYMBOLS=main

all: native llvm

native: 
	cp ../../../../../../../llvm-frontend/newlib-1.15.0/libgloss/openasip/softfloat.h .
	cp ../../../../../../../llvm-frontend/newlib-1.15.0/libgloss/openasip/float_emulation.c .
	$(CC) -o native_executable -O3 timesoftfloat.c float_emulation.c
	rm softfloat.* float_emulation.*



