/*
 * benchmark program:   n_complex_updates.c
 * 
 * benchmark suite:     DSP-kernel
 * 
 * description:         n complex updates - filter benchmarking
 * 
 * This program performs n complex updates of the form 
 *           D(i) = C(i) + A(i)*B(i),
 * where A(i), B(i), C(i) and D(i) are complex numbers,
 * and i = 1,...,N
 * 
 *          A(i) = Ar(i) + j Ai(i)
 *          B(i) = Br(i) + j Bi(i)
 *          C(i) = Cr(i) + j Ci(i)
 *          D(i) = C(i) + A(i)*B(i) =   Dr(i)  +  j Di(i)
 *                      =>  Dr(i) = Cr(i) + Ar(i)*Br(i) - Ai(i)*Bi(i)
 *                      =>  Di(i) = Ci(i) + Ar(i)*Bi(i) + Ai(i)*Br(i)
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
 * history:             13-5-94 creation (Martinez Velarde)
 * 
 *                      $Author: schraut $
 *                      $Date: 1995/01/26 11:00:36 $
 *                      $Revision: 1.2 $
 */

#include "../../../tce_utils/tce_profiling.h"

#define STORAGE_CLASS register
#define TYPE  int
#define N             16

TYPE _Output[2*N];

void
pin_down(TYPE *pa, TYPE *pb, TYPE *pc, TYPE *pd)
{
  STORAGE_CLASS int i ; 

  for (i=0 ; i < N ; i++)
    {
      *pa++ = 2 ;
      *pa++ = 1 ;
      *pb++ = 2 ; 
      *pb++ = 5 ; 
      *pc++ = 3 ;
      *pc++ = 4 ;  
      *pd++ = 0 ; 
      *pd++ = 0 ; 
    }
}

TYPE A[2*N], B[2*N], C[2*N], D[2*N] ; 

int main(int argc,char **argv,char **envp)
{
  STORAGE_CLASS TYPE *p_a = &A[0], *p_b = &B[0] ;
  STORAGE_CLASS TYPE *p_c = &C[0], *p_d = &D[0] ;
  STORAGE_CLASS TYPE i ; 

  pin_down(&A[0], &B[0], &C[0], &D[0]) ; 
  
  START_PROFILING; 
	  
  for (i = 0 ; i < N ; i++, p_a++) 
    {
      *p_d    = *p_c++ + *p_a++ * *p_b++ ;
      *p_d++ -=          *p_a   * *p_b-- ; 
      
      *p_d    = *p_c++ + *p_a-- * *p_b++ ; 
      *p_d++ +=          *p_a++ * *p_b++ ;
    }
  
  END_PROFILING; 

  for (i=0 ; i < 2*N; i++ ) 
      _Output[i] = D[i];
      
  pin_down(&A[0], &B[0], &C[0], &D[0]) ; 
  
  return(0)  ; 
}
