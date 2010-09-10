/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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

#include "OperationExecutor.hh"
#include "SequenceTools.hh"
#include "SimValue.hh"
#include "HWOperation.hh"

class OperationContext;


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
    virtual ~MultiLatencyOperationExecutor();

    virtual void startOperation(Operation& op);

    virtual void advanceClock();
    virtual OperationExecutor* copy();
    virtual void setContext(OperationContext& context);

protected:

    /**
     * Models an operation executing in the FU pipeline.
     */
    class ExecutingOperation {
    public:
        ExecutingOperation() : stage_(0), free_(true) {}

        void start() {
            for (std::size_t i = 0; i < pendingResults_.size(); ++i) 
                pendingResults_[i].reset();
            free_ = false;
            stage_ = 0;
        }

        void stop() {
            free_ = true;            
        }

        void advanceCycle() {
            for (std::size_t i = 0; i < pendingResults_.size(); ++i)
                pendingResults_[i].advanceCycle();
            ++stage_;
        }

        /**
         * Models the latency of an operation result.
         *
         * This is used to make results visible in the output ports of the FU at
         * the correct time.
         */
        struct PendingResult {
            PendingResult(SimValue& result, PortState& targetPort, int latency) : 
                result_(&result), target_(&targetPort), cyclesToGo_(latency),
                resultLatency_(latency) {
            }

            /**
             * Signals a cycle advance.
             *
             * Makes the result visible to the output port in time.
             */
            void advanceCycle() {
                if (--cyclesToGo_ == 0) {
                    target_->setValue(*result_);
                    cyclesToGo_ = INT_MAX;
                } 
            }

            void reset() {
                cyclesToGo_ = resultLatency_;
            }

            /// The value that will be written to the target after the
            /// latency has passed.
            SimValue* result_;
            /// The target port to which the result will be written after the
            /// latency.
            PortState* target_;
            /// How many cycles to wait until the result will be written to
            /// the target.
            int cyclesToGo_;       
            int resultLatency_;
        };

        void initIOVec() {
            for (std::size_t i = 0; i < iostorage_.size(); ++i) 
                iovec_.push_back(&iostorage_.at(i));
        }
        /* The input and output values of operation, in their OSAL order.

           The execution model can freely modify them duing the operation 
           execution simulation.
        */
        std::vector<SimValue> iostorage_;
        /// OSAL simulateTrigger() compatible I/O vector for fast execution 
        std::vector<SimValue*> iovec_;
        /* Buffer for results that have latency cycles left. After cycles
           have passed, the results are removed from the queue.
           The outputs are made visible to the FU output registers 
           automatically after the ADF declared execution latency. */
        std::vector<PendingResult> pendingResults_;
        // the stage of execution the operation is in 
        int stage_;
        bool free_;
    };
    virtual bool simulateStage(ExecutingOperation& operation);

private:
    /// Assignment not allowed.
    MultiLatencyOperationExecutor& operator=(
        const MultiLatencyOperationExecutor&);

    ExecutingOperation& findFreeExecutingOperation();

    /// Operation context.
    OperationContext* context_;
    /// The hardware operation this executor simulates.
    TTAMachine::HWOperation& hwOperation_;
    /// The OSAL operation.
    Operation* operation_;
    /// The operations "on flight" in this operation executor.
    std::vector<ExecutingOperation> executingOps_;
    /// If non-NULL, points to a known free ExecutingOperation slot.
    ExecutingOperation* freeExecOp_;
    bool execOperationsInitialized_;
};

#endif
