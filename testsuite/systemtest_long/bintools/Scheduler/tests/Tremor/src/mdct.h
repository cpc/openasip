/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis 'TREMOR' CODEC SOURCE CODE.   *
 *                                                                  *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis 'TREMOR' SOURCE CODE IS (C) COPYRIGHT 1994-2002    *
 * BY THE Xiph.Org FOUNDATION http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: modified discrete cosine transform prototypes

 ********************************************************************/

#ifndef _OGG_mdct_H_
#define _OGG_mdct_H_

#include "ivorbiscodec.h"
#include "misc.h"

#define DATA_TYPE ogg_int32_t
#define REG_TYPE  register ogg_int32_t
#define cPI3_8 (0x61f78a9bUL>>1)
#define cPI2_8 (0xb504f334UL>>1)
#define cPI1_8 (0xec835e7aUL>>1)

extern void mdct_forward(int n, DATA_TYPE *in, DATA_TYPE *out);
extern void mdct_backward(int n, DATA_TYPE *in, DATA_TYPE *out);

#endif












