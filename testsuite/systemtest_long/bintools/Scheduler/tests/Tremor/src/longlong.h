#pragma GCC diagnostic ignored "-Wunused-value"

#ifndef HAVE_LONGLONG_H
#define HAVE_LONGLONG_H

#ifdef LONGLONGEMUL

#include "debug.h"

typedef struct {
        long hi;
        unsigned long lo;
} longlongemul_t;


static inline
longlongemul_t ulong2longlong(unsigned long ul)
{
  longlongemul_t res;
  DPRINTF(("ulon2longlong(%d)\t", ul));


  res.hi=0;
  res.lo=ul;

  DPRINTF(("res.hi:%d res.lo:%lo\n", res.hi, res.lo));
  return res;
}

/* signed */
static inline
longlongemul_t long2longlong(long sl)
{
  longlongemul_t res;
  DPRINTF(("lon2longlong(%d)\t", sl));

  res.hi=(sl<0?-1L : 0L);
  res.lo=sl;

  DPRINTF(("res.hi:%d res.lo:%lo\n", res.hi, res.lo));
  return res;
}

static inline
long longlong2long(longlongemul_t ll)
{
  DPRINTF(("longlong2long(hi:%d lo:%d)\t ret: %d\n", ll.hi, ll.lo, ll.lo));
  return (long) ll.lo;

  /* will clip when !(hi == 0 || hi == -1) */
}

static inline
longlongemul_t longlongplus(longlongemul_t a, longlongemul_t b)
{
  longlongemul_t res;
  DPRINTF(("longlongplus(hi:%d lo:%d, hi:%d lo:%d)\t", a.hi,a.lo,b.hi,b.lo));

  res.lo=a.lo+b.lo;
  res.hi=a.hi+b.hi;

  if ((res.lo<a.lo) || (res.lo<b.lo))
    res.hi ++;

  DPRINTF(("res.hi:%d res.lo:%lo\n", res.hi, res.lo));
  return res;
}

static inline
longlongemul_t longlongplusulong(longlongemul_t a, unsigned long b)
{
  longlongemul_t res;
  DPRINTF(("longlongpluslong(hi:%d lo:%d, %d)\t", a.hi,a.lo,b));

  res.lo=a.lo+b;
  res.hi=a.hi;

  if ((res.lo<a.lo) || (res.lo<b))
    res.hi ++;

  DPRINTF(("res.hi:%d res.lo:%lo\n", res.hi, res.lo));
  return res;
}

static inline
longlongemul_t longlongminus(longlongemul_t a, longlongemul_t b)
{
  longlongemul_t res;
  DPRINTF(("longlongminus(hi:%d lo:%d, hi:%d lo:%d)\t", a.hi,a.lo,b.hi,b.lo));

  res.lo=a.lo-b.lo;
  res.hi=a.hi-b.hi;

  if (a.lo<b.lo)
    res.hi --;

  DPRINTF(("res.hi:%d res.lo:%lo\n", res.hi, res.lo));
  return res;
}

static inline
longlongemul_t longlongminusulong(longlongemul_t a, unsigned long b)
{
  longlongemul_t res;
  DPRINTF(("longlongminuslong(hi:%d lo:%d, %d)\t", a.hi,a.lo,b));

  res.lo=a.lo-b;
  res.hi=a.hi;

  if (a.lo<b)
    res.hi --;

  DPRINTF(("res.hi:%d res.lo:%lo\n", res.hi, res.lo));
  return res;
}

static inline
longlongemul_t longlongshiftright(longlongemul_t a, int shift)
{
  longlongemul_t res;
  DPRINTF(("longlongshiftright(hi:%d lo:%d, %d)\t", a.hi,a.lo,shift));

  res.lo=a.lo;
  res.hi=a.hi;

  if (shift != 0)
  {
    if (shift < 32)
    {
      res.lo >>= shift;
      res.lo |= res.hi << (32-shift);
      res.hi >>= shift;
    }
    else
    {
      res.lo = res.hi >> (shift-32);
      res.hi = (res.hi < 0 ? -1L : 0);
    }
  }

  DPRINTF(("res.hi:%d res.lo:%lo\n", res.hi, res.lo));
  return res;
}

