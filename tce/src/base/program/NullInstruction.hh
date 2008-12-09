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
 * @file NullInstruction.hh
 *
 * Declaration of NullInstruction class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_INSTRUCTION_HH
#define TTA_NULL_INSTRUCTION_HH

#include "Instruction.hh"

namespace TTAProgram {

/**
 * A singleton class that represents a null instruction.
 *
 * Calling any method causes the program to abort.
 */
class NullInstruction : public Instruction {
public:
    virtual ~NullInstruction();
    static NullInstruction& instance();

    Procedure& parent() const throw (IllegalRegistration);
    void setParent(const Procedure& proc);
    bool isInProcedure() const;

    void addMove(Move& move) throw (ObjectAlreadyExists);
    int moveCount();
    const Move& move(int i) const throw (OutOfRange);

    void addImmediate(Immediate& imm) throw (ObjectAlreadyExists);
    int immediateCount();
    Immediate& immediate(int i) throw (OutOfRange);

    Address& address() const throw (IllegalRegistration);

    int size() const;

protected:
    NullInstruction();

private:
    /// Copying not allowed.
    NullInstruction(const NullInstruction&);
    /// Assignment not allowed.
    NullInstruction& operator=(const NullInstruction&);

    /// Unique instance of NullInstruction.
    static NullInstruction instance_;
};

}

#endif
