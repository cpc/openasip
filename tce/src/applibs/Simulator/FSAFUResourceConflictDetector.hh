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
 * @file FSAFUResourceConflictDetector.hh
 *
 * Declaration of FSAFUResourceConflictDetector class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
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
