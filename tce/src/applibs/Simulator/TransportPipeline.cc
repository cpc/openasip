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
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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

using std::vector;
using std::string;

/**
 * Constructor.
 *
 * @param parent Parent of the pipeline.
 */
TransportPipeline::TransportPipeline(GCUState& parent) : 
    OperationExecutor(parent), operation_(NULL), 
    context_(NULL), tempContext_(NULL, PC_, RA_), parent_(parent) {
}

/**
 * Destructor.
 */
TransportPipeline::~TransportPipeline() {
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
    // assume for now that there's only one operand in all control flow
    // operations
    assert(op.numberOfInputs() + op.numberOfOutputs() == 1);
    SimValue* io[1];
    io[0] = const_cast<SimValue*>(&(binding(1).value()));
    tempContext_.programCounter() = context_->programCounter();
    tempContext_.returnAddress() = context_->returnAddress();
    tempContext_.setSaveReturnAddress(false);
    operation_->simulateTrigger(io, tempContext_);

    /// expect all operations of the transport pipeline to change
    /// the program counter
    parent_.setProgramCounter(tempContext_.programCounter());
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

