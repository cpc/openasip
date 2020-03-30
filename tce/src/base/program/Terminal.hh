/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file Terminal.hh
 *
 * Declaration of Terminal class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_TERMINAL_HH
#define TTA_TERMINAL_HH

#include "BaseType.hh"
#include "SimValue.hh"
#include "Address.hh"
#include "Exception.hh"
#include "TCEString.hh"
#include "Application.hh"

class Operation;

namespace TTAMachine {
    class RegisterFile;
    class ImmediateUnit;
    class FunctionUnit;
    class Port;
}

namespace TTAProgram {

class BasicBlock;
class InstructionReference;

/**
 * Models any possible source or destination of a move.
 */
class Terminal {
public:
    Terminal();
    virtual ~Terminal();

    virtual bool isImmediate() const;
    virtual bool isAddress() const;
    virtual bool isInstructionAddress() const;
    virtual bool isImmediateRegister() const;
    virtual bool isGPR() const;
    virtual bool isFUPort() const;
    virtual bool isRA() const;
    virtual bool isBasicBlockReference() const;
    virtual bool isProgramOperationReference() const;
    virtual bool isCodeSymbolReference() const;
    virtual bool isUniversalMachineRegister() const;
    virtual SimValue value() const;
    virtual Address address() const;
    virtual const InstructionReference& instructionReference() const;
    virtual InstructionReference& instructionReference();
    virtual const TTAMachine::RegisterFile& registerFile() const;
    virtual const TTAMachine::ImmediateUnit& immediateUnit() const;
    virtual const TTAMachine::FunctionUnit& functionUnit() const;
    virtual const BasicBlock& basicBlock() const;
    virtual int index() const;

    virtual bool isOpcodeSetting() const;

    virtual bool isTriggering() const;

    virtual Operation& operation() const;

    virtual Operation& hintOperation() const;

    virtual int operationIndex() const;
    virtual const TTAMachine::Port& port() const;
    virtual void setIndex(int index);
    virtual void setInstructionReference(InstructionReference ref);

    virtual Terminal* copy() const = 0;
    
    bool operator==(const Terminal& other) const;

    virtual bool equals(const Terminal& other) const = 0;

    virtual TCEString toString() const = 0;
};

}

#endif
