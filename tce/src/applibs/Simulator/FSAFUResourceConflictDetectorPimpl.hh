/**
 * @file FSAFUResourceConflictDetectorPimpl.hh
 *
 * Declaration of FSAFUResourceConflictDetectorPimpl (private implementation)
 * class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef FSA_FU_RESOURCE_CONFLICT_DETECTOR_PIMPL_HH
#define FSA_FU_RESOURCE_CONFLICT_DETECTOR_PIMPL_HH

#include <map>
#include <string>

#include "Exception.hh"
#include "FunctionUnit.hh"
#include "FUResourceConflictDetector.hh"
#include "FUFiniteStateAutomaton.hh"
#include "FSAFUResourceConflictDetector.hh"


class FSAFUResourceConflictDetectorPimpl {
public:
    friend class FSAFUResourceConflictDetector;
    ~FSAFUResourceConflictDetectorPimpl();
private:
    FSAFUResourceConflictDetectorPimpl(const TTAMachine::FunctionUnit& fu);
    /// Copying not allowed.
    FSAFUResourceConflictDetectorPimpl(
        const FSAFUResourceConflictDetectorPimpl&);
    /// Assignment not allowed.
    FSAFUResourceConflictDetectorPimpl& operator=(
        const FSAFUResourceConflictDetectorPimpl&);
    
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

#endif
