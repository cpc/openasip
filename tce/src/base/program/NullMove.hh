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
 * @file NullMove.hh
 *
 * Declaration of NullMove class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_MOVE_HH
#define TTA_NULL_MOVE_HH

#include "Move.hh"

namespace TTAProgram {

/**
 * A singleton class that represents a null move.
 *
 * Calling any method causes the program to abort.
 */
class NullMove : public Move {
public:
    virtual ~NullMove();
    static NullMove& instance();

    Instruction& parent() const throw (IllegalRegistration);
    void setParent(Instruction& ins);
    bool isUnconditional() const;
    bool isInInstruction() const;
    Terminal& source() const;
    void setSource(Terminal* src);
    Terminal& destination() const;
    void setDestination(Terminal* dst);
    MoveGuard& guard() const throw (InvalidData);
    void setGuard(MoveGuard* guard);
    TTAMachine::Bus& bus() const;
    void setBus(TTAMachine::Bus& bus);
    TTAMachine::Socket& destinationSocket() const;
    TTAMachine::Socket& sourceSocket() const throw (WrongSubclass);

protected:
    NullMove();

private:
    /// Copying not allowed.
    NullMove(const NullMove&);
    /// Assignment not allowed.
    NullMove& operator=(const NullMove&);

    /// Unique instance of NullMove.
    static NullMove instance_;
};

}

#endif
