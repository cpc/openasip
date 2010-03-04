/*
 * benchmark program:   dot_product.c
 * 
 * benchmark suite:     DSP-kernel
 * 
 * description:         dot product benchmarking
 *
 * This program performs a dot product of the form Z=AB,
 * where A is a [1x2] vector and B is a [2x1] vector.
 * 
 *          A[1 x 2] * B[2 x 1] = Z
 *                  
 * vector A[1 x 2]= |a1 a2|
 *
 * vector B[2 x 1]= | b1 |
 *                  | b2 |
 *
 * dot product Z = a1*b1 + a2* b2
 * 
 * vector elements are stored as
 *
 * A[1 x 2] = { a1, a1 }
 * 
 * B[2 x 1] = { b1, b2 }
 * 
 * 
 * reference code:       none
 * 
 * f. verification:      with printf function
 * 
 * organization:         Aachen University of Technology - IS2 
 *                       DSP Tools Group
 *                       phone:  +49(241)807887 
 *                       fax:    +49(241)8888195
 *                       e-mail: zivojnov@ert.rwth-aachen.de 
 *
 * author:              Juan Martinez Velarde
 * 
 * history:             10-05-94 C Code creation fixed-point (Martinez Velarde)
 *                      16-03-95 adaption for floating-point (Harald L. Schraut)
 *
 *                      $Author: schraut $
 *                      $Date: 1995/04/11 07:13:38 $
 *                      $Revision: 1.3 $
 */

#include "scheduler_tester_macros.h"

#define STORAGE_CLASS  register
#define TYPE           float

void pin_down(TYPE *Z)
{
  OUTPUT_VAR(*Z);
  *Z = 0 ; 
}

// do not let compiler make assumptions of input values
volatile TYPE init_A[2] = {2,1} ; 
volatile TYPE init_B[2] = {2,5} ;

int main()
{ 
  // allow register optimizations anyways
  static  TYPE A[2]; A[0] = init_A[0]; A[1] = init_A[1];
  static  TYPE B[2]; B[0] = init_B[0]; B[1] = init_B[1];
  static  TYPE Z    = 0   ;

  STORAGE_CLASS TYPE *p_a = &A[0] ;
  STORAGE_CLASS TYPE *p_b = &B[0] ;
  STORAGE_CLASS TYPE *p_z = &Z ;
  
  int f ; 

  pin_down(&Z) ; 
  
  START_PROFILING; 
  
  for (f=0;f<2;f++)
    *p_z  += *p_a++ * *p_b++ ;
  
  END_PROFILING; 
  
  pin_down(&Z) ; 
  
  return(0)  ; 
}
