/************************************************************************
 * The possible overflow in every addition is avoided by always
 * dividing the result of addition by 2. This means the final result of 
 * FFT algorithm is heavily scaled but the final user can compensate
 * this in his own implementation.
 ************************************************************************
 * File: sfus.h
 * Description: Declarations of SFUs if gcc-move compiler is used. 
 * If gcc compiler is used this file declares the c-language functions
 * simulating SFUs. 
 ************************************************************************ 
 * Project: FlexDSP                                  
 * Author: Risto Mäkinen <rmmakine@cs.tut.fi>                           
 ************************************************************************/

# ifndef SFUS_H
# define SFUS_H

#if defined(__move__) && !defined(__TCE__)
# include "userdef.h"
/* Declarations of SFUs */
# define ag(r0,r1,a,b,c,d) __userdef_42__(34,PURE_FUNCTION,r0,r1,a,b,c,d)
# define cmul(a,b) __userdef_21__(35,PURE_FUNCTION,a,b)
# define cadd(a,b,c,d,e) __userdef_51__(36,PURE_FUNCTION,a,b,c,d,e)
# define fgen(a,b) __userdef_21__(37,PURE_FUNCTION,a,b)

# else
#define assert(X)
/***DEBUG***
 *# include <stdio.h>
***DEBUG***/

typedef short Scalar;
typedef union {
    int word;
    struct {
#if (defined(__TCE__) && !(defined(__TCELE__)))
        Scalar real;
        Scalar imag;
#else
        Scalar imag;
        Scalar real;
#endif
    }cplx;
}Complex;

#define Real(a) (a.cplx.real)
#define Imag(a) (a.cplx.imag)
#define Word(a) (a.word)

/* This function simulates the functionality of the SFU, that is capable
 * of generating correct memory addresses for both the input permutation
 * and the operand access, in the host environment. The function has four inputs
 * on the basis of whose the function produces always two memory addresses as it s
 * outputs. There are two different adress generation processes simulated
 * inside this function, i.e., these processes are executed concurrently in the
 * HW-implementation of this SFU. The process for the input permutation generate s
 * an address of the input-array by swapping two bits' bit fields
 * from the LSB- and MSB-parts of the bit field consisting of the lowermost
 * ten bits of the linidx in pairs. That is, the lowermost two bits of the
 * linidx are swapped with the bit pair consisting of the 8th and 9th bits
 * of the linidx etc. The process for the operand access of the butterflies
 * generates an address of the output-array by rotating the n lowermost bits
 * of the linidx two bits to the right. The number of the lowermost bits
 * to be rotated two bits to the right, n, is determined always as
 * n = 2s + 2 where s is the current stage of FFT computation,
 * 1 <= s <= log4(N) - 1. In the end of both of these processes, the final
 * memory address to be returned is always formed by adding the manipulated lini dx
 * together with the base address of the input- or output-array.
 */
