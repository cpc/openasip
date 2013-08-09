/*
 * benchmark program:   fir2dim.c
 * 
 * benchmark suite:     DSP-kernel
 * 
 * description:         fir2dim - filter benchmarking
 *
 * The image is an array IMAGEDIM * IMAGEDIM pixels. To provide 
 * conditions for the FIR filtering, the image is surrounded by a
 * set of zeros such that the image is actually stored as a 
 * ARRAYDIM * ARRAYDIM = (IMAGEDIM + 2) * (IMAGEDIM + 2) array
 *
 *          <--ARRAYDIM-->  
 *         |0 0 0 .... 0 0| A
 *         |0 x x .... x 0| |
 *         |0 x x .... x 0| ARRAY_
 *         |0 image area 0| DIM
 *         |0 x x .... x 0| |
 *         |0 x x .... x 0| |
 *         |0 0 0 .... 0 0| V
 * 
 * The image (with boundary) is stored in row major storage. The
 * first element is array(1,1) followed by array(1,2). The last
 * element of the first row is array(1,514) following by the 
 * beginning of the next column array(2,1).
 *
 * The two dimensional FIR uses a 3x3 coefficient mask:
 *
 *         |c11 c12 c13|
 *         |c21 c22 c23|
 *         |c31 c32 c33|
 *
 * The output image is of dimension IMAGEDIM * IMAGEDIM.
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
 * history:             15-05-94 creation fixed-point (Martinez Velarde) 
 *                      16-03-95 adaption floating-point (Harald L. Schraut)
 *
 *                      $Author: schraut $
 *                      $Date: 1995/04/12 06:23:50 $
 *                      $Revision: 1.3 $ 
 */

#include "scheduler_tester_macros.h"

#define STORAGE_CLASS register
#define TYPE          float
#define IMAGEDIM      4
#define ARRAYDIM      (IMAGEDIM + 2)
#define COEFFICIENTS  3

volatile TYPE init_1 = 1, init_0 = 0;

void
pin_down(TYPE *pimage, TYPE *parray, TYPE *pcoeff, TYPE *poutput)
{
    int i,f;
  
    for (i = 0 ; i < IMAGEDIM ; i++)
    {
        for (f = 0 ; f < IMAGEDIM ; f++) {
            OUTPUT_VAR(*pimage);
            *pimage++ = init_1 ; 
        }
    }

    pimage = pimage - IMAGEDIM*IMAGEDIM  ; 

    for (i = 0; i < COEFFICIENTS*COEFFICIENTS; i++) {
        OUTPUT_VAR(*pcoeff);
        *pcoeff++ = init_1;
    }

    for (i = 0 ; i < ARRAYDIM ; i++) {
        OUTPUT_VAR(*parray);
        *parray++ = init_0 ; 
    }
  
  
    for (f = 0 ; f < IMAGEDIM; f++)
    {      
        OUTPUT_VAR(*parray);
        *parray++ = init_0 ; 
        for (i = 0 ; i < IMAGEDIM ; i++) {
            OUTPUT_VAR(*parray);
            *parray++ = *pimage++ ; 
        }
        OUTPUT_VAR(*parray);
        *parray++ = init_0 ;       
    }
  
    for (i = 0 ; i < ARRAYDIM ; i++) {
        OUTPUT_VAR(*parray) ;
        *parray++ = init_0 ; 
    }
  
    for (i = 0 ; i < IMAGEDIM * IMAGEDIM; i++) {
        OUTPUT_VAR(*poutput);
        *poutput++ = init_0 ; 
    }
}


int main()
{

  static TYPE  coefficients[COEFFICIENTS*COEFFICIENTS] ; 
  static TYPE  image[IMAGEDIM*IMAGEDIM]  ;
  static TYPE  array[ARRAYDIM*ARRAYDIM]  ;
  static TYPE  output[IMAGEDIM*IMAGEDIM] ; 

  STORAGE_CLASS TYPE *pimage  = &image[0]        ;
  STORAGE_CLASS TYPE *parray  = &array[0], *parray2, *parray3 ; 
  STORAGE_CLASS TYPE *pcoeff  = &coefficients[0] ;
  STORAGE_CLASS TYPE *poutput = &output[0]       ;  
  int k, f, i;
    
  pin_down(&image[0], &array[0], &coefficients[0], &output[0]);

  // for some reason pointers are assigned again...
  pimage  = &image[0]        ; 
  parray  = &array[0]        ; 
  pcoeff  = &coefficients[0] ; 
  poutput = &output[0]       ; 

  START_PROFILING; 
  
  for (k = 0 ; k < IMAGEDIM ; k++)
    {
      
      for (f = 0 ; f < IMAGEDIM ; f++)
	{
	  pcoeff = &coefficients[0] ; 
	  parray = &array[k*ARRAYDIM + f] ; 
	  parray2 = parray + ARRAYDIM ; 
	  parray3 = parray + ARRAYDIM + ARRAYDIM ; 
	  
	  *poutput = 0 ; 

      	  for (i = 0 ; i < 3 ; i++)
	    *poutput += *pcoeff++ * *parray++ ; 
	  
	  for (i = 0 ; i < 3 ; i++)
	    *poutput += *pcoeff++ * *parray2++ ; 
	  
	  for (i = 0 ; i < 3 ; i++)
	    *poutput += *pcoeff++ * *parray3++ ; 
	  
	  poutput++ ; 
	}
    }

  END_PROFILING;  
  
  pin_down(&image[0], &array[0], &coefficients[0], &output[0]);
  return 0;
} 
     
