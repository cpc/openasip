
/*
    Copyright (c) 2002-2012 Tampere University of Technology.

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
 * @file HalfFloatWord.cc
 * @author Heikki Kultala (pjaaskel-no.spam-cs.tut.fi) 2012
 *
 * This file contains implementation of float16 data type used by simulator.
 */

#include "HalfFloatWord.hh"
#include <cmath>

#ifndef INFINITY
#define INFINITY 1.0/0.0
#endif

#ifndef NAN
#define NAN 0.0/0.0
#endif

union FloatConvUnion {
    int i;
    float f;
};

// constructors
HalfFloatWord::HalfFloatWord(uint16_t binaryRep) : binaryRep_(binaryRep) {}

// constructors
HalfFloatWord::HalfFloatWord() : binaryRep_(0) {}

uint16_t
HalfFloatWord::convertFloatToHalfWordRep(float value) {
    FloatConvUnion u;
    u.f = value;
    int binary16 = (u.i & 0x007FFFFF) >> 13;
    binary16 |=(u.i & 0x0f800000) >> 13;
    binary16 |=(u.i & 0x80000000) >> 16;
    return binary16;
}

#pragma GCC diagnostic warning "-Wstrict-aliasing"

const HalfFloatWord& HalfFloatWord::operator=(float value) {
    binaryRep_ = convertFloatToHalfWordRep(value);
    return *this;
}

const HalfFloatWord& HalfFloatWord::operator=(const HalfFloatWord& hf) {
    binaryRep_ = hf.binaryRep_;
    return *this;
}

HalfFloatWord::HalfFloatWord(float value) : 
    binaryRep_(convertFloatToHalfWordRep(value)) {}


HalfFloatWord::HalfFloatWord(const HalfFloatWord& hw) : 
    binaryRep_(hw.binaryRep_) {}

#pragma GCC diagnostic ignored "-Wdiv-by-zero"

HalfFloatWord::operator float() const {
    if (binaryRep_ == 0xFC00) {
        return -INFINITY;
    }
    if (binaryRep_ == 0x7C00) {
        return INFINITY;
    }

    bool sgn = ((binaryRep_ & 0x8000) >> 15);
    int exp = (binaryRep_ & 0x7C00) >> 10;
    int mant = binaryRep_ & 0x03FF;

    if (exp == 0x1F && mant != 0) {
        return NAN;
    }

    float value = (exp == 0) ? mant : mant | 0x0400; // 1.x if not denormal
    value /= 0x400;
    float mul = exp2(exp - 15);
    value *= mul;
    if (sgn) {
        value *= -1;
    }
    return value;
}


#pragma GCC diagnostic warning "-Wdiv-by-zero"

    // calculations
HalfFloatWord HalfFloatWord::operator+ (const HalfFloatWord& right) const {
    return HalfFloatWord(float(*this) + float(right));
}

HalfFloatWord HalfFloatWord::operator- (const HalfFloatWord& right) const {
    return HalfFloatWord(float(*this) - float(right));
}

HalfFloatWord HalfFloatWord::operator* (const HalfFloatWord& right) const {
    return HalfFloatWord(float(*this) * float(right));
}

HalfFloatWord HalfFloatWord::operator/ (const HalfFloatWord& right) const {
    return HalfFloatWord(float(*this) / float(right));
}
