// /MVG_top/decoder/texture/idct2d
// Source file is "C:\orcc_0.9\RVC\VTL\MPEG\MPEG4\part2\iDCT\Algo_IDCT2D_ISOIEC_23002_1.cal"

#include "orcc.h"
#include "orcc_fifo.h"
#include "fifo_enum.h"

////////////////////////////////////////////////////////////////////////////////
// Input FIFOs
ANNOUNCE_I_FIFO(decoder_texture_idct2d_IN, 0, 0);
ANNOUNCE_I_FIFO(decoder_texture_idct2d_SIGNED, 0, 0);

////////////////////////////////////////////////////////////////////////////////
// Output FIFOs
ANNOUNCE_O_FIFO(decoder_texture_idct2d_OUT);


////////////////////////////////////////////////////////////////////////////////
// Parameter values of the instance
////////////////////////////////////////////////////////////////////////////////
// State variables of the actor
i32 scale[64] = {1024, 1138, 1730, 1609, 1024, 1609, 1730, 1138, 1138, 1264, 1922, 
1788, 1138, 1788, 1922, 1264, 1730, 1922, 2923, 2718, 1730, 2718, 2923, 1922, 1609, 
1788, 2718, 2528, 1609, 2528, 2718, 1788, 1024, 1138, 1730, 1609, 1024, 1609, 1730, 
1138, 1609, 1788, 2718, 2528, 1609, 2528, 2718, 1788, 1730, 1922, 2923, 2718, 1730, 
2718, 2923, 1922, 1138, 1264, 1922, 1788, 1138, 1788, 1922, 1264};

////////////////////////////////////////////////////////////////////////////////
// Functions/procedures

i32 pmul_1_1(i32 X) {
	i32 Temp1_1;

	Temp1_1 = (X >> 3) - (X >> 7);
	return X - Temp1_1;
}


i32 pmul_1_2(i32 X) {
	i32 Temp1_1;
	i32 Temp2_1;

	Temp1_1 = (X >> 3) - (X >> 7);
	Temp2_1 = Temp1_1 - (X >> 11);
	return Temp1_1 + (Temp2_1 >> 1);
}


i32 pmul_2_1(i32 X) {
	i32 Temp1_1;

	Temp1_1 = (X >> 9) - X;
	return (Temp1_1 >> 2) - Temp1_1;
}


i32 pmul_2_2(i32 X) {
	return X >> 1;
}


i32 pmul_3_1(i32 X) {
	i32 Temp1_1;
	i32 Temp2_1;

	Temp1_1 = X + (X >> 5);
	Temp2_1 = Temp1_1 >> 2;
	return Temp2_1 + (X >> 4);
}


i32 pmul_3_2(i32 X) {
	i32 Temp1_1;
	i32 Temp2_1;

	Temp1_1 = X + (X >> 5);
	Temp2_1 = Temp1_1 >> 2;
	return Temp1_1 - Temp2_1;
}


