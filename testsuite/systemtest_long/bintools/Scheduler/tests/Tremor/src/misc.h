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

 function: miscellaneous math and prototypes

 ********************************************************************/


#ifndef _V_RANDOM_H_
#define _V_RANDOM_H_
#include "ivorbiscodec.h"
#include "os_types.h"

#if (defined(__TCE__) && !(defined(__TCE64__)))
#undef long
#endif
#include <stdio.h>
#include <stdlib.h>
#if (defined(__TCE__) && !(defined(__TCE64__)))
#define long int
#endif

extern void *_vorbis_block_alloc(vorbis_block *vb,long bytes);
extern void _vorbis_block_ripcord(vorbis_block *vb);
extern void _analysis_output(char *base,int i,ogg_int32_t *v,int point,
			     int n,int bark,int dB);

#include "asm_arm.h"

#ifndef _V_WIDE_MATH
#define _V_WIDE_MATH


#ifndef LONGLONGEMUL

#if (!(defined (__LITTLE_ENDIAN__)) && !(defined(__BIG_ENDIAN__)))
#include <endian.h>
#else
#ifdef __LITTLE_ENDIAN__
#define BYTE_ORDER __LITTLE_ENDIAN__
#define LITTLE_ENDIAN __LITTLE_ENDIAN__
#else
#define BYTE_ORDER __BIG_ENDIAN__
#define BIG_ENDIAN __BIG_ENDIAN__
#endif
#endif
#if BYTE_ORDER==LITTLE_ENDIAN
union magic {
  struct {
    ogg_int32_t lo;
    ogg_int32_t hi;
  } halves;
  ogg_int64_t whole;
};
#elif BYTE_ORDER==BIG_ENDIAN
union magic {
  struct {
    ogg_int32_t hi;
    ogg_int32_t lo;
  } halves;
  ogg_int64_t whole;
};
#endif

#endif

#ifndef LONGLONGEMUL
static inline ogg_int32_t MULT32(ogg_int32_t x, ogg_int32_t y) {
  union magic magic;
  magic.whole = (ogg_int64_t)x * y;
  return magic.halves.hi;
}
#else
# if 0
static inline ogg_int32_t MULT32(ogg_int32_t x, ogg_int32_t y) {
/* benchmark runtime: 9.23 sec */

  longlongemul_t xll=long2longlong(x);
  longlongemul_t yll=long2longlong(y);
  longlongemul_t res=longlongmult(xll,yll);
  return res.hi;
}
# else
static inline ogg_int32_t MULT32(ogg_int32_t x, ogg_int32_t y) {
/* benchmark runtime: 5.35 sec */
/*
                               --  --  a2  a1     all 16 bit in unsigned _long_
                               --  --  b2  b1
                               -------------- x
                                       c2  c1     c=b1a1
                                   d2  d1         d=b1a2
                               --  --             e=b1a3
                           --  --                 f=b1a4
                                   g2  g1         g=b2a1
                               h2  h1             h=b2a2
                           --  --                 i=b2a3
                       --  --                     j=b2a4
                               --  --             k=b3a1
                           --  --                 l=b3a2
                       --  --                     m=b3a3
                   --  --                         n=b3a4
                           --  --                 o=b4a1
                       --  --                     p=b4a2
                   --  --                         q=b4a3
               --  --                             r=b4a4
               ------------------------------ +
               --  --  --  --  s4  s3  s2  s1
*/

  register unsigned long a2;
  register unsigned long a1;
  register unsigned long b2;
  register unsigned long b1;

  register unsigned long c;

  register unsigned long s4;
  register unsigned long s3;
  register unsigned long s2;
  register unsigned long s1;

  register ogg_int32_t reshi;
  register ogg_int32_t reslo;

  register int negate = 0;

#if 0
signed long testing;
/* fprintf(stderr, "MULT30 in x=%ld y=%ld ", x, y); */
testing=x;
testing=y;
#endif

  /* Note: negating can't overflow, since we'll look at it in _unsigned_ */
  if ( x<0 ) { x= -x; negate = 1; }
  if ( y<0 ) { y= -y; negate = 1 - negate; }

  a2=((unsigned long) x) >> 16;
  a1=((unsigned long) x) & 0x0000ffff; 
  b2=((unsigned long) y) >> 16;
  b1=((unsigned long) y) & 0x0000ffff;

  /* c */
  c=b1*a1;
  s1=c&0x0000ffff;
  s2=c>>16;

  /* d */
  c=b1*a2;
  s2+=c&0x0000ffff;
  s3=c>>16;

  /* g */
  c=b2*a1;
  s2+=c&0x0000ffff;
  s3+=c>>16;

  /* h */
  c=b2*a2;
  s3+=c&0x0000ffff;
  s4=c>>16;

  /* carry */
  s3+=s2>>16;		/* s2+=s1>>16 not needed, s1 is no sum */
  s4+=s3>>16;

  /* construct result */
  reshi=(s4<<16)|(s3&0x0000ffff);
  reslo=(s2<<16)|s1;

  if (negate)
  {
    reshi = ~reshi;
    reslo = ~reslo;
    reslo++;
    if (reslo == 0)
      reshi ++;
  }

/* fprintf(stderr, "out=%ld\n", reshi); */
/* testing=reshi; */

  return reshi;
}
# endif
#endif

