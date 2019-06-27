/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @author Pekka Jääskeläinen 2008,2010 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include "MultiLatencyOperationExecutor.hh"
#include "Operation.hh"
#include "OperationContext.hh"
#include "PortState.hh"
#include "SimulatorToolbox.hh"
#include "OperationPool.hh"
#include "Application.hh"
#include "HWOperation.hh"
#include "DetailedOperationSimulator.hh"

using std::vector;
using std::string;

//#define DEBUG_OPERATION_SIMULATION

/**
 * Constructor.
 *
 * @param parent Parent of the OperationExecutor.
 * @param hwOp The hardware operation to simulate.
 */
MultiLatencyOperationExecutor::MultiLatencyOperationExecutor(
    FUState& parent, TTAMachine::HWOperation& hwOp) :
    OperationExecutor(parent), context_(NULL), hwOperation_(&hwOp),
    freeExecOp_(NULL), execOperationsInitialized_(false),
    opSimulator_(NULL) {

    OperationPool ops;
    operation_ = &ops.operation(hwOp.name().c_str());
    /**
     * Initialize a list of pending executing operations that is the
     * size of the maximum on flight operations. Each element in the
     * list is initialized to contain a large enough I/O vector for
     * inputs and outputs. In addition, the models for simulating the
     * output latencies are initialized so it's as fast as possible
     * to trigger new operations.
     */
    const std::size_t inputOperands = operation_->numberOfInputs();
    const std::size_t outputOperands = operation_->numberOfOutputs();
    const std::size_t operandCount = inputOperands + outputOperands;

    hasPendingOperations_ = true;
    ExecutingOperation eop(*operation_, hwOp.latency());
    eop.iostorage_.resize(operandCount);
    executingOps_.resize(hwOp.latency(), eop);

}

/**
 * Destructor.
 */
MultiLatencyOperationExecutor::~MultiLatencyOperationExecutor() {
}


ExecutingOperation&
MultiLatencyOperationExecutor::findFreeExecutingOperation() {
    ExecutingOperation* execOp = NULL;
    if (freeExecOp_ != NULL) {
        execOp = freeExecOp_;
        freeExecOp_ = NULL;
    } else {
        for (std::size_t i = 0; i < executingOps_.size(); ++i) {
            execOp = &executingOps_[i];
            if (execOp->free_) {
                break;
            }
        }
    }
    assert(execOp != NULL &&
           "Did not find a free ExecutingOperation.");
    return *execOp;
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
MultiLatencyOperationExecutor::startOperation(Operation&) {

    const std::size_t inputOperands = operation_->numberOfInputs();
    const std::size_t outputOperands = operation_->numberOfOutputs();
    const std::size_t operandCount = inputOperands + outputOperands;
    ExecutingOperation& execOp = findFreeExecutingOperation();

    if (!execOperationsInitialized_) {
        // cannot initialize in the constructor as the
        // FUPort -> operand bindings have not been initialized at
        // that point
        for (std::size_t i = 0; i < executingOps_.size(); ++i) {
            ExecutingOperation& execOp = executingOps_[i];
            execOp.initIOVec();
            // set the widths of the storage values to enforce correct 
            // clipping of values
            for (std::size_t o = 1; o <= operandCount; ++o) {
                execOp.iostorage_[o - 1].setBitWidth(
                    binding(o).value().width());
            }
            // set operation output storages to point to the corresponding 
            // output ports and setup their delayed appearance 
            for (std::size_t o = inputOperands + 1; o <= operandCount; ++o) {
                PortState& port = binding(o);
                const int resultLatency = hwOperation_->latency(o);

                ExecutingOperation::PendingResult res(
                    execOp.iostorage_[o - 1], port, resultLatency);
                execOp.pendingResults_.push_back(res);
            }
        }
        execOperationsInitialized_ = true;
    }
    // copy the input values to the on flight operation executor model
    for (std::size_t i = 1; i <= inputOperands; ++i) {
        execOp.iostorage_[i - 1] = binding(i).value();
    }
#ifdef DEBUG_OPERATION_SIMULATION
    Application::logStream()
        << &execOp << " (" << execOp.operation().name() 
        << ") started" << std::endl;
#endif
    execOp.start();
    hasPendingOperations_ = true;
}

/**
 * Advances clock by one cycle.
 */
void
MultiLatencyOperationExecutor::advanceClock() {

    bool foundActiveOperation = false;
    for (std::size_t i = 0; i < executingOps_.size(); ++i) {
        ExecutingOperation& execOp = executingOps_[i];
        if (execOp.free_) {
            freeExecOp_ = &execOp;
            continue;
        }

        foundActiveOperation = true;
        bool customSimulated = 
            opSimulator_ != NULL && opSimulator_->simulateStage(execOp);
        if (execOp.stage_ == 0 && !customSimulated)
            operation_->simulateTrigger(&execOp.iovec_[0], *context_);
        execOp.advanceCycle();
        
        if (execOp.stage_ == hwOperation_->latency()) {
#ifdef DEBUG_OPERATION_SIMULATION
            Application::logStream()
                << &execOp << " (" << execOp.operation().name() 
                << ") finished" << std::endl;
#endif
            execOp.stop();
        }
    }
    hasPendingOperations_ = foundActiveOperation;
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

