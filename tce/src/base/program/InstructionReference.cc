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
 * @file InstructionReference.cc
 *
 * Implementation of InstructionReference class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "InstructionReference.hh"
#include "NullInstruction.hh"

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// InstructionReference
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @note Instruction references should be created using
 * InstructionReferenceManager.
 * @param ins Referred instruction.
 */
InstructionReference::InstructionReference(Instruction& ins):
    ins_(&ins) {
}

/**
 * Destructor.
 */
InstructionReference::~InstructionReference() {
}

/**
 * Sets a new referred instruction.
 *
 * @param ins New referred instruction.
 * @exception IllegalParameters if the instruction is a null instruction.
 */
void
InstructionReference::setInstruction(Instruction& ins)
    throw (IllegalParameters) {

    if (&ins != &NullInstruction::instance()) {
        ins_ = &ins;
    } else {
        throw IllegalParameters(
            __FILE__, __LINE__, "InstructionReference::setInstruction()",
            "Cannot refer to a null instruction.");
    }
}

/**
 * Returns the referred instruction.
 *
 * @return Referred instruction.
 */
Instruction&
InstructionReference::instruction() const {
    return *ins_;
}

}
