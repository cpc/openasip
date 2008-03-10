/**
 * @file NullInstruction.hh
 *
 * Declaration of NullInstruction class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
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
