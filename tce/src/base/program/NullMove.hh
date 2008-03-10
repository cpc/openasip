/**
 * @file NullMove.hh
 *
 * Declaration of NullMove class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
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
