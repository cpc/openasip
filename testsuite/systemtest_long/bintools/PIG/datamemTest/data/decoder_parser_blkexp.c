// /MVG_top/decoder/parser/blkexp
// Source file is "C:\orcc_0.9\RVC\VTL\MPEG\MPEG4\part2\Mgnt_BlockExpand.cal"

#include "orcc.h"
#include "orcc_fifo.h"
#include "fifo_enum.h"

////////////////////////////////////////////////////////////////////////////////
// Input FIFOs
ANNOUNCE_I_FIFO(decoder_parser_blkexp_RUN, 0, 0);
ANNOUNCE_I_FIFO(decoder_parser_blkexp_VALUE, 0, 0);
ANNOUNCE_I_FIFO(decoder_parser_blkexp_LAST, 0, 0);

////////////////////////////////////////////////////////////////////////////////
// Output FIFOs
ANNOUNCE_O_FIFO(decoder_parser_blkexp_OUT);


////////////////////////////////////////////////////////////////////////////////
// Parameter values of the instance
////////////////////////////////////////////////////////////////////////////////
// State variables of the actor
i8 count = 0;
i8 run = -1;
i32 last = 0;
#define BLOCK_SIZE 64
i16 next_value;

////////////////////////////////////////////////////////////////////////////////
// Functions/procedures
////////////////////////////////////////////////////////////////////////////////
// Actions

void done() {
	i8 local_count_2;
	i8 local_run_2;
	i32 local_last_2;

	local_count_2 = 0;
	local_run_2 = -1;
	local_last_2 = 0;
	count = local_count_2;
	run = local_run_2;
	last = local_last_2;
}



i32 isSchedulable_done() {
	i8 local_count_1;
	i32 local_BLOCK_SIZE_1;
	i32 result_1;

	local_count_1 = count;
	local_BLOCK_SIZE_1 = BLOCK_SIZE;
	result_1 = local_count_1 == local_BLOCK_SIZE_1;
	return result_1;
}



void write_value() {
	i32 OUT_buf[1];
	i32 *OUT;
	i8 local_count_1;
	i16 local_next_value_1;
	i8 local_run_2;
	i8 local_count_2;

	OUT = FIFO_WRITE(decoder_parser_blkexp_OUT)(&fifo_o_decoder_parser_blkexp_OUT, OUT_buf, 1);
	local_count_1 = count;
	local_next_value_1 = next_value;
	local_run_2 = -1;
	local_count_2 = local_count_1 + 1;
	OUT[0] = local_next_value_1;FIFO_WRITE_END(decoder_parser_blkexp_OUT)(&fifo_o_decoder_parser_blkexp_OUT, OUT_buf, 1);
	run = local_run_2;
	count = local_count_2;
}



i32 isSchedulable_write_value() {
	i8 local_run_1;
	i32 result_1;

	local_run_1 = run;
	result_1 = local_run_1 == 0;
	return result_1;
}



void write_zero() {
	i32 OUT_buf[1];
	i32 *OUT;
	i8 local_run_1;
	i8 local_count_1;
	i8 local_run_2;
	i8 local_count_2;

	OUT = FIFO_WRITE(decoder_parser_blkexp_OUT)(&fifo_o_decoder_parser_blkexp_OUT, OUT_buf, 1);
	local_run_1 = run;
	local_count_1 = count;
	local_run_2 = local_run_1 - 1;
	local_count_2 = local_count_1 + 1;
	OUT[0] = 0;FIFO_WRITE_END(decoder_parser_blkexp_OUT)(&fifo_o_decoder_parser_blkexp_OUT, OUT_buf, 1);
	run = local_run_2;
	count = local_count_2;
}



i32 isSchedulable_write_zero() {
	i8 local_run_1;
	i32 local_last_1;
	i32 result_1;

	local_run_1 = run;
	local_last_1 = last;
	result_1 = local_run_1 > 0 || local_last_1;
	return result_1;
}



