# if SOURCE_FILES variable is not defined, all .c files in directory
# are selected to this list
SOURCE_FILES=\
	bitwise.c block.c codebook.c floor0.c floor1.c framing.c info.c \
	ivorbisfile_example.c mapping0.c mdct.c registry.c \
	res012.c sharedbook.c synthesis.c vorbisfile.c window.c \
	initial_data_40kB.c

EXTRA_GCCTCECFLAGS=\
	-DTCE \
	-DUSE_TCE_MALLOC \
	-DMEMORY_IO \
	-DLONGLONGEMUL \
	-DDUMP_MEM_BUFFER_TO_SPECIAL_OPERATION \
	-I .

#	-DDUMP_MEM_BUFFER_TO_STDOUT \

# if OBJECT_FILES variable is not defined, list of object files are 
# automatically generated from SOURCE_FILES variable
# OBJECT_FILES=board_test.o

#LLVM_PRESERVE_SYMBOLS=main

include $(SCHEDULER_BENCHMARK_TEST_MAKEFILE_DEFS)

