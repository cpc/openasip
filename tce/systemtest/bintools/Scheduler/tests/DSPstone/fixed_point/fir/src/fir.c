/*
 * benchmark program:   fir.c
 * 
 * benchmark suite:     DSP-kernel
 * 
 * description:         fir - filter benchmarking
 * 
 * reference code:      target assembly
 * 
 * f. verification:     simulator
 * 
 *  organization:        Aachen University of Technology - IS2 
 *                       DSP Tools Group
 *                       phone:  +49(241)807887 
 *                       fax:    +49(241)8888195
 *                       e-mail: zivojnov@ert.rwth-aachen.de 
 *
 * author:              Juan Martinez Velarde
 * 
 * history:             12-4-94 creation (Martinez Velarde) 
 *                      22-1-95 START and END PROFILING Macros included
 *                              pin_down routine -> 3 arguments (Schraut)
 *
 *                      $Date: 1995/01/25 17:14:52 $
 *                      $Author: schraut $
 *                      $Revision: 1.2 $
 */

#include "../../../tce_utils/tce_profiling.h"

#define STORAGE_CLASS register
#define TYPE  int
#define LENGTH 16

TYPE _Output[LENGTH+1];

void
pin_down(TYPE * px, TYPE * ph, TYPE y)
{
  STORAGE_CLASS TYPE    i;
  
  for (i = 1; i <= LENGTH; i++) 
    {
      *px++ = i;
      *ph++ = i;
    }
  
}

TYPE  x[LENGTH];
TYPE  h[LENGTH];
TYPE  x0 = 100;

TYPE
main()
{
  STORAGE_CLASS TYPE i ;
  STORAGE_CLASS TYPE *px, *px2 ;
  STORAGE_CLASS TYPE *ph ;
  STORAGE_CLASS TYPE y;
  
  pin_down(x, h, y);
  
  ph  = &h[LENGTH-1] ; 
  px  = &x[LENGTH-1]  ; 
  px2 = &x[LENGTH-2]  ; 
  
  START_PROFILING ; 
  
  y = 0;

  for (i = 0; i < LENGTH - 1; i++)
    {       
      y += *ph-- * *px ; 
      *px-- = *px2-- ; 
    }
  
  y += *ph * *px ;
  *px = x0 ; 
  
  END_PROFILING ;  

  for( i = 0; i < LENGTH; i++ ) {
      _Output[i] = x[i];
  }
  _Output[LENGTH] = y;
  
  pin_down(x, h, y);

  return ((TYPE) y); 
}
