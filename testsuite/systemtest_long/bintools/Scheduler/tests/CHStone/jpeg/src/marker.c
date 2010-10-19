/*
+--------------------------------------------------------------------------+
| CHStone : a suite of benchmark programs for C-based High-Level Synthesis |
| ======================================================================== |
|                                                                          |
| * Collected and Modified : Y. Hara, H. Tomiyama, S. Honda,               |
|                            H. Takada and K. Ishii                        |
|                            Nagoya University, Japan                      |
|                                                                          |
| * Remark :                                                               |
|    1. This source code is modified to unify the formats of the benchmark |
|       programs in CHStone.                                               |
|    2. Test vectors are added for CHStone.                                |
|    3. If "main_result" is 0 at the end of the program, the program is    |
|       correctly executed.                                                |
|    4. Please follow the copyright of each benchmark program.             |
+--------------------------------------------------------------------------+
*/
/*
 *  Read the head of the marker
 *
 *  @(#) $Id: marker.c,v 1.2 2003/07/18 10:19:21 honda Exp $
 */
/*************************************************************
Copyright (C) 1990, 1991, 1993 Andy C. Hung, all rights reserved.
PUBLIC DOMAIN LICENSE: Stanford University Portable Video Research
Group. If you use this software, you agree to the following: This
program package is purely experimental, and is licensed "as is".
Permission is granted to use, modify, and distribute this program
without charge for any purpose, provided this license/ disclaimer
notice appears in the copies.  No warranty or maintenance is given,
either expressed or implied.  In no event shall the author(s) be
liable to you or a third party for any special, incidental,
consequential, or other damages, arising out of the use or inability
to use the program for any purpose (or the loss of data), even if we
have been advised of such possibilities.  Any public reference or
advertisement of this source code should refer to it as the Portable
Video Research Group (PVRG) code, and not by any author(s) (or
Stanford University) name.
*************************************************************/
/*
************************************************************
marker.c

This file contains the Marker library which uses the direct buffer
access routines bgetc...

************************************************************
*/

/* Only for the marker needed at the baseline */
typedef enum
{				/* JPEG marker codes */
  M_SOI = 0xd8,			/* Start of Image */
  M_SOF0 = 0xc0,		/* Baseline DCT ( Huffman )  */
  M_SOS = 0xda,			/* Start of Scan ( Head of Compressed Data )  */
  M_DHT = 0xc4,			/* Huffman Table */
  M_DQT = 0xdb,			/* Quantization Table */
  M_EOI = 0xd9			/* End of Image */
} JPEG_MARKER;


/*
 * Initialize Quantization Table
 */
const int izigzag_index[64] = {
  0, 1, 8, 16, 9, 2, 3, 10,
  17, 24, 32, 25, 18, 11, 4, 5,
  12, 19, 26, 33, 40, 48, 41, 34,
  27, 20, 13, 6, 7, 14, 21, 28,
  35, 42, 49, 56, 57, 50, 43, 36,
  29, 22, 15, 23, 30, 37, 44, 51,
  58, 59, 52, 45, 38, 31, 39, 46,
  53, 60, 61, 54, 47, 55, 62, 63
};

/*
+--------------------------------------------------------------------------+
| * Test Vector (added for CHStone)                                        |
|     out_length : expected output data                                    |
|                  for "get_sof", "get_sos", "get_dht" and "get_dqt"       |
+--------------------------------------------------------------------------+
*/
const int out_length[8] = { 65, 65, 17, 29, 179, 29, 179, 12 };

/*
+--------------------------------------------------------------------------+
| * Test Vector (added for CHStone)                                        |
|     out_prec : expected output data for "get_dqt"                        |
+--------------------------------------------------------------------------+
*/
const int out_prec[2] = { 0, 0 };

/*
+--------------------------------------------------------------------------+
| * Test Vectors (added for CHStone)                                       |
|     out_h_samp_factor : expected output data for "get_sof"               |
|     out_v_samp_factor : expected output data for "get_sof"               |
|     out_quant_tbl_no : expected output data for "get_sof"                |
+--------------------------------------------------------------------------+
*/
const int out_h_samp_factor[3] = { 2, 1, 1 };
const int out_v_samp_factor[3] = { 2, 1, 1 };
const int out_quant_tbl_no[3] = { 0, 1, 1 };

