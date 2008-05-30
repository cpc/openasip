/**
 * @file DCMFUResourceConflictDetector.cc
 *
 * Definition of DCMFUResourceConflictDetector class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DCMFUResourceConflictDetector.hh"
#include "StringTools.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "FUReservationTableIndex.hh"
#include "FUCollisionMatrixIndex.hh"
#include "TCEString.hh"

/**
 * Constructor.
 *
 * @param fu The function unit to detect conflicts for.
 * @exception InvalidData If the model could not be built from the given FU.
 */
DCMFUResourceConflictDetector::
DCMFUResourceConflictDetector(
    const TTAMachine::FunctionUnit& fu) 
    throw (InvalidData) :
    collisionMatrices_(fu), globalCollisionMatrix_(
        collisionMatrices_.at(0).columnCount(),
        collisionMatrices_.at(0).rowCount(), false), fu_(fu) {
}

/**
 * Destructor.
 */
DCMFUResourceConflictDetector::
~DCMFUResourceConflictDetector() {
}

/**
 * Issues an operation and reports a conflict if detected.
 *
 * @param id The id of the operation to issue.
 * @return False in case a conflict is detected, otherwise true.
 */
bool
DCMFUResourceConflictDetector::issueOperation(OperationID id) {
    return issueOperationInline(id);
}

/**
 * Simulates a cycle advance and reports a conflict if detected.
 *
 * @return False in case a conflict is detected, otherwise true.
 */
bool
DCMFUResourceConflictDetector::advanceCycle() {
    return advanceCycleInline();
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
DCMFUResourceConflictDetector::OperationID 
DCMFUResourceConflictDetector::operationID(
    const TCEString& operationName) const {

    // find the index of the operation in the FU, it corresponds to the
    // operation ID
    for (int i = 0; i < fu_.operationCount(); ++i) {
        if (StringTools::ciEqual(fu_.operation(i)->name(), operationName)) {
            return i;
        }
    }
    throw KeyNotFound(__FILE__, __LINE__, __func__, "Operation not found.");
}

/**
 * Sets the state of the detector to its initial state.
 *
 * This means that the FU state is assumed to be what it is in powerup.
 */
void
DCMFUResourceConflictDetector::reset() {
    globalCollisionMatrix_.setAllToZero();
}
