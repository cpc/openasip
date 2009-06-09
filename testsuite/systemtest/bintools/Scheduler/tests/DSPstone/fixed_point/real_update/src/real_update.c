/*
 * benchmark program:   real_update.c
 * 
 * benchmark suite:     DSP-kernel
 * 
 * description:         real_update - filter benchmarking
 * 
 * This program performs a real update of the form D = C + A*B,
 * where A, B, C and D are real numbers .
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
 * history:             11-05-94 creation (Martinez Velarde)
 *
 *                      $Author: schraut $
 *                      $Date: 1995/01/30 10:06:02 $
 *                      $Revision: 1.3 $
 */

#include "scheduler_tester_macros.h"

#define STORAGE_CLASS register
#define TYPE  int

volatile TYPE init_0 = 0, init_1 = 1, init_2 = 2, init_10 = 10;

void
pin_down(TYPE *p)
{
  OUTPUT_VAR(*p);
  *p = init_0 ; 
}


TYPE
main()
{
  static TYPE A; A = init_10 ;
  static TYPE B; B = init_2 ;
  static TYPE C; C = init_1 ; 
  static TYPE D; D = init_0 ; 
  
  STORAGE_CLASS TYPE *p_a = &A ;
  STORAGE_CLASS TYPE *p_b = &B ;
  STORAGE_CLASS TYPE *p_c = &C ;
  STORAGE_CLASS TYPE *p_d = &D ;
  
  pin_down(&D) ; 
  
  START_PROFILING; 
	  
  *p_d  = *p_c + *p_a * *p_b ;
   
  END_PROFILING; 
  
  pin_down(&D) ; 
  
  return(0)  ; 
}
