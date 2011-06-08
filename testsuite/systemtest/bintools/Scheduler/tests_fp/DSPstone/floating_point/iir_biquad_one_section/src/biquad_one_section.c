/*
 *  benchmark program  : biquad_one_section.c
 *
 *  benchmark suite    : DSP-kernel
 *
 *  description        : benchmarking of an one iir biquad
 *                       
 *
 *	The equations of the filter are:
 *       w(n) =    x(n) - a1*w(n-1) - a2*w(n-2)
 *       y(n) = b0*w(n) + b1*w(n-1) + b2*w(n-2)
 *
 *		             w(n)
 *   x (n)------(-)---------->-|->----b0---(+)-------> y(n)
 *               A             |            A
 *               |           |1/z|          |
 *               |             | w(n-1)     |
 *               |             v            |
 *               |-<--a1-----<-|->----b1-->-|
 *               |             |            |
 *               |           |1/z|          |
 *               |             | w(n-2)     |
 *               |             v            |
 *               |-<--a2-----<--->----b2-->-|
 * 
 *     The values w(n-1) and w(n-2) are stored in w1 and w2
 * 
 *                                              
 *  reference code     
 *
 *  func. verification from separate computation
 *
 *  organization       Aachen University of Technology - IS2
 *                     DSP Tools Group
 *                     phone   : +49(241)807887
 *                     fax     : +49(241)8888195
 *                     e-mail  : zivojnov@ert.rwth-aachen.de
 *
 *  author             Juan Martinez Velarde
 *
 *  history            19-03-95 creation fixed-point (Martinez Velarde)
 *                     16-03-95 adaption floating-point (Harald L. Schraut)
 *
 *                     $Author: DSPstone $
 *                     $Date: 1995/03/16 21:41:48 $
 *                     $Revision: 1.1 $
 */



#include "scheduler_tester_macros.h"

#define STORAGE_CLASS register
#define TYPE float

volatile TYPE init_7 = 7;

TYPE 
pin_down(TYPE x)
{
  OUTPUT_VAR(x);
  return ((TYPE) init_7) ;
}


int main()
{
  
  STORAGE_CLASS TYPE y, w ; 

  static TYPE x = 7, w1 = 7, w2 = 7 ;  
  static TYPE b0 = 7, b1 = 7, b2 = 7 ;
  static TYPE a1 = 7, a2 = 7 ; 
  
  // make sure that there wont be unfair constand precalculations...
  x = pin_down(x) ; 
  w1= pin_down(w1) ;
  w2 = pin_down(w2) ;  
  b0 = pin_down(b0) ;
  b1 = pin_down(b1) ;
  b2 = pin_down(b2) ;
  a1 = pin_down(a1) ; 
  a2 = pin_down(a2) ; 

  START_PROFILING;
  
  w  = x - a1 * w1 ; 
  w -= a2 * w2 ; 
  y  = b0 * w ;
  y += b1 * w1 ; 
  y += b2 * w2 ; 
  
  w2 = w1 ; 
  w1 = w  ; 
  
  END_PROFILING;
  
  x  = pin_down(x) ;
  w1 = pin_down(w1) ; 
  w2 = pin_down(w2) ; 
  
  x = y;
  OUTPUT_VAR(x);
  return 0;
}
