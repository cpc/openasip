/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file TerminalProgramOperation.cc
 *
 * Implementation of the TerminalProgramOperation class.
 *
 * @author Pekka Jääskeläinen 2011
 * @note rating: red
 */

#include "TerminalProgramOperation.hh"
#include "Instruction.hh"
#include "ProgramOperation.hh"

using namespace TTAMachine;

namespace TTAProgram {

/**
 * The constructor.
 *
 * @param po The ProgramOperation to track.
 */
TerminalProgramOperation::TerminalProgramOperation(
    const ProgramOperation& po) :
    TerminalInstructionAddress(), po_(po) {
}

/**
 * The destructor.
 */
TerminalProgramOperation::~TerminalProgramOperation() {
}

/**
 * Returns the instruction address of the trigger move in the tracked
 * ProgramOperation.
 */
Address
TerminalProgramOperation::address() const 
    throw (WrongSubclass) {
    return po_.triggeringMove()->move().parent().address();
}

/**
 * Creates an exact copy of the terminal and returns it.
 */
Terminal*
TerminalProgramOperation::copy() const {
    return new TerminalProgramOperation(po_);
}

/**
 * Checks if terminals are equal.
 */
bool 
TerminalProgramOperation::equals(const Terminal& other) const {

    if (!other.isInstructionAddress()) {
        return false;
    }
    return &dynamic_cast<const TerminalProgramOperation&>(other).po_ == 
        &this->po_;
}

}
