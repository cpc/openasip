/*
 * benchmark program:   n_real_updates.c
 * 
 * benchmark suite:     DSP-kernel
 * 
 * description:         n_real_updates - filter benchmarking
 * 
 * This program performs n real updates of the form 
 *           D(i) = C(i) + A(i)*B(i),
 * where A(i), B(i), C(i) and D(i) are real numbers,
 * and i = 1,...,N
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
 * history:             25-05-94 creation fixed-point (Martinez Velarde)
 *                      16-03-95 adaption floating-point (Harald L. Schraut)
 *
 *                      $Author: schraut $
 *                      $Date: 1995/04/11 07:42:37 $
 *                      $Revision: 1.2 $
 */

#include "scheduler_tester_macros.h"

#define STORAGE_CLASS register
#define TYPE          float
#define N             16

volatile TYPE init_10 = 10, init_0 = 0, init_2 = 2;

void
pin_down(TYPE *pa, TYPE *pb, TYPE *pc, TYPE *pd)
{
  int i ; 

  for (i=0 ; i < N ; i++)
    {
      OUTPUT_VAR(*pa) ;
      *pa++ = init_10 ; 

      OUTPUT_VAR(*pb) ;
      *pb++ = init_2 ; 

      OUTPUT_VAR(*pc) ;
      *pc++ = init_10 ; 

      OUTPUT_VAR(*pd) ;
      *pd++ = init_0 ; 
    }
}

int main()
{
  static TYPE A[N], B[N], C[N], D[N] ; 
  
  STORAGE_CLASS TYPE *p_a = &A[0], *p_b = &B[0] ;
  STORAGE_CLASS TYPE *p_c = &C[0], *p_d = &D[0] ;
  int i ; 

  pin_down(&A[0], &B[0], &C[0], &D[0]) ; 
  
  START_PROFILING; 
	  
  for (i = 0 ; i < N ; i++) 
    *p_d++  = *p_c++ + *p_a++ * *p_b++ ;
  
  END_PROFILING;   
  
  pin_down(&A[0], &B[0], &C[0], &D[0]) ; 
  return(0)  ; 
}
