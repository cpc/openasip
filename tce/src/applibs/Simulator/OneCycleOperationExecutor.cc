/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file OneCycleOperationExecutor.cc
 *
 * Definition of OneCycleOperationExecutor class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
    operation_ = NULL;
    hasPendingOperations_ = false;
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

