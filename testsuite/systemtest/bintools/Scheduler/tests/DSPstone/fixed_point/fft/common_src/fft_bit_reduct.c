/*
 *  benchmark program  : fft.c
 *                       fft_bit_reduct.c
 * 
 *  benchmark suite    : DSP-kernel
 *
 *  description        : benchmarking of an integer stage scaling FFT
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
 *                      (two bits of grow). However, a data value cannot grow
 *                      by the maximum amount in two consecutive stages. 
 *                      The number of guard bits necessary to compensate the 
 *                      maximum bit growth in an N-point FFT is (log_2 (N))+1).
 *  
 *                      In a 16-point FFT (requires 4 stages), each of the 
 *                      input samples should contain 5 guard bits. The input 
 *                      data is then restricted to 10 bits, one sign bit and
 *                      nine magnitude bits, in order to prevent an
 *                      overflow from the integer multiplication with the
 *                      precalculed twiddle coefficients.
 *      
 *                      Another method to compensate bit growth is to scale the
 *                      outputs down by a factor of two unconditionally after
 *                      each stage. This approach is called unconditional scaling
 *                      
 *                      Initially, 2 guard bits are included in the input data 
 *                      to accomodate the maximum overflow in the first stage.
 *                      In each butterfly of a stage calculation, the data can 
 *                      grow into the guard bits. To prevent overflow in the next 
 *                      stage, the guard bits are replaced before the next stage
 *                      is executed by shifting the entire block of data one bit
 *                      to the right.
 * 
 *                      Input data should not be restricted to a 1.9 format.
 *                      Input data can be represented in a 1.13 format,that is
 *                      14 significant bits, one sign and 13 magnitude bits. In 
 *                      the FFT calculation, the data loses a total of (lg2 N)-1 
 *                      bits because of shifting. Unconditional scaling results 
 *                      in the same number of bits lost as in the input data scaling.
 *                      However, it produces more precise results because the 
 *                      FFT starts with more precise input data. The tradeoff 
 *                      is a slower FFT calculation because of the extra cycles
 *                      needed to shift the output of each stage.
 *
 *                      Input data is held on the include file "input16.dat"
 *                      or "input1024.dat" in float format (0 ... 1)
 *                      Data is transformed automatically to 1.Q fract format
 *
 *  reference code     : none
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
 *                     $Date: 1995/01/31 08:12:42 $
 *                     $Revision: 1.3 $
 */

#include "../../../tce_utils/tce_profiling.h"

void
fft_bit_reduct(int_pointer)
     STORAGE_CLASS TYPE *int_pointer ;
{
  #ifdef __PROF56__
  t = __time;
  #endif
  START_PROFILING;  
  {
    STORAGE_CLASS TYPE i, j = 0  ; 
    STORAGE_CLASS TYPE tmpr, max = 2, m, n = N_FFT << 1 ;           ;
     
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
	  
	  /* implement unconditional bit reduction */
	  
	  {
	    register int f ; 	
	    
	    p = int_pointer ;     

	    for (f = 0 ; f < 2 * N_FFT; f++)
		*p++ = *p >> 1 ; 
		
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


