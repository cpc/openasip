/*
    Copyright (c) 2002-2012 Tampere University.

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
 * @file HalfFloatWord.hh
 * @author Heikki Kultala (pjaaskel-no.spam-cs.tut.fi) 2012
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 *
 * This file contains definitions of float16 data type used by simulator.
 */

#ifndef HALF_FLOAT_WORD_HH
#define HALF_FLOAT_WORD_HH

#include "BaseType.hh"
#include <stdint.h>

/**
 * Half-precision, IEE-754-2008 16-bit floating point number.
 */
class HalfFloatWord {
public:
    operator float() const;

    // constructors
    explicit HalfFloatWord(uint16_t binaryRep);
    explicit HalfFloatWord(float value);
    HalfFloatWord(const HalfFloatWord& hw);
    HalfFloatWord();

    // calculations
    HalfFloatWord operator+ (const HalfFloatWord& right) const;
    HalfFloatWord operator- (const HalfFloatWord& right) const;
    HalfFloatWord operator* (const HalfFloatWord& right) const;
    HalfFloatWord operator/ (const HalfFloatWord& right) const;

    const HalfFloatWord& operator= (float value);
    const HalfFloatWord& operator= (const HalfFloatWord& value);

    uint16_t getBinaryRep() const { return binaryRep_; }

    static uint16_t convertFloatToHalfWordRep(float value);

private:
    uint16_t binaryRep_;
};

const Byte HLF_WORD_SIZE = 8 * sizeof(HalfFloatWord);

#endif
