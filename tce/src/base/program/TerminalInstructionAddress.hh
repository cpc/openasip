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
 * @file TerminalInstructionAddress.hh
 *
 * Declaration of TerminalInstructionAddress class.
 *
 * @author Ari Mets‰halme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2011
 * @note rating: red
 */

#ifndef TTA_TERMINAL_INSTRUCTION_ADDRESS_HH
#define TTA_TERMINAL_INSTRUCTION_ADDRESS_HH

#include "TerminalImmediate.hh"

namespace TTAProgram {

/**
 * An interface for Terminals that represent inline immediates that refer 
 * to (absolute) instruction addresses.
 */
class TerminalInstructionAddress : public TerminalImmediate {
public:
    TerminalInstructionAddress();
    virtual ~TerminalInstructionAddress();

    bool isInstructionAddress() const { return true; }
    // the value as an instructionAddress()
    virtual Address address() const 
        throw (WrongSubclass) = 0;
    SimValue value() const { 
        return SimValue(address().location(), WORD_BITWIDTH); 
    }
    virtual Terminal* copy() const = 0;
    virtual bool equals(const Terminal& other) const = 0;

private:
    /// Assignment not allowed.
    TerminalInstructionAddress& operator=(const TerminalInstructionAddress&);
};

}

#endif
