/**
 * @file InstructionReferenceManager.hh
 *
 * Declaration of InstructionReferenceManager class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_REFERENCE_MANAGER_HH
#define TTA_INSTRUCTION_REFERENCE_MANAGER_HH

#include <vector>
#include "Exception.hh"

namespace TTAProgram {

class Instruction;
class InstructionReference;

/**
 * Helps in keeping instructions referenced in POM up-to-date.
 *
 * Instructions are not referenced directly, but through an
 * InstructionReference instance. The actual Instruction instance
 * referred can be changed as needed.
 */
class InstructionReferenceManager {
public:
    InstructionReferenceManager();
    virtual ~InstructionReferenceManager();

    InstructionReference& createReference(Instruction& ins);
    InstructionReference& replace(Instruction& insA, Instruction& insB)
        throw (InstanceNotFound);
    void clearReferences();
    bool hasReference(Instruction& ins) const;
    int referenceCount() const;
    InstructionReference& reference(int index) const throw (OutOfRange);
    InstructionReferenceManager* copy() const;

private:
    /// List for instruction references.
    typedef std::vector<InstructionReference*> RefList;
    /// Instruction references to maintain.
    RefList references_;
};

}

#endif