static inline
longlongemul_t longlongshiftleft(longlongemul_t a, int shift)
{
  longlongemul_t res;
  DPRINTF(("longlongshiftleft(hi:%d lo:%d, %d)\t", a.hi,a.lo,shift));

  res.lo=a.lo;
  res.hi=a.hi;

  if (shift != 0)
  {
    if (shift < 32)
    {
      res.hi <<= shift;
      res.hi |= res.lo >> (32-shift);
      res.lo <<= shift;
    }
    else
    {
      res.hi = res.lo << (shift-32);
      res.lo = 0;
    }
  }

  DPRINTF(("res.hi:%d res.lo:%lo\n", res.hi, res.lo));
  return res;
}

static inline
int longlongequals(longlongemul_t a, longlongemul_t b)
{
  DPRINTF(("longlongequals(hi:%d lo:%d, hi:%d lo:%d)\t res: %d", 
	   a.hi,a.lo,b.hi,b.lo, ((a.lo==b.lo) && (a.hi==b.hi))));

  return ((a.lo==b.lo) && (a.hi==b.hi));
}

static inline
int longlonggreater(longlongemul_t a, longlongemul_t b)
{
  DPRINTF(("longlonggreater(hi:%d lo:%d, hi:%d lo:%d)\t",a.hi,a.lo,b.hi,b.lo));

  if (a.hi > b.hi) {
      DPRINTF(("res: %d\n",1));
      return 1;

  } else if (a.hi == b.hi) {
      DPRINTF(("res: %d\n",a.lo > b.lo));
      return a.lo > b.lo;

  } else {
      DPRINTF(("res: %d\n",0));
      return 0;
  }
}

static inline
int longlongsmaller(longlongemul_t a, longlongemul_t b)
{
    DPRINTF(("longlongsmaller(hi:%d lo:%d, hi:%d lo:%d)\t",a.hi,a.lo,b.hi,b.lo));

    if (a.hi < b.hi) {
	DPRINTF(("res: %d\n",1));
	return 1;

    } else if (a.hi == b.hi) {
	DPRINTF(("res: %d\n",a.lo < b.lo));
	return a.lo < b.lo;

    } else {
	DPRINTF(("res: %d\n",0));
	return 0;
    }
}

/* longlongmult_old */
static inline
longlongemul_t longlongmult_old(longlongemul_t a, longlongemul_t b)
/* benchmark runtime: 280.59 sec */
{
  longlongemul_t res;
  longlongemul_t t;
  longlongemul_t q;
  int counter=0;

  DPRINTF(("longlongmult_old(hi:%d lo:%d, hi:%d lo:%d)\t", a.hi,a.lo,b.hi,b.lo));

  t.lo=a.lo; t.hi=a.hi;  
  q.lo=b.lo; q.hi=b.hi;  

  res.lo=0; res.hi=0;

  do
  {
    if ((q.lo & 1) != 0)
      res = longlongplus(res, t);
    q = longlongshiftright(q, 1);
    t = longlongshiftleft(t, 1);
    counter++;
  }
  while ((counter<64) && ((q.hi != 0) || (q.lo != 0)));

  DPRINTF(("res.hi:%d res.lo:%lo\n", res.hi, res.lo));
  return res;
}

