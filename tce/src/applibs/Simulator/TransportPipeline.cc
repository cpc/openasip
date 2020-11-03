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
 * @file TransportPipeline.cc
 *
 * Definition of TransportPipeline class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005,2009 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <vector>

#include "TransportPipeline.hh"
#include "SimValue.hh"
#include "FUState.hh"
#include "Operation.hh"
#include "Application.hh"
#include "OperationContext.hh"
#include "Operation.hh"
#include "GCUState.hh"
#include "SimulatorToolbox.hh"
#include "OperationPool.hh"
#include "StringTools.hh"
#include "TCEString.hh"

using std::vector;
using std::string;

/**
 * Constructor.
 *
 * @param parent Parent of the pipeline.
 */
TransportPipeline::TransportPipeline(GCUState& parent) : 
    OperationExecutor(parent), operation_(NULL), 
    context_(&parent.context()), 
    tempContext_(NULL, PC_, RA_, parent.context().branchDelayCycles()),
    parent_(parent) {
    // delete the own state registry to avoid mem leaks
    delete &tempContext_.stateRegistry();
    // share the state registry with the actual context
    tempContext_.setStateRegistry(parent.context().stateRegistry());
}

/**
 * Destructor.
 */
TransportPipeline::~TransportPipeline() {
    tempContext_.unsetStateRegistry();
}

/**
 * Returns the latency of the pipeline.
 *
 * @return The latency of the pipeline.
 */
int
TransportPipeline::latency() const {
    return 0;
}

/**
 * Start the execution of the operation.
 *
 * First inputs and outputs of the operation are collected in a vector.
 * Then a new operation context is created. After triggering the operation
 * the registers which values are changed are updated in parent GCU state.
 *
 * @param op Operation to be triggered.
 * @todo This can be optimized a lot.
 */
void
TransportPipeline::startOperation(Operation& op) {

    operation_ = &op;
    int operands = op.numberOfInputs() + op.numberOfOutputs();
    SimValue* io[32];
    assert(operands < 32);
    for (int i = 0; i < operands; ++i)
        io[i] = const_cast<SimValue*>(&(binding(i + 1).value()));

    // use a temporary context here as the original context points to the
    // internal program counter variables of the simulator engine,
    // which we do not want to modify in the middle of simulating 
    // a cycle
    tempContext_.programCounter() = context_->programCounter();
    tempContext_.returnAddress() = context_->returnAddress();
    tempContext_.setSaveReturnAddress(false);
    tempContext_.setUpdateProgramCounter(false);
    operation_->simulateTrigger(io, tempContext_);

    // If the operation is a jump or a call operation, update the program
    // counter after delay slot cycles. If a loop buffer setup op is called,
    // do not update the program counter this way.
    if (tempContext_.updateProgramCounter() && (op.isCall() || op.isBranch())) {
        parent_.setProgramCounter(tempContext_.programCounter());
    }
    if (tempContext_.saveReturnAddress()) {
        parent_.setReturnAddress();        
    }
}

/**
 * Nothing is done when clock cycle changes.
 */
void
TransportPipeline::advanceClock() {
}

/**
 * Copies OperationExecutor.
 *
 * @return The copied OperationExecutor.
 */
OperationExecutor*
TransportPipeline::copy() {
    return new TransportPipeline(*this);
}

/**
 * Sets the OperationContext of the pipeline.
 *
 * @param context The OperationContext.
 */
void
TransportPipeline::setContext(OperationContext& context) {
    context_ = &context;
}

