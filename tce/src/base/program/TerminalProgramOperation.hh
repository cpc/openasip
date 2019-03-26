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
 * @file TerminalProgramOperation.hh
 *
 * Declaration of TerminalProgramOperation class.
 *
 * @author Pekka J‰‰skel‰inen 2011
 * @note rating: red
 */

#ifndef TTA_TERMINAL_PROGRAM_OPERATION_HH
#define TTA_TERMINAL_PROGRAM_OPERATION_HH

#include <boost/shared_ptr.hpp>
#include "TerminalInstructionAddress.hh"
#include "ProgramOperation.hh"

namespace TTAProgram {

/**
 * Represents an inline immediate that refers to a program operation (a
 * set of moves constituting a single execution of an operation in the
 * program).
 *
 * The returned instruction address is the one of the trigger move of the
 * tracked ProgramOperation. This class is used to refer to the location of
 * an another ProgramOperation strictly (not wanting to refer to the 
 * beginning of a BB).
 */
class TerminalProgramOperation : public TerminalInstructionAddress {
public:
    TerminalProgramOperation(TCEString instructionLabel);
    TerminalProgramOperation(ProgramOperationPtr po);
    virtual ~TerminalProgramOperation();

    ProgramOperationPtr programOperation() const {
        return po_;
    }

    void setProgramOperation(ProgramOperationPtr po) {
        po_ = po;
    }

    bool isProgramOperationKnown() const {
        return po_.get() != NULL;
    }

    bool isProgramOperationReference() const { return true; }
    bool isAddressKnown() const;

    virtual Address address() const 
        throw (WrongSubclass);

    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;

    TCEString label() const { return label_; }
    
private:
    /// Assignment not allowed.
    TerminalProgramOperation& operator=(const TerminalProgramOperation&);
    /// The referred PO. The pointer can be copied, e.g. to DDG.
    ProgramOperationPtr po_;
    TCEString label_;
};

}

#endif
