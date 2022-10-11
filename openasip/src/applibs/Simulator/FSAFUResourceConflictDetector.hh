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
 * @file FSAFUResourceConflictDetector.hh
 *
 * Declaration of FSAFUResourceConflictDetector class.
 *
 * @author Pekka Jääskeläinen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: red
 */

#ifndef TTA_FSA_FU_RESOURCE_CONFLICT_DETECTOR_HH
#define TTA_FSA_FU_RESOURCE_CONFLICT_DETECTOR_HH

#include "FUResourceConflictDetector.hh"

namespace TTAMachine {
    class FunctionUnit;
}

class Operation;
class FSAFUResourceConflictDetectorPimpl;

/**
 * An FSA implementation of a FU resource conflict detector.
 */
class FSAFUResourceConflictDetector : public FUResourceConflictDetector {
public:

    FSAFUResourceConflictDetector(
        const TTAMachine::FunctionUnit& fu);
    virtual ~FSAFUResourceConflictDetector();

    virtual bool issueOperation(OperationID id);
    virtual bool advanceCycle();
    virtual bool isIdle();

    bool issueOperationInline(OperationID id);
    bool issueOperationLazyInline(OperationID id);
    bool advanceCycleInline();
    bool advanceCycleLazyInline();

    virtual void reset();   

    void initializeAllStates();

    const char* operationName(OperationID id) const;

    virtual OperationID operationID(const TCEString& operationName) const;

    virtual void writeToDotFile(const TCEString& fileName) const;

private:
    /// Private implementation in a separate source file
    FSAFUResourceConflictDetectorPimpl* pimpl_;
};

#endif
