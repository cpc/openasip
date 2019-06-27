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
 * @file ExecutingOperation.cc
 *
 * @author Pekka J‰‰skel‰inen 2010 (pjaaskel-no.spam-cs.tut.fi)
 */

#include "ExecutingOperation.hh"
#include "PortState.hh"

/**
 * Returns the value bound to the input or output operand
 * with the given ID.
 *
 * @note operand is the OSAL ID, thus 1 is the first input and
 * the outputs follow sequentially.
 */        
SimValue&
ExecutingOperation::io(int operand) {
    return iostorage_[operand - 1];
}

/**
 * Sets the operation execution to be started.
 *
 * Should be called at the beginning of a single operation's simulation.
 */
void
ExecutingOperation::start() {
    for (std::size_t i = 0; i < pendingResults_.size(); ++i) 
        pendingResults_[i].reset();
    free_ = false;
    stage_ = 0;
}

/**
 * Sets the operation execution to be stopped.
 *
 * Should be called at the end of a single operation's simulation. This
 * sets the object to be reusable for other operation execution.
 */
void
ExecutingOperation::stop() {
    free_ = true;            
}

/**
 * Should be called at the end of a simulation cycle.
 */
void
ExecutingOperation::advanceCycle() {
    for (std::size_t i = 0; i < pendingResults_.size(); ++i)
        pendingResults_[i].advanceCycle();
    ++stage_;
}

///////////////////////////////////////////////////////////////////////////////
// PendingResult
///////////////////////////////////////////////////////////////////////////////

/**
 * Signals a cycle advance.
 *
 * Makes the result visible to the output port in time.
 */
void
ExecutingOperation::PendingResult::advanceCycle() {
    if (--cyclesToGo_ == 0) {
        target_->setValue(*result_);
        cyclesToGo_ = INT_MAX;
    } 
}

void
ExecutingOperation::PendingResult::reset() {
    cyclesToGo_ = resultLatency_;
}
