/*
 * benchmark program:   complex_multiply.c
 * 
 * benchmark suite:     DSP-kernel
 * 
 * description:         complex_multiply - filter benchmarking
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
 * history:             09-05-94 creation fixed-point (Martinez Velarde)
 *                      22-03-95 adaption floating-point (Harald L. Schraut)
 * 
 *                      $Author: DSPstone $
 *                      $Revision: 1.1 $
 *                      $Date: 1995/03/22 14:15:54 $
 */

#include "scheduler_tester_macros.h"

#define STORAGE_CLASS static
#define TYPE          float

volatile TYPE init_ar = 2, init_ai = 1;
volatile TYPE init_br = 2, init_bi = 5;
volatile TYPE cr = 0, ci = 0;

void
pin_down(TYPE *ar, TYPE *ai, TYPE *br, TYPE *bi, TYPE *cr, TYPE *ci)
{
  *ar = init_ar ; 
  *ai = init_ai ; 
  *br = init_br ; 
  *bi = init_bi ;
  OUTPUT_VAR(*cr);
  OUTPUT_VAR(*ci);
}

int main()
{
  STORAGE_CLASS TYPE ar, ai ;
  STORAGE_CLASS TYPE br, bi ;

  pin_down(&ar, &ai, &br, &bi, &cr, &ci) ;
  
  START_PROFILING;
  
  cr  = ar*br - ai*bi ;
  ci  = ar*bi + ai*br ;

  END_PROFILING;

  pin_down(&ar, &ai, &br, &bi, &cr, &ci) ; 
  return 0;
}

