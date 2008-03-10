/*************************************************************************
 * In this case, the complex butterfly multiplications and summations
 * are performed with the aid of SFUs, i.e., the multiplication SFU
 * multiplicates the operands of input-array with the twiddle factors,
 * and the summation SFU adds the obtained products together in four
 * slightly different summation formats. The index 
 * generation, i.e. the input permutation and operand access, is
 * performed with the aid of one single, integrated SFU, that is 
 * performing the memory address arithmetics needed in the input 
 * permutation and in the operand access of the butterflies. In addition, 
 * the coefficients are generated using an SFU: the Coefficient generator. 
 *************************************************************************
 * The possible overflow in every addition is avoided by always
 * dividing the result of addition by 2. This means the final result of 
 * FFT algorithm is heavily scaled but the final user can compensate
 * this in his own implementation.
 *************************************************************************
 * File: r4fftditinplace.c
 * Description: Declaration of the function which implements the 1024-point
 * in-place complex radix-4 DIT FFT
 ************************************************************************* 
 * Project: FlexDSP                                  
 * Author: Risto Mäkinen <rmmakine@cs.tut.fi>                           
 ************************************************************************/

/*#define DEBUG_STAGE_OUTPUT*/
/*#define DEBUG_FACTORS*/

#if defined(DEBUG_STAGE_OUTPUT) || defined(DEBUG_FACTORS)   
/*#include <stdio.h>*/
#endif

#if !defined(__move__) && !defined(__TCE__)
#include <assert.h>
#endif

#include "r4fftditinplace.h"
#include "sfus.h"

