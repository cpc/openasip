/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file FUResourceConflictDetector.hh
 *
 * Declaration of FUResourceConflictDetector class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_RESOURCE_CONFLICT_DETECTOR_HH
#define TTA_FU_RESOURCE_CONFLICT_DETECTOR_HH

#include "ClockedState.hh"

class Operation;
class OperationIDIndex;
class TCEString;

/**
 * Interface for classes used for detecting FU resource conflicts.
 */
class FUResourceConflictDetector : public ClockedState {
public:

    /// Type for identifying operations in the conflict detector interface.
    typedef int OperationID;

    FUResourceConflictDetector();
    virtual ~FUResourceConflictDetector();

    virtual bool issueOperation(OperationID id);

    virtual void endClock();
    virtual void advanceClock();
    virtual bool advanceCycle();

    virtual bool isIdle();

    virtual void reset();

    virtual OperationID operationID(const TCEString& operationName) const;

protected:
    /// Map for finding indices for operations quickly.
    OperationIDIndex* operationIndices_;

};

#endif
