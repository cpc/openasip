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
 * @file OperationState.cc
 *
 * Non-inline definitions of OperationState class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
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
