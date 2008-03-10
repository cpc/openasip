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
