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

 function: normalized modified discrete cosine transform
           power of two length transform only [64 <= n ]
 last mod: $Id: mdct.c,v 1.3 2005/04/07 16:54:13 visit0r Exp $

 Original algorithm adapted long ago from _The use of multirate filter
 banks for coding of high quality digital audio_, by T. Sporer,
 K. Brandenburg and B. Edler, collection of the European Signal
 Processing Conference (EUSIPCO), Amsterdam, June 1992, Vol.1, pp
 211-214

 The below code implements an algorithm that no longer looks much like
 that presented in the paper, but the basic structure remains if you
 dig deep enough to see it.

 This module DOES NOT INCLUDE code to generate/apply the window
 function.  Everybody has their own weird favorite including me... I
 happen to like the properties of y=sin(2PI*sin^2(x)), but others may
 vehemently disagree.

 ********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ivorbiscodec.h"
#include "os.h"
#include "mdct.h"
#include "mdct_lookup.h"
#include "misc.h"

typedef struct {
  int n;
  int log2n;
  
  DATA_TYPE *trig;
  DATA_TYPE  scale;
} mdct_lookup;

static void mdct_init(mdct_lookup *lookup,int n){
  lookup->n=n;
  switch(n){
  case 64:
    lookup->log2n=6;
    lookup->trig=triglook_64;
    lookup->scale=0x04000000;
    break;
  case 128:
    lookup->log2n=7;
    lookup->trig=triglook_128;
    lookup->scale=0x02000000;
    break;
  case 256:
    lookup->log2n=8;
    lookup->trig=triglook_256;
    lookup->scale=0x01000000;
    break;
  case 512:
    lookup->log2n=9;
    lookup->trig=triglook_512;
    lookup->scale=0x00800000;
    break;
  case 1024:
    lookup->log2n=10;
    lookup->trig=triglook_1024;
    lookup->scale=0x00400000;
    break;
  case 2048:
    lookup->log2n=11;
    lookup->trig=triglook_2048;
    lookup->scale=0x00200000;
    break;
  case 4096:
    lookup->log2n=12;
    lookup->trig=triglook_4096;
    lookup->scale=0x00100000;
    break;
  case 8192:
    lookup->log2n=13;
    lookup->trig=triglook_8192;
    lookup->scale=0x00080000;
    break;
  default:
    /* die horribly */
    memset(lookup,0,sizeof(*lookup));
  }
}

/* 8 point butterfly (in place, 4 register) */
STIN void mdct_butterfly_8(DATA_TYPE *x){
  REG_TYPE r0   = x[6] + x[2];
  REG_TYPE r1   = x[6] - x[2];
  REG_TYPE r2   = x[4] + x[0];
  REG_TYPE r3   = x[4] - x[0];

	   x[6] = r0   + r2;
	   x[4] = r0   - r2;
	   
	   r0   = x[5] - x[1];
	   r2   = x[7] - x[3];
	   x[0] = r1   + r0;
	   x[2] = r1   - r0;
	   
	   r0   = x[5] + x[1];
	   r1   = x[7] + x[3];
	   x[3] = r2   + r3;
	   x[1] = r2   - r3;
	   x[7] = r1   + r0;
	   x[5] = r1   - r0;
	   
}

/* 16 point butterfly (in place, 4 register) */
STIN void mdct_butterfly_16(DATA_TYPE *x){
  REG_TYPE r0     = x[1]  - x[9];
  REG_TYPE r1     = x[0]  - x[8];

           x[8]  += x[0];
           x[9]  += x[1];
           x[0]   = MULT31((r0   + r1) , cPI2_8);
           x[1]   = MULT31((r0   - r1) , cPI2_8);

           r0     = x[3]  - x[11];
           r1     = x[10] - x[2];
           x[10] += x[2];
           x[11] += x[3];
           x[2]   = r0;
           x[3]   = r1;

           r0     = x[12] - x[4];
           r1     = x[13] - x[5];
           x[12] += x[4];
           x[13] += x[5];
           x[4]   = MULT31((r0   - r1) , cPI2_8);
           x[5]   = MULT31((r0   + r1) , cPI2_8);

           r0     = x[14] - x[6];
           r1     = x[15] - x[7];
           x[14] += x[6];
           x[15] += x[7];
           x[6]  = r0;
           x[7]  = r1;

	   mdct_butterfly_8(x);
	   mdct_butterfly_8(x+8);
}

