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
 * Copyright (C) 2008
 * Y. Hara, H. Tomiyama, S. Honda, H. Takada and K. Ishii
 * Nagoya University, Japan
 * All rights reserved.
 *
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any license fee or
 * royalty on an "as is" basis. The authors disclaims any and all warranties, 
 * whether express, implied, or statuary, including any implied warranties or 
 * merchantability or of fitness for a particular purpose. In no event shall the
 * copyright-holder be liable for any incidental, punitive, or consequential damages
 * of any kind whatsoever arising from the use of these programs. This disclaimer
 * of warranty extends to the user of these programs and user's customers, employees,
 * agents, transferees, successors, and assigns.
 *
 */
/*
 * Read the header information from buffer in JFIF format and begin decoding
 *
 *  @(#) $Id: jfif_read.c,v 1.2 2003/07/18 10:19:21 honda Exp $ 
 */

int *p_xhtbl_bits;
int p_dhtbl_ml;
int p_dhtbl_maxcode[36], p_dhtbl_mincode[36], p_dhtbl_valptr[36];

/*
 * Initialize JPEG_DECODE_INFO after reading markers
 */
void
jpeg_init_decompress ()
{
  int i;
  /*
   * Get MCU number
   */
  DecodeInfo_MCUHeight = (DecodeInfo_image_height - 1) / 8 + 1;
  DecodeInfo_MCUWidth = (DecodeInfo_image_width - 1) / 8 + 1;
  DecodeInfo_NumMCU = DecodeInfo_MCUHeight * DecodeInfo_MCUWidth;

  /*
   *  Create Huffman Table for decoding
   */
  p_xhtbl_bits = DecodeInfo_dc_xhuff_tbl_bits[0];
  huff_make_dhuff_tb ();
  DecodeInfo_dc_dhuff_tbl_ml[0] = p_dhtbl_ml;
  for (i = 0; i < 36; i++)
    {
      DecodeInfo_dc_dhuff_tbl_maxcode[0][i] = p_dhtbl_maxcode[i];
      DecodeInfo_dc_dhuff_tbl_mincode[0][i] = p_dhtbl_mincode[i];
      DecodeInfo_dc_dhuff_tbl_valptr[0][i] = p_dhtbl_valptr[i];
    }

  p_xhtbl_bits = DecodeInfo_dc_xhuff_tbl_bits[1];
  huff_make_dhuff_tb ();
  DecodeInfo_dc_dhuff_tbl_ml[1] = p_dhtbl_ml;
  for (i = 0; i < 36; i++)
    {
      DecodeInfo_dc_dhuff_tbl_maxcode[1][i] = p_dhtbl_maxcode[i];
      DecodeInfo_dc_dhuff_tbl_mincode[1][i] = p_dhtbl_mincode[i];
      DecodeInfo_dc_dhuff_tbl_valptr[1][i] = p_dhtbl_valptr[i];
    }

  p_xhtbl_bits = DecodeInfo_ac_xhuff_tbl_bits[0];
  huff_make_dhuff_tb ();
  DecodeInfo_ac_dhuff_tbl_ml[0] = p_dhtbl_ml;
  for (i = 0; i < 36; i++)
    {
      DecodeInfo_ac_dhuff_tbl_maxcode[0][i] = p_dhtbl_maxcode[i];
      DecodeInfo_ac_dhuff_tbl_mincode[0][i] = p_dhtbl_mincode[i];
      DecodeInfo_ac_dhuff_tbl_valptr[0][i] = p_dhtbl_valptr[i];
    }

  p_xhtbl_bits = DecodeInfo_ac_xhuff_tbl_bits[1];
  huff_make_dhuff_tb ();
  DecodeInfo_ac_dhuff_tbl_ml[1] = p_dhtbl_ml;
  for (i = 0; i < 36; i++)
    {
      DecodeInfo_ac_dhuff_tbl_maxcode[1][i] = p_dhtbl_maxcode[i];
      DecodeInfo_ac_dhuff_tbl_mincode[1][i] = p_dhtbl_mincode[i];
      DecodeInfo_ac_dhuff_tbl_valptr[1][i] = p_dhtbl_valptr[i];
    }
}



void
jpeg_read (unsigned char *read_buf)
{
  /*
   *  Read markers
   */
  read_markers (read_buf);


  /*
   * Initialize the information used for decoding
   * 
   */
  jpeg_init_decompress ();

  /*
   *  Start decoding
   */
  decode_start ();
}
