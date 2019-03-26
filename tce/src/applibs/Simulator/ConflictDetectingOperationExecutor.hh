/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file ConflictDetectingOperationExecutor.hh
 *
 * Declaration of ConflictDetectingOperationExecutor class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
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
