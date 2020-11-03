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
 * @file MathTools.cc
 *
 * Implementations. Dummy file just to force the MathTools.icc to be compiled.
 *
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 */

#include "MathTools.hh"


/**
 * Concatenates two bit fields together.
 *
 * Example:
 * @verbatim
 * concatenateBits(12, 4, 13, 3)
 *   -> (0b1100, 0b1101)
 *   -> 0b1100 + 0b101
 *   -> 0b1100101
 * @endverbatim
 */
unsigned long int
MathTools::concatenateBits(
    unsigned long int msbBitField, unsigned msbWidth,
    unsigned long int lsbBitField, unsigned lsbWidth) {

    const unsigned long int allOnes = -1ul;

    return ((msbBitField & ~(allOnes << msbWidth)) << lsbWidth) |
        (lsbBitField & ~(allOnes << lsbWidth));
}

