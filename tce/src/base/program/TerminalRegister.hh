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
    TerminalRegister(const TTAMachine::Port& port, int index);

    /// Copying is allowed.
    //  TerminalRegister(const TerminalRegister&);

    virtual ~TerminalRegister();

    virtual bool isImmediateRegister() const;
    virtual bool isGPR() const;
    virtual bool isUniversalMachineRegister() const;

    virtual const TTAMachine::RegisterFile& registerFile() const;
    virtual const TTAMachine::ImmediateUnit& immediateUnit() const;
    virtual int index() const;
    virtual const TTAMachine::Port& port() const;
    virtual void setIndex(int index);

    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;

    virtual TCEString toString() const;

private:
    /// Assignment not allowed.
    TerminalRegister& operator=(const TerminalRegister&);

    /// Unit of the terminal.
    const TTAMachine::Unit& unit_;
    /// Port of the unit.
    const TTAMachine::Port& port_;
    /// Index of the register of the register file or immediate unit.
    short index_;
    /// Unit type flag: true if immediate unit, false if register file.
    bool isImmUnit_;
};

#include "TerminalRegister.icc"

}

#endif
