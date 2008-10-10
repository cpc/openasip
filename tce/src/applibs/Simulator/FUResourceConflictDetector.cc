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
 * @file FUResourceConflictDetector.cc
 *
 * Definition of FUResourceConflictDetector class.
 *
 * @author Pekka Jääskeläinen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "FUResourceConflictDetector.hh"
#include "StringTools.hh"
#include "OperationIDIndex.hh"
#include "TCEString.hh"
#include "Exception.hh"

/**
 * Constructor.
 */
FUResourceConflictDetector::FUResourceConflictDetector() :
    operationIndices_(new OperationIDIndex()) {
    reset();
}

/**
 * Destructor.
 */
FUResourceConflictDetector::~FUResourceConflictDetector() {
    delete operationIndices_;
    operationIndices_ = NULL;
}

/**
 * Issues an operation and reports a conflict if detected.
 *
 * The default implementation always returns true, thus detects no conflicts.
 *
 * @param id The id of the operation to issue.
 * @return False in case a conflict is detected, otherwise true.
 */
bool
FUResourceConflictDetector::issueOperation(OperationID) {
    return true;
}

/**
 * Simulates a cycle advance and reports a conflict if detected.
 *
 * The default implementation always returns true, thus detects no conflicts.
 * This interface is provided for allowing the model to update its state at
 * the point simulation advances to the next cycle.
 *
 * @return False in case a conflict is detected, otherwise true.
 */
bool
FUResourceConflictDetector::advanceCycle() {
    return true;
}

/**
 * Simulates a cycle advance and reports a conflict by throwing an exception.
 */
void
FUResourceConflictDetector::advanceClock() {
    advanceCycle();
}


/**
 * Called after advanceClock() has been called to all clocked states.
 */
void
FUResourceConflictDetector::endClock() {
}

/**
 * Should return true in case the model needs to receive clock advance calls.
 *
 * @return By default always returns false.
 */
bool
FUResourceConflictDetector::isIdle() {
    return false;    
}

/**
 * Returns an operation id for the given operation.
 *
 * Operation IDs are used in the interface for optimizing the access. This
 * method converts OSAL Operations to operation IDs. 
 *
 * @param operation The OSAL Operation to find ID for.
 * @return The operation ID.
 * @exception KeyNotFound if the operation of given name couldn't be found
 */
FUResourceConflictDetector::OperationID 
FUResourceConflictDetector::operationID(const TCEString& operationName) const
{
    OperationIDIndex::const_iterator i = 
        operationIndices_->find(StringTools::stringToLower(operationName));
    if (i == operationIndices_->end())
        throw KeyNotFound(
            __FILE__, __LINE__, __func__, "Operation not found.");
    return (*i).second;
}

/**
 * Sets the state of the detector to its initial state.
 *
 * This means that the FU state is assumed to be what it is in powerup.
 */
void
FUResourceConflictDetector::reset() {
}
