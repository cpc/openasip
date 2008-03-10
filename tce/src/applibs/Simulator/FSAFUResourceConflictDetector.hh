/**
 * @file FSAFUResourceConflictDetector.hh
 *
 * Declaration of FSAFUResourceConflictDetector class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FSA_FU_RESOURCE_CONFLICT_DETECTOR_HH
#define TTA_FSA_FU_RESOURCE_CONFLICT_DETECTOR_HH

#include <map>
#include <string>

#include "Exception.hh"
#include "FunctionUnit.hh"
#include "FUResourceConflictDetector.hh"
#include "FUFiniteStateAutomaton.hh"

class Operation;

/**
 * An FSA implementation of a FU resource conflict detector.
 */
class FSAFUResourceConflictDetector : public FUResourceConflictDetector {
public:

    FSAFUResourceConflictDetector(
        const TTAMachine::FunctionUnit& fu)
        throw (InvalidData);
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

    std::string operationName(OperationID id) const;

    virtual OperationID operationID(const std::string& operationName) const
        throw (KeyNotFound);

    virtual void writeToDotFile(const std::string& fileName) const;

private:
    /// The FSA.
    FUFiniteStateAutomaton fsa_;
    /// Current state of the FSA.
    FiniteStateAutomaton::FSAStateIndex currentState_;
    /// The next state of the FSA (move to currentState in cycle advance).
    FiniteStateAutomaton::FSAStateIndex nextState_;
    /// True if operation was issued at the current cycle.
    bool operationIssued_;
    /// The transition index of a NOP operation.
    const FiniteStateAutomaton::FSAStateTransitionIndex NOP;
    /// The name of the FU that is being simulated (for debugging).
    const std::string fuName_;
};

#include "FSAFUResourceConflictDetector.icc"

#endif
