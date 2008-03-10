/**
 * @file InstructionReference.hh
 *
 * Declaration of InstructionReference class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_REFERENCE_HH
#define TTA_INSTRUCTION_REFERENCE_HH

#include "Exception.hh"

namespace TTAProgram {

class Instruction;

/**
 * Represents a reference to an Instruction instance.
 *
 * The target of the reference can be changed as needed.
 */
class InstructionReference {
public:
    explicit InstructionReference(Instruction& ins);
    virtual ~InstructionReference();

    void setInstruction(Instruction& ins) throw (IllegalParameters);
    Instruction& instruction() const;

private:
    /// Copying not allowed.
    InstructionReference(const InstructionReference&);
    /// Assignment not allowed.
    InstructionReference& operator=(const InstructionReference&);

    /// Referred instruction.
    Instruction* ins_;
};

}

#endif
