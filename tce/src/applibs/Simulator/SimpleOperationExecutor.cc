/**
 * @file SimpleOperationExecutor.cc
 *
 * Definition of SimpleOperationExecutor class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @author Viljami Korhonen 2007 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#include "SimpleOperationExecutor.hh"
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
 * @param latency Latency of the operation.
 * @param parent Parent of the OperationExecutor.
 */
SimpleOperationExecutor::SimpleOperationExecutor(
    int latency, 
    FUState& parent) : 
    OperationExecutor(parent), nextSlot_(0), context_(NULL), 
    pendingOperations_(0) {
    
    for (int i = 0; i < latency; i++) {
        BufferCell cell;
        buffer_.push_back(cell);
    }
    hasPendingOperations_ = true;
}

/**
 * Destructor.
 */
SimpleOperationExecutor::~SimpleOperationExecutor() {
}

/**
 * Returns the latency of the executor.
 *
 * @return The latency of the executor.
 */
int
SimpleOperationExecutor::latency() const {
    return buffer_.size();
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
SimpleOperationExecutor::startOperation(Operation& op) {

    BufferCell& nextSlot = buffer_[nextSlot_];

    const bool reuseBindings = 
        nextSlot.boundOperation_ != NULL && nextSlot.boundOperation_ == &op;

    if (!reuseBindings) {
        const std::size_t inputOperands = op.numberOfInputs();
        const std::size_t outputOperands = op.numberOfOutputs();
        const std::size_t operandCount = inputOperands + outputOperands;

        assert(operandCount <= EXECUTOR_MAX_OPERAND_COUNT);
        // let the operation access the input port values directly,
        for (std::size_t i = 1; i <= inputOperands; ++i) {
            /// @todo create valueConst() and value() to avoid these uglies
            nextSlot.io_[i - 1] = &(const_cast<SimValue&>(binding(i).value()));
        }

        // create new temporary SimValues for the outputs, assume
        // indexing of outputs starts after inputs 
        /// @todo Fix! This should not probably be assumed, or at least
        /// user should be notified if his operand ids are not what
        /// are expected.
        for (std::size_t i = inputOperands + 1; i <= operandCount; ++i) {    
            nextSlot.io_[i - 1] = &nextSlot.ioOrig_[i - 1];
        }
        nextSlot.boundOperation_ = &op;
    }

    nextSlot.operation_ = &op;
    nextSlot.ready_ = op.simulateTrigger(nextSlot.io_, *context_);
    ++pendingOperations_;
    hasPendingOperations_ = true;
}

/**
 * Advances clock by one cycle.
 *
 * Takes the oldest result in the pipeline and makes it visible in the
 * function unit's ports.
 */
void
SimpleOperationExecutor::advanceClock() {

    BufferCell& nextSlot = buffer_[nextSlot_];
   
    int index = (nextSlot_ + 1) % buffer_.size();
    
    BufferCell& oldestEntry = buffer_[index];
    Operation* oldestOperation = oldestEntry.operation_;
    if (oldestOperation != NULL) {

        const std::size_t inputOperands = oldestOperation->numberOfInputs();
        const std::size_t outputOperands = oldestOperation->numberOfOutputs();
        const std::size_t operandCount = inputOperands + outputOperands;
        for (std::size_t i = inputOperands + 1; i <= operandCount; ++i) {
            PortState& port = binding(i);
            port.setValue(*oldestEntry.io_[i - 1]);
        }
        // results now moved to the FU output ports, "lifetime" of this
        // operation execution ends in this FU
        oldestEntry.operation_ = NULL;
        --pendingOperations_;
        hasPendingOperations_ = (pendingOperations_ > 0);
    }
    nextSlot_++;
    nextSlot_ = nextSlot_ % buffer_.size();
}

/**
 * Copies OperationExecutor.
 *
 * @return The copied OperationExecutor.
 */
OperationExecutor*
SimpleOperationExecutor::copy() {
    return new SimpleOperationExecutor(*this);
}

/**
 * Sets the OperationContext for the OperationExecutor.
 *
 * @param context New OperationContext.
 */
void
SimpleOperationExecutor::setContext(OperationContext& context) {
    context_ = &context;
}