void read_immediate() {
	i32 RUN_buf[1];
	i32 *RUN;
	i32 VALUE_buf[1];
	i32 *VALUE;
	i32 LAST_buf[1];
	i32 *LAST;
	i32 OUT_buf[1];
	i32 *OUT;
	i8 local_count_1;
	i16 v_1;
	i32 l_1;
	i32 local_last_2;
	i8 local_count_2;

	OUT = FIFO_WRITE(decoder_parser_blkexp_OUT)(&fifo_o_decoder_parser_blkexp_OUT, OUT_buf, 1);
	local_count_1 = count;
	RUN = FIFO_READ(decoder_parser_blkexp_RUN)(&fifo_i_decoder_parser_blkexp_RUN, RUN_buf, 1);
	VALUE = FIFO_READ(decoder_parser_blkexp_VALUE)(&fifo_i_decoder_parser_blkexp_VALUE, VALUE_buf, 1);
	v_1 = VALUE[0];
	LAST = FIFO_READ(decoder_parser_blkexp_LAST)(&fifo_i_decoder_parser_blkexp_LAST, LAST_buf, 1);
	l_1 = LAST[0];
	local_last_2 = l_1;
	local_count_2 = local_count_1 + 1;
	OUT[0] = v_1;FIFO_WRITE_END(decoder_parser_blkexp_OUT)(&fifo_o_decoder_parser_blkexp_OUT, OUT_buf, 1);
	last = local_last_2;
	count = local_count_2;
	FIFO_READ_END(decoder_parser_blkexp_RUN)(&fifo_i_decoder_parser_blkexp_RUN, 1);
	FIFO_READ_END(decoder_parser_blkexp_VALUE)(&fifo_i_decoder_parser_blkexp_VALUE, 1);
	FIFO_READ_END(decoder_parser_blkexp_LAST)(&fifo_i_decoder_parser_blkexp_LAST, 1);
}



i32 isSchedulable_read_immediate() {
	i32 RUN_buf[1];
	i32 *RUN;
	i8 local_count_1;
	i32 local_BLOCK_SIZE_1;
	i8 r_1;
	i32 result_1;

	local_count_1 = count;
	local_BLOCK_SIZE_1 = BLOCK_SIZE;
	RUN = FIFO_PEEK(decoder_parser_blkexp_RUN)(&fifo_i_decoder_parser_blkexp_RUN, RUN_buf, 1);
	r_1 = RUN[0];
	result_1 = r_1 == 0 && local_count_1 != local_BLOCK_SIZE_1;
	return result_1;
}



void read_save() {
	i32 RUN_buf[1];
	i32 *RUN;
	i32 VALUE_buf[1];
	i32 *VALUE;
	i32 LAST_buf[1];
	i32 *LAST;
	i32 OUT_buf[1];
	i32 *OUT;
	i8 local_count_1;
	i8 r_1;
	i16 v_1;
	i32 l_1;
	i8 local_run_2;
	i16 local_next_value_2;
	i32 local_last_2;
	i8 local_count_2;

	OUT = FIFO_WRITE(decoder_parser_blkexp_OUT)(&fifo_o_decoder_parser_blkexp_OUT, OUT_buf, 1);
	local_count_1 = count;
	RUN = FIFO_READ(decoder_parser_blkexp_RUN)(&fifo_i_decoder_parser_blkexp_RUN, RUN_buf, 1);
	r_1 = RUN[0];
	VALUE = FIFO_READ(decoder_parser_blkexp_VALUE)(&fifo_i_decoder_parser_blkexp_VALUE, VALUE_buf, 1);
	v_1 = VALUE[0];
	LAST = FIFO_READ(decoder_parser_blkexp_LAST)(&fifo_i_decoder_parser_blkexp_LAST, LAST_buf, 1);
	l_1 = LAST[0];
	local_run_2 = r_1 - 1;
	local_next_value_2 = v_1;
	local_last_2 = l_1;
	local_count_2 = local_count_1 + 1;
	OUT[0] = 0;FIFO_WRITE_END(decoder_parser_blkexp_OUT)(&fifo_o_decoder_parser_blkexp_OUT, OUT_buf, 1);
	run = local_run_2;
	next_value = local_next_value_2;
	last = local_last_2;
	count = local_count_2;
	FIFO_READ_END(decoder_parser_blkexp_RUN)(&fifo_i_decoder_parser_blkexp_RUN, 1);
	FIFO_READ_END(decoder_parser_blkexp_VALUE)(&fifo_i_decoder_parser_blkexp_VALUE, 1);
	FIFO_READ_END(decoder_parser_blkexp_LAST)(&fifo_i_decoder_parser_blkexp_LAST, 1);
}



