/**
 * @file ConflictDetectingOperationExecutor.cc
 *
 * Definition of ConflictDetectingOperationExecutor class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "ConflictDetectingOperationExecutor.hh"
#include "Exception.hh"
#include "Operation.hh"
#include "TCEString.hh"

using std::vector;
using std::string;

/**
 * Constructor.
 *
 * @param detector The conflict detector.
 * @param operationID The conflict detector ID of the operation executed by
 * this operation executor.
 * @param parent Parent of the OperationExecutor.
 * @param throwWhenConflict In case an exception should be thrown in case of
 * a resource conflict (otherwise the conflicts are only counted).
 */
ConflictDetectingOperationExecutor::ConflictDetectingOperationExecutor(
    FUResourceConflictDetector& detector,
    FUResourceConflictDetector::OperationID operationID,
    FUState& parent, 
    TTAMachine::HWOperation& hwOp,
    bool throwWhenConflict) :
    MultiLatencyOperationExecutor(parent, hwOp), detector_(detector), 
    operationID_(operationID), throwWhenConflict_(throwWhenConflict),
    conflictCounter_(0) {
}

/**
 * Destructor.
 */
ConflictDetectingOperationExecutor::~ConflictDetectingOperationExecutor() {
#if 0
    if (conflictCounter_ > 0)
        std::cout << " (" << conflictCounter_ << " conflicts) ";
#endif
}

/**
 * Starts new operation.
 *
 * Checks for resource conflicts before actually simulating the operation
 * using the parent class implementation.
 *
 * @param op Operation to be triggered.
 */
void
ConflictDetectingOperationExecutor::startOperation(Operation& op) {

    if (!detector_.issueOperation(operationID_)) {
        ++conflictCounter_;
        if (throwWhenConflict_)
            throw SimulationExecutionError(
                __FILE__, __LINE__, __func__, 
                (boost::format(
                    "Resource conflict detected: operation %s.") 
                 % op.name()).str());
        return;
    }

    MultiLatencyOperationExecutor::startOperation(op);

    // we need to receive the advanceClock() on every clock advance to update
    // the resource conflict model
    hasPendingOperations_ = true;
}

/**
 * Advances clock by one cycle.
 *
 * Updates the resource conflict model. 
 */
void
ConflictDetectingOperationExecutor::advanceClock() {

    MultiLatencyOperationExecutor::advanceClock();
    // we need to receive the advanceClock() on every clock advance to update
    // the resource conflict model
    hasPendingOperations_ = true;
}

/**
 * Copies OperationExecutor.
 *
 * @return The copied OperationExecutor.
 */
OperationExecutor*
ConflictDetectingOperationExecutor::copy() {
    return new ConflictDetectingOperationExecutor(*this);
}
