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

class Operation;

namespace TTAMachine {
    class RegisterFile;
    class ImmediateUnit;
    class FunctionUnit;
    class Port;
}

namespace TTAProgram {

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
    virtual SimValue value() const throw (WrongSubclass);
    virtual Address address() const throw (WrongSubclass);
    virtual InstructionReference& instructionReference() const
        throw (WrongSubclass);
    virtual const TTAMachine::RegisterFile& registerFile() const
        throw (WrongSubclass);
    virtual const TTAMachine::ImmediateUnit& immediateUnit() const
        throw (WrongSubclass);
    virtual const TTAMachine::FunctionUnit& functionUnit() const
        throw (WrongSubclass);
    virtual int index() const 
        throw (WrongSubclass);

    virtual bool isOpcodeSetting() const 
        throw (WrongSubclass);

    virtual bool isTriggering() const 
        throw (WrongSubclass);

    virtual Operation& operation() const 
        throw (WrongSubclass, InvalidData);

    virtual Operation& hintOperation() const 
        throw (WrongSubclass, InvalidData);

    virtual int operationIndex() const 
        throw (WrongSubclass, InvalidData);

    virtual const TTAMachine::Port& port() const throw (WrongSubclass);
    virtual void setIndex(int index) throw (WrongSubclass, OutOfRange);
    virtual void setInstructionReference(InstructionReference& ref)
        throw(WrongSubclass);

    virtual Terminal* copy() const = 0;
    
    bool operator==(const Terminal& other) const;

    virtual bool equals(const Terminal& other) const = 0;
};

}

#endif
