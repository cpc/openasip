
# NOTE: with older gcc's (as in MOVE), any -O >=2 will produce faulty code
#       UNLESS accompanied by -fno-strength-reduce and/or -funroll-loops.
#       (Or don't "inline" MULT32 in misc.h)
CFLAGS += -Wall -O9 -funroll-loops -g -I. -fsigned-char -DLONGLONGEMUL \
	-DMEMORY_IO 

# -DDUMP_MEMORY_BUFFER_TO_STDOUT \
#	 -pg

#LDFLAGS = -pg

#LF     = -mno-multiply -mno-divide -mno-sign-extend -mno-ldst-byte \
#         -mno-floating-point -mno-insert-extract -mint32bits -Wall

#LF     = -mno-multiply -mno-divide -mno-sign-extend -mno-ldst-byte \
#                             -mno-insert-extract -mint32bits -Wall

CC = /home/visit0r/MOVE_installation/bin/linux-gnu/1.5/gcc-move
LDFLAGS = -L ${MOVE_HOMEDIR}/lib

SRC =	bitwise.c block.c codebook.c floor0.c floor1.c framing.c info.c \
	ivorbisfile_example.c mapping0.c mdct.c registry.c res012.c \
	sharedbook.c synthesis.c vorbisfile.c window.c initial_data.o 

OBJ = $(SRC:.c=.o)

.PHONY: makesymlink example clean

all: makesymlink ivorbisfile_example

example: all

makesymlink:
	rm -f vorbis
	ln -dfs . vorbis

ivorbisfile_example: ${OBJ}

clean:
	rm -f $(SRC:.c=.o) ivorbisfile_example