void idct1d(i32 X[64], i32 Y[64]) {
	i32 foreach1_i_1;
	i32 local_X_1;
	i32 local_X0_1;
	i32 xa_1;
	i32 local_X1_1;
	i32 local_X2_1;
	i32 xb_1;
	i32 local_X3_1;
	i32 local_X4_1;
	i32 local_X5_1;
	i32 local_X6_1;
	i32 local_X7_1;
	i32 call_pmul_1_1_1;
	i32 temp1_1;
	i32 local_X8_1;
	i32 call_pmul_1_2_1;
	i32 temp4_1;
	i32 local_X9_1;
	i32 call_pmul_1_10_1;
	i32 temp3_1;
	i32 local_X10_1;
	i32 call_pmul_1_20_1;
	i32 temp2_1;
	i32 local_X11_1;
	i32 call_pmul_2_1_1;
	i32 temp1_3;
	i32 local_X12_1;
	i32 call_pmul_2_2_1;
	i32 temp4_3;
	i32 local_X13_1;
	i32 call_pmul_2_10_1;
	i32 temp3_3;
	i32 local_X14_1;
	i32 call_pmul_2_20_1;
	i32 temp2_3;
	i32 local_X15_1;
	i32 call_pmul_3_1_1;
	i32 temp1_4;
	i32 local_X16_1;
	i32 call_pmul_3_2_1;
	i32 temp4_4;
	i32 local_X17_1;
	i32 call_pmul_3_10_1;
	i32 temp3_4;
	i32 local_X18_1;
	i32 call_pmul_3_20_1;
	i32 temp2_4;
	i32 local_X19_1;
	i32 local_X20_1;
	i32 xa_3;
	i32 local_X21_1;
	i32 local_X22_1;
	i32 xb_3;
	i32 local_X23_1;
	i32 local_X24_1;
	i32 local_X25_1;
	i32 local_X26_1;
	i32 local_X27_1;
	i32 local_X28_1;
	i32 local_X29_1;
	i32 local_X30_1;
	i32 local_X31_1;
	i32 local_X32_1;
	i32 local_X33_1;
	i32 local_X34_1;
	i32 local_X35_1;
	i32 local_X36_1;
	i32 local_X37_1;
	i32 local_X38_1;
	i32 local_X39_1;
	i32 local_X40_1;
	i32 local_X41_1;
	i32 local_X42_1;
	i32 foreach1_i_2;
	i32 foreach1_i_3;

	foreach1_i_1 = 0;
	foreach1_i_3 = foreach1_i_1;
	while (foreach1_i_3 <= 7) {
		local_X_1 = X[8 * foreach1_i_3 + 1];
		local_X0_1 = X[8 * foreach1_i_3 + 7];
		xa_1 = local_X_1 + local_X0_1;
		local_X1_1 = X[8 * foreach1_i_3 + 1];
		local_X2_1 = X[8 * foreach1_i_3 + 7];
		xb_1 = local_X1_1 - local_X2_1;
		local_X3_1 = X[8 * foreach1_i_3 + 3];
		X[8 * foreach1_i_3 + 1] = xa_1 + local_X3_1;
		local_X4_1 = X[8 * foreach1_i_3 + 3];
		X[8 * foreach1_i_3 + 3] = xa_1 - local_X4_1;
		local_X5_1 = X[8 * foreach1_i_3 + 5];
		X[8 * foreach1_i_3 + 7] = xb_1 + local_X5_1;
		local_X6_1 = X[8 * foreach1_i_3 + 5];
		X[8 * foreach1_i_3 + 5] = xb_1 - local_X6_1;
		local_X7_1 = X[8 * foreach1_i_3 + 3];
		call_pmul_1_1_1 = pmul_1_1(local_X7_1);
		temp1_1 = call_pmul_1_1_1;
		local_X8_1 = X[8 * foreach1_i_3 + 3];
		call_pmul_1_2_1 = pmul_1_2(local_X8_1);
		temp4_1 = call_pmul_1_2_1;
		local_X9_1 = X[8 * foreach1_i_3 + 5];
		call_pmul_1_10_1 = pmul_1_1(local_X9_1);
		temp3_1 = call_pmul_1_10_1;
		local_X10_1 = X[8 * foreach1_i_3 + 5];
		call_pmul_1_20_1 = pmul_1_2(local_X10_1);
		temp2_1 = call_pmul_1_20_1;
		X[8 * foreach1_i_3 + 3] = temp1_1 - temp2_1;
		X[8 * foreach1_i_3 + 5] = temp3_1 + temp4_1;
		local_X11_1 = X[8 * foreach1_i_3 + 1];
		call_pmul_2_1_1 = pmul_2_1(local_X11_1);
		temp1_3 = call_pmul_2_1_1;
		local_X12_1 = X[8 * foreach1_i_3 + 1];
		call_pmul_2_2_1 = pmul_2_2(local_X12_1);
		temp4_3 = call_pmul_2_2_1;
		local_X13_1 = X[8 * foreach1_i_3 + 7];
		call_pmul_2_10_1 = pmul_2_1(local_X13_1);
		temp3_3 = call_pmul_2_10_1;
		local_X14_1 = X[8 * foreach1_i_3 + 7];
		call_pmul_2_20_1 = pmul_2_2(local_X14_1);
		temp2_3 = call_pmul_2_20_1;
		X[8 * foreach1_i_3 + 1] = temp1_3 + temp2_3;
		X[8 * foreach1_i_3 + 7] = temp3_3 - temp4_3;
		local_X15_1 = X[8 * foreach1_i_3 + 2];
		call_pmul_3_1_1 = pmul_3_1(local_X15_1);
		temp1_4 = call_pmul_3_1_1;
		local_X16_1 = X[8 * foreach1_i_3 + 2];
		call_pmul_3_2_1 = pmul_3_2(local_X16_1);
		temp4_4 = call_pmul_3_2_1;
		local_X17_1 = X[8 * foreach1_i_3 + 6];
		call_pmul_3_10_1 = pmul_3_1(local_X17_1);
		temp3_4 = call_pmul_3_10_1;
		local_X18_1 = X[8 * foreach1_i_3 + 6];
		call_pmul_3_20_1 = pmul_3_2(local_X18_1);
		temp2_4 = call_pmul_3_20_1;
		X[8 * foreach1_i_3 + 2] = temp1_4 - temp2_4;
		X[8 * foreach1_i_3 + 6] = temp3_4 + temp4_4;
		local_X19_1 = X[8 * foreach1_i_3 + 0];
		local_X20_1 = X[8 * foreach1_i_3 + 4];
		xa_3 = local_X19_1 + local_X20_1;
		local_X21_1 = X[8 * foreach1_i_3 + 0];
		local_X22_1 = X[8 * foreach1_i_3 + 4];
		xb_3 = local_X21_1 - local_X22_1;
		local_X23_1 = X[8 * foreach1_i_3 + 6];
		X[8 * foreach1_i_3 + 0] = xa_3 + local_X23_1;
		local_X24_1 = X[8 * foreach1_i_3 + 6];
		X[8 * foreach1_i_3 + 6] = xa_3 - local_X24_1;
		local_X25_1 = X[8 * foreach1_i_3 + 2];
		X[8 * foreach1_i_3 + 4] = xb_3 + local_X25_1;
		local_X26_1 = X[8 * foreach1_i_3 + 2];
		X[8 * foreach1_i_3 + 2] = xb_3 - local_X26_1;
		local_X27_1 = X[8 * foreach1_i_3 + 0];
		local_X28_1 = X[8 * foreach1_i_3 + 1];
		Y[foreach1_i_3 + 8 * 0] = local_X27_1 + local_X28_1;
		local_X29_1 = X[8 * foreach1_i_3 + 4];
		local_X30_1 = X[8 * foreach1_i_3 + 5];
		Y[foreach1_i_3 + 8 * 1] = local_X29_1 + local_X30_1;
		local_X31_1 = X[8 * foreach1_i_3 + 2];
		local_X32_1 = X[8 * foreach1_i_3 + 3];
		Y[foreach1_i_3 + 8 * 2] = local_X31_1 + local_X32_1;
		local_X33_1 = X[8 * foreach1_i_3 + 6];
		local_X34_1 = X[8 * foreach1_i_3 + 7];
		Y[foreach1_i_3 + 8 * 3] = local_X33_1 + local_X34_1;
		local_X35_1 = X[8 * foreach1_i_3 + 6];
		local_X36_1 = X[8 * foreach1_i_3 + 7];
		Y[foreach1_i_3 + 8 * 4] = local_X35_1 - local_X36_1;
		local_X37_1 = X[8 * foreach1_i_3 + 2];
		local_X38_1 = X[8 * foreach1_i_3 + 3];
		Y[foreach1_i_3 + 8 * 5] = local_X37_1 - local_X38_1;
		local_X39_1 = X[8 * foreach1_i_3 + 4];
		local_X40_1 = X[8 * foreach1_i_3 + 5];
		Y[foreach1_i_3 + 8 * 6] = local_X39_1 - local_X40_1;
		local_X41_1 = X[8 * foreach1_i_3 + 0];
		local_X42_1 = X[8 * foreach1_i_3 + 1];
		Y[foreach1_i_3 + 8 * 7] = local_X41_1 - local_X42_1;
		foreach1_i_2 = foreach1_i_3 + 1;
		foreach1_i_3 = foreach1_i_2;
	}

}


