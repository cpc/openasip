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
