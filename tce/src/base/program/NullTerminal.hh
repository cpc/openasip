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