i32 clip(i32 x, i32 lim) {
	i32 _tmp_if_1;
	i32 _tmp_if_2;
	i32 _tmp_if0_1;
	i32 _tmp_if0_2;
	i32 _tmp_if0_3;
	i32 _tmp_if_3;

	if (x < lim) {
		_tmp_if_1 = lim;
		_tmp_if_2 = _tmp_if_1;
	} else {
		if (x > 255) {
			_tmp_if0_1 = 255;
			_tmp_if0_2 = _tmp_if0_1;
		} else {
			_tmp_if0_3 = x;
			_tmp_if0_2 = _tmp_if0_3;
		}
		_tmp_if_3 = _tmp_if0_2;
		_tmp_if_2 = _tmp_if_3;
	}
	return _tmp_if_2;
}


////////////////////////////////////////////////////////////////////////////////
// Actions

void intra() {
	i32 IN_buf[64];
	i32 *IN;
	i16 x[64];
	i32 SIGNED_buf[1];
	i32 *SIGNED;
	i32 block1[64];
	i32 block2[64];
	i32 OUT_buf[64];
	i32 *OUT;
	i32 num_repeats_1;
	i16 token_1;
	i32 num_repeats_2;
	i32 num_repeats_3;
	i32 generator0_n_1;
	i32 local_scale_1;
	i16 local_x_1;
	i32 generator0_n_2;
	i32 generator0_n_3;
	i32 local_block1_1;
	i32 generator2_n_1;
	i32 local_block10_1;
	i32 call_clip_1;
	i32 generator2_n_2;
	i32 generator2_n_3;
	i32 num_repeats0_1;
	i16 token0_1;
	i32 num_repeats0_2;
	i32 num_repeats0_3;

	OUT = FIFO_WRITE(decoder_texture_idct2d_OUT)(&fifo_o_decoder_texture_idct2d_OUT, OUT_buf, 64);
	IN = FIFO_READ(decoder_texture_idct2d_IN)(&fifo_i_decoder_texture_idct2d_IN, IN_buf, 64);
	num_repeats_1 = 0;
	num_repeats_3 = num_repeats_1;
	while (num_repeats_3 < 64) {
		token_1 = IN[1 * num_repeats_3 + 0];
		x[num_repeats_3] = token_1;
		num_repeats_2 = num_repeats_3 + 1;
		num_repeats_3 = num_repeats_2;
	}

	SIGNED = FIFO_READ(decoder_texture_idct2d_SIGNED)(&fifo_i_decoder_texture_idct2d_SIGNED, SIGNED_buf, 1);
	generator0_n_1 = 0;
	generator0_n_3 = generator0_n_1;
	while (generator0_n_3 <= 63) {
		local_scale_1 = scale[generator0_n_3];
		local_x_1 = x[generator0_n_3];
		block1[generator0_n_3] = local_scale_1 * local_x_1;
		generator0_n_2 = generator0_n_3 + 1;
		generator0_n_3 = generator0_n_2;
	}

	local_block1_1 = block1[0];
	block1[0] = local_block1_1 + (1 << 12);
	idct1d(block1, block2);
	idct1d(block2, block1);
	generator2_n_1 = 0;
	generator2_n_3 = generator2_n_1;
	while (generator2_n_3 <= 63) {
		local_block10_1 = block1[generator2_n_3];
		call_clip_1 = clip(local_block10_1 >> 13, 0);
		block1[generator2_n_3] = call_clip_1;
		generator2_n_2 = generator2_n_3 + 1;
		generator2_n_3 = generator2_n_2;
	}

	num_repeats0_1 = 0;
	num_repeats0_3 = num_repeats0_1;
	while (num_repeats0_3 < 64) {
		token0_1 = block1[num_repeats0_3];
		OUT[1 * num_repeats0_3 + 0] = token0_1;
		num_repeats0_2 = num_repeats0_3 + 1;
		num_repeats0_3 = num_repeats0_2;
	}
	FIFO_WRITE_END(decoder_texture_idct2d_OUT)(&fifo_o_decoder_texture_idct2d_OUT, OUT_buf, 64);
	FIFO_READ_END(decoder_texture_idct2d_IN)(&fifo_i_decoder_texture_idct2d_IN, 64);
	FIFO_READ_END(decoder_texture_idct2d_SIGNED)(&fifo_i_decoder_texture_idct2d_SIGNED, 1);
}



