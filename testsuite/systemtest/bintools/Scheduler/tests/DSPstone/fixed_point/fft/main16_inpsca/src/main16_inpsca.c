/*
 *  benchmark program  : fft.c (main program)
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
 *                      overflow.
 *
 *                      The number of guard bits necessary to compensate the 
 *                      maximum bit growth in an N-point FFT is (log_2 (N))+1).
 * 
 *                      In a 16-point FFT (requires 4 stages), each of the 
 *                      input samples must contain 5 guard bits. Indeed, the 
 *                      input data is restricted to 9 bits, in order to prevent
 *                      a overflow from the integer multiplication with the
 *                      7 bit precalculed twiddle coefficients.
 *                     
 *                      Input data is held on the include file "input16.dat"
 *                      in float format (0 ... 1)
 *                      Data is transformed automatically to 1.9 fract format
 *
 *  reference code     : none
 *
 *  func. verification : comparison with known float 16 point FFT
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

#define N_FFT 16
#define NUMBER_OF_BITS 9     /* fract format 1.NUMBER_OF_BITS = 1.9 */

#define BITS_PER_TWID 7      /* bits per twiddle coefficient */
#define SHIFT BITS_PER_TWID  /* fractional shift after each multiplication */

#include "twids16-7.dat"   /* precalculated twiddle factors 
                            for an integer 16 point FFT 
                            in format 1.7 => table twidtable[2*(N_FFT-1)] ; */

#include "input16.dat"    /* 16 real values as
                             input data in float format */

#include "convert.c"   /* conversion function to 1.NUMBER_OF_BITS format */

#include "fft_inpsca.c"

void
pin_down(input_data)
     TYPE input_data[] ; 
{
  
  /* conversion from input16.dat to a 1.9 format */
  
  float2fract() ; 
  
  {
    int          *pd, *ps, f  ;
    
    pd = &input_data[0];
    ps = &inputfract[0] ; 
    
    for (f = 0; f < N_FFT; f++) 
      {
	*pd++ = *ps++  ; /* fill in with real data */
	*pd++ = 0 ;      /* imaginary data is equal zero */
      }    
  }
}


TYPE 
main()
{
  int i;
  STORAGE_CLASS TYPE input_data[2*N_FFT];   
  pin_down(&input_data[0]) ; 
  
  fft_inpsca(&input_data[0]);
  
  for (i=0;i<2*N_FFT;i++) OUTPUT_VAR(input_data[i]);

  return (0) ; 
  
}

