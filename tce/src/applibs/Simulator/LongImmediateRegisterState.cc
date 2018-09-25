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
 * @file LongImmediateRegisterState.cc
 *
 * Definition of LongImmediateRegisterState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "LongImmediateRegisterState.hh"
#include "LongImmediateUnitState.hh"
#include "SimValue.hh"

using std::string;

/**
 * Constructor.
 *
 * @param parent Parent unit of the register.
 * @param index Index of the register.
 * @param width The bit width of the register.
 * @param signExtend True in case written value should be sign extended 
 *                   (default is zero extension).
 */
LongImmediateRegisterState::LongImmediateRegisterState(
    LongImmediateUnitState* parent, int index, int width, bool signExtend) : 
    StateData(), parent_(parent), index_(index), width_(width), 
    signExtend_(signExtend) {
}

/**
 * Destructor.
 */
LongImmediateRegisterState::~LongImmediateRegisterState() {
}

/**
 * Sets the value of the register.
 *
 * Also extends the value if it's narrower than the register width.
 *
 * @param value New value.
 */
void
LongImmediateRegisterState::setValue(const SimValue& value) {
    // The value is extended in case it's smaller than WORD_BITWIDTH,
    // which is the width of (int) otherwise we won't extend, as it's
    // probably a double. In the future when we support 64 bit or larger
    // integers, this check is not needed.
    if (value.width() < width_) {
        SimValue newValue(width_);
        int ext = width_ - value.width();

        if (signExtend_) {
            SLongWord toBeExtended = value.sLongWordValue();
            newValue = ((long)(toBeExtended << ext) >> ext);
        } else {
            // @todo: this might not be needed, we could assume the top bits 
            // to be zero already
            ULongWord toBeExtended = value.uLongWordValue();
            newValue = ((toBeExtended << ext) >> ext);
        }
        parent_->setRegisterValue(index_, newValue);
    } else {
        parent_->setRegisterValue(index_, value);
    }
}

/**
 * Returns the register value.
 *
 * @return Register value.
 */
const SimValue&
LongImmediateRegisterState::value() const {
    return parent_->registerValue(index_);
}

