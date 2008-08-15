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
 * @file OperationBehavior.cc
 *
 * Implementations of OperationBehavior class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
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

using std::string;


/**
 * Constructor.
 */
OperationBehavior::OperationBehavior() {
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
 * invokations are successful.
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

