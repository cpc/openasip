/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
