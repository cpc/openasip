/**
 * @file OperationBehavior.cc
 *
 * Implementations of OperationBehavior class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#include <string>

#include "OperationBehavior.hh"
#include "Application.hh"
#include "Exception.hh"
#include "OperationState.hh"
#include "OperationContext.hh"
#include "OperationPool.hh"

using std::string;

std::ostream* OperationBehavior::outputStream_(NULL);

/**
 * Constructor.
 */
OperationBehavior::OperationBehavior() {
}

/**
 * Destructor.
 */
OperationBehavior::~OperationBehavior() {
}

/**
 * Simulates the process of starting the execution of an operation.
 *
 * Clients should invoke isTriggerLocking() before any attempt to call 
 * simulateTrigger() in current clock cycle. By default, an operation 
 * invokations are successful.
 *
 *
 * @param io The input operands and the results of the operation.
 * @param context The operation context affecting the operation results.
 * @return bool True if all values could be computed, false otherwise.
 * @exception Exception Depends on the implementation.
 */
bool 
OperationBehavior::simulateTrigger(
    SimValue**,
    OperationContext&) const {
    return true;
}

/**
 * Checks whether any of the pending results of an operation initiated in
 * earlier cycle is ready.
 * 
 * @param io The results of the operation.
 * @param context The operation context affecting the operation results.
 * @return bool True if at least one new result of the operation could be
 * computed, false otherwise. Returns false when all results are already
 * computed. 
 */
bool 
OperationBehavior::lateResult(
    SimValue**,
    OperationContext&) const {
    
    return false;
}

/**
 * Returns the output stream which can be used by the OperationBehavior
 * definitions to print out debug information to simulator console, or to 
 * simulate output from the TTA.
 *
 * Default output stream is std::cout.
 *
 * @return the output stream.
 */
std::ostream&
OperationBehavior::outputStream() {
    if (outputStream_ == NULL) {
        outputStream_ = &std::cout;
    }
    return *outputStream_;
}

/**
 * Sets the output stream which can be used by the OperationBehavior
 * definitions to print out debug information to simulator console, or to 
 * simulate output from the TTA.
 *
 * Default output stream is std::cout.
 *
 * @param newOutputStream the output stream to use.
 */
void
OperationBehavior::setOutputStream(std::ostream& newOutputStream) {
    outputStream_ = &newOutputStream;
}


/**
 * Creates the instance of operation state for this operation and adds it to
 * its operation context.
 *
 * By default this function does nothing (assumes that the operation has no
 * state). If the operation context already contains the required operation
 * state instance, nothing is done.
 *
 * @param context The operation context to add the state to.
 */
void
OperationBehavior::createState(OperationContext&) const {
}

/**
 * Deletes the instance of operation state for this operation from its
 * operation context.
 * 
 * By default this function does nothing (assumes that the operation has no
 * state). If the operation context does not contain the required operation
 * state instance, nothing is done.
 *
 * @param context The operation context to delete the state from.
 */
void
OperationBehavior::deleteState(OperationContext&) const {
}

/**
 * Returns the name of the state of this operation behavior.
 *
 * By default returns an empty string which denotes that there is no state.
 *
 * @return The state name for this operation behavior.
 */
std::string 
OperationBehavior::stateName() const {
    return "";
}

/**
 * If behavior can be simulated.
 *
 * @return true If simulation of behavior is possible.
 */
bool 
OperationBehavior::canBeSimulated() const {
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// NullOperationBehavior
///////////////////////////////////////////////////////////////////////////////

NullOperationBehavior NullOperationBehavior::instance_;
const char* ERROR_MESSAGE =
    "Attempted to use a NULL OperationBehavior object.";

/**
 * Constructor.
 */
NullOperationBehavior::NullOperationBehavior() {
}

/**
 * Destructor.
 */
NullOperationBehavior::~NullOperationBehavior() {
}

/**
 * Writes an error message to error log and aborts the program.
 *
 * @param io Not used.
 * @param context Not used.
 * @return Never returns.
 */
bool 
NullOperationBehavior::simulateTrigger(
    SimValue**,
    OperationContext&) const {

    abortWithError(ERROR_MESSAGE);
    return true;
}

/**
 * Writes an error message to error log and aborts the program.
 *
 * @param io Not used.
 * @param context Not used.
 * @return Never returns.
 */
bool 
NullOperationBehavior::lateResult(
    SimValue**,
    OperationContext&) const {

    abortWithError(ERROR_MESSAGE);
    return true;
}

/**
 * Writes an error message to error log and aborts the program.
 */
void
NullOperationBehavior::createState(OperationContext&) const {
    abortWithError(ERROR_MESSAGE);
}

/**
 * Writes an error message to error log and aborts the program.
 */
void
NullOperationBehavior::deleteState(OperationContext&) const {
    abortWithError(ERROR_MESSAGE);
}

/**
 * Writes an error message to error log and aborts the program.
 *
 * @return Never returns.
 */
std::string 
NullOperationBehavior::stateName() const {
    abortWithError(ERROR_MESSAGE);
    return "";
}

/**
 * If behavior can be simulated.
 *
 * @return true If simulation of behavior is possible.
 */
bool 
NullOperationBehavior::canBeSimulated() const {
    return false;
}
