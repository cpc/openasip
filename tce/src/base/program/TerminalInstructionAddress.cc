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
 * @file TerminalInstructionAddress.cc
 *
 * Implementation of TerminalInstructionAddress class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "TerminalInstructionAddress.hh"
#include "InstructionReference.hh"
#include "Instruction.hh"

using namespace TTAMachine;

namespace TTAProgram {

/**
 * The constructor.
 *
 * @param value The value of the inline immediate (the address).
 * @param space The address space of the address the immediate refers to.
 * @param ref The instruction this address refers to.
 */
TerminalInstructionAddress::TerminalInstructionAddress(
    InstructionReference& ref):
    TerminalImmediate(SimValue(0, WORD_BITWIDTH)), ref_(&ref) {
}

/**
 * The destructor.
 */
TerminalInstructionAddress::~TerminalInstructionAddress() {
}

/**
 * Tells whether the terminal is an inline immediate that refers to an
 * instruction address.
 *
 * @return true always.
 */
bool
TerminalInstructionAddress::isInstructionAddress() const {
    return true;
}

/**
 * Returns the value of the immediate (instruction address).
 *
 * @return The value of the immediate (instruction address).
 */
SimValue
TerminalInstructionAddress::value() const throw (WrongSubclass) {
    try {
        return SimValue(
            ref_->instruction().address().location(), WORD_BITWIDTH);
    } catch (const Exception& e) {
        // Instruction::address() might throw IllegalRegistration in case of
        // a broken program
        abortWithError(e.errorMessage());
    }
    return NullSimValue::instance();
}

/**
 * Returns a reference to the instruction to which the immediate points.
 *
 * @return A reference to the instruction to which the immediate points.
 * @exception WrongSubclass never.
 */
InstructionReference&
TerminalInstructionAddress::instructionReference() const
    throw (WrongSubclass) {
    return *ref_;
}

/**
 * Set a new referred instruction.
 *
 * @param ref The new instruction reference.
 * @exception WrongSubclass never.
 */
void
TerminalInstructionAddress::setInstructionReference(
    InstructionReference& ref)
    throw(WrongSubclass) {
    ref_ = &ref;
}

/**
 * Creates an exact copy of the terminal and returns it.
 *
 * @return A copy of the terminal.
 */
Terminal*
TerminalInstructionAddress::copy() const {
    return new TerminalInstructionAddress(*ref_);
}

/**
 * Checks if terminals are equal.
 *
 * @param other Terminal to compare.
 * @return true if terminals are equal.
 */
bool 
TerminalInstructionAddress::equals(const Terminal& other) const {

    if (other.isInstructionAddress() != true) {
        return false;
    }
    return value() == other.value();
}

}