/* longlongmult_full128bit */
static inline
longlongemul_t longlongmult_full128bit(longlongemul_t a, longlongemul_t b)
/* benchmark runtime: 7.08 sec */
/* NOTE: don't use this, it calculates all 128 bits of the product, while
   it only returns 64. It merely serves as an example. See below.

   Also see http://www.8052.com/mul16.phtml

                               a4  a3  a2  a1     all 16 bit in unsigned _long_
                               b4  b3  b2  b1
                               -------------- x
                                       c2  c1     c=b1a1
                                   d2  d1         d=b1a2
                               e2  e1             e=b1a3
                           f2  f1                 f=b1a4
                                   g2  g1         g=b2a1
                               h2  h1             h=b2a2
                           i2  i1                 i=b2a3
                       j2  j1                     j=b2a4
                               k2  k1             k=b3a1
                           l2  l1                 l=b3a2
                       m2  m1                     m=b3a3
                   n2  n1                         n=b3a4
                           o2  o1                 o=b4a1
                       p2  p1                     p=b4a2
                   q2  q1                         q=b4a3
               r2  r1                             r=b4a4
               ------------------------------ +
               s8  s7  s6  s5  s4  s3  s2  s1

   To handle the carry, we have s1-8 as _longs_ containing the running
   sums. When finished, we do the carry stuff all at once.

   The full-128-bit result is only correct for positive*positive (can be
   corrected by negating separately); the 64-bit result is correct for
   all values.

                       ff ff   = -1
                       ff ff   = -1
                       ----- x
                       fe 01
                    fe 01
                    fe 01
                 fe 01
                 ----------- +
                 01 01         carry
                 ff fe 00 01   = -65538
                 -- -- 00 01   = 1

   When looking at the right 2 bytes only, 00 01, the result is correct.
*/
{
  register unsigned long a4=((unsigned long) a.hi) >> 16;
  register unsigned long a3=((unsigned long) a.hi) & 0x0000ffff;
  register unsigned long a2=a.lo >> 16;
  register unsigned long a1=a.lo & 0x0000ffff;

  register unsigned long b4=((unsigned long) b.hi) >> 16;
  register unsigned long b3=((unsigned long) b.hi) & 0x0000ffff;
  register unsigned long b2=b.lo >> 16;
  register unsigned long b1=b.lo & 0x0000ffff;

  register unsigned long c;

  register unsigned long s8=0;
  register unsigned long s7=0;
  register unsigned long s6=0;
  register unsigned long s5=0;
  register unsigned long s4=0;
  register unsigned long s3=0;
  register unsigned long s2=0;
  register unsigned long s1=0;

  register longlongemul_t res;

  DPRINTF(("longlongmult_full128bit(hi:%d lo:%d, hi:%d lo:%d)\t", a.hi,a.lo,b.hi,b.lo));

  /* c */
  c=b1*a1;
  s1+=c&0x0000ffff;
  s2+=c>>16;

  /* d */
  c=b1*a2;
  s2+=c&0x0000ffff;
  s3+=c>>16;

  /* e */
  c=b1*a3;
  s3+=c&0x0000ffff;
  s4+=c>>16;

  /* f */
  c=b1*a4;
  s4+=c&0x0000ffff;
  s5+=c>>16;

  /* g */
  c=b2*a1;
  s2+=c&0x0000ffff;
  s3+=c>>16;

  /* h */
  c=b2*a2;
  s3+=c&0x0000ffff;
  s4+=c>>16;

  /* i */
  c=b2*a3;
  s4+=c&0x0000ffff;
  s5+=c>>16;

  /* j */
  c=b2*a4;
  s5+=c&0x0000ffff;
  s6+=c>>16;

  /* k */
  c=b3*a1;
  s3+=c&0x0000ffff;
  s4+=c>>16;

  /* l */
  c=b3*a2;
  s4+=c&0x0000ffff;
  s5+=c>>16;

  /* m */
  c=b3*a3;
  s5+=c&0x0000ffff;
  s6+=c>>16;

  /* n */
  c=b3*a4;
  s6+=c&0x0000ffff;
  s7+=c>>16;

  /* o */
  c=b4*a1;
  s4+=c&0x0000ffff;
  s5+=c>>16;

  /* p */
  c=b4*a2;
  s5+=c&0x0000ffff;
  s6+=c>>16;

  /* q */
  c=b4*a3;
  s6+=c&0x0000ffff;
  s7+=c>>16;

  /* r */
  c=b4*a4;
  s7+=c&0x0000ffff;
  s8+=c>>16;

  /* carry */
  s2+=s1>>16;
  s3+=s2>>16;
  s4+=s3>>16;
  s5+=s4>>16;
  s6+=s5>>16;
  s7+=s6>>16;
  s8+=s7>>16;

  /* construct result */
  res.hi=(s4<<16)|(s3&0x0000ffff);
  res.lo=(s2<<16)|(s1&0x0000ffff);

  DPRINTF(("res.hi:%d res.lo:%lo\n", res.hi, res.lo));
  return res;
}

