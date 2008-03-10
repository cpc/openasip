/* convert.c */

#include <math.h>

int convert(value)
     float value ; 
{
  double man, t_val, frac, m, exponent = NUMBER_OF_BITS;
  int rnd_val;
  unsigned long int_val;
  unsigned long pm_val;
  
#ifdef __DSP56156__
  m = pow(2,exponent+1) - 1 ;
#elif __DSP56000__ 
  m = pow(2,exponent+1) - 1 ; 
#elif __TMS320C50__
  m = pow(2,exponent+1) - 1 ; 
#elif __ADSP2101__
  m = pow(2,exponent+1) - 1 ; 
#else
  m = exp2(exponent+1)  - 1 ; 
#endif

  t_val = value * m ;
  frac = modf(t_val,&man);
  if (frac < 0) 
    {
      rnd_val = (-1);
      if (frac > -0.5) rnd_val = 0;
    }
  else 
    {
      rnd_val = 1;
      if (frac < 0.5) rnd_val = 0;
    }
  
  int_val = man + rnd_val;
  
  pm_val = int_val ; 
  return ((int) (pm_val)) ; 
  
}

void float2fract()
{
  float f ; 
  int   j, i ; 
  
  for (j = 0 ; j < N_FFT ; j++) 
    {
      f = input[j] ; 
      i = convert(f) ; 
      inputfract[j] = i ; 
    } 
}

