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
 * @file NullTerminal.hh
 *
 * Declaration of NullTerminal class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_TERMINAL_HH
#define TTA_NULL_TERMINAL_HH

#include "Terminal.hh"

namespace TTAProgram {

/**
 * A singleton class that represents a null terminal.
 *
 * Calling any method causes the program to abort.
 */
class NullTerminal : public Terminal {
public:
    virtual ~NullTerminal();
    static NullTerminal& instance();

    virtual bool isImmediate() const;
    virtual bool isImmediateRegister() const;
    virtual bool isGPR() const;
    virtual bool isFUPort() const;
    virtual bool isOpcodeSetting() const throw (WrongSubclass);

    virtual SimValue value() const throw (WrongSubclass);
    virtual const TTAMachine::RegisterFile& registerFile() const
        throw (WrongSubclass);
    virtual const TTAMachine::ImmediateUnit& immediateUnit() const
        throw (WrongSubclass);
    virtual const TTAMachine::FunctionUnit& functionUnit() const
        throw (WrongSubclass);
    virtual int index() const throw (WrongSubclass);
    virtual Operation& operation() const throw (InvalidData);
    virtual const TTAMachine::Port& port() const throw (WrongSubclass);
    virtual void setIndex(int index) throw (OutOfRange);
    virtual void setOperation(Operation& operation) throw (WrongSubclass);
    virtual void setInstructionReference(InstructionReference& ref)
        throw(WrongSubclass);

    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;
protected:
    NullTerminal();

private:
    /// Copying not allowed.
    NullTerminal(const NullTerminal&);
    /// Assignment not allowed.
    NullTerminal& operator=(const NullTerminal&);

    /// Unique instance of NullTerminal.
    static NullTerminal instance_;
};

}

#endif
