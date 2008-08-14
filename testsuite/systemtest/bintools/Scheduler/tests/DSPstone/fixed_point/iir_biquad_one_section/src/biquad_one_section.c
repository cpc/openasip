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
 *  reference code     : 
 *
 *  func. verification : from separate computation
 *
 *  organization       : Aachen University of Technology - IS2
 *                     : DSP Tools Group
 *                     : phone   : +49(241)807887
 *                     : fax     : +49(241)8888195
 *                     : e-mail  : zivojnov@ert.rwth-aachen.de
 *
 *  author             : Juan Martinez Velarde
 *
 *  history            : creation 19-3-1994
 *
 *                     $Author: schraut $
 *                     $Date: 1995/01/30 07:33:38 $
 *                     $Revision: 1.2 $
 */

#include "../../../tce_utils/tce_profiling.h"

#define STORAGE_CLASS register
#define TYPE int

TYPE _Output[3];

TYPE 
pin_down(TYPE x)
{
  return ((TYPE) 7) ;
}

TYPE x = 7, w1= 7 , w2 = 7 ;  
TYPE b0 = 7, b1 = 7 , b2 = 7  ;
TYPE a1 = 7, a2 = 7 ; 

TYPE main()
{
  
  STORAGE_CLASS TYPE y, w ; 

  START_PROFILING;
  
  w  = x - a1 * w1 ; 
  w -= a2 * w2 ; 
  y  = b0 * w ;
  y += b1 * w1 ; 
  y += b2 * w2 ; 
  
  w2 = w1 ; 
  w1 = w  ; 
  
  END_PROFILING;
  
  _Output[0] = y;
  _Output[1] = w2;
  _Output[2] = w1;

  x  = pin_down(x) ;
  w1 = pin_down(w1) ; 
  w2 = pin_down(w2) ; 

  return((TYPE) y) ; 
}