i32 isSchedulable_intra() {
	i32 result_1;

	result_1 = 1;
	return result_1;
}



void inter() {
	i32 IN_buf[64];
	i32 *IN;
	i16 x[64];
	i32 SIGNED_buf[1];
	i32 *SIGNED;
	i32 block1[64];
	i32 block2[64];
	i32 OUT_buf[64];
	i32 *OUT;
	i32 num_repeats_1;
	i16 token_1;
	i32 num_repeats_2;
	i32 num_repeats_3;
	i32 generator3_n_1;
	i32 local_scale_1;
	i16 local_x_1;
	i32 generator3_n_2;
	i32 generator3_n_3;
	i32 local_block1_1;
	i32 generator4_n_1;
	i32 local_block10_1;
	i32 call_clip_1;
	i32 generator4_n_2;
	i32 generator4_n_3;
	i32 num_repeats0_1;
	i16 token0_1;
	i32 num_repeats0_2;
	i32 num_repeats0_3;

	OUT = FIFO_WRITE(decoder_texture_idct2d_OUT)(&fifo_o_decoder_texture_idct2d_OUT, OUT_buf, 64);
	IN = FIFO_READ(decoder_texture_idct2d_IN)(&fifo_i_decoder_texture_idct2d_IN, IN_buf, 64);
	num_repeats_1 = 0;
	num_repeats_3 = num_repeats_1;
	while (num_repeats_3 < 64) {
		token_1 = IN[1 * num_repeats_3 + 0];
		x[num_repeats_3] = token_1;
		num_repeats_2 = num_repeats_3 + 1;
		num_repeats_3 = num_repeats_2;
	}

	SIGNED = FIFO_READ(decoder_texture_idct2d_SIGNED)(&fifo_i_decoder_texture_idct2d_SIGNED, SIGNED_buf, 1);
	generator3_n_1 = 0;
	generator3_n_3 = generator3_n_1;
	while (generator3_n_3 <= 63) {
		local_scale_1 = scale[generator3_n_3];
		local_x_1 = x[generator3_n_3];
		block1[generator3_n_3] = local_scale_1 * local_x_1;
		generator3_n_2 = generator3_n_3 + 1;
		generator3_n_3 = generator3_n_2;
	}

	local_block1_1 = block1[0];
	block1[0] = local_block1_1 + (1 << 12);
	idct1d(block1, block2);
	idct1d(block2, block1);
	generator4_n_1 = 0;
	generator4_n_3 = generator4_n_1;
	while (generator4_n_3 <= 63) {
		local_block10_1 = block1[generator4_n_3];
		call_clip_1 = clip(local_block10_1 >> 13, -255);
		block1[generator4_n_3] = call_clip_1;
		generator4_n_2 = generator4_n_3 + 1;
		generator4_n_3 = generator4_n_2;
	}

	num_repeats0_1 = 0;
	num_repeats0_3 = num_repeats0_1;
	while (num_repeats0_3 < 64) {
		token0_1 = block1[num_repeats0_3];
		OUT[1 * num_repeats0_3 + 0] = token0_1;
		num_repeats0_2 = num_repeats0_3 + 1;
		num_repeats0_3 = num_repeats0_2;
	}
	FIFO_WRITE_END(decoder_texture_idct2d_OUT)(&fifo_o_decoder_texture_idct2d_OUT, OUT_buf, 64);
	FIFO_READ_END(decoder_texture_idct2d_IN)(&fifo_i_decoder_texture_idct2d_IN, 64);
	FIFO_READ_END(decoder_texture_idct2d_SIGNED)(&fifo_i_decoder_texture_idct2d_SIGNED, 1);
}



