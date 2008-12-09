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
 * @file SimpleOperationExecutor.hh
 *
 * Declaration of SimpleOperationExecutor class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMPLE_OPERATION_EXECUTOR_HH
#define TTA_SIMPLE_OPERATION_EXECUTOR_HH

#include <string>

#include "OperationExecutor.hh"
#include "SequenceTools.hh"
#include "SimValue.hh"

class OperationContext;

/**
 * Simple implementation of OperationExecutor.
 */
class SimpleOperationExecutor : public OperationExecutor {
public:
    SimpleOperationExecutor(
        int latency,  
        FUState& parent);
    virtual ~SimpleOperationExecutor();

    virtual int latency() const;
    virtual void startOperation(Operation& op);

    virtual void advanceClock();
    virtual OperationExecutor* copy();
    virtual void setContext(OperationContext& context);

private:
    /// Assignment not allowed.
    SimpleOperationExecutor& operator=(const SimpleOperationExecutor&);

    /**
     * Models one cell of the ring buffer of operations.
     */
    struct BufferCell {
        /**
         * Constructor.
         */
        BufferCell() : 
            operation_(NULL), ready_(false), boundOperation_(NULL) {}
        /// Inputs and outputs for the operation.
        SimValue* io_[EXECUTOR_MAX_OPERAND_COUNT];
        /// Original inputs and outputs of the operation. 
        /// @note the limitation on max io ports to
        /// avoid dynamic allocation of the array.
        SimValue ioOrig_[EXECUTOR_MAX_OPERAND_COUNT];
        /// Operation to be executed, set to NULL when results have been 
        /// moved to output ports.
        Operation* operation_;
        /// Flag telling whether results are ready or not.
        bool ready_;
        /// Tells the operation for which the bindings are initialized in
        /// this "pipeline cell", NULL in case bindings have not been 
        /// initialized.
        Operation* boundOperation_;
    };

    /// Ring buffer type for the pipeline slots.
    typedef std::vector<BufferCell> Buffer;
    /// Position of the ring buffer where to put the next triggered operation.
    int nextSlot_;
    /// Ring buffer for the pipeline slots.
    Buffer buffer_;
    /// Operation context.
    OperationContext* context_;
    /// Count of pending operations in the pipeline.
    std::size_t pendingOperations_;
};

#endif
