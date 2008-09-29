/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
#include "NullTerminal.hh"
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

//Immediate::Immediate(
//    SimValue value, Terminal* dst)
//    throw (IllegalParameters) :
//    value_(value), dst_(dst) {
//}

Immediate::Immediate(
    TerminalImmediate* value, Terminal* dst)
    throw (IllegalParameters) :
    value_(value), dst_(dst) {
}

/**
 * The destructor.
 */
Immediate::~Immediate() {
    if (dst_ != NULL && dst_ != &NullTerminal::instance()) {
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
Immediate*
Immediate::copy() const {
    return new Immediate(
        dynamic_cast<TerminalImmediate*>(value_->copy()), dst_->copy());
}

}