/*
+--------------------------------------------------------------------------+
| * Test Vector (added for CHStone)                                        |
|     out_count : expected output data for "get_dht"                       |
+--------------------------------------------------------------------------+
*/
const int out_count[4] = { 12, 162, 12, 162 };

/*
+--------------------------------------------------------------------------+
| * Test Vectors (added for CHStone)                                       |
|     out_dc_tbl_no, out_ac_tbl_no : expected output data for "get_sos"    |
+--------------------------------------------------------------------------+
*/
const int out_dc_tbl_no[3] = { 0, 1, 1 };
const int out_ac_tbl_no[3] = { 0, 1, 1 };

/*
+--------------------------------------------------------------------------+
| * Test Vectors (added for CHStone)                                       |
|     out_data_precision : expected output data for "get_sof"              |
|     out_image_height : expected output data for "get_sof"                |
|     out_image_width : expected output data for "get_sof"                 |
|     out_num_components : expected output data for "get_sof"              |
+--------------------------------------------------------------------------+
*/
#define out_data_precision 8
#define out_image_height 113
#define out_image_width 150
#define out_num_components 3

int i0 = 0;
int i1 = 0;
int i2 = 0;

/*
 *  Read Buffer
 */
static unsigned char *ReadBuf;


/*
 *  Read from Buffer
 */
int
read_byte (void)
{
  return *ReadBuf++;
}


short
read_word (void)
{
  short c;

  c = *ReadBuf++ << 8;
  c |= *ReadBuf++;

  return c;
}

int
first_marker (void)
{
  int c1, c2;
  c1 = read_byte ();
  c2 = read_byte ();

  if (c1 != 0xFF || c2 != (int) M_SOI)
    {
      main_result++;
      //printf ("Not Jpeg File!\n");
      EXIT;
    }

  return c2;
}


int
next_marker (void)
{
  int c;

  for (;;)
    {
      c = read_byte ();

      while (c != 0xff)
	{
	  c = read_byte ();
	}

      do
	{
	  c = read_byte ();
	}
      while (c == 0xff);
      if (c != 0)
	{
	  break;
	}
    }
  return c;
}


/*
 *  Baseline DCT ( Huffman )  
 */
