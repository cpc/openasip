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
 * @file TerminalFUPort.hh
 *
 * Declaration of TerminalFUPort class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_TERMINAL_FU_PORT_HH
#define TTA_TERMINAL_FU_PORT_HH

#include "Exception.hh"
#include "Terminal.hh"

namespace TTAMachine {
    class HWOperation;
    class BaseFUPort;
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
    TerminalFUPort(const TTAMachine::BaseFUPort& port)
        throw (IllegalParameters);
    TerminalFUPort(TTAMachine::HWOperation& operation, int opIndex)
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
    virtual std::string HWOperationName() const;
    virtual Operation& operation() const 
        throw (WrongSubclass, InvalidData);
    virtual Operation& hintOperation() const
        throw (WrongSubclass, InvalidData);
    virtual int operationIndex() const 
        throw (WrongSubclass, InvalidData);
    virtual const TTAMachine::Port& port() const throw (WrongSubclass);
    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;
    
    virtual TTAMachine::HWOperation* hwOperation() const;

private:
    // copy constructor used internally by copy();
    TerminalFUPort(const TerminalFUPort& tfup);

    /// Assignment not allowed.
    TerminalFUPort& operator=(const TerminalFUPort&);
    int findNewOperationIndex() const;

    /// Port of the unit.
    const TTAMachine::BaseFUPort& port_;
    /// Operation code transported to the port.
    TTAMachine::HWOperation* operation_;
    /// The OSAL operation.
    Operation* opcode_;
    /// Operation index.
    int opIndex_;
};

}

#endif
