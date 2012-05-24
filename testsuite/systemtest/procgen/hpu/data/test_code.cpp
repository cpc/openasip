#include <stdlib.h>
#include "tceops.h"

#define N 2


class half;
half mac( const half& a, const half& b, const half& c );

class half {
public:
    inline half() {
    };
    inline half( float a ) {
        _TCE_CFH( a, data );
    };
    inline half( double ad ) {
        float a = ad;
        _TCE_CFH( a, data );
    };
    inline half( const half& a ) {
        data=a.data;
    };
    inline void operator=( float a ) {
        _TCE_CFH( a, data );
    }   
    inline void operator=( double ad ) {
        float a = ad;
        _TCE_CFH( a, data );
    }   
    inline void operator=( const half& a ) {
        data = a.data;
    }   
    inline half operator+( const half& a ) const {
        half result;
        _TCE_ADDH( data, a.data, result.data );
        return result;
    }
    inline half operator-( const half& a ) const {
        half result;
        _TCE_SUBH( data, a.data, result.data );
        return result;
    }
    inline half operator*( const half& a ) const {
        half result;
        _TCE_MULH( data, a.data, result.data );
        return result;
    }
    inline half operator/( const half& a ) const {
        half result;
        _TCE_DIVH( data, a.data, result.data );
        return result;
    }
    
    //TODO should rewrite with just EQH and GTH?
    inline bool operator<( const half& a ) const { 
        bool result;
        _TCE_LTH( data, a.data, result );
        return result;
    }
    inline bool operator<=( const half& a ) const {
        bool result;
        _TCE_LEH( data, a.data, result );
        return result;
    }
    inline bool operator==( const half& a ) const {
        bool result;
        _TCE_EQH( data, a.data, result );
        return result;
    }
    inline bool operator>=( const half& a ) const {
        bool result;
        _TCE_GEH( data, a.data, result );
        return result;
    }
    inline bool operator>( const half& a ) const {
        bool result;
        _TCE_GTH( data, a.data, result );
        return result;
    }
    inline bool operator!=( const half& a ) const {
        bool result;
        _TCE_NEH( data, a.data, result );
        return result;
    }
    float to_float() const {
        float f;
        _TCE_CHF( data, f );
        return f;
    }
    
    inline half abs() const {
        half result;
        _TCE_ABSH( data, result.data );
        return result;
    }
    inline half operator-( ) const {
        half result;
        _TCE_NEGH( data, result.data );
        return result;
    }
    
    inline half invsqrt() const {
        half result;
        _TCE_INVSQRTH( data, result.data );
        return result;
    }
    
    inline half square() const {
        half result;
        //_TCE_SQUAREH( data, result.data );
        _TCE_MULH( data, data, result.data );
        return result;
    }
    
    friend half mac( const half& a, const half& b, const half& c );
    friend half msu( const half& a, const half& b, const half& c );
private:
    int data;
};

half mac( const half& a, const half& b, const half& c ) {
    half result;
    _TCE_MACH( a.data, b.data, c.data, result.data );
    return result;
}

half msu( const half& a, const half& b, const half& c ) {
    half result;
    _TCE_MSUH( a.data, b.data, c.data, result.data );
    return result;
}

inline void test( int a ) {
    if( a ) {
        _TCE_STDOUT('O'); 
    }
    else {
        _TCE_STDOUT('N'); 
    }
}



