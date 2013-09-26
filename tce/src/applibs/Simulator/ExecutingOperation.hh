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
 * @file ExecutingOperation.hh
 *
 * @author Pekka J‰‰skel‰inen 2010 (pjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_EXECUTING_OPERATION_HH
#define TTA_EXECUTING_OPERATION_HH

#include <climits>
#include <vector>

class Operation;
class PortState;
class SimValue;

/**
 * Models an operation executing in the FU pipeline.
 *
 * This is a helper class used by MultiCycleOperationExecutor to produce
 * a more detailed simulation of operations executing in the FUs. Notably,
 * this model is used in the interface to override operation simulation
 * models from SystemC simulations (see tce_systemc.hh).
 */
class ExecutingOperation {
public:
    ExecutingOperation(Operation& op, unsigned stages) : 
    stage_(0), stages_(stages), free_(true), op_(&op) {}

    /**
     * Returns the value bound to the input or output operand
     * with the given ID.
     *
     * @note operand is the OSAL ID, thus 1 is the first input and
     * the outputs follow sequentially.
     */
         
    SimValue& io(int operand);

    const Operation& operation() const { return *op_; }

    unsigned stage() const { return stage_; }

    /**
     * Returns true in case the currently simulated stage for the
     * operation is the last before finishing the execution.
     *
     * The next advanceCycle call will push the operation out from
     * the FU pipeline after which all results are assumed to have
     * arrived.
     */
    bool isLastPipelineStage() const { return stage_ == stages_ - 1; }

    /**
     * Methods that should not be called from SystemC models.
     */
    void start();
    void stop();
    void advanceCycle();

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
        void advanceCycle();
        void reset();

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
    // how many stage to simulate (the maximum of the result latencies or the
    // last pipeline resource usage)
    int stages_;
    bool free_;
    // the OSAL operation being executed
    const Operation* op_;
};

#endif
