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
 * @file MultiLatencyOperationExecutor.hh
 *
 * Declaration of MultiLatencyOperationExecutor class.
 *
 * @author Pekka J‰‰skel‰inen 2008,2010 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MULTILATENCY_OPERATION_EXECUTOR_HH
#define TTA_MULTILATENCY_OPERATION_EXECUTOR_HH

#include <string>
#include <list>
#include <climits>

#include "OperationExecutor.hh"
#include "ExecutingOperation.hh"
#include "SimValue.hh"

namespace TTAMachine {
    class HWOperation;
}
class OperationContext;
class DetailedOperationSimulator;

/**
 * OperationExecutor that supports multi-output operation pipelines with 
 * different latencies for results.
 *
 * Can be also used for cycle-accurate modeling of the operation pipeline.
 * In addition to simulate the execution of operations with multiple outputs
 * with different latencies in the standalone TTA simulation, this execution 
 * model is used in the SystemC TTA core function unit simulation model.
 *
 * @see tce_systemc.hh
 */
class MultiLatencyOperationExecutor : public OperationExecutor {
public:
    MultiLatencyOperationExecutor(
        FUState& parent, TTAMachine::HWOperation& hwOp);
    MultiLatencyOperationExecutor() : hwOperation_(NULL) {}
    virtual ~MultiLatencyOperationExecutor();

    virtual void startOperation(Operation& op);

    virtual void advanceClock();
    virtual OperationExecutor* copy();
    virtual void setContext(OperationContext& context);
    virtual void setOperationSimulator(
        DetailedOperationSimulator& sim) {
        opSimulator_ = &sim;
    }

private:
    /// Assignment not allowed.
    MultiLatencyOperationExecutor& operator=(
        const MultiLatencyOperationExecutor&);

    ExecutingOperation& findFreeExecutingOperation();

    /// Operation context.
    OperationContext* context_;
    /// The hardware operation this executor simulates.
    TTAMachine::HWOperation* hwOperation_;
    /// The OSAL operation.
    Operation* operation_;
    /// The operations "on flight" in this operation executor.
    std::vector<ExecutingOperation> executingOps_;
    /// If non-NULL, points to a known free ExecutingOperation slot.
    ExecutingOperation* freeExecOp_;
    bool execOperationsInitialized_;
    /// If non-NULL, points to a detailed cycle-by-cycle simulation
    /// model for the operation simulated by this executor.
    DetailedOperationSimulator* opSimulator_;
};

#endif
