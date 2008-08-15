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
 * @file MultiLatencyOperationExecutor.hh
 *
 * Declaration of MultiLatencyOperationExecutor class.
 *
 * @author Pekka J‰‰skel‰inen 2008 (pjaaskel@cs.tut.fi)
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

private:
    /// Assignment not allowed.
    MultiLatencyOperationExecutor& operator=(
        const MultiLatencyOperationExecutor&);

    /**
     * Models the latency of an operation result.
     */
    class PendingResult {
    public:
        /**
         * Constructor.
         *
         * @param result A result of an operation.
         * @param target The target where to write the result.
         * @param cyclesToGo How many cycles to wait before writing.
         */
        PendingResult(PortState& target, int cyclesToGo) : 
            target_(target), cyclesToGo_(cyclesToGo) {
        }

        /**
         * Signals a cycle advance.
         *
         * In case the set latency has passed, writes the result to the
         * target. This method should not be called anymore after the cycle 
         * count has reached 0!
         *
         * @return Return true in case the latency was passed and the
         * result is not pending anymore, thus the object can be deleted.
         */
        bool
        advanceCycle() {
            --cyclesToGo_;
            if (cyclesToGo_ <= 0) {
                target_.setValue(result_);
                return true;
            } else {
                return false;
            }            
        }

        SimValue& 
        resultValue() {
            return result_;
        }

    private:
        /// The value that will be written to the target after the
        /// latency has passed.
        SimValue result_;
        /// The target to which the result will be written after the
        /// latency.
        PortState& target_;
        /// How many cycles to wait until the result will be written to
        /// the target.
        int cyclesToGo_;        
    };

    /// Queue for pending (waiting for the latency to pass) results.
    typedef std::list<PendingResult*> PendingResultQueue;
    /// Buffer for results that have latency cycles left. After cycles
    /// have passed, the results are removed from the queue.
    PendingResultQueue pendingResults_;
    /// Operation context.
    OperationContext* context_;
    /// The hardware operation this executor simulates.
    TTAMachine::HWOperation& hwOperation_;
    /// Vector for the inputs and ouputs for the operation, required
    /// by the OSAL interface. Allocated here to avoid runtime overhead
    /// of allocation.
    SimValue* iovec_[EXECUTOR_MAX_OPERAND_COUNT];

};

#endif
