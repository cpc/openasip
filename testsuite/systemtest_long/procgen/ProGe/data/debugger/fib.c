/*
        Copyright (c) 2016 Tampere University of Technology.

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

#define __global__ __attribute__((address_space(5)))
#define __param__ __attribute__((address_space(6)))
#include "tceops.h"

#define N 64

__global__ char test[4]={1,2,3,4};

volatile __global__ int results[N];

volatile __param__ int done = 0;

int fib(int n) {
    int a1=0, a2=1, retval;

    if (n <= 1) {
        return n;
    }

    for (int i=0; i<n-1; ++i) {
        retval = a1+a2;
        a1 = a2;
        a2 = retval;
    }
    return retval;
}

int
main(void) {
    for(int i=0; i<N; ++i) {
        int val = fib(i);
        val = ((val & 0xFF000000) >> 24)
            | ((val & 0x00FF0000) >> 8)
            | ((val & 0x0000FF00) << 8)
            | ((val & 0x000000FF) << 24);
        results[i] = val;
    }
    done = 1;
    return 0;
}
