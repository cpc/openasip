/*
 *  benchmark program  : fft.c
 *                       fft_inpsca.c
 * 
 *  benchmark suite    : DSP-kernel
 *
 *  description        : benchmarking of an integer input scaled FFT
 *
 *                      To avoid errors caused by overflow and bit growth, 
 *                      the input data is scaled. Bit growth occurs potentially
 *                      at butterfly operations, which involve a complex 
 *                      multiplication, a complex addition and a complex 
 *                      subtraction. Maximal bit growth from butterfly input 
 *                      to butterfly output is two bits. 
 *
 *                      The input data includes enough extra sign bits, called 
 *                      guard bits, to ensure that bit growth never results in 
 *                      overflow (Rabiner and Gold, 1975). Data can grow by a
 *                      maximum factor of 2.4 from butterfly input to output
 *                      (two bits of grow). However, a data value cannot grow by
 *                      maximum amount in two consecutive stages. 
 *                      The number of guard bits necessary to compensate the 
 *                      maximum bit growth in an N-point FFT is (log_2 (N))+1).
 *  
 *                      In a 16-point FFT (requires 4 stages), each of the 
 *                      input samples must contain 5 guard bits. Indeed, the 
 *                      input data is restricted to 10 bits, one sign bit and
 *                      nine magnitude bits, in order to prevent an
 *                      overflow from the integer multiplication with the
 *                      precalculed twiddle coefficients.
 *                     
 *                      Input data is held on the include file "input16.dat"
 *                      or "input1024.dat" in float format (0 ... 1)
 *                      Data is transformed automatically to 1.Q fract format
 *
 *  reference code     : c5cx0016.asm in arc_16.asm, arc_1024.asm
 *
 *  func. verification : comparison with known float N point FFT
 *
 *  organization       : Aachen University of Technology - IS2
 *                     : DSP Tools Group
 *                     : phone   : +49(241)807887
 *                     : fax     : +49(241)8888195
 *                     : e-mail  : zivojnov@ert.rwth-aachen.de
 *
 *  author             : Juan Martinez Velarde
 *
 *  history            : 07-02-94 - creation
 *                       
 *                     $Author: schraut $
 *                     $Date: 1995/01/31 08:20:51 $
 *                     $Revision: 1.3 $             
 */

#include "../../../../tce_utils/tce_profiling.h"

void
fft_inpsca(int_pointer)
    
    STORAGE_CLASS TYPE *int_pointer ;
{
  #ifdef __PROF56__
  t = __time;
  #endif
  
  START_PROFILING;
  
  {
    STORAGE_CLASS TYPE i, j = 0  ; 
    STORAGE_CLASS TYPE tmpr, max = 2, m, n = N_FFT << 1 ; 
    
    /* do the bit reversal scramble of the input data */
    
    for (i = 0; i < (n-1) ; i += 2) 
      {
	if (j > i)
	  {
	    tmpr = *(int_pointer + j) ;
	    *(int_pointer + j) = *(int_pointer + i) ;
	    *(int_pointer + i) = tmpr ; 
	    
	    tmpr = *(int_pointer + j + 1) ; 
	    *(int_pointer + j + 1) = *(int_pointer + i + 1) ; 
	    *(int_pointer + i + 1) = tmpr ; 
	  }
	
	m = N_FFT;
	while (m >= 2 && j >= m) 
	  {
	    j -= m ;
	    m >>= 1;
	  }
	j += m ;
      }
    
    {
      STORAGE_CLASS TYPE *data_pointer = &twidtable[0] ; 
      STORAGE_CLASS TYPE *p, *q ; 
      STORAGE_CLASS TYPE tmpi, fr = 0, level, k, l ; 
      
      while (n > max)
	{      
	  level = max << 1 ;
	  for (m = 1; m < max; m += 2) 
	    {
	      l = *(data_pointer + fr) ; 
	      k = *(data_pointer + fr + 1) ;
	      fr += 2 ; 
	      
	      for (i = m; i <= n; i += level) 
		{
		  j = i + max;
		  p = int_pointer + j ; 
		  q = int_pointer + i ; 
		  
		  tmpr  = l * *(p-1) ; 
		  tmpr -= (k * *p ) ; 
		  
		  tmpi  = l * *p  ; 
		  tmpi += (k * *(p-1)) ; 
		  
		  tmpr  = tmpr >> SHIFT ; 
		  tmpi  = tmpi >> SHIFT ; 
		  
		  *(p-1) = *(q-1) - tmpr ; 
		  *p     = *q - tmpi ; 
		  
		  *(q-1) += tmpr ;
		  *q     += tmpi ; 
		}
	    }      
	  max = level;
	}
    }
    
  }
  
  END_PROFILING;    
  
  #ifdef __PROF56__
  t = __time - t;
  printf("time = %d\n", t);
  #endif
  
}



