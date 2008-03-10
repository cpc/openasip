/**
 * @file NullInstructionReferenceManager.hh
 *
 * Declaration of NullInstructionReferenceManager class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_INSTRUCTIONREFERENCEMANAGER_HH
#define TTA_NULL_INSTRUCTIONREFERENCEMANAGER_HH

#include "InstructionReferenceManager.hh"

namespace TTAProgram {

/**
 * A singleton class that represents a null global scope.
 */
class NullInstructionReferenceManager : public InstructionReferenceManager {
public:
    virtual ~NullInstructionReferenceManager();
    static NullInstructionReferenceManager& instance();

protected:
    NullInstructionReferenceManager();

private:
    /// Copying not allowed.
    NullInstructionReferenceManager(const NullInstructionReferenceManager&);
    /// Assignment not allowed.
    NullInstructionReferenceManager& operator=(
        const NullInstructionReferenceManager&);

    /// Unique instance of NullInstructionReferenceManager.
    static NullInstructionReferenceManager instance_;
};

}

#endif
