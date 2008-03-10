# generate lsit of source files to compile if necessary
ifeq ($(SOURCE_FILES),)
SOURCE_FILES=$(wildcard *.c)
endif

# generate object file list if necessary
ifeq ($(OBJECT_FILES),)
OBJECT_FILES=$(addsuffix .o, $(basename $(SOURCE_FILES)))
endif

# default name for the compiler
ifeq ($(GCCTCE),)
GCCTCE=gcc-tce
endif

# default name for the llvm frontend compiler
ifeq ($(GCCLLVM),)
GCCLLVM=tcecc
endif

ifeq ($(CC),)
CC=gcc
endif



gcc-tce:
	$(GCCTCE) -c $(GCCTCECFLAGS) $(SCHEDULER_TESTER_FLAGS) $(EXTRA_GCCTCECFLAGS) $(SOURCE_FILES)
	$(GCCTCE) -o generated_seq_program $(GCCTCELDFLAGS) $(OBJECT_FILES)

llvm:
	$(GCCLLVM) -o generated_program.bc $(EXTRA_GCCTCECFLAGS) $(SCHEDULER_TESTER_FLAGS) $(SOURCE_FILES)

schedule: llvm
	echo "There must be link to tce/scheduler/testbench/ADF directory in src dir."
	$(GCCLLVM) -o 1_bus_scheduled.tpef -a ADF/1_bus_full_connectivity.adf -O3 generated_program.bc

host:
	$(CC) -o native_executable -O3 $(SOURCE_FILES)

clean:
	@rm -f *.o generated_seq_program* generated_program.bc 1_bus_scheduled.tpef native_executable
