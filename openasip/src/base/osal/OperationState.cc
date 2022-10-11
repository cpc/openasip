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
 * @file OperationState.cc
 *
 * Non-inline definitions of OperationState class.
 *
 * @author Pekka Jääskeläinen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#include "Application.hh"
#include "OperationState.hh"
#include "OperationContext.hh"

NullOperationState* NullOperationState::instance_ = NULL;

/**
 * Constructor.
 */
OperationState::OperationState() {
}

/**
 * Destructor.
 */
OperationState::~OperationState() {
}

/**
 * Returns true if operation state is in "available" state. 
 *
 * If operation state is available, it means that new operations can be
 * started. If it's not avaiable, no further operations should be invoked.
 *
 * @param context The operation context of which availability to check.
 * @return True if operation state is in available state. By default, operation
 *              state is available.
 */
bool 
OperationState::isAvailable(const OperationContext&) const {
    return true;
}

/**
 * This method should be invoced in every clock cycle.
 *
 * If this method is not invoced on every clock cycle, some pending results 
 * may get lost.
 *
 * @param context The operation context which should be used for calculating
 *                the pending results.
 */
void 
OperationState::advanceClock(OperationContext&) {
}

///////////////////////////////////////////////////////////////////////////////
// NullOperationState
///////////////////////////////////////////////////////////////////////////////


const std::string NULL_ERROR_MESSAGE = 
    "Tried to access NULL OperationState object.";
/**
 * Returns the name of the state instance, in this case an empty string.
 *
 * @return Empty string.
 */
const char*
NullOperationState::name() {
    return "";
}

/**
 * Prints an error message and aborts the program.
 *
 * @return Never returns.
 */
bool 
NullOperationState::isAvailable(const OperationContext&) const {
    abortWithError(NULL_ERROR_MESSAGE);
    return false;
}

/**
 * Prints an error message and aborts the program.
 *
 * @return Never returns.
 */
void 
NullOperationState::advanceClock(OperationContext&) {
    abortWithError(NULL_ERROR_MESSAGE);
}
