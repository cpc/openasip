/* 
 * Test code for double-precision operations/operands.
 */

//#include <stdio.h>
#include <math.h>
#include <string.h>
#include <lwpr.h>
#include "tceops.h"


// this really belongs in tce/bclib/include/lwpr.h
void lwpr_print_str(const char* str)
{
    lwpr_print_str(const_cast<char*>(str));
}


// check storing double data to DATA section
volatile double data64[16] = { 1.0, -2.0,  2.0/3.0, 10.0, 
                              11.0, 12.0, 12.0+5.0,  0.9, //tanh(0.9), 
                               1.2,  3.1, -4.0-2.0,  4.4, //atan2(-4.0, 3.0), 
                               2.3,  4.5,  1.1*4.9,  4.5};//pow(1.3, 4.5)};


// helper function until TUT comes back from Christmas...
//#define my_fabs(val)  (((val)>=0) ? (val) : (-(val)))
//inline double my_fabs(double val)
//{
//    if (val >= 0.0)
//        return val;
//    else
//        return -val;
//}

// helper function, check for approximate equality, update success
void check_approx_eql(int &succ, double desired, double actual, const char *msg, double tol=5e-15)
{
    // gentle variation from absolute tolerance at actual==0 to |actual|==1.0
    double abs_tol;
    if (fabs(actual) < 1.0) {
        // hybrid absolute/relative tolerance: absolute tolerance of tol/2 at
        // actual==0, increases quadratically to tol*actual (with the right
        // derivative) at |actual|==1.0
        abs_tol = 0.5*tol * (1.0 + actual * actual);
    } else {
        // typical relative error measure, i.e. the absolute tolerance is
        // proportional to the actual value
        abs_tol = fabs(tol * actual);
    }

    // tolerance test
    if (fabs(desired - actual) > abs_tol) {
        lwpr_print_str(msg);
        succ = 0;
    }
}


// main test function
int main()
{
    int succ = 1;
    double a, b;

    // quick, simple tests
    check_approx_eql(succ, 1.567, fabs(-1.567), "fabs (absd) failed");

    // test that memory stores are correct
    a = 2.0;
    b = 3.0;
    check_approx_eql(succ, a/b, data64[2], "stored divide result failed");
    a = 12.0;
    b = 5.0;
    check_approx_eql(succ, a+b, data64[6], "stored add result failed");
    a = -4.0;
    b = 2.0;
    check_approx_eql(succ, a-b, data64[10], "stored subtract result failed");
    a = 1.1;
    b = 4.9;
    check_approx_eql(succ, a*b, data64[14], "stored multiply result failed");

    a = data64[14];
    b = data64[15];
    
    a = 1.4;

    data64[2] = a + b;
    data64[3] = a - b;
    data64[4] = a * b;
    data64[5] = a / b;


    return !succ;
}


/* FPU opcode list:
 *   absd 
 *   negd
 *   addd
 *   subd
 *   muld
 *   divd
 *   squared
 *   sqrtd
 *   invsqrtd
 *   eqd   equd
 *   ged   geud
 *   gtd   gtud
 *   led   leud
 *   ltd   ltud
 *   ned   neud
 *   ordd  uordd
 *
 * FPU doesn't contain LDD/STD or MACD.  The LSDU handles LDD/STD.
 *
 * Found instruction MSUD, not in FPU, not sure what it does.  ..UD is probably
 * "unordered double".  Tried to add it anyway, ProDe says I have the wrong
 * number/type of operands or something like that.
 */
