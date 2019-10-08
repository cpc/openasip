# generate lsit of source files to compile if necessary
ifeq ($(SOURCE_FILES),)
SOURCE_FILES=$(wildcard *.c) $(wildcard *.cpp)
endif

# generate object file list if necessary
ifeq ($(OBJECT_FILES),)
OBJECT_FILES=$(addsuffix .o, $(basename $(SOURCE_FILES)))
endif

# default name for the llvm frontend compiler
ifeq ($(GCCLLVM),)
GCCLLVM=tcecc
endif

ifeq ($(CC),)
CC=gcc
endif

TESTBENCH_INCLUDE_DIR=$(dir $(SCHEDULER_BENCHMARK_TEST_MAKEFILE_DEFS))

all: host llvm

llvm:
	$(GCCLLVM) -I$(TESTBENCH_INCLUDE_DIR) $(EXTRA_FLAGS) $(EXTRA_TCECC_FLAGS) -o generated_program.be.bc --emit-llvm $(SCHEDULER_TESTER_FLAGS) $(SOURCE_FILES)
	$(GCCLLVM) -I$(TESTBENCH_INCLUDE_DIR) $(EXTRA_FLAGS) $(EXTRA_TCECC_FLAGS) -o generated_program.le.bc --little-endian --emit-llvm $(SCHEDULER_TESTER_FLAGS) $(SOURCE_FILES)

schedule: llvm
	echo "There must be link to tce/scheduler/testbench/ADF directory in src dir."
	$(GCCLLVM) -o 1_bus_scheduled.tpef -a ADF/1_bus_full_connectivity.adf -O3 generated_program.bc

host:
    # sometimes native gcc optimize floating points too much with -O2 optimizaitions
	$(CC) -I$(TESTBENCH_INCLUDE_DIR)  $(EXTRA_FLAGS) -o native_executable -O1 $(SOURCE_FILES)

clean:
	@rm -f *.o generated_seq_program* generated_program.bc 1_bus_scheduled.tpef native_executable
