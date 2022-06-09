/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file
 *
 * Behavior definition file.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 */

#include <cassert>
#include "OSAL.hh"
#include "OperationGlobals.hh"

// **************************************************
// Operation definitions for radix-4 DIT FFT
// **************************************************

// Typedef used for representing complex numbers.
typedef short Scalar;
typedef union {
    int word;
    struct {
#if WORDS_BIGENDIAN == 1
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


OPERATION(INTEGRATED2ADDRGEN)

TRIGGER
   // Local variables for input permutation.
   unsigned int bit_pair0, bit_pair1, bit_pair3, bit_pair4, permuted_idx;
   // Local variables for operand access.
   unsigned int upper_part = 0; unsigned int rotated_part = 0;
   unsigned int rotated_MSB = 0; unsigned int rotated_LSB_part = 0;
   unsigned int rotated_idx = 0; unsigned int address = 0;
   unsigned int rotated_part_width = 0;

   switch(UINT(3)) {
     // Generate an address for the input permutation.
   case 0:
     bit_pair0 = (UINT(4) >> 8);
     bit_pair1 = ((UINT(4) >> 4) & 0x0000000C);
     bit_pair3 = ((UINT(4) << 4) & 0x000000C0);
     bit_pair4 = ((UINT(4) << 8) & 0x00000300);
     permuted_idx = (bit_pair4 | bit_pair3 | (UINT(4) & 0x00000030)
		     | bit_pair1 | bit_pair0);

     // The result addresses for the input permutation.
     IO(5) = (UINT(1) + (permuted_idx << 2));
     IO(6) = (UINT(2) + (UINT(4) << 2));

     break;

     // Generate an address for the operand access.
   default:
     assert(UINT(3) > 0);

     rotated_part_width = (UINT(3) << 1) + 2;

     /* Rotate the rotated part two bits to the right. */
     upper_part = ((UINT(4) >> rotated_part_width)
		   << rotated_part_width);
     rotated_part = ((UINT(4) << (32 - rotated_part_width)) >>
		     (32 - rotated_part_width));

   rotated_LSB_part = rotated_part >> 2;
   rotated_MSB = ((rotated_part << 30) >> 30);
   rotated_MSB = (rotated_MSB << (rotated_part_width - 2));

   /* Concatenate different parts of the final index by orring bit
    * by bit and calculate the correct memory address for accessing
    * the output vector.
    */

   rotated_idx =  (upper_part | rotated_MSB | rotated_LSB_part);
   /* The memory address to be returned is (4 * idx) + start address
      of input table. (4*idx) because size of int is 4 bytes.*/
   address = ((rotated_idx << 2) + UINT(2));
   
   // The result addresses for the operand access.
   IO(5) = 0;
   IO(6) = address;


   //Assertions that can be used to verify that results are legal.
   /*
    *   assert(result[1].ui % 4 == 0);
    */

   break;
}
   RETURN_READY;
END_TRIGGER;

END_OPERATION(INTEGRATED2ADDRGEN)



OPERATION(R4MUL)

TRIGGER
   Complex num1, num2, prod;
   Word(num1) = INT(1);
   Word(num2) = INT(2);

   /* If the value of multiplier, i.e. twiddle-factor, equals 1,
    * multiplicand, i.e. a value of input table of FFT, can be returned.
    */
   if (INT(2) == 0x7FFF0000) {
       IO(3) = INT(1);
   } else {
       Real(prod) = ((Real(num1) * Real(num2)) >> 15) -
       ((Imag(num1) * Imag(num2)) >> 15);
       Imag(prod) = ((Real(num1) * Imag(num2)) >> 15) +
       ((Imag(num1) * Real(num2)) >> 15);

       IO(3) = Word(prod);
   }
   RETURN_READY;
END_TRIGGER;

END_OPERATION(R4MUL)


OPERATION(R4ADD2)

TRIGGER
   Complex num1, num2, num3, num4, sum;
   Word(num1) = INT(1);
   Word(num2) = INT(2);
   Word(num3) = INT(3);
   Word(num4) = INT(4);

   switch(UINT(5)) {
   case 0:
        Real(sum) = ((((Real(num1) + Real(num2)) >> 1) + ((Real(num3) + Real(num4)) >> 1)) >> 1);
        Imag(sum) = ((((Imag(num1) + Imag(num2)) >> 1) + ((Imag(num3) + Imag(num4)) >> 1)) >> 1);
        break;
   case 1:
        Real(sum) = ((((Real(num1) + Imag(num2)) >> 1) - ((Real(num3) - Imag(num4)) >> 1)) >> 1);
        Imag(sum) = ((((Imag(num1) - Real(num2)) >> 1) - ((Imag(num3) + Real(num4)) >> 1)) >> 1);
        break;
   case 2:
        Real(sum) = ((((Real(num1) - Real(num2)) >> 1) + ((Real(num3) - Real(num4)) >> 1)) >> 1);
        Imag(sum) = ((((Imag(num1) - Imag(num2)) >> 1) + ((Imag(num3) - Imag(num4)) >> 1)) >> 1);
        break;
   case 3:
        Real(sum) = ((((Real(num1) - Imag(num2)) >> 1) - ((Real(num3) + Imag(num4)) >> 1)) >> 1);
        Imag(sum) = ((((Imag(num1) + Real(num2)) >> 1) - ((Imag(num3) - Real(num4)) >> 1)) >> 1);
        break;
   }

   IO(6) =  Word(sum);
   RETURN_READY;
END_TRIGGER;

END_OPERATION(R4ADD2)

OPERATION(AG)

TRIGGER
   // Local variables for input permutation.
   unsigned int bit_pair0, bit_pair1, bit_pair3, bit_pair4, permuted_idx;
   // Local variables for operand access.
   unsigned int upper_part = 0; unsigned int rotated_part = 0;
   unsigned int rotated_MSB = 0; unsigned int rotated_LSB_part = 0;
   unsigned int rotated_idx = 0; unsigned int address = 0;
   unsigned int rotated_part_width = 0;

   switch(UINT(3)) {
     // Generate an address for the input permutation.
   case 0:
     bit_pair0 = (UINT(4) >> 8);
     bit_pair1 = ((UINT(4) >> 4) & 0x0000000C);
     bit_pair3 = ((UINT(4) << 4) & 0x000000C0);
     bit_pair4 = ((UINT(4) << 8) & 0x00000300);
     permuted_idx = (bit_pair4 | bit_pair3 | (UINT(4) & 0x00000030)
		     | bit_pair1 | bit_pair0);

     // The result addresses for the input permutation.
     IO(5) = (UINT(1) + (permuted_idx << 2));
     IO(6) = (UINT(2) + (UINT(4) << 2));

     break;

     // Generate an address for the operand access.
   default:
     assert(UINT(3) > 0);

     rotated_part_width = (UINT(3) << 1) + 2;

     /* Rotate the rotated part two bits to the right. */
     upper_part = ((UINT(4) >> rotated_part_width)
		   << rotated_part_width);
     rotated_part = ((UINT(4) << (32 - rotated_part_width)) >>
		     (32 - rotated_part_width));

   rotated_LSB_part = rotated_part >> 2;
   rotated_MSB = ((rotated_part << 30) >> 30);
   rotated_MSB = (rotated_MSB << (rotated_part_width - 2));

   /* Concatenate different parts of the final index by orring bit
    * by bit and calculate the correct memory address for accessing
    * the output vector.
    */

   rotated_idx =  (upper_part | rotated_MSB | rotated_LSB_part);
   /* The memory address to be returned is (4 * idx) + start address
      of input table. (4*idx) because size of int is 4 bytes.*/
   address = ((rotated_idx << 2) + UINT(2));
   
   // The result addresses for the operand access.
   IO(5) = 0;
   IO(6) = address;


   //Assertions that can be used to verify that results are legal.
   /*
    *   assert(result[1].ui % 4 == 0);
    */

   break;
}
   RETURN_READY;
END_TRIGGER;

END_OPERATION(AG)

OPERATION(CMUL)

TRIGGER
   Complex num1, num2, prod;
   Word(num1) = INT(1);
   Word(num2) = INT(2);

   /* If the value of multiplier, i.e. twiddle-factor, equals 1,
    * multiplicand, i.e. a value of input table of FFT, can be returned.
    */
   if (INT(2) == 0x7FFF0000) {
       Real(num1) = (Real(num1) >> 1);
       Imag(num1) = (Imag(num1) >> 1);
       IO(3) = Word(num1);
   } else {
       Real(prod) = ((Real(num1) * Real(num2)) >> 16) -
       ((Imag(num1) * Imag(num2)) >> 16);
       Imag(prod) = ((Real(num1) * Imag(num2)) >> 16) +
       ((Imag(num1) * Real(num2)) >> 16);

       IO(3) = Word(prod);
   }
   RETURN_READY;
END_TRIGGER;

END_OPERATION(CMUL)

OPERATION(CADD)

TRIGGER
   Complex num1, num2, num3, num4, sum;
   Word(num1) = INT(1);
   Word(num2) = INT(2);
   Word(num3) = INT(3);
   Word(num4) = INT(4);

   switch(UINT(5)) {
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

   IO(6) =  Word(sum);
   RETURN_READY;
END_TRIGGER;

END_OPERATION(CADD)

/************************************************************************************
 * N-point radix-4 DIT FFT needs N/8+1 separate coefficients. Include the needed 
 * coefficients for N = 64, N = 256, and N = 1024 into separate tables.
 ************************************************************************************/
const int Coefficients64[(64/8) + 1] = {
0x7FFF0000,
0x7F62F374,
0x7D8AE707,
0x7A7DDAD7,
0x7641CF04,
0x70E2C3A9,
0x6A6DB8E3,
0x62F2AECC,
0x5A82A57D
};

const int Coefficients256[(256/8) + 1] = {
0x7FFF0000,
0x7FF6FCDB,
0x7FD8F9B8,
0x7FA7F695,
0x7F62F374,
0x7F09F054,
0x7E9DED37,
0x7E1DEA1D,
0x7D8AE707,
0x7CE3E3F4,
0x7C29E0E6,
0x7B5DDDDC,
0x7A7DDAD7,
0x798AD7D9,
0x7884D4E0,
0x776CD1EE,
0x7641CF04,
0x7504CC21,
0x73B5C945,
0x7255C673,
0x70E2C3A9,
0x6F5FC0E8,
0x6DCABE31,
0x6C24BB85,
0x6A6DB8E3,
0x68A6B64B,
0x66CFB3C0,
0x64E8B140,
0x62F2AECC,
0x60ECAC64,
0x5ED7AA0A,
0x5CB4A7BD,
0x5A82A57D
};

const int Coefficients1024[(1024/8) + 1] = {
2147418112,
2147483446,
2147352173,
2147155364,
2146893019,
2146499602,
2146040649,
2145516160,
2144926136,
2144270575,
2143483942,
2142631773,
2141714069,
2140665292,
2139616516,
2138436668,
2137191284,
2135814828,
2134438372,
2132930844,
2131357780,
2129719181,
2127949510,
2126179838,
2124279095,
2122312817,
2120215466,
2118118116,
2115889693,
2113595735,
2111236242,
2108745676,
2106255111,
2103633474,
2100946301,
2098193592,
2095309812,
2092426032,
2089411180,
2086330793,
2083119334,
2079907875,
2076565344,
2073222814,
2069749212,
2066144538,
2062539865,
2058804120,
2055068375,
2051201559,
2047203671,
2043205784,
2039142361,
2034947866,
2030687836,
2026362270,
2021971168,
2017514531,
2012926823,
2008339114,
2003620334,
1998836019,
1993986168,
1989070782,
1984024324,
1978977866,
1973800337,
1968557273,
1963248673,
1957874537,
1952434866,
1946929659,
1941293381,
1935657104,
1929889755,
1924122407,
1918223987,
1912260031,
1906230541,
1900135514,
1893909417,
1887683320,
1881391687,
1874968983,
1868546280,
1861992506,
1855373196,
1848688350,
1842003505,
1835187589,
1828306138,
1821359151,
1814346629,
1807268571,
1800059442,
1792850314,
1785575651,
1778235452,
1770829718,
1763292912,
1755756107,
1748153767,
1740485892,
1732686945,
1724888000,
1717023518,
1709027966,
1701032414,
1692971328,
1684844705,
1676652548,
1668394856,
1660071628,
1651617329,
1643228566,
1634708733,
1626123364,
1617472460,
1608756021,
1600039583,
1591192074,
1582344565,
1573365986,
1564387407,
1555343293,
1546233643,
1537058459,
1527817740,
1518511485
};

static int modifyCoeff(int coeff, unsigned char oper);

/* The following operation simulates the functionality of the coefficient generator in different
 * computation stages of 1024-point FFT.
 */
OPERATION(FGEN)

TRIGGER
  unsigned k = 0; unsigned i = 0;
  unsigned block_size = 4; unsigned bits_to_pick = 0;
  unsigned upper_index = 0; unsigned addr = 0;
  unsigned char oper = 0;
  unsigned int stage = UINT(1);
  unsigned int linidx = UINT(2);

  if ((stage==0) || (linidx==0)) {
    IO(3) = 0x7FFF0000;
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
    addr = (1024 * addr / block_size);
    /*printf("address: %d\n", addr);*/
    /*Step 7: Load the coefficient from the coefficient memory and return 
     *the modified coefficient.
     */
    IO(3) = modifyCoeff(Coefficients1024[addr], oper);
  }  
  RETURN_READY;
END_TRIGGER;

END_OPERATION(FGEN)

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
  return 0;
}


OPERATION(R4AG)

TRIGGER
   // Local variables for input permutation.
    unsigned int bit_pair0, bit_pair1, bit_pair2, bit_pair3, bit_pair4;
    unsigned int permuted_idx = 0; // warning fix
   // Local variables for operand access.
   unsigned int upper_part = 0; unsigned int rotated_part = 0;
   unsigned int rotated_MSB = 0; unsigned int rotated_LSB_part = 0;
   unsigned int rotated_idx = 0; unsigned int address = 0;
   unsigned int rotated_part_width = 0;

   switch(UINT(3)) {
     // Generate an address for the input permutation.
   case 0:
     // Check N and permute the linear index.
     if (UINT(4) == 64) {
       bit_pair0 = (UINT(5) >> 4);
       bit_pair1 = ((UINT(5) << 4) & 0x00000030);
       permuted_idx = (bit_pair1 | (UINT(5) & 0x0000000C) | bit_pair0);
     } else if (UINT(4) == 256) {
       bit_pair0 = (UINT(5) >> 6);
       bit_pair1 = ((UINT(5) >> 2) & 0x0000000C);
       bit_pair2 = ((UINT(5) << 2) & 0x00000030);
       bit_pair3 = ((UINT(5) << 6) & 0x000000C0);
       permuted_idx = (bit_pair3 | bit_pair2 | bit_pair1 | bit_pair0);
     } else if (UINT(4) == 1024) {
       bit_pair0 = (UINT(5) >> 8);
       bit_pair1 = ((UINT(5) >> 4) & 0x0000000C);
       bit_pair3 = ((UINT(5) << 4) & 0x000000C0);
       bit_pair4 = ((UINT(5) << 8) & 0x00000300);
       permuted_idx = (bit_pair4 | bit_pair3 | (UINT(5) & 0x00000030)
		       | bit_pair1 | bit_pair0);
     } else {
       // No other radix-4 FFT sizes are supported.
       assert(false);
     }
     // The result addresses for the input permutation.
     IO(6) = (UINT(1) + (permuted_idx << 2));
     IO(7) = (UINT(2) + (UINT(5) << 2));

     break;

     // Generate an address for the operand access.
   default:
     assert(UINT(3) > 0);

     rotated_part_width = (UINT(3) << 1) + 2;

     /* Rotate the rotated part two bits to the right. */
     upper_part = ((UINT(5) >> rotated_part_width)
		   << rotated_part_width);
     rotated_part = ((UINT(5) << (32 - rotated_part_width)) >>
		     (32 - rotated_part_width));

     rotated_LSB_part = rotated_part >> 2;
     rotated_MSB = ((rotated_part << 30) >> 30);
     rotated_MSB = (rotated_MSB << (rotated_part_width - 2));

   /* Concatenate different parts of the final index by orring bit
    * by bit and calculate the correct memory address for accessing
    * the output vector.
    */

   rotated_idx =  (upper_part | rotated_MSB | rotated_LSB_part);
   /* The memory address to be returned is (4 * idx) + start address
      of input table. (4*idx) because size of int is 4 bytes.*/
   address = ((rotated_idx << 2) + UINT(2));
   
   // The result addresses for the operand access.
   IO(6) = 0;
   IO(7) = address;


   //Assertions that can be used to verify that results are legal.
   /*
    *   assert(result[1].ui % 4 == 0);
    */

   break;
}
   RETURN_READY;
END_TRIGGER;

END_OPERATION(R4AG)


/* The following operation simulates the functionality of the coefficient generator in different
 * computation stages of 64-, 256-, and 1024-point radix-4 DIT FFTs.
 */
OPERATION(R4FGEN)

TRIGGER
  unsigned k = 0; unsigned i = 0;
  unsigned block_size = 4; unsigned bits_to_pick = 0;
  unsigned upper_index = 0; unsigned addr = 0;
  unsigned char oper = 0;
  unsigned int stage = UINT(1);
  unsigned int linidx = UINT(3);
  unsigned int fft_points = UINT(2);

  if ((stage==0) || (linidx==0)) {
    IO(4) = 0x7FFF0000;
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
    if (fft_points == 64) {
      addr = (64 * addr / block_size);
    } else if (fft_points == 256) {
      addr = (256 * addr / block_size);
    } else if (fft_points == 1024) {
      addr = (1024 * addr / block_size);
    } else {
      // At the moment, this operation supports only FFT sizes of 64, 256, and 1024.
      assert(false);
    }
    /*printf("address: %d\n", addr);*/
    /*Step 7: Load the coefficient from the correct coefficient table and return 
     *the modified coefficient.
     */
    if (fft_points == 64) {
      IO(4) = modifyCoeff(Coefficients64[addr], oper);
    } else if (fft_points == 256) {
      IO(4) = modifyCoeff(Coefficients256[addr], oper);
    } else if (fft_points == 1024) {
      IO(4) = modifyCoeff(Coefficients1024[addr], oper);
    } else {
      assert(false);
    }
  }  
  RETURN_READY;
END_TRIGGER;

END_OPERATION(R4FGEN)
