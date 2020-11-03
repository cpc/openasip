/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @author Ari Mets‰halme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2011
 * @note rating: red
 */

#include "TerminalInstructionReference.hh"
#include "InstructionReference.hh"
#include "Instruction.hh"
#include "TerminalBasicBlockReference.hh"
#include "BasicBlock.hh"

using namespace TTAMachine;

namespace TTAProgram {

/**
 * The constructor.
 *
 * @param value The value of the inline immediate (the address).
 * @param space The address space of the address the immediate refers to.
 * @param ref The instruction this address refers to.
 */
TerminalInstructionReference::TerminalInstructionReference(
    InstructionReference ref) : TerminalInstructionAddress(), ref_(ref) {
}

/**
 * The destructor.
 */
TerminalInstructionReference::~TerminalInstructionReference() {
}

/**
 * Returns the value of the immediate (instruction address).
 *
 * @return The value of the immediate (instruction address).
 */
Address
TerminalInstructionReference::address() const {
    return ref_.instruction().address();
}

/**
 * Returns a reference to the instruction to which the immediate points.
 *
 * @return A reference to the instruction to which the immediate points.
 * @exception WrongSubclass never.
 */
const InstructionReference&
TerminalInstructionReference::instructionReference() const {
    return ref_;
}

/**
 * Returns a reference to the instruction to which the immediate points.
 *
 * @return A reference to the instruction to which the immediate points.
 * @exception WrongSubclass never.
 */
InstructionReference&
TerminalInstructionReference::instructionReference() {
    return ref_;
}

/**
 * Set a new referred instruction.
 *
 * @param ref The new instruction reference.
 * @exception WrongSubclass never.
 */
void
TerminalInstructionReference::setInstructionReference(
    InstructionReference ref) {
    ref_ = ref;
}

/**
 * Creates an exact copy of the terminal and returns it.
 *
 * @return A copy of the terminal.
 */
Terminal*
TerminalInstructionReference::copy() const {
    return new TerminalInstructionReference(ref_);
}

/**
 * Checks if terminals are equal.
 *
 * @param other Terminal to compare.
 * @return true if terminals are equal.
 */
bool 
TerminalInstructionReference::equals(const Terminal& other) const {

    const TerminalBasicBlockReference* otherBBRef =
        dynamic_cast<const TerminalBasicBlockReference*>(&other);
    if (otherBBRef) {
        const BasicBlock& bb = otherBBRef->basicBlock();
        return &ref_.instruction() == &(bb.firstInstruction());
    }

    const TerminalInstructionReference* otherInsRef =
        dynamic_cast<const TerminalInstructionReference*>(&other);
    if (otherInsRef) {
        return otherInsRef->instructionReference() ==
            ref_;
    }
    return false;
}

}
