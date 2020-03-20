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
 * @file Immediate.cc
 *
 * Implementation of Immediate class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "Immediate.hh"
#include "Terminal.hh"
#include "InstructionTemplate.hh"
#include "TerminalImmediate.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// Immediate
/////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * Creates a long immediate with the given value. The value specifies,
 * implicitly, also the width of the instruction field(s) where the
 * immediate bits are encoded). The destination register and the set
 * of instruction slots where the immediate bits are encoded are also
 * given as parameters.
 *
 * The ownership of the destination terminal object will be passed to
 * the immediate.
 *
 * @param value Value of the immediate.
 * @param dst Destination register.
 */

// Immediate::Immediate(
//    SimValue value, Terminal* dst)
//     :
//    value_(value), dst_(dst) {
//}

Immediate::Immediate(TerminalImmediate* value, Terminal* dst)
    : value_(value), dst_(dst) {}

/**
 * The destructor.
 */
Immediate::~Immediate() {
    if (dst_ != NULL) {
        delete dst_;
        dst_ = NULL;
    }

    if (value_ != NULL) {
        delete value_;
        value_ = NULL;
    }
}

/**
 * Returns the destination register of this immediate.
 *
 * @return The destination register of this immediate.
 */
const Terminal&
Immediate::destination() const {
    assert(dst_ != NULL);
    return *dst_;
}

/**
 * Returns the value of this immediate.
 *
 * @return The value of this immediate.
 */
TerminalImmediate&
Immediate::value() const {
    assert(value_ != NULL);
    return *value_;
}

/**
 * Sets the value of immediate.
 *
 * @param Value to set for immediate.
 */
void
Immediate::setValue(TerminalImmediate* value) {
    if (value_ != NULL) {
        delete value_;
    }
    value_ = value;
}

/**
 * Makes a copy of the immediate.
 *
 * The copy is identical, except that it is not registered to the
 * instruction of the original immediate (and therefore, any address it
 * refers to is not meaningful).
 *
 * @return A copy of the immediate.
 */
std::shared_ptr<Immediate>
Immediate::copy() const {
    return std::make_shared<Immediate>(
        dynamic_cast<TerminalImmediate*>(value_->copy()), dst_->copy());
}

}
