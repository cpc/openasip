/* Copyright (c) 2017  SiFive Inc. All rights reserved.

   This copyrighted material is made available to anyone wishing to use,
   modify, copy, or redistribute it subject to the terms and conditions
   of the FreeBSD License.   This program is distributed in the hope that
   it will be useful, but WITHOUT ANY WARRANTY expressed or implied,
   including the implied warranties of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  A copy of this license is available at
   http://www.opensource.org/licenses.
*/

#include "sys/asm.h"

/* int setjmp (jmp_buf);  */
  .globl  setjmp
  .type   setjmp, @function
setjmp:
	REG_S ra,  (0)(x10)
	REG_S x8,  1*SZREG(x10)
	REG_S x9,  2*SZREG(x10)
	REG_S x18,  3*SZREG(x10)
	REG_S x19,  4*SZREG(x10)
	REG_S x20,  5*SZREG(x10)
	REG_S x21,  6*SZREG(x10)
	REG_S x22,  7*SZREG(x10)
	REG_S x23,  8*SZREG(x10)
	REG_S x24,  9*SZREG(x10)
	REG_S x25, 10*SZREG(x10)
	REG_S x26,11*SZREG(x10)
	REG_S x27,12*SZREG(x10)
	REG_S x2, 13*SZREG(x10)
	li    x10, 0
	ret
	.size	setjmp, .-setjmp

/* volatile void longjmp (jmp_buf, int);  */
  .globl  longjmp
  .type   longjmp, @function
longjmp:
	REG_L ra,  0*SZREG(x10)
	REG_L x8,  1*SZREG(x10)
	REG_L x9,  2*SZREG(x10)
	REG_L x18,  3*SZREG(x10)
	REG_L x19,  4*SZREG(x10)
	REG_L x20,  5*SZREG(x10)
	REG_L x21,  6*SZREG(x10)
	REG_L x22,  7*SZREG(x10)
	REG_L x23,  8*SZREG(x10)
	REG_L x24,  9*SZREG(x10)
	REG_L x25, 10*SZREG(x10)
	REG_L x26,11*SZREG(x10)
	REG_L x27,12*SZREG(x10)
	REG_L x2, 13*SZREG(x10)

	sltiu x10, x11, 1
	add  x10, x10, x11   # x10 = (x11 == 0) ? 1 : x11
	ret
	.size	longjmp, .-longjmp
