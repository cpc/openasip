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
 * @file OperationBehavior.cc
 *
 * Implementations of OperationBehavior class.
 *
 * @author Pekka Jääskeläinen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#include <string>

#include "OperationBehavior.hh"
#include "Application.hh"
#include "Exception.hh"
#include "OperationState.hh"
#include "OperationContext.hh"
#include "OperationPool.hh"
#include "OperationGlobals.hh"
#include "Operation.hh"
#include "Operand.hh"

using std::string;


/**
 * Constructor with NULL parent.
 */
OperationBehavior::OperationBehavior()
    : parent_(NullOperation::instance()){
}

/**
 * Constructor with parent.
 */
OperationBehavior::OperationBehavior(const Operation& parent)
    :   parent_(parent) {
}

/**
 * Destructor.
 */
OperationBehavior::~OperationBehavior() {
}

/**
 * Simulates the process of starting the execution of an operation.
 *
 * Clients should invoke isTriggerLocking() before any attempt to call 
 * simulateTrigger() in current clock cycle. By default, an operation 
 * invocations are successful.
 *
 *
 * @param io The input operands and the results of the operation.
 * @param context The operation context affecting the operation results.
 * @return bool True if all values could be computed, false otherwise.
 * @exception Exception Depends on the implementation.
 */
bool 
OperationBehavior::simulateTrigger(
    SimValue**,
    OperationContext&) const {
    return true;
}

/**
 * The default implementation of input operand validation.
 *
 * This always return true. This is meant to be overridden in derived classes
 * when necessary.
 */
bool
OperationBehavior::areValid(
    const InputOperandVector& /*inputs*/,
    const OperationContext& /*context*/) const {

    return true;
}


/**
 * Writes text to the output stream specified
 * 
 * @param text text to be written to the output stream
 */
void 
OperationBehavior::writeOutput(
    const char* text) const {
    OperationGlobals::outputStream() << text;
}


/**
 * Creates the instance of operation state for this operation and adds it to
 * its operation context.
 *
 * By default this function does nothing (assumes that the operation has no
 * state). If the operation context already contains the required operation
 * state instance, nothing is done.
 *
 * @param context The operation context to add the state to.
 */
void
OperationBehavior::createState(OperationContext&) const {
}

/**
 * Deletes the instance of operation state for this operation from its
 * operation context.
 * 
 * By default this function does nothing (assumes that the operation has no
 * state). If the operation context does not contain the required operation
 * state instance, nothing is done.
 *
 * @param context The operation context to delete the state from.
 */
void
OperationBehavior::deleteState(OperationContext&) const {
}

/**
 * Returns the name of the state of this operation behavior.
 *
 * By default returns an empty string which denotes that there is no state.
 *
 * @return The state name for this operation behavior.
 */
const char* 
OperationBehavior::stateName() const {
    return "";
}

/**
 * If behavior can be simulated.
 *
 * @return true If simulation of behavior is possible.
 */
bool 
OperationBehavior::canBeSimulated() const {
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// NullOperationBehavior
///////////////////////////////////////////////////////////////////////////////

NullOperationBehavior NullOperationBehavior::instance_;
const char* ERROR_MESSAGE =
    "Attempted to use a NULL OperationBehavior object.";

/**
 * Writes an error message to error log and aborts the program.
 *
 * @param io Not used.
 * @param context Not used.
 * @return Never returns.
 */
bool 
NullOperationBehavior::simulateTrigger(
    SimValue**,
    OperationContext&) const {

    abortWithError(ERROR_MESSAGE);
    return true;
}

/**
 * Writes an error message to error log and aborts the program.
 *
 * @param io Not used.
 * @param context Not used.
 * @return Never returns.
 */
bool 
NullOperationBehavior::lateResult(
    SimValue**,
    OperationContext&) const {

    abortWithError(ERROR_MESSAGE);
    return true;
}

