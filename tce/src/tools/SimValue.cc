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
 * @file SimValue.icc
 *
 * Non-inline definitions of SimValue class.
 *
 * @author Pekka Jääskeläinen 2004 (pjaaskel-no.spam-cs.tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: red
 */

#include "SimValue.hh"
#include "MathTools.hh"

//////////////////////////////////////////////////////////////////////////////
// NullSimValue
//////////////////////////////////////////////////////////////////////////////

SimValue NullSimValue::instance_(0);

/**
 * Returns an instance of NullSimValue class (singleton).
 *
 * @return Singleton instance of NullSimValue class.
 */
SimValue&
NullSimValue::instance() {
    return instance_;
}

/**
 * Returns the value as a signed integer.
 *
 * @return Signed integer value of the SimValue.
 */
int
SimValue::intValue() const {
    int bitWidth = (bitWidth_ > 32) ? 32 : bitWidth_;
    return MathTools::fastSignExtendTo(value_.sIntWord, bitWidth);
}

/**
 * Returns the value as a signed integer.
 *
 * @return Unsigned integer value of the SimValue.
 */
unsigned int
SimValue::unsignedValue() const {
    int bitWidth = (bitWidth_ > 32) ? 32 : bitWidth_;
    return MathTools::fastZeroExtendTo(value_.uIntWord, bitWidth);
}

