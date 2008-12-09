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
 * @file TerminalInstructionAddress.cc
 *
 * Implementation of TerminalInstructionAddress class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
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