void
get_sof ()
{
  int ci, c, length;
  int *p_comp_info_index;
  char *p_comp_info_id;
  char *p_comp_info_h_samp_factor;
  char *p_comp_info_v_samp_factor;
  char *p_comp_info_quant_tbl_no;
  char *p_comp_info_dc_tbl_no;
  char *p_comp_info_ac_tbl_no;

  length = read_word ();
  DecodeInfo_data_precision = read_byte ();
  DecodeInfo_image_height = read_word ();
  DecodeInfo_image_width = read_word ();
  DecodeInfo_num_components = read_byte ();

  //printf ("length         = %d\n", length);
  //printf ("data_precision = %d\n", DecodeInfo_data_precision);
  //printf ("image_height   = %d\n", DecodeInfo_image_height);
  //printf ("image_width    = %d\n", DecodeInfo_image_width);
  //printf ("num_components = %d\n", DecodeInfo_num_components);
  if (length != out_length[i0++])
    {
      main_result++;
    }
  if (DecodeInfo_data_precision != out_data_precision)
    {
      main_result++;
    }
  if (DecodeInfo_image_height != out_image_height)
    {
      main_result++;
    }
  if (DecodeInfo_image_width != out_image_width)
    {
      main_result++;
    }
  if (DecodeInfo_num_components != out_num_components)
    {
      main_result++;
    }

  length -= 8;

  /* Error check is omitted. */

  /* Check components */
  for (ci = 0; ci < DecodeInfo_num_components; ci++)
    {
      p_comp_info_index = &DecodeInfo_comps_info_index[ci];
      p_comp_info_id = &DecodeInfo_comps_info_id[ci];
      p_comp_info_h_samp_factor = &DecodeInfo_comps_info_h_samp_factor[ci];
      p_comp_info_v_samp_factor = &DecodeInfo_comps_info_v_samp_factor[ci];
      p_comp_info_quant_tbl_no = &DecodeInfo_comps_info_quant_tbl_no[ci];
      p_comp_info_dc_tbl_no = &DecodeInfo_comps_info_dc_tbl_no[ci];
      p_comp_info_ac_tbl_no = &DecodeInfo_comps_info_ac_tbl_no[ci];

      *p_comp_info_index = ci;
      *p_comp_info_id = read_byte ();
      c = read_byte ();
      *p_comp_info_h_samp_factor = (c >> 4) & 15;
      *p_comp_info_v_samp_factor = (c) & 15;
      *p_comp_info_quant_tbl_no = read_byte ();

      //printf (" index         = %d\n", *p_comp_info_index);
      //printf (" id            = %d\n", *p_comp_info_id);
      //printf (" h_samp_factor = %d\n", *p_comp_info_h_samp_factor);
      //printf (" v_samp_factor = %d\n", *p_comp_info_v_samp_factor);
      //printf (" quant_tbl_no  = %d\n\n", *p_comp_info_quant_tbl_no);
      if (*p_comp_info_h_samp_factor != out_h_samp_factor[*p_comp_info_index])
	{
	  main_result++;
	}
      if (*p_comp_info_v_samp_factor != out_v_samp_factor[*p_comp_info_index])
	{
	  main_result++;
	}
      if (*p_comp_info_quant_tbl_no != out_quant_tbl_no[*p_comp_info_index])
	{
	  main_result++;
	}

    }


  /*
   *  Determinination of Sampling Factor
   *  Only 1_1_1 and 4_1_1 are supported
   */
  if (DecodeInfo_comps_info_h_samp_factor[0] == 2)
    {
      DecodeInfo_smp_fact = SF4_1_1;
      //printf ("\nSampling Factor is 4:1:1\n");
    }
  else
    {
      DecodeInfo_smp_fact = SF1_1_1;
      //printf ("\nSampling Factor is 1:1:1\n");
    }
}


void
get_sos ()
{
  int length, num_comp;
  int i, c, cc, ci, j;
  int *p_comp_info_index;
  char *p_comp_info_id;
  char *p_comp_info_h_samp_factor;
  char *p_comp_info_v_samp_factor;
  char *p_comp_info_quant_tbl_no;
  char *p_comp_info_dc_tbl_no;
  char *p_comp_info_ac_tbl_no;

  length = read_word ();
  num_comp = read_byte ();

  //printf (" length = %d\n", length);
  //printf (" num_comp = %d\n", num_comp);
  if (length != out_length[i0++])
    {
      main_result++;
    }

  /* Decode each component */
  for (i = 0; i < num_comp; i++)
    {
      cc = read_byte ();
      c = read_byte ();

      for (ci = 0; ci < DecodeInfo_num_components; ci++)
	{
	  p_comp_info_index = &DecodeInfo_comps_info_index[ci];
	  p_comp_info_id = &DecodeInfo_comps_info_id[ci];
	  p_comp_info_h_samp_factor =
	    &DecodeInfo_comps_info_h_samp_factor[ci];
	  p_comp_info_v_samp_factor =
	    &DecodeInfo_comps_info_v_samp_factor[ci];
	  p_comp_info_quant_tbl_no = &DecodeInfo_comps_info_quant_tbl_no[ci];
	  p_comp_info_dc_tbl_no = &DecodeInfo_comps_info_dc_tbl_no[ci];
	  p_comp_info_ac_tbl_no = &DecodeInfo_comps_info_ac_tbl_no[ci];

	  if (cc == *p_comp_info_id)
	    {
	      goto id_found;
	    }
	}

      main_result++;

      //printf ("Bad Component ID!\n");
      EXIT;
    id_found:
      *p_comp_info_dc_tbl_no = (c >> 4) & 15;
      *p_comp_info_ac_tbl_no = (c) & 15;

      //printf (" comp_id       = %d\n", cc);
      //printf (" dc_tbl_no     = %d\n", *p_comp_info_dc_tbl_no);
      //printf (" ac_tbl_no     = %d\n", *p_comp_info_ac_tbl_no);
      if (*p_comp_info_dc_tbl_no != out_dc_tbl_no[cc - 1])
	{
	  main_result++;
	}
      if (*p_comp_info_ac_tbl_no != out_ac_tbl_no[cc - 1])
	{
	  main_result++;
	}


    }

  /* pass parameters; Ss, Se, Ah and Al for progressive JPEG */
  j = 3;
  while (j--)
    {
      c = read_byte ();
    }

  /*
   * Define the Buffer at this point as the head of data
   */
  CurHuffReadBuf = ReadBuf;

}


