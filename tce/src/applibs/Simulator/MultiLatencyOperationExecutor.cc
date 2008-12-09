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
 * @file MultiLatencyOperationExecutor.cc
 *
 * Definition of MultiLatencyOperationExecutor class.
 *
 * @author Pekka J‰‰skel‰inen 2008 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include "MultiLatencyOperationExecutor.hh"
#include "Operation.hh"
#include "SequenceTools.hh"
#include "OperationContext.hh"
#include "PortState.hh"
#include "SimulatorToolbox.hh"
#include "OperationPool.hh"
#include "Application.hh"

using std::vector;
using std::string;

/**
 * Constructor.
 *
 * @param parent Parent of the OperationExecutor.
 * @param hwOp The hardware operation to simulate.
 */
MultiLatencyOperationExecutor::MultiLatencyOperationExecutor(
    FUState& parent, TTAMachine::HWOperation& hwOp) :
    OperationExecutor(parent), context_(NULL), hwOperation_(hwOp) {
    hasPendingOperations_ = true;
}

/**
 * Destructor.
 */
MultiLatencyOperationExecutor::~MultiLatencyOperationExecutor() {
}

/**
 * Starts new operation.
 *
 * First original inputs and outputs are stored. Then outputs of the operation
 * are stored. Then operation is triggered.
 *
 * @param op Operation to be triggered.
 * @todo This can be optimized a lot: try to initialize the vector as rarely
 *       as possible.
 */
void
MultiLatencyOperationExecutor::startOperation(Operation& op) {

    const std::size_t inputOperands = op.numberOfInputs();
    const std::size_t outputOperands = op.numberOfOutputs();
    const std::size_t operandCount = inputOperands + outputOperands;

    // set the inputs to point directly to the input ports
    for (std::size_t i = 1; i <= inputOperands; ++i) {
        /// @todo create valueConst() and value() to avoid these uglies
        iovec_[i - 1] = &(const_cast<SimValue&>(binding(i).value()));
    }
    
    // set outputs to point to a value in a result queue
    for (std::size_t i = inputOperands + 1; i <= operandCount; ++i) {
        PortState& port = binding(i);
        const int resultLatency = hwOperation_.latency(i);

        PendingResult* res = new PendingResult(port, resultLatency);

        iovec_[i - 1] = &res->resultValue();
        pendingResults_.push_back(res);
    }
    op.simulateTrigger(iovec_, *context_);
    hasPendingOperations_ = true;
}

/**
 * Advances clock by one cycle.
 *
 * Takes the oldest result in the pipeline and makes it visible in the
 * function unit's ports.
 */
void
MultiLatencyOperationExecutor::advanceClock() {

    // advance clock of all pending results and remove from
    // queue if ready
    for (PendingResultQueue::iterator i = pendingResults_.begin();
         i != pendingResults_.end();) {
        PendingResult* res = *i;
        if (res->advanceCycle()) {
            i = pendingResults_.erase(i);
            delete res;
        } else {
            ++i;
        }
    }
    hasPendingOperations_ = pendingResults_.size() > 0;
}

/**
 * Copies OperationExecutor.
 *
 * @return The copied OperationExecutor.
 */
OperationExecutor*
MultiLatencyOperationExecutor::copy() {
    return new MultiLatencyOperationExecutor(*this);
}

/**
 * Sets the OperationContext for the OperationExecutor.
 *
 * @param context New OperationContext.
 */
void
MultiLatencyOperationExecutor::setContext(OperationContext& context) {
    context_ = &context;
}

