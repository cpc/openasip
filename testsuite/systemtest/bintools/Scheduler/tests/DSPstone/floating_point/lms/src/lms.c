/*
 * benchmark program:   lms.c
 * 
 * benchmark suite:     DSP-kernel
 * 
 * description:         lms - filter benchmarking
 *
 *  
 *               x(n-0)  ---- x(n-1)  ----- x(n-i) -----  x(n-N+1)
 *    x(n) --->---------| 1/z|-------| 1/z|-.....-| 1/z|----
 *                  |   -----    |   ------   |   ------   |
 *                  h0           h1           hi          hN-1
 *                    \________  |____   _____|  _________/
 *                             \______(+)_______/
 *                                     |
 *                                     |---------> y(n)
 *                                     v
 *          d(n) -------------------->(+)--------> e(n)
 *                                   -
 * 
 * 
 * Notation and symbols:
 * x(n)  - Input sample at time n.
 * d(n)  - Desired signal at time n.
 * y(n)  - FIR filter output at time n.
 * H(n)  - Filter coefficient vector at time n.  H={h0,h1,..,hN-1}
 * X(n)  - Filter state variable vector at time N.
 *         X={x(0),x(n-1),...,x(n-N+1)}
 * delta - Adaptation gain.
 * N     - Number of coefficient taps in the filter. 
 * 
 * PROCESSING:
 *
 *     True LMS Algorithm       
 *     ------------------       
 *     Get input sample         
 *     Save input sample        
 *     Do FIR                   
 *     Shift vector X           
 *     Get d(n), calculate e(n)      
 *     Update coefficients      
 *     Output y(n)              
 *
 *
 * System equations:
 * 
 * e(n)  = d(n) - H(n)  X(n)       (FIR filter and error)
 * H(n+1)= H(n) + delta X(n) e(n)  (Coefficient update)
 * 
 * References:
 *  "Adaptive Digital Filters and Signal Analysis", Maurice G. Bellanger
 *     Marcel Dekker, Inc. New York and Basel
 *  
 *  "The DLMS Algorithm Suitable for the Pipelined Realization of Adaptive
 *      Filters", Proc. IEEE ASSP Workshop, Academia Sinica, Beijing, 1986
 *
 *
 * 
 * reference code:      target assembly
 * 
 * f. verification:     none
 * 
 *  organization:        Aachen University of Technology - IS2 
 *                       DSP Tools Group
 *                       phone:  +49(241)807887 
 *                       fax:    +49(241)8888195
 *                       e-mail: zivojnov@ert.rwth-aachen.de 
 *
 * author:              Juan Martinez Velarde
 * 
 * history:             29-3-94 creation (Martinez Velarde)
 *                      03-4-94 first revision, optimized (Martinez Velarde)
 *
 *                      $Author: DSPstone $
 *                      $Date: 1995/04/11 07:33:10 $
 *                      $Revision: 1.2 $  
 */

#include "scheduler_tester_macros.h"

#define STORAGE_CLASS  register
#define TYPE           float

#define N 16  /* number of coefficient taps */

volatile TYPE init_7 = 7, init_8 = 8, init_1 = 1;

void
pin_down(TYPE *d, TYPE *x, TYPE *delta, TYPE *p_H, TYPE *p_X)
{
  int f ; 

  OUTPUT_VAR(*d);
  *d = init_7 ; 

  OUTPUT_VAR(*x);
  *x = init_8 ;

  OUTPUT_VAR(*delta);
  *delta = init_1 ; 
  
  for (f = 0 ; f < N ; f++) 
    {
      OUTPUT_VAR(*p_H);
      *p_H++ = init_1 ; 

      OUTPUT_VAR(*p_X);
      *p_X++ = init_1 ; 
    }

}

int main()
{ 
  static TYPE H[N] ; /* Filter Coefficient Vector */
  static TYPE X[N] ; /* Filter State Variable Vector */
  
  TYPE delta = 0;  /* Adaption Gain */
  TYPE d = 0;       /* Desired signal */
  TYPE x = 0;       /* Input Sample */
  TYPE y = 0;       /* FIR LMS Filter Output */
  STORAGE_CLASS TYPE error ;   /* FIR error */
  
  int f ; 
  
  STORAGE_CLASS TYPE *p_H, *p_X, *p_X2 ; 
  
  pin_down(&d,&x,&delta,&H[0],&X[0]) ;
  
  p_H = &H[N-1] ; 
  p_X = &X[N-1] ; 
  p_X2= &X[N-2] ; 
  
  START_PROFILING; 
  
  y = 0 ; 
  
  /* FIR filtering and State Variable Update */
  
  for (f = 1 ; f < N  ; f++)
    y += *p_H-- * (*p_X-- = *p_X2--) ;  
  
  /* last convolution tap, get input sample */
  
  y += *p_H * (*p_X = x)  ;      
  
  /* 
   *  error as the weighted difference 
   *  between desired and calculated signal 
   *
   */
  
  error = (d - y) * delta ; 
  
  for (f =  0; f < N ; f++) 
    *p_H++ += error * *p_X++ ;   /* update the coefficients */ 
  
  END_PROFILING; 
  
  pin_down(&d,&x,&y,&H[0],&X[0]) ; 
  
  return(0)  ; 
  
}

