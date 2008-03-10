/**
 * @file OneCycleOperationExecutor.cc
 *
 * Definition of OneCycleOperationExecutor class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "OneCycleOperationExecutor.hh"
#include "OperationContext.hh"
#include "SimValue.hh"
#include "Operation.hh"
#include "SequenceTools.hh"
#include "PortState.hh"
#include "OperationPool.hh"
#include "SimulatorToolbox.hh"
#include "Application.hh"

using std::vector;
using std::string;

/// a dummy simvalue which is given for operands that are not bound
SimValue dummy(32);

/**
 * Constructor.
 *
 * @param parent Parent of the executor.
 */
OneCycleOperationExecutor::OneCycleOperationExecutor(FUState& parent) : 
    OperationExecutor(parent), context_(NULL), operation_(NULL), 
    io_(NULL), ready_(false), ioVectorInitialized_(false) {
}

/**
 * Destructor.
 */
OneCycleOperationExecutor::~OneCycleOperationExecutor() {
    delete[] io_;
    io_ = NULL;
}

/**
 * Returns the latency of the executor.
 *
 * @return The latency of the executor.
 */
int
OneCycleOperationExecutor::latency() const {
    return 1;
}

/**
 * Initializes the operation input/output vector.
 *
 * The I/O vector points directly to the function unit port register
 * values. This is possible because there cannot be any other pending
 * operations at once because of latency of 1.
 */
inline void
OneCycleOperationExecutor::initializeIOVector() {
    if (ioVectorInitialized_)
        return;
    io_ = new SimValue*[bindings_.size()];
    for (size_t i = 0; i < bindings_.size(); ++i) {
        io_[i] = const_cast<SimValue*>(&bindings_[i]->value());
    }
    ioVectorInitialized_ = true;
}

/**
 * Starts the execution of the operation.
 *
 * @param op Operation to be triggered.
 */
void
OneCycleOperationExecutor::startOperation(Operation& op) {
    operation_ = &op;
    initializeIOVector();
    ready_ = operation_->simulateTrigger(io_, *context_);
    hasPendingOperations_ = !ready_;
}

/**
 * Advances clock by one cycle.
 */
void
OneCycleOperationExecutor::advanceClock() {

    if (!ready_ && operation_ != NULL) {
        ready_ = operation_->lateResult(io_, *context_);
        if (ready_) {
            operation_ = NULL;
        }
    }
    hasPendingOperations_ = !ready_;
}

/**
 * Copies OperationExecutor.
 *
 * @return The copied OperationExecutor.
 */
OperationExecutor*
OneCycleOperationExecutor::copy() {
    return new OneCycleOperationExecutor(*this);
}

/**
 * Sets new context for the OperationExecutor.
 *
 * @param context New OperationContext.
 */
void
OneCycleOperationExecutor::setContext(OperationContext& context) {
    context_ = &context;
}

