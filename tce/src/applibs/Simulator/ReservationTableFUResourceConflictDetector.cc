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
 * @file ReservationTableFUResourceConflictDetector.cc
 *
 * Definition of ReservationTableFUResourceConflictDetector class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ReservationTableFUResourceConflictDetector.hh"
#include "StringTools.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "TCEString.hh"

/**
 * Constructor.
 *
 * @param fu The function unit to detect conflicts for.
 * @exception InvalidData If the model could not be built from the given FU.
 */
ReservationTableFUResourceConflictDetector::
ReservationTableFUResourceConflictDetector(
    const TTAMachine::FunctionUnit& fu) 
    throw (InvalidData) :
    reservationTables_(fu), globalReservationTable_(fu), fu_(fu) {
}

/**
 * Destructor.
 */
ReservationTableFUResourceConflictDetector::
~ReservationTableFUResourceConflictDetector() {
}

/**
 * Issues an operation and reports a conflict if detected.
 *
 * @param id The id of the operation to issue.
 * @return False in case a conflict is detected, otherwise true.
 */
bool
ReservationTableFUResourceConflictDetector::issueOperation(OperationID id) {
    return issueOperationInline(id);
}

/**
 * Simulates a cycle advance and reports a conflict if detected.
 *
 * @return False in case a conflict is detected, otherwise true.
 */
bool
ReservationTableFUResourceConflictDetector::advanceCycle() {
    return advanceCycleInline();
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
ReservationTableFUResourceConflictDetector::OperationID 
ReservationTableFUResourceConflictDetector::operationID(
    const TCEString& operationName) const {

    // find the index of the operation in the FU, it corresponds to the
    // operation ID
    for (int i = 0; i < fu_.operationCount(); ++i) {
        if (StringTools::ciEqual(fu_.operation(i)->name(), operationName)) {
            return i;
        }
    }
    throw KeyNotFound(__FILE__, __LINE__, __func__, "Operation not found.");
}

/**
 * Sets the state of the detector to its initial state.
 *
 * This means that the FU state is assumed to be what it is in powerup.
 */
void
ReservationTableFUResourceConflictDetector::reset() {
    globalReservationTable_.setAllToZero();
}
