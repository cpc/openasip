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
 * @file MathTools.hh
 *
 * Mathematical tools.
 *
 * Declarations.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 *
 */

#ifndef TTA_MATH_TOOLS_HH
#define TTA_MATH_TOOLS_HH

#include "BaseType.hh"

class MathTools {
public:
    static int requiredBits(long unsigned int number);
    static int requiredBitsSigned(long int number);
    static int requiredBitsSigned(int number);
    static int requiredBitsSigned(UInt32 number);
    static int requiredBitsSigned(unsigned long number);
    static int signExtendTo(int value, int width);
    static int zeroExtendTo(int value, int width);
    
    static int fastSignExtendTo(int value, int width);
    static int fastZeroExtendTo(int value, int width);

    static bool bit(unsigned int integer, unsigned int index);
    
    static int random(int, int);
    static unsigned int roundDownToPowerTwo(unsigned int number);
};

#include "MathTools.icc"

#endif