/*
 * Get Huffman Table
 */
void
get_dht ()
{
  int length, index;
  int i, count;
  int *pp_xhtbl_bits;
  int *pp_xhtbl_huffval;

  length = read_word ();
  length -= 2;

  //printf (" length = %d\n", length);
  if (length != out_length[i0++])
    {
      main_result++;
    }


  while (length > 16)
    {
      index = read_byte ();

      //printf (" index = 0x%x\n", index);
      if (index & 0x10)
	{
	  /* AC */
	  index -= 0x10;
	  pp_xhtbl_bits = DecodeInfo_ac_xhuff_tbl_bits[index];
	  pp_xhtbl_huffval = DecodeInfo_ac_xhuff_tbl_huffval[index];
	}
      else
	{
	  /* DC */
	  pp_xhtbl_bits = DecodeInfo_dc_xhuff_tbl_bits[index];
	  pp_xhtbl_huffval = DecodeInfo_dc_xhuff_tbl_huffval[index];
	}
      count = 0;

      for (i = 1; i <= 16; i++)
	{
	  pp_xhtbl_bits[i] = read_byte ();
	  count += pp_xhtbl_bits[i];
	}

      //printf (" count = %d\n", count);
      if (count != out_count[i2++])
	{
	  main_result++;
	}

      length -= 1 + 16;

      for (i = 0; i < count; i++)
	{
	  pp_xhtbl_huffval[i] = read_byte ();
	}

      length -= count;
    }
}


void
get_dqt ()
{
  int length, prec;
  int num, i;
  unsigned int tmp;
  unsigned int *p_quant_tbl;

  length = read_word ();
  length -= 2;

  //printf (" length = %d\n", length);
  if (length != out_length[i0++])
    {
      main_result++;
    }


  while (length > 0)
    {
      num = read_byte ();
      /* Precision 0:8bit, 1:16bit */
      prec = num >> 4;
      /* Table Number */
      num &= 0x0f;

      //printf (" prec = %d\n", prec);
      //printf (" num  = %d\n", num);
      if (prec != out_prec[i1])
	{
	  main_result++;
	}
      if (prec != out_prec[i1++])
	{
	  main_result++;
	}

      p_quant_tbl = DecodeInfo_quant_tbl_quantval[num];
      for (i = 0; i < DCTSIZE2; i++)
	{
	  if (prec)
	    {
	      tmp = read_word ();
	    }
	  else
	    {
	      tmp = read_byte ();
	    }
	  p_quant_tbl[izigzag_index[i]] = (unsigned short) tmp;
	}
      length -= DCTSIZE2 + 1;
      if (prec)
	{
	  length -= DCTSIZE2;
	}
    }
}

void
read_markers (unsigned char *buf)
{
  int unread_marker;
  int sow_SOI;

  ReadBuf = buf;

  sow_SOI = 0;

  unread_marker = 0;

  /* Read the head of the marker */
  for (;;)
    {
      if (!sow_SOI)
	{
	  unread_marker = first_marker ();
	}
      else
	{
	  unread_marker = next_marker ();
	}

      //printf ("\nmarker = 0x%x\n", unread_marker);
      switch (unread_marker)
	{
	case M_SOI:		/* Start of Image */
	  sow_SOI = 1;
	  break;

	case M_SOF0:		/* Baseline DCT ( Huffman )  */
	  get_sof ();
	  break;

	case M_SOS:		/* Start of Scan ( Head of Compressed Data )  */
	  get_sos ();
	  return;

	case M_DHT:
	  get_dht ();
	  break;

	case M_DQT:
	  get_dqt ();
	  break;

	case M_EOI:
	  return;
	}
    }
}
