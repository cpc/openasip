/*
 *  benchmark program  : fft.c (main program)
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
 *                      (two bits of grow). However, a data value cannot grow by
 *                      maximum amount in two consecutive stages. 
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
 *                      Initially, 2 guard bits are included in the input data to 
 *                      accomodate the maximum overflow in the first stage.
 *                      In each butterfly of a stage calculation, the data can 
 *                      grow into the guard bits. To prevent overflow in the next 
 *                      stage, the guard bits are replaced before the next stage is
 *                      executed by shifting the entire block of data one bit
 *                      to the right.
 * 
 *                      Input data should not be restricted to a 1.9 format.
 *                      Input data can be represented in a 1.13 format,that is
 *                      14 significant bits, one sign and 13 magnitude bits. In 
 *                      the FFT calculation, the data loses a total of (log2 N) -1 
 *                      bits because of shifting. Unconditional scaling results 
 *                      in the same number of bits lost as in the input data scaling.
 *                      However, it produces more precise results because the 
 *                      FFT starts with more precise input data. The tradeoff is
 *                      a slower FFT calculation because of the extra cycles needed
 *                      to shift the output of each stage.
 *
 *                      Input data is held on the include file "input16.dat"
 *                      or "input1024.dat" in float format (0 ... 1)
 *                      Data is transformed automatically to 1.Q fract format
 *
 *  reference code     : none
 *
 *  func. verification : comparison with known float 1024 point FFT
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
 *                       16-02-94 - c50 profiling
 *
 */

#include "scheduler_tester_macros.h"

/* gcc did not like register because address references */
// #define STORAGE_CLASS register
#define STORAGE_CLASS 
#define TYPE int

#define N_FFT 1024
#define NUMBER_OF_BITS 13    /* fract format 1.NUMBER_OF_BITS = 1.13 */

#define BITS_PER_TWID 13      /* bits per twiddle coefficient */
#define SHIFT BITS_PER_TWID  /* fractional shift after each multiplication */

#include "twids1024-13.dat" /* precalculated twiddle factors 
                            for an integer 1024 point FFT 
                            in format 1.13 => table twidtable[2*(N_FFT-1)] ; */

#include "input1024.dat"   /* 1024 real values as
                            input data in float format */

#include "convert.c"   /* conversion function to 1.NUMBER_OF_BITS format */

#include "fft_bit_reduct.c"

void
pin_down(input_data)
     TYPE input_data[] ; 
{
  /* conversion from input1024.dat to a 1.13 format */
  
  float2fract() ; 
  
  {
    int           *pd, *ps, f; 
        
    pd = &input_data[0];
    ps = &inputfract[0] ; 
    
    for (f = 0; f < N_FFT; f++) 
      {
	*pd++ = *ps++  ; /* fill in with real data */
	*pd++ = 0 ;      /* imaginary data is equal zero */
      }    
  }
}


int main()
{
  int i;
  STORAGE_CLASS TYPE input_data[2*N_FFT];   
  pin_down(&input_data[0]) ; 
  
  fft_bit_reduct(&input_data[0]);  

  for (i=0;i<2*N_FFT;i++) OUTPUT_VAR(input_data[i]);

  return (0) ; 
  
}