/*Implementation of the algorithm begins.*/
void
r4fftditinplace (const int Input[], int Output[]) {
    int oper0, oper1, oper2, oper3;
    int factor0, factor1, factor2, factor3;
    int *oper0in_addr = 0; 
    int *oper1in_addr = 0; 
    int *oper2in_addr = 0; 
    int *oper3in_addr = 0;
    int *oper0out_addr = 0;
    int *oper1out_addr = 0; 
    int *oper2out_addr = 0; 
    int *oper3out_addr = 0;
    unsigned int stage = 0;
    unsigned int linidx = 0;
    int prod0, prod1, prod2, prod3;

    /* Loop Counters:
     * stage -> counter for stages of FFT
     * linidx -> counter for butterfly input operands in a stage of FFT
     */


    /* In the stage zero, the operands are read from the input buffer
     * according to the input permutation principle of the algorithm. 
     * Meanwhile, the results of evaluation of a butterfly are written 
     * linearly to the output buffer.
     */
    for (linidx = 0; linidx < (FFT_POINTS - 1); linidx += 4) {
      /*Generate the needed addresses of operands.*/	 
#if defined(__move__) && !defined(__TCE__)
      ag(oper0in_addr,oper0out_addr,Input,Output,stage,linidx);
      ag(oper1in_addr,oper1out_addr,Input,Output,stage,linidx+1);
      ag(oper2in_addr,oper2out_addr,Input,Output,stage,linidx+2);
      ag(oper3in_addr,oper3out_addr,Input,Output,stage,linidx+3);
#else
      ag(&oper0in_addr,&oper0out_addr,Input,Output,stage,linidx);
      ag(&oper1in_addr,&oper1out_addr,Input,Output,stage,linidx+1);
      ag(&oper2in_addr,&oper2out_addr,Input,Output,stage,linidx+2);
      ag(&oper3in_addr,&oper3out_addr,Input,Output,stage,linidx+3);
#endif	
      /*Store operands and factors to local variables.*/
      oper0 = *oper0in_addr;
      factor0 = fgen(stage,linidx);
      oper1 = *oper1in_addr;
      factor1 = fgen(stage,linidx+1);
      oper2 = *oper2in_addr;
      factor2 = fgen(stage,linidx+2);
      oper3 = *oper3in_addr;
      factor3 = fgen(stage,linidx+3);

#ifdef DEBUG_FACTORS
      printf("%08X\n", factor0);
      printf("%08X\n", factor1);
      printf("%08X\n", factor2);
      printf("%08X\n", factor3);
#endif

      /*Complex multiplications are performed using the cmul-SFU.
       *Results of multiplications are stored to local variables. 
       */
      prod0 = cmul(oper0,factor0); 
      prod1 = cmul(oper1,factor1);
      prod2 = cmul(oper2,factor2);
      prod3 = cmul(oper3,factor3);
            
      /* Complex summations are performed using the cadd-SFU.
       * The four results of the summations are written linearly 
       * back to the output buffer.
       */
      *oper0out_addr = cadd(prod0,prod1,prod2,prod3,0);
      *oper1out_addr = cadd(prod0,prod1,prod2,prod3,1);
      *oper2out_addr = cadd(prod0,prod1,prod2,prod3,2);
      *oper3out_addr = cadd(prod0,prod1,prod2,prod3,3);
    }

#ifdef DEBUG_STAGE_OUTPUT
    printf("STAGE %d\n", stage);
      for (linidx = 0; linidx < (FFT_POINTS); linidx++) {
	/*printf("%d: %X\n", 1024 + linidx, Output[linidx]);*/
	printf("%08X\n", Output[linidx]);
      }
#endif	       

    /*In the other stages (1,2,3,and 4), the operands are both read from 
     *and written to the output buffer according to the operand access 
     *order of the algorithm. Otherwise the kernels of the inner loops 
     *are the same.
     */
    for (stage = 1; stage < FFT_POINTS_LOG4; stage++) { 
#ifdef DEBUG_FACTORS
      printf("STAGE %d\n", stage);
#endif
      for (linidx = 0; linidx < (FFT_POINTS - 1); linidx += 4) {	 

#if defined(__move__) && !defined(__TCE__)
	ag(oper0in_addr,oper0out_addr,Input,Output,stage,linidx);
	ag(oper1in_addr,oper1out_addr,Input,Output,stage,linidx+1);
	ag(oper2in_addr,oper2out_addr,Input,Output,stage,linidx+2);
	ag(oper3in_addr,oper3out_addr,Input,Output,stage,linidx+3);
#else
	ag(&oper0in_addr,&oper0out_addr,Input,Output,stage,linidx);
	ag(&oper1in_addr,&oper1out_addr,Input,Output,stage,linidx+1);
	ag(&oper2in_addr,&oper2out_addr,Input,Output,stage,linidx+2);
	ag(&oper3in_addr,&oper3out_addr,Input,Output,stage,linidx+3);
#endif	
	oper0 = *oper0out_addr;
	factor0 = fgen(stage,linidx);
	oper1 = *oper1out_addr;
	factor1 = fgen(stage,linidx+1);
	oper2 = *oper2out_addr;
	factor2 = fgen(stage,linidx+2);
	oper3 = *oper3out_addr;
	factor3 = fgen(stage,linidx+3);

#ifdef DEBUG_FACTORS
	printf("%08X\n", factor0);
	printf("%08X\n", factor1);
	printf("%08X\n", factor2);
	printf("%08X\n", factor3);
#endif

	prod0 = cmul(oper0,factor0); 
	prod1 = cmul(oper1,factor1);
	prod2 = cmul(oper2,factor2);
	prod3 = cmul(oper3,factor3);
            
	*oper0out_addr = cadd(prod0,prod1,prod2,prod3,0);
	*oper1out_addr = cadd(prod0,prod1,prod2,prod3,1);
	*oper2out_addr = cadd(prod0,prod1,prod2,prod3,2);
	*oper3out_addr = cadd(prod0,prod1,prod2,prod3,3);
      }
#ifdef DEBUG_STAGE_OUTPUT
	printf("STAGE %d\n", stage);
	for (linidx = 0; linidx < (FFT_POINTS); linidx++) {
	  /*printf("%d: %X\n", 1024 + linidx, Output[linidx]);*/
	  printf("%08X\n", Output[linidx]);
	}
#endif
    }
}
  
