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