/* longlongmult_64bit */
static inline
longlongemul_t longlongmult(longlongemul_t a, longlongemul_t b)
/* benchmark runtime: 6.88 sec */
/* Also see http://www.8052.com/mul16.phtml

                               a4  a3  a2  a1     all 16 bit in unsigned _long_
                               b4  b3  b2  b1
                               -------------- x
                                       c2  c1     c=b1a1
                                   d2  d1         d=b1a2
                               e2  e1             e=b1a3
                           --  f1                 f=b1a4
                                   g2  g1         g=b2a1
                               h2  h1             h=b2a2
                           --  i1                 i=b2a3
                       --  --                     j=b2a4
                               k2  k1             k=b3a1
                           --  l1                 l=b3a2
                       --  --                     m=b3a3
                   --  --                         n=b3a4
                           --  o1                 o=b4a1
                       --  --                     p=b4a2
                   --  --                         q=b4a3
               --  --                             r=b4a4
               ------------------------------ +
               --  --  --  --  s4  s3  s2  s1

   To handle the carry, we have s1-4 as _longs_ containing the running
   sums. When finished, we do the carry stuff all at once.

   The 64-bit result is correct for all values.

                       ff ff   = -1
                       ff ff   = -1
                       ----- x
                       fe 01
                    -- 01
                    -- 01
                 -- --
                 ----------- +
                    01         carry
                       00 01   = 1
*/
{
  register unsigned long a4=((unsigned long) a.hi) >> 16;
  register unsigned long a3=((unsigned long) a.hi) & 0x0000ffff;
  register unsigned long a2=a.lo >> 16;
  register unsigned long a1=a.lo & 0x0000ffff;

  register unsigned long b4=((unsigned long) b.hi) >> 16;
  register unsigned long b3=((unsigned long) b.hi) & 0x0000ffff;
  register unsigned long b2=b.lo >> 16;
  register unsigned long b1=b.lo & 0x0000ffff;

  register unsigned long c;

  register unsigned long s4=0;
  register unsigned long s3=0;
  register unsigned long s2=0;
  register unsigned long s1=0;

  register longlongemul_t res;

  DPRINTF(("longlongmult(hi:%d lo:%d, hi:%d lo:%d)\t", a.hi,a.lo,b.hi,b.lo));

  /* c */
  c=b1*a1;
  s1+=c&0x0000ffff;
  s2+=c>>16;

  /* d */
  c=b1*a2;
  s2+=c&0x0000ffff;
  s3+=c>>16;

  /* e */
  c=b1*a3;
  s3+=c&0x0000ffff;
  s4+=c>>16;

  /* f */
  c=b1*a4;
  s4+=c&0x0000ffff;

  /* g */
  c=b2*a1;
  s2+=c&0x0000ffff;
  s3+=c>>16;

  /* h */
  c=b2*a2;
  s3+=c&0x0000ffff;
  s4+=c>>16;

  /* i */
  c=b2*a3;
  s4+=c&0x0000ffff;

  /* k */
  c=b3*a1;
  s3+=c&0x0000ffff;
  s4+=c>>16;

  /* l */
  c=b3*a2;
  s4+=c&0x0000ffff;

  /* o */
  c=b4*a1;
  s4+=c&0x0000ffff;

  /* carry */
  s2+=s1>>16;
  s3+=s2>>16;
  s4+=s3>>16;

  /* construct result */
  res.hi=(s4<<16)|(s3&0x0000ffff);
  res.lo=(s2<<16)|(s1&0x0000ffff);

  DPRINTF(("res.hi:%d res.lo:%lo\n", res.hi, res.lo));
  return res;
}

static inline
longlongemul_t longlongorulong(longlongemul_t a, unsigned long ul)
{
  longlongemul_t res = a;

  DPRINTF(("longlongorulong(hi:%d lo:%d, %d)\t",a.hi,a.lo,ul));

  a.lo |= ul;

  DPRINTF(("res.hi:%d res.lo:%lo\n", res.hi, res.lo));
  return res;
}

