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
 * @file TerminalRegister.hh
 *
 * Declaration of TerminalRegister class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_TERMINAL_REGISTER_HH
#define TTA_TERMINAL_REGISTER_HH

#include "Terminal.hh"

namespace TTAMachine {
    class Unit;
    class Port;
}

namespace TTAProgram {

/**
 * Represents a reference to registers from general-purpose register
 * files and immediate units.
 *
 * These types of terminals are characterised by the fact that
 * multiple, functionally identical registers are accessed through the
 * same port. Registers are identified by an index.
 */
class TerminalRegister : public Terminal{
public:
    TerminalRegister(
        const TTAMachine::Port& port,
        int index)
        throw (InvalidData);

    /// Copying is allowed.
    //  TerminalRegister(const TerminalRegister&);

    virtual ~TerminalRegister();

    virtual bool isImmediateRegister() const;
    virtual bool isGPR() const;

    virtual const TTAMachine::RegisterFile& registerFile() const
        throw (WrongSubclass);
    virtual const TTAMachine::ImmediateUnit& immediateUnit() const
        throw (WrongSubclass);
    virtual int index() const throw (WrongSubclass);
    virtual const TTAMachine::Port& port() const throw (WrongSubclass);
    virtual void setIndex(int index) throw (OutOfRange);

    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;

private:
    /// Assignment not allowed.
    TerminalRegister& operator=(const TerminalRegister&);

    /// Unit of the terminal.
    const TTAMachine::Unit& unit_;
    /// Port of the unit.
    const TTAMachine::Port& port_;
    /// Index of the register of the register file or immediate unit.
    int index_;
    /// Unit type flag: true if immediate unit, false if register file.
    bool isImmUnit_;
};

#include "TerminalRegister.icc"

}

#endif