static inline ogg_int32_t MULT31(ogg_int32_t x, ogg_int32_t y) {
  return MULT32(x,y)<<1;
}

#if 1
static inline ogg_int32_t MULT30(ogg_int32_t x, ogg_int32_t y) {
  return MULT32(x,y)<<2;
}
#else
static inline ogg_int32_t MULT30(ogg_int32_t x, ogg_int32_t y) {
  longlongemul_t xll;
  longlongemul_t yll;
  longlongemul_t res;
  xll.lo=x;
  xll.hi=(x<0?-1L : 0L);
  yll.lo=y;
  yll.hi=(y<0?-1L : 0L);
  res=longlongmult(xll,yll);
  return res.hi<<2;
}
#endif

#ifndef LONGLONGEMUL
static inline ogg_int32_t MULT31_SHIFT15(ogg_int32_t x, ogg_int32_t y) {
  union magic magic;
  magic.whole  = (ogg_int64_t)x * y;
  return ((ogg_uint32_t)(magic.halves.lo)>>15) | ((magic.halves.hi)<<17);
}
#else
static inline ogg_int32_t MULT31_SHIFT15(ogg_int32_t x, ogg_int32_t y) {
  longlongemul_t xll=long2longlong(x);
  longlongemul_t yll=long2longlong(y);
  longlongemul_t res=longlongmult(xll,yll);
  return ((ogg_uint32_t)(res.lo)>>15) | ((res.hi)<<17);
}
#endif

static inline ogg_int32_t CLIP_TO_15(ogg_int32_t x) {
  int ret=x;
  ret-= ((x<=32767)-1)&(x-32767);
  ret-= ((x>=-32768)-1)&(x+32768);
  return(ret);
}

#endif

static inline ogg_int32_t VFLOAT_MULT(ogg_int32_t a,ogg_int32_t ap,
				      ogg_int32_t b,ogg_int32_t bp,
				      ogg_int32_t *p){
  if(a && b){
    *p=ap+bp+32;
    return MULT32(a,b);
  }else
    return 0;
}

extern int _ilog(unsigned int v);

static inline ogg_int32_t VFLOAT_MULTI(ogg_int32_t a,ogg_int32_t ap,
				      ogg_int32_t i,
				      ogg_int32_t *p){

  int ip=_ilog(abs(i))-31;
  return VFLOAT_MULT(a,ap,i<<-ip,ip,p);
}

static inline ogg_int32_t VFLOAT_ADD(ogg_int32_t a,ogg_int32_t ap,
				      ogg_int32_t b,ogg_int32_t bp,
				      ogg_int32_t *p){

  if(!a){
    *p=bp;
    return b;
  }else if(!b){
    *p=ap;
    return a;
  }

  /* yes, this can leak a bit. */
  if(ap>bp){
    int shift=ap-bp+1;
    *p=ap+1;
    a>>=1;
    if(shift<32){
      b=(b+(1<<(shift-1)))>>shift;
    }else{
      b=0;
    }
  }else{
    int shift=bp-ap+1;
    *p=bp+1;
    b>>=1;
    if(shift<32){
      a=(a+(1<<(shift-1)))>>shift;
    }else{
      a=0;
    }
  }

  a+=b;
  if((a&0xc0000000)==0xc0000000 || 
     (a&0xc0000000)==0){
    a<<=1;
    (*p)--;
  }
  return(a);
}

#endif




