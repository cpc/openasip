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
 * @file ResourceVectorFUResourceConflictDetector.cc
 *
 * Definition of ResourceVectorFUResourceConflictDetector class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <fstream>

#include "ResourceVectorFUResourceConflictDetector.hh"
#include "StringTools.hh"
#include "ResourceVectorSet.hh"
#include "TCEString.hh"

/**
 * Constructor.
 *
 * @param fu The function unit to detect conflicts for.
 * @exception InvalidData If the model could not be built from the given FU.
 */
ResourceVectorFUResourceConflictDetector::
ResourceVectorFUResourceConflictDetector(
    const TTAMachine::FunctionUnit& fu) throw (InvalidData) :
    vectors_(fu) {
}

/**
 * Destructor.
 */
ResourceVectorFUResourceConflictDetector::
~ResourceVectorFUResourceConflictDetector() {
}

/**
 * Issues an operation and reports a conflict if detected.
 *
 * @param id The id of the operation to issue.
 * @return False in case a conflict is detected, otherwise true.
 */
bool
ResourceVectorFUResourceConflictDetector::issueOperation(OperationID id) {

    const ResourceVector& operationVector = vectors_.resourceVector(id);
    if (compositeVector_.conflictsWith(operationVector, 1))
        return false;
    compositeVector_.mergeWith(operationVector, 1);
    return true;
}

/**
 * Simulates a cycle advance and reports a conflict if detected.
 *
 * @return False in case a conflict is detected, otherwise true.
 */
bool
ResourceVectorFUResourceConflictDetector::advanceCycle() {

    compositeVector_.shiftLeft();
    return true;
}

/**
 * Returns an operation id for the given operation.
 *
 * Operation IDs are used in the interface for optimizing the access. This
 * method converts OSAL Operations to operation IDs. 
 *
 * @param operation The OSAL Operation to find ID for.
 * @return The operation ID.
 */
ResourceVectorFUResourceConflictDetector::OperationID 
ResourceVectorFUResourceConflictDetector::operationID(
    const TCEString& operationName) const {

    return vectors_.operationIndex(StringTools::stringToUpper(operationName));
}

/**
 * Sets the state of the detector to its initial state.
 *
 * This means that the FU state is assumed to be what it is in powerup.
 */
void
ResourceVectorFUResourceConflictDetector::reset() {
    compositeVector_.clear();
}
