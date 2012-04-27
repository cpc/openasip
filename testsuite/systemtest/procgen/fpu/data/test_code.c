
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
    
    
    float a, b, c, d;
    int i;
    unsigned int ui;

    union {
    int di;
    float df;
    } uni;
    
    // *
    // * Generic tests. These should succeed with f.ex. half-floats even if the others fail.
    // *
    //_TCE_STDOUT('\n'); 
    /*_TCE_STDOUT('1'); 
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
    
    _TCE_DIVF( a, b, c );
    i = (int)c;
    test( i == 24 );
    
    i = 13;
    a = (float)i;
    i = 24;
    b = (float)i;
    
    _TCE_ADDF( a, b, c );
    
    i = (int)c;
    
    test( i==37 );
    
    _TCE_MULF( a, b, c );
    
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
    _TCE_CFI( a, i );
    test( i == 12 );
    
    a = -12.f;
    _TCE_CFI( a, i );
    test( i == -12 );
    
    // Signed int-to-float conversion
    i=12;
    _TCE_CIF( i, c );
    test( c == 12.f );
    
    i=-12;
    _TCE_CIF( i, c );
    test( c == -12.f );
    
    // Unsigned int-to-float conversion
    ui = 0x80000000;
    _TCE_CIFU( ui, c );
    test( c == 2147483648.f );
    
    // Unsigned float-to-int conversion
    _TCE_CFIU( c, ui );
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
    _TCE_ADDF( a, b, c );
    test( c == 7.f );
    
    // Simple substraction
    _TCE_SUBF( b, a, c );
    test( c == 1.f );
    
    // Negative numbers
    b = -4.f;
    _TCE_ADDF( a, b, c );
    test( c == -1.f );
    
    _TCE_SUBF( a, b, c );
    test( c == 7.f );
    
    // Overflow should generate QNAN
    uni.di = MAXFLOAT_BITPATTERN;
    _TCE_ADDF( uni.df, uni.df, uni.df );
    test( uni.di == INF_BITPATTERN );
    
    // 
    uni.di = MAXFLOAT_BITPATTERN;
    _TCE_ADDF( uni.df, a, uni.df );
    test( uni.di == INF_BITPATTERN );
    
    // This produces a nonzero number with IEEE floats
    uni.di = MAXDENORMALFLOAT_BITPATTERN;
    _TCE_ADDF( uni.df, uni.df, uni.df );
    test( uni.df == 0.f );    
    
    // This also works
    uni.di = MINNORMALFLOAT_BITPATTERN;
    _TCE_ADDF( uni.df, uni.df, uni.df );
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
    _TCE_EQF( b, c, i );
    test( i );
    _TCE_EQF( a, b, i );
    test( !i );
    
    //NEF
    _TCE_NEF( a, b, i );
    test( i );
    _TCE_NEF( b, c, i );
    test( !i );
    
    //LTF
    _TCE_LTF( a, b, i );
    test( i );
    _TCE_LTF( b, a, i );
    test( !i );
    
    //GTF
    _TCE_GTF( b, a, i );
    test( i );
    _TCE_GTF( a, b, i );
    test( !i );
    
    //GEF
    _TCE_GEF( b, c, i );
    test( i );
    _TCE_GEF( a, c, i );
    test( !i );
    
    //LEF
    _TCE_LEF( b, c, i );
    test( i ); 
    _TCE_LEF( c, a, i );
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
    _TCE_MULF( a, b, c );
    test( c == 56088.f );
    
    a = 3.f;
    b = 4.f;
    _TCE_MULF( a, b, c );
    test( c == 12.f );
    
    b = -4.f;
    _TCE_MULF( a, b, c );
    test( c == -12.f );
    
    // Overflow should generate INF
    uni.di = MAXFLOAT_BITPATTERN;
    _TCE_MULF( uni.df, uni.df, uni.df );
    test( uni.di == INF_BITPATTERN );
    
    // Small-number behavior should be same as adder..
    uni.di = MAXDENORMALFLOAT_BITPATTERN;
    b=2.f;
    _TCE_MULF( uni.df, b, uni.df );
    test( uni.df == 0.f );    //!!
    
    uni.di = MINNORMALFLOAT_BITPATTERN;
    b=2.f;
    _TCE_MULF( uni.df, b, uni.df );
    test( uni.di == MINNORMALFLOATx2_BITPATTERN );
    
    // Denormal results should also work...
    uni.di = MINNORMALFLOAT_BITPATTERN;
    b=0.5f;
    _TCE_MULF( uni.df, b, uni.df );
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
    _TCE_DIVF( a, b, c );
    test( c == 325.f );
    
//#if 0
    // Simple division
    a = 21.f;
    b = 4.f;
    _TCE_DIVF( a, b, c );
    test( c == 5.25f );
    
    a = 12.f;
    b = 4.f;
    _TCE_DIVF( a, b, c );
    test( c == 3.f );
    
    b = -4.f;
    _TCE_DIVF( a, b, c );
    c = a/b;
    test( c == -3.f );
    
    // Overflow should generate INF
    uni.di = MAXFLOAT_BITPATTERN;
    b = 0.5f;
    _TCE_DIVF( uni.df, b, uni.df );
    test( uni.di == INF_BITPATTERN );
    
    b = 0.f;
    _TCE_DIVF( a, b, c );
    test( uni.di == INF_BITPATTERN );
    
    // Small-number behavior should be same as adder and multiplier..
    uni.di = MAXDENORMALFLOAT_BITPATTERN;
    b = 0.5f;
    _TCE_DIVF( uni.df, b, uni.df );
    test( uni.df == 0.f );    
    
    uni.di = MINNORMALFLOAT_BITPATTERN;
    _TCE_DIVF( uni.df, b, uni.df );
    test( uni.di == MINNORMALFLOATx2_BITPATTERN );
    
    uni.di = MINNORMALFLOAT_BITPATTERN;
    b = 2.f;
    _TCE_DIVF( uni.df, b, uni.df );
    test( uni.df == 0.f );
    
    
    // * 
    // * Multiply-accumulate tests
    // *
    */
    _TCE_STDOUT('\n'); 
    _TCE_STDOUT('9'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    float d_;
    
    a = 10.f; b = 20.f; c = 30.f;
    
    _TCE_MACF( a, b, c, d_ );
    
    test( d_ == 610.f );
    
    _TCE_MSUF( a, b, c, d_ );
    
    test( d_ == -590.f );
    
    a = 10.f, b = 10.f, c = 10.f;
    
    _TCE_MACF( a, b, c, d_ );
    
    test( d_ == 110.f );
    
    _TCE_MSUF( a, b, c, d_ );
    
    test( d_ == -90.f );
    
    
    
    _TCE_STDOUT('\n'); 
    
    return 0;
}