i32 isSchedulable_read_save() {
	i8 local_count_1;
	i32 local_BLOCK_SIZE_1;
	i32 result_1;

	local_count_1 = count;
	local_BLOCK_SIZE_1 = BLOCK_SIZE;
	result_1 = local_count_1 != local_BLOCK_SIZE_1;
	return result_1;
}


////////////////////////////////////////////////////////////////////////////////
// Action scheduler
int decoder_parser_blkexp_RUN_tokens;
int decoder_parser_blkexp_VALUE_tokens;
int decoder_parser_blkexp_LAST_tokens;


void refresh_inputs()
{
	decoder_parser_blkexp_RUN_tokens = FIFO_COUNT_TOKENS(decoder_parser_blkexp_RUN)(&fifo_i_decoder_parser_blkexp_RUN);
	decoder_parser_blkexp_VALUE_tokens = FIFO_COUNT_TOKENS(decoder_parser_blkexp_VALUE)(&fifo_i_decoder_parser_blkexp_VALUE);
	decoder_parser_blkexp_LAST_tokens = FIFO_COUNT_TOKENS(decoder_parser_blkexp_LAST)(&fifo_i_decoder_parser_blkexp_LAST);

}

int main(void) {

	while (1) {

		refresh_inputs();

		if (isSchedulable_done()) {
			done();
		} else if (isSchedulable_write_value()) {
			int ports = 0;
			if (!FIFO_HAS_ROOM(decoder_parser_blkexp_OUT)(&fifo_o_decoder_parser_blkexp_OUT, 1)) {
				ports |= 0x01;
			}
			if (ports != 0) {
				continue;
			}
			write_value();
		} else if (isSchedulable_write_zero()) {
			int ports = 0;
			if (!FIFO_HAS_ROOM(decoder_parser_blkexp_OUT)(&fifo_o_decoder_parser_blkexp_OUT, 1)) {
				ports |= 0x01;
			}
			if (ports != 0) {
				continue;
			}
			write_zero();
		} else if ((decoder_parser_blkexp_RUN_tokens >= 1) && (decoder_parser_blkexp_VALUE_tokens >= 1) && (decoder_parser_blkexp_LAST_tokens >= 1) && isSchedulable_read_immediate()) {
			int ports = 0;
			if (!FIFO_HAS_ROOM(decoder_parser_blkexp_OUT)(&fifo_o_decoder_parser_blkexp_OUT, 1)) {
				ports |= 0x01;
			}
			if (ports != 0) {
				continue;
			}
			read_immediate();
		} else if ((decoder_parser_blkexp_RUN_tokens >= 1) && (decoder_parser_blkexp_VALUE_tokens >= 1) && (decoder_parser_blkexp_LAST_tokens >= 1) && isSchedulable_read_save()) {
			int ports = 0;
			if (!FIFO_HAS_ROOM(decoder_parser_blkexp_OUT)(&fifo_o_decoder_parser_blkexp_OUT, 1)) {
				ports |= 0x01;
			}
			if (ports != 0) {
				continue;
			}
			read_save();
		} else {
			continue;
		}
	}
}