/* 32 point butterfly (in place, 4 register) */
STIN void mdct_butterfly_32(DATA_TYPE *x){
  REG_TYPE r0     = x[30] - x[14];
  REG_TYPE r1     = x[31] - x[15];

           x[30] +=         x[14];           
	   x[31] +=         x[15];
           x[14]  =         r0;              
	   x[15]  =         r1;

           r0     = x[28] - x[12];   
	   r1     = x[29] - x[13];
           x[28] +=         x[12];           
	   x[29] +=         x[13];
           x[12]  = MULT31( r0 , cPI1_8 ) - MULT31( r1 , cPI3_8 );
	   x[13]  = MULT31( r0 , cPI3_8 ) + MULT31( r1 , cPI1_8 );

           r0     = x[26] - x[10];
	   r1     = x[27] - x[11];
	   x[26] +=         x[10];
	   x[27] +=         x[11];
	   x[10]  = MULT31(( r0  - r1 ) , cPI2_8);
	   x[11]  = MULT31(( r0  + r1 ) , cPI2_8);

	   r0     = x[24] - x[8];
	   r1     = x[25] - x[9];
	   x[24] += x[8];
	   x[25] += x[9];
	   x[8]   = MULT31( r0 , cPI3_8 ) - MULT31( r1 , cPI1_8 );
	   x[9]   = MULT31( r1 , cPI3_8 ) + MULT31( r0 , cPI1_8 );

	   r0     = x[22] - x[6];
	   r1     = x[7]  - x[23];
	   x[22] += x[6];
	   x[23] += x[7];
	   x[6]   = r1;
	   x[7]   = r0;

	   r0     = x[4]  - x[20];
	   r1     = x[5]  - x[21];
	   x[20] += x[4];
	   x[21] += x[5];
	   x[4]   = MULT31( r1 , cPI1_8 ) + MULT31( r0 , cPI3_8 );
	   x[5]   = MULT31( r1 , cPI3_8 ) - MULT31( r0 , cPI1_8 );

	   r0     = x[2]  - x[18];
	   r1     = x[3]  - x[19];
	   x[18] += x[2];
	   x[19] += x[3];
	   x[2]   = MULT31(( r1  + r0 ) , cPI2_8);
	   x[3]   = MULT31(( r1  - r0 ) , cPI2_8);

	   r0     = x[0]  - x[16];
	   r1     = x[1]  - x[17];
	   x[16] += x[0];
	   x[17] += x[1];
	   x[0]   = MULT31( r1 , cPI3_8 ) + MULT31( r0 , cPI1_8 );
	   x[1]   = MULT31( r1 , cPI1_8 ) - MULT31( r0 , cPI3_8 );

	   mdct_butterfly_16(x);
	   mdct_butterfly_16(x+16);

}

