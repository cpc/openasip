/**
 * @file InstructionReference.cc
 *
 * Implementation of InstructionReference class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "InstructionReference.hh"
#include "NullInstruction.hh"

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// InstructionReference
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @note Instruction references should be created using
 * InstructionReferenceManager.
 * @param ins Referred instruction.
 */
InstructionReference::InstructionReference(Instruction& ins):
    ins_(&ins) {
}

/**
 * Destructor.
 */
InstructionReference::~InstructionReference() {
}

/**
 * Sets a new referred instruction.
 *
 * @param ins New referred instruction.
 * @exception IllegalParameters if the instruction is a null instruction.
 */
void
InstructionReference::setInstruction(Instruction& ins)
    throw (IllegalParameters) {

    if (&ins != &NullInstruction::instance()) {
        ins_ = &ins;
    } else {
        throw IllegalParameters(
            __FILE__, __LINE__, "InstructionReference::setInstruction()",
            "Cannot refer to a null instruction.");
    }
}

/**
 * Returns the referred instruction.
 *
 * @return Referred instruction.
 */
Instruction&
InstructionReference::instruction() const {
    return *ins_;
}

}
