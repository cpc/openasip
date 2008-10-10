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