/* N/stage point generic N stage butterfly (in place, 2 register) */
STIN void mdct_butterfly_generic(DATA_TYPE *x,
				 int points,
				 int step){
  DATA_TYPE *T=quarter_sin+1024;
  DATA_TYPE *V=quarter_sin;
  DATA_TYPE *x1        = x          + points      - 8;
  DATA_TYPE *x2        = x          + (points>>1) - 8;
  REG_TYPE   r0;
  REG_TYPE   r1;

  do{
               r0      = x1[6]      -  x2[6];
	       r1      = x1[7]      -  x2[7];
	       x1[6]  += x2[6];
	       x1[7]  += x2[7];
	       x2[6]   = MULT30(r1 , *V ) + MULT30( r0 , -*T);
	       x2[7]   = MULT30(r1 , -*T ) - MULT30( r0 , *V);
	T-=step;
	V+=step;
	       r0      = x1[4]      -  x2[4];
	       r1      = x1[5]      -  x2[5];
	       x1[4]  += x2[4];
	       x1[5]  += x2[5];
	       x2[4]   = MULT30(r1 , *V ) + MULT30( r0 , -*T);
	       x2[5]   = MULT30(r1 , -*T ) - MULT30( r0 , *V);
	T-=step;
	V+=step;	
	       r0      = x1[2]      -  x2[2];
	       r1      = x1[3]      -  x2[3];
	       x1[2]  += x2[2];
	       x1[3]  += x2[3];
	       x2[2]   = MULT30(r1 , *V ) + MULT30( r0 , -*T);
	       x2[3]   = MULT30(r1 , -*T ) - MULT30( r0 , *V);
	T-=step;
	V+=step;	
	       r0      = x1[0]      -  x2[0];
	       r1      = x1[1]      -  x2[1];
	       x1[0]  += x2[0];
	       x1[1]  += x2[1];
	       x2[0]   = MULT30(r1 , *V ) + MULT30( r0 , -*T);
	       x2[1]   = MULT30(r1 , -*T ) - MULT30( r0 , *V);
	       
        x1-=8;
        x2-=8;
        T-=step;
	V+=step;
  }while(T>quarter_sin);

  do{
    
               r0      = x1[6]      -  x2[6];
	       r1      = x1[7]      -  x2[7];
	       x1[6]  += x2[6];
	       x1[7]  += x2[7];
	       x2[6]   = MULT30(r1 , *V ) + MULT30( r0 , *T);
	       x2[7]   = MULT30(r1 , *T ) - MULT30( r0 , *V);
	T+=step;
	V-=step;
	       r0      = x1[4]      -  x2[4];
	       r1      = x1[5]      -  x2[5];
	       x1[4]  += x2[4];
	       x1[5]  += x2[5];
	       x2[4]   = MULT30(r1 , *V ) + MULT30( r0 , *T);
	       x2[5]   = MULT30(r1 , *T ) - MULT30( r0 , *V);
	T+=step;
	V-=step;	
	       r0      = x1[2]      -  x2[2];
	       r1      = x1[3]      -  x2[3];
	       x1[2]  += x2[2];
	       x1[3]  += x2[3];
	       x2[2]   = MULT30(r1 , *V ) + MULT30( r0 , *T);
	       x2[3]   = MULT30(r1 , *T ) - MULT30( r0 , *V);
	T+=step;
	V-=step;	
	       r0      = x1[0]      -  x2[0];
	       r1      = x1[1]      -  x2[1];
	       x1[0]  += x2[0];
	       x1[1]  += x2[1];
	       x2[0]   = MULT30(r1 , *V ) + MULT30( r0 , *T);
	       x2[1]   = MULT30(r1 , *T ) - MULT30( r0 , *V);
	       
        x1-=8;
        x2-=8;
        T+=step;
	V-=step;

  }while(x2>=x);
}

STIN void mdct_butterflies(mdct_lookup *init,
			     DATA_TYPE *x,
			     int points,
			     int step){
  int stages=init->log2n-5;
  int i,j;
  
  for(i=0;--stages>0;i++){
    for(j=0;j<(1<<i);j++)
      mdct_butterfly_generic(x+(points>>i)*j,points>>i,2<<(i+step));
  }

  for(j=0;j<points;j+=32)
    mdct_butterfly_32(x+j);

}

STIN int bitrev12(int x){
  int temp = (x<<8) | (x>>8) | (x & 0x0f0);
  temp = ((temp & 0xccc) >> 2) | ((temp & 0x333) << 2);
  return ((temp & 0xaaa) >> 1) | ((temp & 0x555) << 1);
}

STIN void mdct_bitreverse(mdct_lookup *init, 
			  DATA_TYPE *x,
			  int shift){

  int          n     = init->n;
  int          bit   = 0;
  DATA_TYPE   *w0    = x;
  DATA_TYPE   *w1    = x = w0+(n>>1);
  DATA_TYPE   *T     = init->trig+(n>>1);

  do{
    REG_TYPE  r3     = bitrev12(bit++);
    DATA_TYPE *x0    = x + ((r3 ^ 0xfff)>>shift) -1;
    DATA_TYPE *x1    = x + (r3>>shift);

    REG_TYPE  r0     = x0[1]  - x1[1];
    REG_TYPE  r1     = x0[0]  + x1[0];
    REG_TYPE  r2     = MULT30(r1     , T[0] )  + MULT30(r0 , T[1]);
              r3     = MULT30(r1     , T[1] )  - MULT30(r0 , T[0]);

	      w1    -= 4;

              r0     = (x0[1] + x1[1])/2;
              r1     = (x0[0] - x1[0])/2;
      
	      w0[0]  = r0     + r2;
	      w1[2]  = r0     - r2;
	      w0[1]  = r1     + r3;
	      w1[3]  = r3     - r1;

	      r3     = bitrev12(bit++);
              x0     = x + ((r3 ^ 0xfff)>>shift) -1;
              x1     = x + (r3>>shift);

              r0     = x0[1]  - x1[1];
              r1     = x0[0]  + x1[0];
              r2     = MULT30(r1     , T[2] )  + MULT30(r0 , T[3]);
              r3     = MULT30(r1     , T[3] )  - MULT30(r0 , T[2]);

              r0     = (x0[1] + x1[1])/2;
              r1     = (x0[0] - x1[0])/2;
      
	      w0[2]  = r0     + r2;
	      w1[0]  = r0     - r2;
	      w0[3]  = r1     + r3;
	      w1[1]  = r3     - r1;

	      T     += 4;
	      w0    += 4;

  }while(w0<w1);
}

