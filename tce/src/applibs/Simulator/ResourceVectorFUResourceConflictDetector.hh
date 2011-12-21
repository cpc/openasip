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
 * @file ResourceVectorFUResourceConflictDetector.hh
 *
 * Declaration of ResourceVectorFUResourceConflictDetector class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RV_FU_RESOURCE_CONFLICT_DETECTOR_HH
#define TTA_RV_FU_RESOURCE_CONFLICT_DETECTOR_HH

#include <map>
#include <string>

#include "Exception.hh"
#include "FUResourceConflictDetector.hh"
#include "FUFiniteStateAutomaton.hh"
#include "ResourceVectorSet.hh"

namespace TTAMachine {
    class FunctionUnit;
}

class Operation;

/**
 * An FSA implementation of a FU resource conflict detector.
 */
class ResourceVectorFUResourceConflictDetector : 
    public FUResourceConflictDetector {
public:

    ResourceVectorFUResourceConflictDetector(
        const TTAMachine::FunctionUnit& fu)
        throw (InvalidData);
    virtual ~ResourceVectorFUResourceConflictDetector();

    virtual bool issueOperation(OperationID id);

    virtual bool advanceCycle();

    virtual void reset();

    virtual OperationID operationID(const TCEString& operationName) const;

private:
    /// The resource vectors of operations.
    ResourceVectorSet vectors_;
    /// The composite resource vector.
    ResourceVector compositeVector_;
};

#endif
