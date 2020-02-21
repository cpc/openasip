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
 * @file Move.hh
 *
 * Declaration of Move class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_MOVE_HH
#define TTA_MOVE_HH

#include <memory>

#include "Exception.hh"
#include "AnnotatedInstructionElement.hh"

namespace TTAMachine {
    class Bus;
    class Socket;
}

namespace TTAProgram {

class Terminal;
class MoveGuard;
class Instruction;

/**
 * Represents a data transport through the TTA programmable
 * interconnection network.
 */
class Move : public AnnotatedInstructionElement {
public:
    Move(
        Terminal* src,
        Terminal* dst,
        const TTAMachine::Bus& bus,
        MoveGuard* guard);
    Move(
        Terminal* src, Terminal* dst, const TTAMachine::Bus& bus);
    ~Move();

    Instruction& parent() const;
    void setParent(Instruction& ins);
    bool isInInstruction() const;
    bool isUnconditional() const;

    bool isJump() const;
    bool isCall() const;
    bool isControlFlowMove() const;
    bool isReturn() const;

    bool isTriggering() const;

    Terminal& source() const;
    void setSource(Terminal* src);
    Terminal& destination() const;
    void setDestination(Terminal* dst);
    MoveGuard& guard() const;
    void setGuard(MoveGuard* guard);
    const TTAMachine::Bus& bus() const;
    void setBus(const TTAMachine::Bus& bus);
    TTAMachine::Socket& destinationSocket() const;
    TTAMachine::Socket& sourceSocket() const;
    int guardLatency() const;
    std::shared_ptr<Move> copy() const;

    std::string toString() const;

    bool hasSourceLineNumber() const;
    int sourceLineNumber() const;
    bool hasSourceFileName() const;
    std::string sourceFileName() const;   
    
private:
    /// Copying not allowed.
    Move(const Move&);
    /// Assignment not allowed.
    Move& operator=(const Move&);

    /// Parent instruction of the move.
    Instruction* parent_;
    /// The source of the move.
    Terminal* src_;
    /// The destination of the move.
    Terminal* dst_;
    /// The bus where the transport is carried.
    const TTAMachine::Bus* bus_;
    /// The boolean expression that predicates the move.
    MoveGuard* guard_;
};

}

#endif
