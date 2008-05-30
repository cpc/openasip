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
