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
 * history:             11-5-94 creation (Martinez Velarde)
 *
 *                      $Author: schraut $
 *                      $Date: 1995/01/26 10:27:31 $
 *                      $Revision: 1.2 $ 
 */

#include "../../../tce_utils/tce_profiling.h"

#define STORAGE_CLASS register
#define TYPE  int

TYPE _Output[2];

TYPE A[2] = { 2,1 } ;
TYPE B[2] = { 2,5 } ;
TYPE C[2] = { 3,4 } ; 
TYPE D[2] = { 0,0 } ; 
  
void
pin_down(TYPE *p)
{
   
  *p++ = 0 ; 
  *p   = 0 ; 

}


TYPE
main()
{
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
  
  _Output[0] = D[0];
  _Output[1] = D[1];

  pin_down(&D[0]) ; 
  
  return(0)  ; 
}
