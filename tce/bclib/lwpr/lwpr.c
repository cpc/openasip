/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * Light Weight PRinting library.
 *
 * For stdout printing needs where putchar() or puts() do not suffice 
 * but printf() is overkill. Uses putchar() for character output.
 * 
 * @author Pekka Jääskeläinen 2011
 */
#include <lwpr.h>
#include <stdio.h>

__attribute__((noinline))
void lwpr_print_str(const char* str) {
    while (*str) {
        putchar(*str++);
    }
}

__attribute__((noinline))
void lwpr_print_int(int n) {
   /* inspired by itoa() of the K&R book (from the Wikipedia 
      itoa article) */
     int i, sign, size;
     char s[50];
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         putchar('-');

     --i;
     for (; i >= 0; --i) {
         putchar(s[i]);
     }
}

__attribute__((noinline))
void lwpr_print_hex(unsigned int n) {
     int i, sign, size;
     char s[50];
     char hexdigits[16];
     char ch;

     // create hex string
     ch = '0';
     for (i = 0; i < 10; i++) {
         hexdigits[i] = ch;
         ch++;
     }
     ch = 'a';
     for (i = 10; i < 16; i++) {
         hexdigits[i] = ch;
         ch++;
     }
 
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = hexdigits[n % 16];   /* get next digit */
     } while ((n /= 16) > 0);     /* delete it */
     --i;
     for (; i >= 0; --i) {
         putchar(s[i]);
     }    
}
