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
 * @file TerminalInstructionAddress.hh
 *
 * Declaration of TerminalInstructionAddress class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_TERMINAL_INSTRUCTION_ADDRESS_HH
#define TTA_TERMINAL_INSTRUCTION_ADDRESS_HH

#include "TerminalAddress.hh"

namespace TTAProgram {

class InstructionReference;

/**
 * Represents an inline immediate that refers to an instruction address.
 */
class TerminalInstructionAddress : public TerminalImmediate {
public:
    TerminalInstructionAddress(InstructionReference& ref);
    virtual ~TerminalInstructionAddress();

    virtual bool isInstructionAddress() const;
    virtual SimValue value() const throw (WrongSubclass);
    virtual InstructionReference& instructionReference() const
        throw (WrongSubclass);
    virtual void setInstructionReference(InstructionReference& ref)
        throw(WrongSubclass);
    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;

private:
    /// Assignment not allowed.
    TerminalInstructionAddress& operator=(const TerminalInstructionAddress&);
    /// Referred instruction.
    InstructionReference* ref_;
};

}

#endif
