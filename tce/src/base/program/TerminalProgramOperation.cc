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
#include "MoveNode.hh"
#include "Move.hh"

using namespace TTAMachine;

namespace TTAProgram {

/**
 * The constructor.
 *
 * @param po The ProgramOperation to track.
 */
TerminalProgramOperation::TerminalProgramOperation(
    std::shared_ptr<ProgramOperation> po) :
    TerminalInstructionAddress(), po_(po) {
}

/**
 * Constructs an incomplete TerminalProgramOperation.
 *
 * This can be used when a TerminalProgramOperation might point to
 * an non-existing PO during builing of the program.
 *
 * @param label A textual label of the target PO. Should be used later
 * to find the correct PO.
 */
TerminalProgramOperation::TerminalProgramOperation(
    TCEString label) : 
    TerminalInstructionAddress(), label_(label) {
}


/**
 * The destructor.
 */
TerminalProgramOperation::~TerminalProgramOperation() {
}

/**
 * Returns the instruction address of the trigger move in the tracked
 * ProgramOperation.
 *
 * Returns 0 in case the target PO is not in a program, thus its address
 * is not known.
 */
Address
TerminalProgramOperation::address() const {
    assert(isAddressKnown());
    return po_->triggeringMove()->move().parent().address();
}

bool
TerminalProgramOperation::isAddressKnown() const {
    return isProgramOperationKnown() && 
        po_->triggeringMove() != NULL &&
        po_->triggeringMove()->move().isInInstruction() &&
        po_->triggeringMove()->move().parent().isInProcedure();
}

/**
 * Creates an exact copy of the terminal and returns it.
 */
Terminal*
TerminalProgramOperation::copy() const {
    TerminalProgramOperation* newObj = new TerminalProgramOperation(po_);
    newObj->label_ = this->label_;
    return newObj;
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
