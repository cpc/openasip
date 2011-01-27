
#ifdef _DEBUG
#include <stdio.h>
#endif /* _DEBUG */

#include "tceops.h"

#include <math.h>

inline void test( int a ) {
  if( a ) {
    _TCE_STDOUT('O'); 
  }
  else {
    _TCE_STDOUT('N'); 
  }
}

inline float abs( float a ) {
  float returnval;
  _TCE_ABSF( a, returnval );
  return returnval;
}

volatile float a, b, c;
volatile int i;
volatile unsigned int ui;

static union {
  volatile int di;
  volatile float df;
} uni;

#define MAXFLOAT_BITPATTERN 0x7f7fffff
#define MINNORMALFLOAT_BITPATTERN 0x00800000
#define MINNORMALFLOATx2_BITPATTERN 0x01000000
#define MAXDENORMALFLOAT_BITPATTERN 0x007fffff
#define NAN_BITPATTERN 0x7f800001

int
main(void) {
	
    // *
    // * Adder tests
    // *
  
    // Simple addition
    a = 3.f;
    b = 4.f;
    c = a + b;
    test( c == 7.f );
    
    // Simple substraction
    c = b - a;
    test( c == 1.f );
    
    // Negative numbers
    b = -4.f;
    c = a + b;
    test( c == -1.f );
    
    c = a - b;
    test( c == 7.f );
    
    // Overflow should generate QNAN
    uni.di = MAXFLOAT_BITPATTERN;
    uni.df = uni.df + uni.df;
    test( uni.di == NAN_BITPATTERN );
    
    // This produces INF with IEEE compliant floats, but not with rounding mode round_to_zero
    uni.di = MAXFLOAT_BITPATTERN;
    uni.df = uni.df+a;
    test( uni.di == MAXFLOAT_BITPATTERN );    
    
    // This would produce nonzero with IEEE floats, but denormal operands are flushed to zero
    uni.di = MAXDENORMALFLOAT_BITPATTERN;
    uni.df = uni.df+uni.df;
    test( uni.df == 0.f );    
    
    // However, this works...
    uni.di = MINNORMALFLOAT_BITPATTERN;
    uni.df = uni.df+uni.df;
    test( uni.di == MINNORMALFLOATx2_BITPATTERN );    
    
    
    // *
    // * Multiplier tests
    // *
    
    // Simple multiplication
    a = 3.f;
    b = 4.f;
    c = a*b;
    test( c == 12.f );
    
    b = -4.f;
    c = a*b;
    test( c == -12.f );
    
    // Overflow should generate QNAN
    uni.di = MAXFLOAT_BITPATTERN;
    uni.df = uni.df * uni.df;
    test( uni.di == NAN_BITPATTERN );
    
    // Small-number behavior should be same as adder..
    uni.di = MAXDENORMALFLOAT_BITPATTERN;
    uni.df = uni.df*2.f;
    test( uni.df == 0.f );    
    
    uni.di = MINNORMALFLOAT_BITPATTERN;
    uni.df = uni.df*2.f;
    test( uni.di == MINNORMALFLOATx2_BITPATTERN );
    
    // Also, denormal results should be flushed to zero, even if operands were normal.
    uni.di = MINNORMALFLOAT_BITPATTERN;
    uni.df = uni.df*0.5f;
    test( uni.df == 0.f );
    
    // *
    // * Divider tests
    // *
    
    // Simple division
    a = 12.f;
    b = 4.f;
    c = a/b;
    test( c == 3.f );
    
    b = -4.f;
    c = a/b;
    test( c == -3.f );
    
    // Overflow should generate QNAN
    uni.di = MAXFLOAT_BITPATTERN;
    uni.df = uni.df / 0.5f;
    test( uni.di == NAN_BITPATTERN );
    
    b = 0.f;
    c = a/b;
    test( uni.di == NAN_BITPATTERN );
    
    // Small-number behavior should be same as adder and multiplier..
    uni.di = MAXDENORMALFLOAT_BITPATTERN;
    uni.df = uni.df/0.5f;
    test( uni.df == 0.f );    
    
    uni.di = MINNORMALFLOAT_BITPATTERN;
    uni.df = uni.df/0.5f;
    test( uni.di == MINNORMALFLOATx2_BITPATTERN );
    
    uni.di = MINNORMALFLOAT_BITPATTERN;
    uni.df = uni.df/2.f;
    test( uni.df == 0.f );
    
    
    // *
    // * SQRT tests
    // *
    
    // Simple SQRT
    b=4.f;
    _TCE_SQRTF( b, c );
    test( abs( c - 2.f ) < 0.000001 ); // TODO exact error bound from OpenCL standard
    
    
    _TCE_SQRTF( b, c );
    test( abs( c - 2.f ) < 0.000001 ); // TODO exact error bound from OpenCL standard
    
    
    // *
    // * CIF and CFI converter tests
    // *
    
    // Signed float-to-int conversion
    a = 12.f;
    i = (int)a;
    test( i == 12 );
    
    a = -12.f;
    i = (int)a;
    test( i == -12 );
    
    // Signed int-to-float conversion
    i=12;
    c = (float)i;
    test( c == 12.f );
    
    i=-12;
    c = (float)i;
    test( c == -12.f );
    
    // Unsigned int-to-float conversion
    ui = 0x80000000;
    c = (float)ui;
    test( c == 2147483648.f );
    
    // Unsigned float-to-int conversion
    ui = (unsigned int)c;
    test( ui == 0x80000000 );
    
    c = -2.f; // Negative float -> unsigned int = zero
    ui = (unsigned int)c;
    test( ui == 0 );
    
    
    
    // *
    // * Comparator tests
    // *
    
    a=2.f;
    b=4.f;
    c=4.f;
    
    //EQF
    i = (b == c);  // With the syntax test(b==c), the compiler would only use EQF and GEF in these tests.
    test( i );
    i = (a == b);
    test( !i );
    
    //NEQF
    i = (a != b);
    test( i );
    i = (b != c);
    test( !i );
    
    //LTF
    i = (a < b);
    test( i );
    i = (b < a);
    test( !i );
    
    //GTF
    i = (b > a);
    test( i );
    i = (a > b);
    test( !i );
    
    //GEF
    i = (b >= c);
    test( i );
    i = (a >= c);
    test( !i );
    
    //LEF
    i = (b <= c);
    test( i ); 
    i = (c <= a);
    test( !i ); 
    
    //ABS
    test( abs( -a ) == 2.f );
    
    //NEG
    b = -a;
    test( b == -2.f );
    
    // End output with '.' to show during debugging that there was enough runtime
    _TCE_STDOUT('.'); 
    
    return 0;
}