void mdct_backward(int n, DATA_TYPE *in, DATA_TYPE *out){
  int n2=n>>1;
  int n4=n>>2;
  mdct_lookup init;
  DATA_TYPE *iX;
  DATA_TYPE *oX;
  DATA_TYPE *T;
  DATA_TYPE *V;
  int step;

  mdct_init(&init,n);
  step=1<<(13-(init.log2n));
   
  /* rotate */

  iX            = in+n2-7;
  oX            = out+n2+n4;
  T             = quarter_sin;
  V             = quarter_sin+1024;

  do{

    oX         -= 4;
    oX[2]       = MULT30(-iX[6] , *V) - MULT30(iX[4]  , *T);
    oX[3]       = MULT30 (iX[4] , *V) - MULT30(iX[6]  , *T);
    T          += step;
    V          -= step;
    oX[0]       = MULT30(-iX[2] , *V) - MULT30(iX[0]  , *T);
    oX[1]       = MULT30 (iX[0] , *V) - MULT30(iX[2]  , *T);
    iX         -= 8;
    T          += step;
    V	       -= step;

  }while(iX>=in);

  iX            = in+n2-8;
  oX            = out+n2+n4;
  T             = quarter_sin;
  V             = quarter_sin+1024;

  do{
   
    V          -= step;
    T          += step;
    oX[0]       =  MULT30 (iX[4] ,  *V) + MULT30(iX[6] , -*T);
    oX[1]       =  MULT30 (iX[4] , -*T) - MULT30(iX[6] ,  *V);
    V          -= step;
    T          += step;
    oX[2]       =  MULT30 (iX[0] ,  *V) + MULT30(iX[2] , -*T);
    oX[3]       =  MULT30 (iX[0] , -*T) - MULT30(iX[2] ,  *V);
    
    iX         -= 8;
    oX         += 4;

  }while(iX>=in);

  mdct_butterflies(&init,out+n2,n2,13-(init.log2n));
  mdct_bitreverse(&init,out,13-init.log2n);

  /* roatate + window */

  {
    DATA_TYPE *oX1=out+n2+n4;
    DATA_TYPE *oX2=out+n2+n4;
    DATA_TYPE *iX =out;
    T             =init.trig;
    
    do{
      oX1-=4;

      oX1[3]  =  MULT30 (iX[0] , T[1]) - MULT30(iX[1] , T[0]);
      oX2[0]  =-(MULT30 (iX[0] , T[0]) + MULT30(iX[1] , T[1]));

      oX1[2]  =  MULT30 (iX[2] , T[3]) - MULT30(iX[3] , T[2]);
      oX2[1]  =-(MULT30 (iX[2] , T[2]) + MULT30(iX[3] , T[3]));

      oX1[1]  =  MULT30 (iX[4] , T[5]) - MULT30(iX[5] , T[4]);
      oX2[2]  =-(MULT30 (iX[4] , T[4]) + MULT30(iX[5] , T[5]));

      oX1[0]  =  MULT30 (iX[6] , T[7]) - MULT30(iX[7] , T[6]);
      oX2[3]  =-(MULT30 (iX[6] , T[6]) + MULT30(iX[7] , T[7]));

      oX2+=4;
      iX    +=   8;
      T     +=   8;
    }while(iX<oX1);

    iX=out+n2+n4;
    oX1=out+n4;
    oX2=oX1;

    do{
      oX1-=4;
      iX-=4;

      oX2[0] = -(oX1[3] = iX[3]);
      oX2[1] = -(oX1[2] = iX[2]);
      oX2[2] = -(oX1[1] = iX[1]);
      oX2[3] = -(oX1[0] = iX[0]);

      oX2+=4;
    }while(oX2<iX);

    iX=out+n2+n4;
    oX1=out+n2+n4;
    oX2=out+n2;
    do{
      oX1-=4;
      oX1[0]= iX[3];
      oX1[1]= iX[2];
      oX1[2]= iX[1];
      oX1[3]= iX[0];
      iX+=4;
    }while(oX1>oX2);
  }
}

