/**
 * @file GCUState.cc
 *
 * Definition of GCUState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "GCUState.hh"
#include "SequenceTools.hh"
#include "OperationContext.hh"
#include "GlobalLock.hh"
#include "Application.hh"

using std::string;

/**
 * Constructor.
 *
 * @param latency Latency of the unit.
 * @param nww Natural word width as MAUs.
 * @param lock Global lock signal.
 */
GCUState::GCUState(
    int latency, 
    int nww, 
    GlobalLock& lock) : 
    FUState(lock), naturalWordWidth_(nww), returnAddressRegister_(32), 
    programCounter_(0), newProgramCounter_(0), latency_(latency), 
    operationPending_(false), operationPendingTime_(0),
    operationContext_(
        NULL, programCounter_, returnAddressRegister_) {
    programCounter_ = 0;
    returnAddressRegister_ = 0;
}

/**
 * Destructor.
 */
GCUState::~GCUState() {
}

/**
 * Returns the operation context.
 *
 * This is basically a "template method" to allow differently initialized
 * OperationContext-classes in FUState subclasses.
 *
 * @return The operation context for the FU.
 */
OperationContext&
GCUState::context() {
    return operationContext_;
}

/**
 * Handles the actions when clock is advanced.
 *
 * The value of program counter is updated. Pipeline is advanced.
 */
void
GCUState::advanceClock() {

    if (operationPending_) {
        operationPendingTime_--;
    }
    if (operationPending_ && operationPendingTime_ <= 0) {
        programCounter_ = newProgramCounter_;
        operationPending_ = false;
    } 
    FUState::advanceClock();
    idle_ = !operationPending_;
}
