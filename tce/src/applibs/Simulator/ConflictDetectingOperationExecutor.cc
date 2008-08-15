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
