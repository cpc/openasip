#
# Makefile for compiling Tremor Vorbis decoder to TCE.
#
# Does not use syscall emulation.
#
# Pekka J‰‰skel‰inen 2005 (pekka.jaaskelainen@iki.fi)
#
COMPILER=${TCE_HOME_PATH}/bin/gcc-tce
LINKER=${TCE_HOME_PATH}/bin/ld
COMPILE_SWITCHES=-g -O2 -c -I . -I ${TCE_HOME_PATH}/include/machine \
	-DTCE -DUSE_TCE_MALLOC \
	-DMEMORY_IO \
        -DLONGLONGEMUL \
		-DDUMP_MEM_BUFFER_TO_SPECIAL_OPERATION

#        -DDEBUG_PRINT \
#	-DSPECIAL_OPERATION_IO \
#	-DDUMP_MEM_BUFFER_TO_SPECIAL_OPERATION \

MAKEFILE = TCE/Makefile
#INITIAL_DATA_SOURCE ?= initial_data_full.c

TCE_DIR=TCE

# Files required from TCE's libc 
TCE_LIBC_DIR=${TCE_HOME_PATH}/lib
TCE_LIBC_FILES=memset.o memmove.o memcmp.o memchr.o bcopy.o qsort.o ctype_.o \
	strlen.o strcpy.o strcat.o errlst.o printf.o fflush.o vfprintf.o \
	fvwrite.o missing.o findfp.o

TCE_LIBC = $(addprefix ${TCE_LIBC_DIR}/,${TCE_LIBC_FILES})

#TCE_LIBGCC_DIR=${TCE_HOME_PATH}/gcc-2.7.0
#TCE_LIBGCC= $(addprefix ${TCE_LIBGCC_DIR}/,${TCE_LIBGCC_FILES})

TCE_CODES=${TCE_LIBGCC} ${TCE_LIBC}

#c-codes = $(wildcard *.c ${TCE_DIR}/tcemalloc.c) ${INITIAL_DATA_SOURCE}

c-codes =  bitwise.c block.c codebook.c floor0.c floor1.c framing.c info.c \
 ivorbisfile_example.c mapping0.c mdct.c registry.c \
res012.c sharedbook.c synthesis.c vorbisfile.c window.c

c-codes_40kB= ${c-codes} initial_data_40kB.c
c-codes_full= ${c-codes} initial_data_full.c


objects_40kB = $(c-codes_40kB:.c=.o)
objects_full = $(c-codes_full:.c=.o)

.PHONY: makesymlink clean

all: makesymlink tremor_40kB tremor_full

vorbis:
	ln -dfs . vorbis

makesymlink:
	rm -f vorbis
	ln -dfs . vorbis

tremor_40kB: ${TCE_DIR}/crt0.o ${TCE_DIR}/tcemalloc.o ${objects_40kB} ${MAKEFILE}
	@echo "Linking ${objects}"
	@${COMPILER} -nostartfiles ${TCE_DIR}/crt0.o ${TCE_CODES} \
${objects_40kB} -o tremor_40kB


tremor_full: ${TCE_DIR}/crt0.o ${TCE_DIR}/tcemalloc.o ${objects_full} ${MAKEFILE}
	@echo "Linking ${objects}"
	@${COMPILER} -nostartfiles ${TCE_DIR}/crt0.o ${TCE_CODES} \
${objects_full} -o tremor_full

${TCE_DIR}/crt0.o: ${TCE_DIR}/crt0asm.s
	@${COMPILER} ${SWITCHES} ${TCE_DIR}/crt0asm.s ${COMPILE_SWITCHES} -c -o ${TCE_DIR}/crt0.o

tests-pekka/generate: tests-pekka/generate.c 
	gcc -o tests-pekka/generate tests-pekka/generate.c

initial_data_full.c: tests-pekka/The_Clash_-_The_Guns_of_Brixton.ogg \
tests-pekka/generate
	tests-pekka/generate 33502976 <tests-pekka/The_Clash_-_The_Guns_of_Brixton.ogg >initial_data_full.c

initial_data_40kB.c: tests-heikki/40kB.ogg TCE/Makefile tests-pekka/generate
	tests-pekka/generate 460032 <tests-heikki/40kB.ogg >initial_data_40kB.c

.c.o: $*.c Makefile vorbis
	@echo -e "Compile: $*.c"
	@${COMPILER} ${SWITCHES} $*.c ${COMPILE_SWITCHES} -o $*.o

clean:
	rm -f *.o tremor_40kB tremor_full ${TCE_DIR}/tcemalloc.o initial_data_40kB.c initial_data_full.c 