static inline void
ag(int **in_addr, int **out_addr, int *base_address_in,
   int *base_address_out, unsigned int stage, unsigned int linidx) {
    /* Local variables for the input permutation. */
    unsigned int bit_pair0, bit_pair1, bit_pair3, bit_pair4, permuted_idx;
    /* Local variables for the operand access. */
    unsigned int upper_part = 0; unsigned int rotated_part = 0;
    unsigned int rotated_MSB = 0; unsigned int rotated_LSB_part = 0;
    unsigned int rotated_idx = 0;
    unsigned int rotated_part_width = 0;

    switch(stage) {
    /* Generate an address for the input permutation. */
    case 0:
        bit_pair0 = (linidx >> 8);
        bit_pair1 = ((linidx >> 4) & 0x0000000C);
        bit_pair3 = ((linidx << 4) & 0x000000C0);
        bit_pair4 = ((linidx << 8) & 0x00000300);
        permuted_idx = (bit_pair4 | bit_pair3 | (linidx & 0x00000030)
                        | bit_pair1 | bit_pair0);

        /* The result addresses for the input permutation. */
        *in_addr = (base_address_in + permuted_idx);
        *out_addr = (base_address_out + linidx);
        break;

    /* Generate an address for the operand access. */
    default:
        assert(stage > 0);

        rotated_part_width = (stage << 1) + 2;

        /* Rotate the rotated part two bits to the right. */
        upper_part = ((linidx >> rotated_part_width) << rotated_part_width);
        rotated_part = ((linidx << (32 - rotated_part_width)) >>
                        (32 - rotated_part_width));
        rotated_LSB_part = rotated_part >> 2;
        rotated_MSB = ((rotated_part << 30) >> 30);
        rotated_MSB = (rotated_MSB << (rotated_part_width - 2));

        /* Concatenate different parts of the final index by orring bit by bit
           and calculate the correct memory address for accessing the output
           vector.
        */
        rotated_idx = (upper_part | rotated_MSB | rotated_LSB_part);
        /* Assertions that can be used to verify that results are legal. */
        assert((rotated_idx + base_address_out) >= base_address_out);
        assert((rotated_idx + base_address_out) <= (base_address_out + 1023));

        /* The result addresses for the operand access. */
        *in_addr = 0;
        *out_addr = (rotated_idx + base_address_out);
    }
}

/*This function simulates the multiplication of two complex numbers.*/
static inline int 
cmul(int in1, int in2) {
    Complex num1, num2, prod;
    Word(num1) = in1;
    Word(num2) = in2;
    
/* If the value of multiplier, i.e. twiddle-factor, equals 1,
 * multiplicand, i.e. a value of input table of FFT, can be returned.
 */ 
    if (in2 == 0x7FFF0000) {
        Real(num1) = (Real(num1) >> 1);
        Imag(num1) = (Imag(num1) >> 1);
	return Word(num1);

    } else {
	Real(prod) = ((Real(num1) * Real(num2)) >> 16) - ((Imag(num1) * Imag(num2)) >> 16);
	Imag(prod) = ((Real(num1) * Imag(num2)) >> 16) + ((Imag(num1) * Real(num2)) >> 16);
    
	return Word(prod);
    }
}


/* The possible overflow in every addition in the function cadd is 
 * avoided by always
 * dividing the result of addition by 2. This means the final result of FFT
 * algorithm is heavily scaled but the final user can compensate
 * this in his own implementation.
 */

/* This function simulates the addition of four complex numbers in four
 * sligthly different forms needed in the butterfly calculations of radix-4 
 * DIT FFT.
 */
static inline int 
cadd(int in1, int in2, int in3, int in4, unsigned char format) {
    Complex num1, num2, num3, num4, sum;
    Word(num1) = in1;
    Word(num2) = in2;
    Word(num3) = in3;
    Word(num4) = in4;

    switch(format) {
    case 0:
        Real(sum) = ((((Real(num1) + Real(num2)) >> 1) + ((Real(num3) + Real(num4)) >> 1)) >> 1);
        Imag(sum) = ((((Imag(num1) + Imag(num2)) >> 1) + ((Imag(num3) + Imag(num4)) >> 1)) >> 1);
        break;
    case 1:
        Real(sum) = ((((Real(num1) + Imag(num2)) >> 1) - ((Real(num3) + Imag(num4)) >> 1)) >> 1);
        Imag(sum) = ((((Imag(num1) - Real(num2)) >> 1) - ((Imag(num3) - Real(num4)) >> 1)) >> 1);
        break;
    case 2:
        Real(sum) = ((((Real(num1) - Real(num2)) >> 1) + ((Real(num3) - Real(num4)) >> 1)) >> 1);
        Imag(sum) = ((((Imag(num1) - Imag(num2)) >> 1) + ((Imag(num3) - Imag(num4)) >> 1)) >> 1);
        break;
    case 3:
        Real(sum) = ((((Real(num1) - Imag(num2)) >> 1) - ((Real(num3) - Imag(num4)) >> 1)) >> 1);
        Imag(sum) = ((((Imag(num1) + Real(num2)) >> 1) - ((Imag(num3) + Real(num4)) >> 1)) >> 1);
        break;
    }

    /****DEBUG******
    printf("%08X  %08X  %08X  %08X  %08X  %08X\n", in1, in2, in3, in4, order, Word(sum));
    ****DEBUG*******/

    return Word(sum);
}

