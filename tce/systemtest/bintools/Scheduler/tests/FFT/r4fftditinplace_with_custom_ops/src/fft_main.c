/*************************************************************************
 * File: fft_main.c
 * Description: Entry point of the program 
 *************************************************************************
 * Project: FlexDSP                                  
 * Author: Risto Mäkinen <rmmakine@cs.tut.fi>                           
 ************************************************************************/

#include "r4fftditinplace.h"

/*Define DEBUG_MEMORY if you want to examine data memory contents
  after the execution of FFT computation.*/
/*#define DEBUG_MEMORY*/
/*#define DEBUG_INPUT*/
/*#define DEBUG_OUTPUT*/
/*#define DEBUG_DPRAM_TRACE*/

#if defined(DEBUG_MEMORY) || defined(DEBUG_INPUT) || defined(DEBUG_OUTPUT)   
/*#include <stdio.h>*/
#endif

const int Input[FFT_POINTS] = {
#include "input.dat"
};

int Output[FFT_POINTS] = {0
};

const int Reference[FFT_POINTS] = {
#include "reference.dat"
};

/*Function declarations*/

int main(void) {
  int result = 0;
#if defined(DEBUG_MEMORY) || defined(DEBUG_INPUT) || defined(DEBUG_OUTPUT) || defined(DEBUG_DPRAM_TRACE) 
  int i = 0;
#endif

#ifdef DEBUG_INPUT
  for (i = 0; i < FFT_POINTS; i++) {
    printf("4:0h%08x\n", Input[i]);
  }
#endif

  r4fftditinplace(Input, Output);

#ifdef DEBUG_MEMORY
  for (i = 0; i < FFT_POINTS; i++) {
    printf("%08X\n", Input[i]);
  }
  for (i = 0; i < FFT_POINTS; i++) {
    printf("%08X\n", Output[i]);
  }
#endif

#ifdef DEBUG_OUTPUT
  for (i = 0; i < FFT_POINTS; i++) {
    printf("%08X\n", Output[i]);
  }

  for (i = 0; i < FFT_POINTS; i++) {
    if (Output[i] != Reference[i]) {
      printf("Output[%d] == %08X != Reference[%d] == %08X\n",
	     i, Output[i], i, Reference[i]);
      result = -1;
    }
  }
#endif

#ifdef DEBUG_DPRAM_TRACE
 for (i = 0; i < FFT_POINTS; i++) {
    printf("%08X\n", Input[i]);
  }
 for (i = 0; i < FFT_POINTS; i++) {
    printf("%08X\n", Output[i]);
  }
#endif

  return result;
}




