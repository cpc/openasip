/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