i32 isSchedulable_inter() {
	i32 SIGNED_buf[1];
	i32 *SIGNED;
	i32 s_1;
	i32 result_1;

	SIGNED = FIFO_PEEK(decoder_texture_idct2d_SIGNED)(&fifo_i_decoder_texture_idct2d_SIGNED, SIGNED_buf, 1);
	s_1 = SIGNED[0];
	result_1 = s_1;
	return result_1;
}


////////////////////////////////////////////////////////////////////////////////
// Action scheduler
int decoder_texture_idct2d_IN_tokens;
int decoder_texture_idct2d_SIGNED_tokens;


void refresh_inputs()
{
	decoder_texture_idct2d_IN_tokens = FIFO_COUNT_TOKENS(decoder_texture_idct2d_IN)(&fifo_i_decoder_texture_idct2d_IN);
	decoder_texture_idct2d_SIGNED_tokens = FIFO_COUNT_TOKENS(decoder_texture_idct2d_SIGNED)(&fifo_i_decoder_texture_idct2d_SIGNED);

}

int main(void) {

	while (1) {

		refresh_inputs();

		if ((decoder_texture_idct2d_IN_tokens >= 64) && (decoder_texture_idct2d_SIGNED_tokens >= 1) && isSchedulable_inter()) {
			int ports = 0;
			if (!FIFO_HAS_ROOM(decoder_texture_idct2d_OUT)(&fifo_o_decoder_texture_idct2d_OUT, 64)) {
				ports |= 0x01;
			}
			if (ports != 0) {
				continue;
			}
			inter();
		} else if ((decoder_texture_idct2d_IN_tokens >= 64) && (decoder_texture_idct2d_SIGNED_tokens >= 1) && isSchedulable_intra()) {
			int ports = 0;
			if (!FIFO_HAS_ROOM(decoder_texture_idct2d_OUT)(&fifo_o_decoder_texture_idct2d_OUT, 64)) {
				ports |= 0x01;
			}
			if (ports != 0) {
				continue;
			}
			intra();
		} else {
			continue;
		}
	}
}

