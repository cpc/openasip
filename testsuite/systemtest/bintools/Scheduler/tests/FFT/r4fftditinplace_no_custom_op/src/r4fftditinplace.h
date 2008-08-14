/*************************************************************************
 * File: r4fftditinplace.h
 * Description: Introduction of the function which implements the 
 * 1024-point complex radix-4 DIT FFT.
 ************************************************************************* 
 * Project: FlexDSP                                  
 * Author: Risto Mäkinen <rmmakine@cs.tut.fi>                           
 ************************************************************************/

#if !defined(R4FFTDITINPLACE_H)
#define R4FFTDITINPLACE_H

void r4fftditinplace(const int Input[], int Output[]);

#define FFT_POINTS      1024 /* 4^5 */
#define FFT_POINTS_LOG2  10
#define FFT_POINTS_LOG4  5

#endif
