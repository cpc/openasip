/**
 * @file NullInstructionReferenceManager.cc
 *
 * Implementation of NullInstructionReferenceManager class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "NullInstructionReferenceManager.hh"
#include "NullProcedure.hh"

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// NullInstructionReferenceManager
/////////////////////////////////////////////////////////////////////////////

NullInstructionReferenceManager NullInstructionReferenceManager::instance_;

/**
 * The constructor.
 */
NullInstructionReferenceManager::NullInstructionReferenceManager():
    InstructionReferenceManager() {
}

/**
 * The destructor.
 */
NullInstructionReferenceManager::~NullInstructionReferenceManager() {
}

/**
 * Returns an instance of NullInstructionReferenceManager class (singleton).
 *
 * @return Singleton instance of NullInstructionReferenceManager class.
 */
NullInstructionReferenceManager&
NullInstructionReferenceManager::instance() {
    return instance_;
}

}