int
main() {
    
    // Absolute value, negation
    
    _TCE_STDOUT('1'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    
    test( half(63.f).abs() == half(63.f) );
    test( half(-63.f).abs() == half(63.f) );
    test( -half(36.f) == half(-36.f) );
    test( -half(-36.f) == half(36.f) );
    _TCE_STDOUT('\n'); 
    
    // Comparisons
    
    _TCE_STDOUT('2'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    test( half(1.f) != half(2.f) );
    test( half(1.f) != half(2.f) );
    test( !( half(1.f) == half(2.f) ) );
    test( half(2.f) == half(2.f) );
    test( !( half(2.f) != half(2.f) ) );
    test( half(1.f) < half(2.f) );
    test( !( half(2.f) < half(1.f) ) );
    test( !( half(2.f) < half(2.f) ) );
    test( half(1.f) <= half(2.f) );
    test( half(2.f) <= half(2.f) );
    test( !( half(2.f) <= half(1.f) ) );
    _TCE_STDOUT('\n'); 
    
    // Addition and subtraction
    
    _TCE_STDOUT('3'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    test( half(1.f) + half(1.f) == half(2.f) );
    test( half(1.f) + half(0.f) == half(1.f) );
    test( half(0.f) + half(0.f) == half(0.f) );
    test( half(235.f) + half(432.f) == half(667.f) );
    test( half(235.f) + half(-432.f) == half(-197.f) );
    
    test( half(1.f) - half(0.f) == half(1.f) );
    test( half(790.f) - half(123.f) == half(667.f) );
    _TCE_STDOUT('\n'); 
    
    // Multiplication and square
    
    _TCE_STDOUT('4'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    test( half(1334.f) * half(0.5f) == half(667.f) );
    test( half(11.f) * half(-12.f) == half(-132.f) );
    test( half(-0.75f) * half(-40.f) == half(30.f) );
    test( half(10.f).square() == half(100.f) );
    test( half(0.4f).square() == half(0.16f) );
    _TCE_STDOUT('\n');
    
    // Inverse square root
    
    _TCE_STDOUT('5'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    // Result is approximate even with simple outputs, so we use treshold
    test( ( half(0.25f).invsqrt() - half(2.f) ).abs() < 0.01 );
    test( ( half(100.f).invsqrt() - half(0.1f) ).abs() < 0.01 );
    _TCE_STDOUT('\n'); 
    
    // Multiply-accumulate
    
    _TCE_STDOUT('6'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    test( mac( half( 3.f ), half(3.f), half(3.f) ) == half(12.f) );
    test( mac( half( 3.f ), half(4.f), half(4.f) ) == half(19.f) );
    test( mac( half( 1.f ), half(2.f), half(3.f) ) == half(7.f) );
    test( msu( half( 3.f ), half(3.f), half(3.f) ) == half(-6.f) );
    test( msu( half( 3.f ), half(4.f), half(4.f) ) == half(-13.f) );
    test( msu( half( 1.f ), half(2.f), half(3.f) ) == half(-5.f) );
    test( mac( half( 32768.f ), half(0.125f), half(0.125f) ) == half(32768.f) );
    test( msu( half( 32768.f ), half(0.125f), half(0.125f) ) == half(32752.f) ); //Round to zero
    _TCE_STDOUT('\n'); 
    
    // Pipeline behavior
    // TODO mac
    
    _TCE_STDOUT('7'); 
    _TCE_STDOUT(':'); 
    _TCE_STDOUT(' '); 
    half imm1( 0.5f );
    half imm2( 0.25f );
    half imm3( 0.375f );
    half correct(1.f);
    half a( imm1 + imm1 );
    half b( imm2 + imm2 );
    half c( imm3 + imm3 );
    test( (a==half(1.f)) && (b==half(0.5f)) && (c==half(0.75f)) );
    
    imm1 = 0.5f;
    imm2 = 3.f;
    imm3 = 4.f;
    a = imm1 * imm1;
    b = imm2 * imm2;
    c = imm3 * imm3;
    test( (a==half(0.25f)) && (b==half(9.f)) && (c==half(16.f)) );
    
    // This isn't very comprehensive
    imm1 = half(0.25f);
    imm2 = half(4.f);
    a = imm1.invsqrt();
    b = imm2.invsqrt();
    c = imm1.invsqrt();
    half d = imm2.invsqrt();
    half e = imm1.invsqrt();
    test( (a==c) && (a==e) && (b==d) ); 
    _TCE_STDOUT('\n');
}
                         
