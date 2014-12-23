#include <stdlib.h>
#include "tceops.h"

#define N 2


class half;
half mac( const half& a, const half& b, const half& c );

class half {
public:
    __attribute__((always_inline)) half() {
    };
    __attribute__((always_inline)) half( float a ) {
        _TCE_CFH( a, data );
    };
    __attribute__((always_inline)) half( double ad ) {
        float a = ad;
        _TCE_CFH( a, data );
    };
    __attribute__((always_inline)) half( const half& a ) {
        data=a.data;
    };
    __attribute__((always_inline)) void operator=( float a ) {
        _TCE_CFH( a, data );
    }   
    __attribute__((always_inline)) void operator=( double ad ) {
        float a = ad;
        _TCE_CFH( a, data );
    }   
    __attribute__((always_inline)) void operator=( const half& a ) {
        data = a.data;
    }   
    __attribute__((always_inline)) half operator+( const half& a ) const {
        half result;
        _TCE_ADDH( data, a.data, result.data );
        return result;
    }
    __attribute__((always_inline)) half operator-( const half& a ) const {
        half result;
        _TCE_SUBH( data, a.data, result.data );
        return result;
    }
    __attribute__((always_inline)) half operator*( const half& a ) const {
        half result;
        _TCE_MULH( data, a.data, result.data );
        return result;
    }
    __attribute__((always_inline)) half operator/( const half& a ) const {
        half result;
        _TCE_DIVH( data, a.data, result.data );
        return result;
    }
    
    //TODO should rewrite with just EQH and GTH?
    __attribute__((always_inline)) bool operator<( const half& a ) const { 
        bool result;
        _TCE_LTH( data, a.data, result );
        return result;
    }
    __attribute__((always_inline)) bool operator<=( const half& a ) const {
        bool result;
        _TCE_LEH( data, a.data, result );
        return result;
    }
    __attribute__((always_inline)) bool operator==( const half& a ) const {
        bool result;
        _TCE_EQH( data, a.data, result );
        return result;
    }
    __attribute__((always_inline)) bool operator>=( const half& a ) const {
        bool result;
        _TCE_GEH( data, a.data, result );
        return result;
    }
    __attribute__((always_inline)) bool operator>( const half& a ) const {
        bool result;
        _TCE_GTH( data, a.data, result );
        return result;
    }
    __attribute__((always_inline)) bool operator!=( const half& a ) const {
        bool result;
        _TCE_NEH( data, a.data, result );
        return result;
    }
    float to_float() const {
        float f;
        _TCE_CHF( data, f );
        return f;
    }
    
    __attribute__((always_inline)) half abs() const {
        half result;
        _TCE_ABSH( data, result.data );
        return result;
    }
    __attribute__((always_inline)) half operator-( ) const {
        half result;
        _TCE_NEGH( data, result.data );
        return result;
    }
    
    __attribute__((always_inline)) half invsqrt() const {
        half result;
        _TCE_INVSQRTH( data, result.data );
        return result;
    }
    
    __attribute__((always_inline)) half square() const {
        half result;
        //_TCE_SQUAREH( data, result.data );
        _TCE_MULH( data, data, result.data );
        return result;
    }
    
    friend half mac( const half& a, const half& b, const half& c );
    friend half msu( const half& a, const half& b, const half& c );
    int data;
private:
};

__attribute__((always_inline)) half mac( const half& a, const half& b, const half& c ) {
    half result;
    _TCE_MACH( a.data, b.data, c.data, result.data );
    return result;
}

__attribute__((always_inline)) half msu( const half& a, const half& b, const half& c ) {
    half result;
    _TCE_MSUH( a.data, b.data, c.data, result.data );
    return result;
}

__attribute__((always_inline)) void test( int a ) {
    if( a ) {
        _TCE_STDOUT('O'); 
    }
    else {
        _TCE_STDOUT('N'); 
    }
}


int pack( int hi, int lo ){
    return (hi<<16) | (lo&0xffff);
}
int unpackhi( int a ) {
    return (a>>16)&0xffff;
}
int unpacklo( int a ) {
    return a&0xffff;
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

    int a, b, c, d;
    half hi, lo;

    a = pack( half(1.f).data, half(2.f).data );
    b = pack( half(3.f).data, half(3.f).data );
    c = pack( half(4.f).data, half(5.f).data );
    _TCE_MAC2H( a,b,c,d );
    hi.data = unpackhi(d);
    lo.data = unpacklo(d);
    test( hi == half(13.f) );  
    test( lo == half(17.f) );  
    //test( lo == half(19.f) );  
    
    b = pack( half(3.f).data, half(3.f).data );
    c = pack( half(4.f).data, half(5.f).data );
    _TCE_ADD2H( b,c,d );
    hi.data = unpackhi(d);
    lo.data = unpacklo(d);
    test( (hi-half(7.f)).abs() < half(0.01f) );  
    test( (lo-half(8.f)).abs() < half(0.01f) );  
    
    b = pack( half(3.f).data, half(3.f).data );
    c = pack( half(4.f).data, half(5.f).data );
    _TCE_SUB2H( b,c,d );
    hi.data = unpackhi(d);
    lo.data = unpacklo(d);
    test( (hi-half(-1.f)).abs() < half(0.01f) );  
    test( (lo-half(-2.f)).abs() < half(0.01f) );  
    
    b = pack( half(3.f).data, half(3.f).data );
    c = pack( half(4.f).data, half(5.f).data );
    _TCE_MUL2H( b,c,d );
    hi.data = unpackhi(d);
    lo.data = unpacklo(d);
    test( (hi-half(12.f)).abs() < half(0.01f) );  
    test( (lo-half(15.f)).abs() < half(0.01f) );  
    
    b = pack( half(2.f).data, half(1.f).data ); // (1,2)
    c = pack( half(4.f).data, half(3.f).data ); // (3,4)
    _TCE_CMUL2H( b,c,d );
    hi.data = unpackhi(d);
    lo.data = unpacklo(d);
    test( (lo-half(-5.f)).abs() < half(0.01f) );  
    test( (hi-half(10.f)).abs() < half(0.01f) );  
    
    a = pack( half(1.f).data, half(1.f).data );
    b = pack( half(2.f).data, half(1.f).data ); // (1,2)
    c = pack( half(4.f).data, half(3.f).data ); // (3,4)
    _TCE_CMAC2H( a,b,c,d );
    hi.data = unpackhi(d);
    lo.data = unpacklo(d);
    test( (lo-half(-4.f)).abs() < half(0.01f) );  
    test( (hi-half(11.f)).abs() < half(0.01f) );  
    

    _TCE_STDOUT('\n');
}
                         
