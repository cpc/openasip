/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file GCUState.cc
 *
 * Definition of GCUState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005,2010
 * @note rating: red
 */

#include "GCUState.hh"
#include "SequenceTools.hh"
#include "OperationContext.hh"
#include "Application.hh"
#include "OperationExecutor.hh"

using std::string;

/**
 * Constructor.
 *
 * @param latency Latency of the unit.
 * @param nww Natural word width as MAUs.
 * @param lock Global lock signal.
 */
GCUState::GCUState(
    int latency, 
    int nww,
    int cuDelaySlots,
    int instructionAddressIncrement) :
    naturalWordWidth_(nww), returnAddressRegister_(64),
    latency_(latency), 
    branchDelayCycles_(cuDelaySlots),
    operationContext_(
        NULL, programCounter_, returnAddressRegister_,
        branchDelayCycles_),
        instructionAddressIncrement_(instructionAddressIncrement) {
    reset();
}

void
GCUState::reset() {
    programCounter_ = 0;
    returnAddressRegister_ = 0;
    newProgramCounter_ = 0;
    operationPending_ = false;
    operationPendingTime_ = 0;
}

/**
 * Destructor.
 */
GCUState::~GCUState() {
}

/**
 * Returns the operation context.
 *
 * This is basically a "template method" to allow differently initialized
 * OperationContext-classes in FUState subclasses.
 *
 * @return The operation context for the FU.
 */
OperationContext&
GCUState::context() {
    return operationContext_;
}

/**
 * Handles the actions when clock is advanced.
 *
 * The value of program counter is updated. Pipeline is advanced.
 */
void
GCUState::advanceClock() {

    // For call and jump operations
    if (operationPending_) {
        operationPendingTime_--;
    }
    if (operationPending_ && operationPendingTime_ <= 0) {
        programCounter_ = newProgramCounter_;
        operationPending_ = false;
    } 

    // Special handling for Loop Buffer Setup operation
    if (!context().isEmpty()) {
        idle_ = false;
        context().advanceClock();
    } else {
        idle_ = !operationPending_;
    }
}
