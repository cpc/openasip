/**
 * @file SimpleOperationExecutor.hh
 *
 * Declaration of SimpleOperationExecutor class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
