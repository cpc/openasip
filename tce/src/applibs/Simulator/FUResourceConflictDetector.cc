/**
 * @file FUResourceConflictDetector.cc
 *
 * Definition of FUResourceConflictDetector class.
 *
 * @author Pekka Jääskeläinen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "FUResourceConflictDetector.hh"
#include "StringTools.hh"
#include "OperationIDIndex.hh"
#include "TCEString.hh"
#include "Exception.hh"

/**
 * Constructor.
 */
FUResourceConflictDetector::FUResourceConflictDetector() :
    operationIndices_(new OperationIDIndex()) {
    reset();
}

/**
 * Destructor.
 */
FUResourceConflictDetector::~FUResourceConflictDetector() {
    delete operationIndices_;
    operationIndices_ = NULL;
}

/**
 * Issues an operation and reports a conflict if detected.
 *
 * The default implementation always returns true, thus detects no conflicts.
 *
 * @param id The id of the operation to issue.
 * @return False in case a conflict is detected, otherwise true.
 */
bool
FUResourceConflictDetector::issueOperation(OperationID) {
    return true;
}

/**
 * Simulates a cycle advance and reports a conflict if detected.
 *
 * The default implementation always returns true, thus detects no conflicts.
 * This interface is provided for allowing the model to update its state at
 * the point simulation advances to the next cycle.
 *
 * @return False in case a conflict is detected, otherwise true.
 */
bool
FUResourceConflictDetector::advanceCycle() {
    return true;
}

/**
 * Simulates a cycle advance and reports a conflict by throwing an exception.
 */
void
FUResourceConflictDetector::advanceClock() {
    advanceCycle();
}


/**
 * Called after advanceClock() has been called to all clocked states.
 */
void
FUResourceConflictDetector::endClock() {
}

/**
 * Should return true in case the model needs to receive clock advance calls.
 *
 * @return By default always returns false.
 */
bool
FUResourceConflictDetector::isIdle() {
    return false;    
}

/**
 * Returns an operation id for the given operation.
 *
 * Operation IDs are used in the interface for optimizing the access. This
 * method converts OSAL Operations to operation IDs. 
 *
 * @param operation The OSAL Operation to find ID for.
 * @return The operation ID.
 * @exception KeyNotFound if the operation of given name couldn't be found
 */
FUResourceConflictDetector::OperationID 
FUResourceConflictDetector::operationID(const TCEString& operationName) const
{
    OperationIDIndex::const_iterator i = 
        operationIndices_->find(StringTools::stringToLower(operationName));
    if (i == operationIndices_->end())
        throw KeyNotFound(
            __FILE__, __LINE__, __func__, "Operation not found.");
    return (*i).second;
}

/**
 * Sets the state of the detector to its initial state.
 *
 * This means that the FU state is assumed to be what it is in powerup.
 */
void
FUResourceConflictDetector::reset() {
}
