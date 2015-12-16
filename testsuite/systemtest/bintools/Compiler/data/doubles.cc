/* 
 * Test code for double-precision operations/operands.
 */

//#include <stdio.h>
#include <math.h>
#include <string.h>
#include "lwpr.h"
#include "tceops.h"


// check storing double data to DATA section
volatile double data64[16] = { 1.0, -2.0,  2.0/3.0, 10.0, 
                              11.0, 12.0, 12.0+5.0,  0.9, //tanh(0.9), 
                               1.2,  3.1, -4.0-2.0,  4.4, //atan2(-4.0, 3.0), 
                               2.3,  4.5,  1.1*4.9,  4.5};//pow(1.3, 4.5)};


// helper function until TUT comes back from Christmas...
#define my_fabs(val)  (((val)>=0) ? (val) : (-(val)))
//inline double my_fabs(double val)
//{
//    if (val >= 0.0)
//        return val;
//    else
//        return -val;
//}

// helper function, check for approximate equality, update success
void check_approx_eql(int &succ, double desired, double actual, const char *msg, double tol=1e-15)
{
    double norm = actual;
    double val_abs;

    // handle normalization, don't want to divide by zero
    //if (fabs(desired) < tol) { // newlib making a mess of this
    //if (desired < tol) { // newlib making a mess of this
    if (my_fabs(desired) < tol) { // newlib making a mess of this
        succ = 0;
    }
    //if (fabs(desired) < tol) { // newlib making a mess of this
    //_TCE_ABSD(desired, val_abs);
    //if (val_abs < tol) {
    //    norm = 1.0;
    //} 

//compiler:    // check for approximately equal within tolerance
//compiler:    //if (fabs(actual - desired)/actual > tol) {
//compiler:    _TCE_ABSD(actual - desired, val_abs);
//compiler:    if (val_abs / actual > tol) {
//compiler:        lwpr_print_str((char*)msg);
//compiler:        succ = 0;
//compiler:    }
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
