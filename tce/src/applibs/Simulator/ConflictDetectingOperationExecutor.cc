/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file ConflictDetectingOperationExecutor.cc
 *
 * Definition of ConflictDetectingOperationExecutor class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
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