static inline
void longlong_dividehelp(longlongemul_t a, longlongemul_t divisorIn,
                          longlongemul_t * quotient,
                          longlongemul_t * remainder)
{
    /* init everything */
    longlongemul_t dividend = a;
    longlongemul_t divisor = divisorIn;

    /*
    // VZ: I'm writing this in a hurry and it's surely not the fastest way to
    //     do this - any improvements are more than welcome
    //
    //     code inspired by the snippet at
    //          http://www.bearcave.com/software/divide.htm
    //
    //     Copyright notice:
    //
    //     Use of this program, for any purpose, is granted the author, Ian
    //     Kaplan, as long as this copyright notice is included in the source
    //     code or any source code derived from this program. The user assumes
    //     all responsibility for using this code.

    // always do unsigned division and adjust the signs later: in C integer
    // division, the sign of the remainder is the same as the sign of the
    // dividend, while the sign of the quotient is the product of the signs of
    // the dividend and divisor. Of course, we also always have
    //
    //      dividend = quotient*divisor + remainder
    //
    // with 0 <= abs(remainder) < abs(divisor)
    */
    int negRemainder = dividend.hi < 0;
    int negQuotient = 0;   /* assume positive */

    if ((divisorIn.lo == 0) && (divisorIn.hi == 0))
    {
	/*
        // provoke division by zero error and silence the compilers warnings
        // about an expression without effect and unused variable
	*/
        long dummy = divisorIn.lo/divisorIn.hi;
        dummy += 0;
    }

    quotient->lo=0; quotient->hi=0;
    remainder->lo=0; remainder->hi=0;

    if ( dividend.hi < 0 )
    {
        negQuotient = !negQuotient;
        dividend = longlongminus(ulong2longlong(0), dividend);
    }
    if ( divisor.hi < 0 )
    {
        negQuotient = !negQuotient;
        divisor = longlongminus(ulong2longlong(0), divisor);
    }

    /* check for some particular cases */
    if ( longlonggreater(divisor,dividend) )
    {
        *remainder = dividend;
    }
    else if ( longlongequals(divisor,dividend) )
    {
        *quotient = ulong2longlong(1);
    }
    else
    {
        /* here: dividend > divisor and both are positibe: do unsigned division */
        long nBits = 64u;
        longlongemul_t d;
        long i;

        #define IS_MSB_SET(ll)  ((ll.hi) & (1 << (8*sizeof(long) - 1)))

        while ( longlongsmaller(*remainder , divisor) )
        {
            *remainder=longlongshiftleft(*remainder,1);
            if ( IS_MSB_SET(dividend) )
            {
                *remainder=longlongorulong(*remainder,1);
            }

            d = dividend;
            dividend=longlongshiftleft(dividend,1);

            nBits--;
        }

        /* undo the last loop iteration */
        dividend = d;
        *remainder=longlongshiftright(*remainder,1);
        nBits++;

        for ( i = 0; i < nBits; i++ )
        {
            longlongemul_t t;
            *remainder=longlongshiftleft(*remainder,1);
            if ( IS_MSB_SET(dividend) )
            {
                *remainder=longlongorulong(*remainder,1);
            }

            t = longlongminus(*remainder , divisor);
            dividend=longlongshiftleft(dividend,1);
            *quotient=longlongshiftleft(*quotient,1);
            if ( !IS_MSB_SET(t) )
            {
                *quotient=longlongorulong(*quotient,1);

                *remainder = t;
            }
        }
    }

    /* adjust signs */
    if ( negRemainder )
    {
        *remainder = longlongminus(ulong2longlong(0), *remainder);
    }

    if ( negQuotient )
    {
        *quotient = longlongminus(ulong2longlong(0), *quotient);
    }
}

static inline
longlongemul_t longlongdivide(longlongemul_t a, longlongemul_t b)
{
  longlongemul_t quotient;
  longlongemul_t remainder;

  DPRINTF(("longlongdivide(hi:%d lo:%d, hi:%d lo:%d)\t", a.hi,a.lo,b.hi,b.lo));

  longlong_dividehelp(a, b, &quotient, &remainder);

  DPRINTF(("quotient.hi:%d quotient.lo:%lo\n", quotient.hi, quotient.lo));
  return quotient;
}


#else /* no LONGLONGEMUL */

typedef long long longlongemul_t;

#define ulong2longlong(ul)	((long long) (ul))
#define long2longlong(sl)	((long long) (sl))
#define longlong2long(ll)	((long) (ll))
#define longlongplus(a,b)	((a) + (b))
#define longlongplusulong(a,b)	((a) + (b))
#define longlongminus(a,b)	((a) - (b))
#define longlongminusulong(a,b)	((a) - (b))
#define longlongshiftright(a,s) ((a) >> (s))
#define longlongshiftleft(a,s)  ((a) << (s))
#define longlongequals(a,b)	((a) == (b))
#define longlonggreater(a,b)	((a) > (b))
#define longlongsmaller(a,b)	((a) < (b))
#define longlongmult(a,b)	((a) * (b))
#define longlongdivide(a,b)	((a) / (b))
#define longlongorulong(a,ul)	((a) | (ul))

#endif /* no LONGLONGEMUL */

#endif /* HAVE_LONGLONG_H */
