
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

inline float abs( float a ) { // TODO hardware instruction
  //if( a < 0 ) {
  //  return -a;
  //}
  //return a;
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
#define INF_BITPATTERN 0x7f800000
#define MINNORMALFLOAT_BITPATTERN 0x00800000
#define MINNORMALFLOATx2_BITPATTERN 0x01000000
#define MINNORMALFLOATdiv2_BITPATTERN 0x00400000
#define MAXDENORMALFLOAT_BITPATTERN 0x007fffff
#define MAXDENORMALFLOATx2_BITPATTERN 0x00fffffe
#define NAN_BITPATTERN 0x7f800001

int
main(void) {
    // *
    // * Generic tests. These should succeed with f.ex. half-floats even if the others fail.
    // *
    //_TCE_STDOUT('\n'); 
    _TCE_STDOUT('1'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    
    
    // Simple SQRT
    i=81;
    a = (float) i;
    _TCE_SQRTF( a, c );
    i = (int) c;
    test( i == 9 ); 
    
    
    i = 312;
    a = (float)i;
    i = 13;
    b = (float)i;
    
    c = a / b;
    i = (int)c;
    test( i == 24 );
    
    i = 13;
    a = (float)i;
    i = 24;
    b = (float)i;
    
    c = a + b;
    
    i = (int)c;
    
    test( i==37 );
    
    c = a * b;
    
    i = (int)c;
    test( i==312 );
    
    
    
    // *
    // * CIF and CFI converter tests
    // *
    _TCE_STDOUT('\n'); 
    _TCE_STDOUT('2'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    
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
    // TODO is this correct IEEE behavior?
    ui = (unsigned int)c;
    // Undefined!
    //test( ui == 0 );
    
    // *
    // * SQRTF tests
    // *
    _TCE_STDOUT('\n'); 
    _TCE_STDOUT('3'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    
    
    b=32084023.f;
    _TCE_SQRTF( b, c );
    test( abs( c - 5664.2760349403879f ) < 0.001 );
    
    // Simple SQRT
    b=90000.f;
    _TCE_SQRTF( b, c );
    test( abs( c - 300.f ) < 0.00001 ); 
    
    b=9.f;
    _TCE_SQRTF( b, c );
    test( abs( c - 3.f ) < 0.00001 ); 
    
    b=-1.f;
    _TCE_SQRTF( b, uni.df );
    test( uni.di == NAN_BITPATTERN );
    
    
    
    // *
    // * ABSF, NEGF tests
    // *
    _TCE_STDOUT('\n'); 
    _TCE_STDOUT('4'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    a=-2.f;
    b=2.f;
    
    //ABS
    test( abs( a ) == 2.f );
    
    test( abs( -a ) == 2.f );
    
    //NEG
    b = -a;
    test( b == 2.f );
    
    a = -b;
    test( a == -2.f );
    
    
    // *
    // * Adder tests
    // *
  
    //_TCE_STDOUT('\n'); 
    _TCE_STDOUT('\n'); 
    _TCE_STDOUT('5'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    
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
    test( uni.di == INF_BITPATTERN );
    
    // 
    uni.di = MAXFLOAT_BITPATTERN;
    uni.df = uni.df+a;
    test( uni.di == INF_BITPATTERN );
    
    // This produces a nonzero number with IEEE floats
    uni.di = MAXDENORMALFLOAT_BITPATTERN;
    uni.df = uni.df+uni.df;
    test( uni.df == 0.f );    
    
    // This also works
    uni.di = MINNORMALFLOAT_BITPATTERN;
    uni.df = uni.df+uni.df;
    test( uni.di == MINNORMALFLOATx2_BITPATTERN );    
    
    
    
    
    // *
    // * Comparator tests
    // *
    _TCE_STDOUT('\n'); 
    _TCE_STDOUT('6'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    
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
    
    a=2.f;
    b=4.f;    
//#endif
  
    
    
    // *
    // * Multiplier tests
    // *
    _TCE_STDOUT('\n'); 
    _TCE_STDOUT('7'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    
    // Simple multiplication
    a = 123.f;
    b = 456.f;
    c = a*b;
    test( c == 56088.f );
    
    a = 3.f;
    b = 4.f;
    c = a*b;
    test( c == 12.f );
    
    b = -4.f;
    c = a*b;
    test( c == -12.f );
    
    // Overflow should generate INF
    uni.di = MAXFLOAT_BITPATTERN;
    uni.df = uni.df * uni.df;
    test( uni.di == INF_BITPATTERN );
    
    // Small-number behavior should be same as adder..
    uni.di = MAXDENORMALFLOAT_BITPATTERN;
    uni.df = uni.df*2.f;
    test( uni.df == 0.f );    //!!
    
    uni.di = MINNORMALFLOAT_BITPATTERN;
    uni.df = uni.df*2.f;
    test( uni.di == MINNORMALFLOATx2_BITPATTERN );
    
    // Denormal results should also work...
    uni.di = MINNORMALFLOAT_BITPATTERN;
    uni.df = uni.df*0.5f;
    test( uni.df == 0.f );
  
    // *
    // * Divider tests
    // *
    _TCE_STDOUT('\n'); 
    _TCE_STDOUT('8'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    
    // Simple division
    a = 137475.f;
    b = 423.f;
    c = a/b;
    test( c == 325.f );
    
//#if 0
    // Simple division
    a = 21.f;
    b = 4.f;
    c = a/b;
    test( c == 5.25f );
    
    a = 12.f;
    b = 4.f;
    c = a/b;
    test( c == 3.f );
    
    b = -4.f;
    c = a/b;
    test( c == -3.f );
    
    // Overflow should generate INF
    uni.di = MAXFLOAT_BITPATTERN;
    uni.df = uni.df / 0.5f;
    test( uni.di == INF_BITPATTERN );
    
    b = 0.f;
    c = a/b;
    test( uni.di == INF_BITPATTERN );
    
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
    
    _TCE_STDOUT('\n'); 
    c=3;
    
    
    return 0;
}
