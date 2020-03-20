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
    ResourceVectorFUResourceConflictDetector(const TTAMachine::FunctionUnit& fu)
    : vectors_(fu) {}

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
