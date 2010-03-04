/*
 * benchmark program:   complex_update.c
 * 
 * benchmark suite:     DSP-kernel
 * 
 * description:         complex_update - filter benchmarking
 * 
 * This program performs a complex update of the form D = C + A*B,
 * where A, B, C and D are complex numbers .
 * 
 *          A = Ar + j Ai
 *          B = Br + j Bi
 *          C = Cr + j Ci
 *          D = C + A*B =   Dr  +  j Di
 *                      =>  Dr = Cr + Ar*Br - Ai*Bi
 *                      =>  Di = Ci + Ar*Bi + Ai*Br
 * 
 * reference code:      target assembly
 * 
 * f. verification:     simulator based
 * 
 *  organization:        Aachen University of Technology - IS2 
 *                       DSP Tools Group
 *                       phone:  +49(241)807887 
 *                       fax:    +49(241)8888195
 *                       e-mail: zivojnov@ert.rwth-aachen.de 
 *
 * author:              Juan Martinez Velarde
 * 
 * history:             11-05-94 creation for fixed-point (Martinez Velarde)
 *                      16-03-95 adaption for floating-point (Harald L. Schraut)
 *
 *                      $Author: DSPstone $
 *                      $Date: 1995/03/20 11:36:26 $
 *                      $Revision: 1.2 $ 
 */

#define STORAGE_CLASS register
#define TYPE  float

#include "scheduler_tester_macros.h"

void
pin_down(TYPE *p)
{
  OUTPUT_VAR(*p);
  *p++ = 0 ; 
  *p   = 0 ; 
}

volatile TYPE init_A[2] = { 2,1 } ;
volatile TYPE init_B[2] = { 2,5 } ;
volatile TYPE init_C[2] = { 3,4 } ; 

int main()
{
  // Load from memory but allow register optimize in between calculation
  static TYPE A[2]; A[0] = init_A[0]; A[1] = init_A[1];
  static TYPE B[2]; B[0] = init_B[0]; B[1] = init_B[1];
  static TYPE C[2]; C[0] = init_C[0]; C[1] = init_C[1];
  static TYPE D[2] = { 0,0 } ; 
  
  STORAGE_CLASS TYPE *p_a = &A[0] ;
  STORAGE_CLASS TYPE *p_b = &B[0] ;
  STORAGE_CLASS TYPE *p_c = &C[0] ;
  STORAGE_CLASS TYPE *p_d = &D[0] ;
  
  pin_down(&D[0]) ; 
  
  START_PROFILING;
	  
  *p_d    = *p_c++ + *p_a++ * *p_b++ ;
  *p_d++ -=          *p_a   * *p_b-- ; 

  *p_d    = *p_c   + *p_a-- * *p_b++ ; 
  *p_d   +=          *p_a   * *p_b ; 
   
  END_PROFILING;
  
  pin_down(&D[0]) ; 
  
  return(0)  ; 
}
