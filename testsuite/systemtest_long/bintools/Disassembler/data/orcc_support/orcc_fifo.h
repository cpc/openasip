#ifndef FIFO_H
#define FIFO_H

#include "tceops.h"
#include "fifo_enum.h"

struct fifo_i32_i {
	int index;
	int fo;
	int fo_filled;
};

struct fifo_i32_o {
	int index;
};

// declare FIFO with a size equal to (size)
#define ANNOUNCE_I_FIFO(index, fo, fo_filled) \
static struct FIFO_I(index) fifo_i_##index = {index, fo, fo_filled};

#define ANNOUNCE_O_FIFO(index) \
static struct FIFO_O(index) fifo_o_##index = {index};

#define FIFO_I(I) FIFO_I_EXPAND(I)
#define FIFO_I_EXPAND(I) fifo_i32_i

#define FIFO_O(O) FIFO_O_EXPAND(O)
#define FIFO_O_EXPAND(O) fifo_i32_o


#define FIFO_I_STATUS(I) FIFO_I_STATUS_EXPAND(I)
#define FIFO_I_STATUS_EXPAND(I) _TCE_STREAM_IN_STATUS_V##I

#define FIFO_I_PEEK(I) FIFO_I_PEEK_EXPAND(I)
#define FIFO_I_PEEK_EXPAND(I) _TCE_STREAM_IN_PEEK_V##I

#define FIFO_O_STATUS(O) FIFO_O_STATUS_EXPAND(O)
#define FIFO_O_STATUS_EXPAND(O) _TCE_STREAM_OUT_STATUS_V##O 

#define FIFO_INPUT(I) FIFO_INPUT_EXPAND(I)
#define FIFO_INPUT_EXPAND(I) _TCE_STREAM_IN_V##I

#define FIFO_OUTPUT(O) FIFO_OUTPUT_EXPAND(O)
#define FIFO_OUTPUT_EXPAND(O) _TCE_STREAM_OUT_V##O 


#define FIFO_HAS_TOKENS(I) FIFO_HAS_TOKENS_EXPAND(I)
#define FIFO_HAS_TOKENS_EXPAND(I) fifo_ ## I ## _has_tokens

#define FIFO_COUNT_TOKENS(I) FIFO_COUNT_TOKENS_EXPAND(I)
#define FIFO_COUNT_TOKENS_EXPAND(I) fifo_ ## I ## _count_tokens

#define FIFO_PEEK(I) FIFO_PEEK_EXPAND(I)
#define FIFO_PEEK_EXPAND(I) fifo_ ## I ## _peek

#define FIFO_READ(I) FIFO_READ_EXPAND(I)
#define FIFO_READ_EXPAND(I) fifo_ ## I ## _read

#define FIFO_READ_END(I) FIFO_READ_END_EXPAND(I)
#define FIFO_READ_END_EXPAND(I) fifo_ ## I ## _read_end


#define FIFO_HAS_ROOM(O) FIFO_HAS_ROOM_EXPAND(O)
#define FIFO_HAS_ROOM_EXPAND(O) fifo_ ## O ## _has_room

#define FIFO_WRITE(O) FIFO_WRITE_EXPAND(O)
#define FIFO_WRITE_EXPAND(O) fifo_ ## O ## _write

#define FIFO_WRITE_END(O) FIFO_WRITE_END_EXPAND(O)
#define FIFO_WRITE_END_EXPAND(O) fifo_ ## O ## _write_end

#define I 1
#include "orcc_input_fifo.h"
#undef I

#define I 2
#include "orcc_input_fifo.h"
#undef I

#define I 3
#include "orcc_input_fifo.h"
#undef I

#define I 4
#include "orcc_input_fifo.h"
#undef I

#define I 5
#include "orcc_input_fifo.h"
#undef I

#define I 6
#include "orcc_input_fifo.h"
#undef I


#define O 1
#include "orcc_output_fifo.h"
#undef O

#define O 2
#include "orcc_output_fifo.h"
#undef O

#define O 3
#include "orcc_output_fifo.h"
#undef O

#define O 4
#include "orcc_output_fifo.h"
#undef O

#define O 5
#include "orcc_output_fifo.h"
#undef O

#define O 6
#include "orcc_output_fifo.h"
#undef O

#define O 7
#include "orcc_output_fifo.h"
#undef O

#define O 8
#include "orcc_output_fifo.h"
#undef O

#endif
