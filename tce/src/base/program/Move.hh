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
 * @file Move.hh
 *
 * Declaration of Move class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_MOVE_HH
#define TTA_MOVE_HH

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
        TTAMachine::Bus& bus,
        MoveGuard* guard);
    Move(
        Terminal* src, Terminal* dst, TTAMachine::Bus& bus);
    virtual ~Move();

    Instruction& parent() const throw (IllegalRegistration);
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
    MoveGuard& guard() const throw (InvalidData);
    void setGuard(MoveGuard* guard);
    TTAMachine::Bus& bus() const;
    void setBus(TTAMachine::Bus& bus);
    TTAMachine::Socket& destinationSocket() const;
    TTAMachine::Socket& sourceSocket() const throw (WrongSubclass);

    Move* copy() const;

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
    TTAMachine::Bus* bus_;
    /// The boolean expression that predicates the move.
    MoveGuard* guard_;
};

}

#endif
