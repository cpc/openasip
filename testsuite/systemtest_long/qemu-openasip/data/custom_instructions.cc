/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * OSAL behavior definition file.
 */

#include "OSAL.hh"
OPERATION(REFLECT8)
TRIGGER

unsigned long data = UINT(1);
unsigned char nBits = 8;

unsigned long reflection = 0x00000000;
unsigned char bit;

/*
 * Reflect the data about the center bit.
 */
for (bit = 0; bit < nBits; ++bit) {
    /*
     * If the LSB bit is set, set the reflection of it.
     */
    if (data & 0x01) {
        reflection |= (1 << ((nBits - 1) - bit));
    }

    data = (data >> 1);
}

IO(2) = static_cast<unsigned>(reflection);

return true;
END_TRIGGER;
END_OPERATION(REFLECT8)

OPERATION(REFLECT32)
TRIGGER

unsigned long data = UINT(1);
unsigned char nBits = 32;

unsigned long reflection = 0x00000000;
unsigned char bit;

/*
 * Reflect the data about the center bit.
 */
for (bit = 0; bit < nBits; ++bit) {
    /*
     * If the LSB bit is set, set the reflection of it.
     */
    if (data & 0x01) {
        reflection |= (1 << ((nBits - 1) - bit));
    }

    data = (data >> 1);
}

IO(2) = static_cast<unsigned>(reflection);

return true;
END_TRIGGER;
END_OPERATION(REFLECT32)
