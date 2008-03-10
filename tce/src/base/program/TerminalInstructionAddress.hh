/**
 * @file TerminalInstructionAddress.hh
 *
 * Declaration of TerminalInstructionAddress class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_TERMINAL_INSTRUCTION_ADDRESS_HH
#define TTA_TERMINAL_INSTRUCTION_ADDRESS_HH

#include "TerminalAddress.hh"

namespace TTAProgram {

class InstructionReference;

/**
 * Represents an inline immediate that refers to an instruction address.
 */
class TerminalInstructionAddress : public TerminalImmediate {
public:
    TerminalInstructionAddress(InstructionReference& ref);
    virtual ~TerminalInstructionAddress();

    virtual bool isInstructionAddress() const;
    virtual SimValue value() const throw (WrongSubclass);
    virtual InstructionReference& instructionReference() const
        throw (WrongSubclass);
    virtual void setInstructionReference(InstructionReference& ref)
        throw(WrongSubclass);
    virtual Terminal* copy() const;
    virtual bool equals(const Terminal& other) const;

private:
    /// Assignment not allowed.
    TerminalInstructionAddress& operator=(const TerminalInstructionAddress&);
    /// Referred instruction.
    InstructionReference* ref_;
};

}

#endif