/*Include the needed 129 coefficients into table "Coefficients".*/
const int Coefficients[(FFT_POINTS/8) + 1] = {
#include "coefficients.dat"
};
static int modifyCoeff(int coeff, unsigned char oper);

/* The following function simulates the functionality of the coefficient generator in different
 * computation stages.
 */
static inline int
fgen(unsigned int stage, unsigned int linidx) {
  unsigned k = 0; unsigned i = 0;
  unsigned block_size = 4; unsigned bits_to_pick = 0;
  unsigned upper_index = 0; unsigned addr = 0;
  unsigned char oper = 0;

  if ((stage==0) || (linidx==0)) {
    return 0x7FFF0000;
  } else {
    for (k=0; k < stage;++k) {
      block_size *= 4;
    }
    /*Step 2.*/
    i = linidx & 0x00000003;
    /*Step 3: Scale linidx to match multiplication block size.*/
    bits_to_pick = (stage << 1) + 1 + 1;
    linidx = ((linidx << (32 - bits_to_pick)) >> (32 - bits_to_pick));
    /*Step 4: calculate the upper index.*/
    upper_index = i * (linidx >> 2);
    /*printf("upper index: %d\n", upper_index);*/
    /*Step 5: Find out to which sector upper_index belongs to and set the address 
     *and the operation according to that sector.
     */
    if (upper_index <= (block_size >> 3)) {
      addr = upper_index; oper = 0;
    } else if ( (upper_index > (block_size >> 3)) && (upper_index < (block_size >> 2)) ) {
      addr = (block_size >> 2) - upper_index; oper = 1;
    } else if ( (upper_index >= (block_size >> 2)) && (upper_index <= (3*block_size/8)) ) {
      addr = upper_index - (block_size >> 2); oper = 2;
    } else if ( (upper_index > (3*block_size/8)) && (upper_index < (block_size >> 1)) ) {
      addr = (block_size >> 1) - upper_index; oper = 3;
    } else if ( (upper_index >= (block_size >> 1)) && (upper_index < (5*block_size/8)) ) { 
      addr = upper_index - (block_size >> 1); oper = 4;
    } else if ( (upper_index >= (5*block_size/8)) && (upper_index < (3*block_size/4)) ) {
      addr = 3*block_size/4 - upper_index; oper = 5;
    }
    /*Step 6: Scale address to the block size of N.*/
    addr = (FFT_POINTS * addr / block_size);
    /*printf("address: %d\n", addr);*/
    /*Step 7: Load the coefficient from the coefficient memory and return 
     *the modified coefficient.
     */
    return modifyCoeff(Coefficients[addr], oper);
  }
}

/*This function modifies the loaded coefficient in an appropriate manner.*/
static int
modifyCoeff(int coeff, unsigned char oper) {
  Complex coeffi, temp;
  Word(temp) = coeff;
  if (oper == 0) {
    return coeff;
  } else if (oper == 1) {
    Real(coeffi) = -Imag(temp);
    Imag(coeffi) = -Real(temp);
    return Word(coeffi);
  } else if (oper == 2) {
    Real(coeffi) = Imag(temp);
    Imag(coeffi) = -Real(temp);
    return Word(coeffi);
  } else if (oper == 3) {
    Real(coeffi) = -Real(temp);
    Imag(coeffi) = Imag(temp);
    return Word(coeffi);
  } else if (oper == 4) {
    Real(coeffi) = -Real(temp); 
    Imag(coeffi) = -Imag(temp);
    return Word(coeffi);
  } else if (oper == 5) {
    Real(coeffi) = Imag(temp);
    Imag(coeffi) = Real(temp);
    return Word(coeffi);
  } 
}
# endif

# endif
