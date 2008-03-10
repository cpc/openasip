/**
 * @file ResourceVectorFUResourceConflictDetector.cc
 *
 * Definition of ResourceVectorFUResourceConflictDetector class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <fstream>

#include "ResourceVectorFUResourceConflictDetector.hh"
#include "StringTools.hh"
#include "ResourceVectorSet.hh"


/**
 * Constructor.
 *
 * @param fu The function unit to detect conflicts for.
 * @exception InvalidData If the model could not be built from the given FU.
 */
ResourceVectorFUResourceConflictDetector::
ResourceVectorFUResourceConflictDetector(
    const TTAMachine::FunctionUnit& fu) throw (InvalidData) :
    vectors_(fu) {
}

/**
 * Destructor.
 */
ResourceVectorFUResourceConflictDetector::
~ResourceVectorFUResourceConflictDetector() {
}

/**
 * Issues an operation and reports a conflict if detected.
 *
 * @param id The id of the operation to issue.
 * @return False in case a conflict is detected, otherwise true.
 */
bool
ResourceVectorFUResourceConflictDetector::issueOperation(OperationID id) {

    const ResourceVector& operationVector = vectors_.resourceVector(id);
    if (compositeVector_.conflictsWith(operationVector, 1))
        return false;
    compositeVector_.mergeWith(operationVector, 1);
    return true;
}

/**
 * Simulates a cycle advance and reports a conflict if detected.
 *
 * @return False in case a conflict is detected, otherwise true.
 */
bool
ResourceVectorFUResourceConflictDetector::advanceCycle() {

    compositeVector_.shiftLeft();
    return true;
}

/**
 * Returns an operation id for the given operation.
 *
 * Operation IDs are used in the interface for optimizing the access. This
 * method converts OSAL Operations to operation IDs. 
 *
 * @param operation The OSAL Operation to find ID for.
 * @return The operation ID.
 */
ResourceVectorFUResourceConflictDetector::OperationID 
ResourceVectorFUResourceConflictDetector::operationID(
    const std::string& operationName) const
    throw (KeyNotFound) {

    return vectors_.operationIndex(StringTools::stringToUpper(operationName));
}

/**
 * Sets the state of the detector to its initial state.
 *
 * This means that the FU state is assumed to be what it is in powerup.
 */
void
ResourceVectorFUResourceConflictDetector::reset() {
    compositeVector_.clear();
}
