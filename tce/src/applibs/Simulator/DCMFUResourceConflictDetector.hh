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
 * @file DCMFUResourceConflictDetector.hh
 *
 * Declaration of DCMFUResourceConflictDetector class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DCM_FU_RESOURCE_CONFLICT_DETECTOR_HH
#define TTA_DCM_FU_RESOURCE_CONFLICT_DETECTOR_HH

#include <map>
#include <string>

#include "Exception.hh"
#include "FunctionUnit.hh"
#include "FUResourceConflictDetector.hh"
#include "FUCollisionMatrixIndex.hh"
#include "CollisionMatrix.hh"

class Operation;
class TCEString;

namespace TTAMachine {
    class FunctionUnit;
}

/**
 * A Dynamic Collision Matrix (DCM) implementation of an FU resource conflict 
 * detector.
 */
class DCMFUResourceConflictDetector : 
    public FUResourceConflictDetector {
public:

    DCMFUResourceConflictDetector(
        const TTAMachine::FunctionUnit& fu)
        throw (InvalidData);
    virtual ~DCMFUResourceConflictDetector();

    virtual bool issueOperation(OperationID id);
    virtual bool advanceCycle();
    virtual void reset();

    bool issueOperationInline(OperationID id);
    bool advanceCycleInline();

    virtual OperationID operationID(const TCEString& operationName) const;

private:
    /// The reservation tables of operations.
    FUCollisionMatrixIndex collisionMatrices_;
    /// The global collision matrix.
    CollisionMatrix globalCollisionMatrix_;
    /// The modeled FU.
    const TTAMachine::FunctionUnit& fu_;
};

#include "DCMFUResourceConflictDetector.icc"

#endif
