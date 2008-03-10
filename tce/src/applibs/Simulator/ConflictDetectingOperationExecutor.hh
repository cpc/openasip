/**
 * @file ConflictDetectingOperationExecutor.hh
 *
 * Declaration of ConflictDetectingOperationExecutor class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONFLICT_DETECTING_OPERATION_EXECUTOR_HH
#define TTA_CONFLICT_DETECTING_OPERATION_EXECUTOR_HH

#include "FUResourceConflictDetector.hh"
#include "SimulatorConstants.hh"
#include "MultiLatencyOperationExecutor.hh"

/**
 * OperationExecutor that checks for FU resource conflicts.
 *
 * This kind of OperationExecutor is created for each operation in the
 * simulated FU. This is purely for optimization purposes: as we need to
 * check the resource conflicts using a per operation ID, it's faster to
 * use a constant ID for requesting the conflict detection data instead of
 * first fetching the constant from a map for the executed operation.
 */
class ConflictDetectingOperationExecutor : 
    public MultiLatencyOperationExecutor {
public:
    ConflictDetectingOperationExecutor(
        FUResourceConflictDetector& detector,
        FUResourceConflictDetector::OperationID operationID,
        FUState& parent,
        TTAMachine::HWOperation& hwOp,
        bool throwWhenConflict);

    virtual ~ConflictDetectingOperationExecutor();

    virtual void startOperation(Operation& op);
    virtual void advanceClock();

    virtual OperationExecutor* copy();

private:
    /// Assignment not allowed.
    ConflictDetectingOperationExecutor& 
    operator=(const ConflictDetectingOperationExecutor&);
    /// The conflict detector used to test whether starting operation is
    /// legal.
    FUResourceConflictDetector& detector_;
    /// The conflict detector ID of the operation executed with this executor.
    const FUResourceConflictDetector::OperationID operationID_;
    /// True in case an exception should be thrown in case of a resource 
    /// conflict.
    const bool throwWhenConflict_;
    /// Counter for detected conflicts.
    ClockCycleCount conflictCounter_;
};

#endif
