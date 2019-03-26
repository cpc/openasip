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
 * @file TerminalFUPort.hh
 *
 * Declaration of TerminalFUPort class.
 *
 * @author Ari Mets‰halme 2005
 * @author Pekka J‰‰skel‰inen 2008,2011
 * @note rating: red
 */

#ifndef TTA_TERMINAL_FU_PORT_HH
#define TTA_TERMINAL_FU_PORT_HH

#include "Exception.hh"
#include "Terminal.hh"
#include "ProgramOperation.hh"

namespace TTAMachine {
    class HWOperation;
    class BaseFUPort;
    class FUPort;
}

namespace TTAProgram {

/**
 * Represents an input or output port of a function unit and (when
 * applicable) the operation code written into it.
 *
 * Notice that, in principle, operation codes can be written into FU output 
 * ports.
 */
class TerminalFUPort : public Terminal {
public:
    TerminalFUPort(
        const TTAMachine::FUPort& opcodeSettingPort, 
        const TTAMachine::HWOperation& opcode);

    TerminalFUPort(const TTAMachine::BaseFUPort& port)
        throw (IllegalParameters);
    TerminalFUPort(const TTAMachine::HWOperation& operation, int opIndex)
        throw (IllegalParameters);
    virtual ~TerminalFUPort();

    virtual bool isOpcodeSetting() const 
        throw (WrongSubclass);

    virtual bool isTriggering() const 
        throw (WrongSubclass);

    virtual bool isFUPort() const;
    virtual bool isRA() const;

    virtual const TTAMachine::FunctionUnit& functionUnit() const
        throw (WrongSubclass);
    virtual Operation& operation() const 
        throw (WrongSubclass, InvalidData);
    virtual Operation& hintOperation() const
        throw (WrongSubclass, InvalidData);
    // sets the "hint operation" for moves which are not opcode setting
    // but are known to be part of an operation execution
    virtual void setOperation(const TTAMachine::HWOperation& hwOp);
    virtual int operationIndex() const 
        throw (WrongSubclass, InvalidData);
    virtual void setOperationIndex(int index) { opIndex_ = index; }
    virtual const TTAMachine::Port& port() const throw (WrongSubclass);
    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;
    
    virtual const TTAMachine::HWOperation* hwOperation() const;

    /// these methods are used to group terminals belonging to a single
    /// program operation invocation
    bool hasProgramOperation() const { 
        return po_ != NULL && po_.get() != NULL; 
    }
    void setProgramOperation(ProgramOperationPtr po) { 
        po_ = po; 
    }
    ProgramOperationPtr programOperation() const { 
        return po_; 
    }

    virtual TCEString toString() const;


private:
    // copy constructor used internally by copy();
    TerminalFUPort(const TerminalFUPort& tfup);

    /// Assignment not allowed.
    TerminalFUPort& operator=(const TerminalFUPort&);
    int findNewOperationIndex() const;

    /// Port of the unit.
    const TTAMachine::BaseFUPort& port_;
    /// Operation code transported to the port.
    const TTAMachine::HWOperation* operation_;
    /// The OSAL operation.
    Operation* opcode_;
    /// Operation index.
    int opIndex_;
    /// The ProgramOperation this terminal belongs to, if applicable.
    /// The instance is shared by all the TerminalFUs belonging to 
    /// the operation.
    ProgramOperationPtr po_;
};

}

#endif
