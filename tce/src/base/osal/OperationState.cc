/**
 * @file OperationState.cc
 *
 * Non-inline definitions of OperationState class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#include "Application.hh"
#include "OperationState.hh"
#include "OperationContext.hh"

NullOperationState* NullOperationState::instance_ = NULL;

/**
 * Constructor.
 */
OperationState::OperationState() {
}

/**
 * Destructor.
 */
OperationState::~OperationState() {
}

/**
 * Returns true if operation state is in "available" state. 
 *
 * If operation state is available, it means that new operations can be
 * started. If it's not avaiable, no further operations should be invoked.
 *
 * @param context The operation context of which availability to check.
 * @return True if operation state is in available state. By default, operation
 *              state is available.
 */
bool 
OperationState::isAvailable(const OperationContext&) const {
    return true;
}

/**
 * This method should be invoced in every clock cycle.
 *
 * If this method is not invoced on every clock cycle, some pending results 
 * may get lost.
 *
 * @param context The operation context which should be used for calculating
 *                the pending results.
 */
void 
OperationState::advanceClock(OperationContext&) {
}

///////////////////////////////////////////////////////////////////////////////
// NullOperationState
///////////////////////////////////////////////////////////////////////////////


const std::string NULL_ERROR_MESSAGE = 
    "Tried to access NULL OperationState object.";
/**
 * Returns the name of the state instance, in this case an empty string.
 *
 * @return Empty string.
 */
std::string
NullOperationState::name() {
    return "";
}

/**
 * Prints an error message and aborts the program.
 *
 * @return Never returns.
 */
bool 
NullOperationState::isAvailable(const OperationContext&) const {
    abortWithError(NULL_ERROR_MESSAGE);
    return false;
}

/**
 * Prints an error message and aborts the program.
 *
 * @return Never returns.
 */
void 
NullOperationState::advanceClock(OperationContext&) {
    abortWithError(NULL_ERROR_MESSAGE);
}
