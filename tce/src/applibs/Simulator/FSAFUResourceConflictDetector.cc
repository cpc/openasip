/**
 * @file FSAFUResourceConflictDetector.cc
 *
 * Definition of FSAFUResourceConflictDetector class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "FSAFUResourceConflictDetector.hh"
#include "StringTools.hh"
#include "ResourceVectorSet.hh"
#include "FSAFUResourceConflictDetectorPimpl.hh"
#include "Machine.hh"
#include "TCEString.hh"
#include <fstream>
#include <string>

/**
 * Constructor.
 *
 * Initializes the FSA to a lazy mode in which the states are built when
 * they are needed the first time. In order to initialize all states, 
 * call initializeAllStates().
 *
 * @param fu The function unit to detect conflicts for.
 * @exception InvalidData If the model could not be built from the given FU.
 */
FSAFUResourceConflictDetector::FSAFUResourceConflictDetector(
    const TTAMachine::FunctionUnit& fu) :
    FUResourceConflictDetector(), 
    pimpl_(new FSAFUResourceConflictDetectorPimpl(fu)) {
    issueOperation(pimpl_->NOP);
}

/**
 * Destructor.
 */
FSAFUResourceConflictDetector::~FSAFUResourceConflictDetector() {
    delete pimpl_;
    pimpl_ = NULL;
}

/**
 * Initializes all states in the state machine.
 *
 * By default, only the initial state is constructed and rest when 
 * visited the first time.
 */
void
FSAFUResourceConflictDetector::initializeAllStates() {
    pimpl_->fsa_.buildStateMachine();
}

/**
 * Writes the state machine to a Graphviz dot file.
 *
 * @param fileName The file name.
 */
void
FSAFUResourceConflictDetector::writeToDotFile(
    const TCEString& fileName) const {

    std::ofstream dot(fileName.c_str());
    dot << pimpl_->fsa_.toDotString() << std::endl;
    dot.close();
}

/**
 * Issues an operation and reports a conflict if detected.
 *
 * @param id The id of the operation to issue.
 * @return False in case a conflict is detected, otherwise true.
 */
bool
FSAFUResourceConflictDetector::issueOperation(OperationID id) {
    return issueOperationLazyInline(id);
}

/**
 * Returns the name of the operation associated to the given transition id.
 *
 * @param id The transition id.
 * @return The name of the operation.
 */
const char*
FSAFUResourceConflictDetector::operationName(OperationID id) const {
    return pimpl_->fsa_.transitionName(id).c_str();
}

/**
 * Simulates a cycle advance and reports a conflict if detected.
 *
 * @return False in case a conflict is detected, otherwise true.
 */
bool
FSAFUResourceConflictDetector::advanceCycle() {
    return advanceCycleLazyInline();
}

/**
 * Returns an operation id for the given operation.
 *
 * Operation IDs are used in the interface for optimizing the access. This
 * method converts OSAL Operations to operation IDs. 
 *
 * @param operation The OSAL Operation to find ID for.
 * @return The operation ID.
 */
FSAFUResourceConflictDetector::OperationID 
FSAFUResourceConflictDetector::operationID(
    const TCEString& operationName) const {

    return pimpl_->fsa_.transitionIndex(
        StringTools::stringToUpper(operationName));
}

/**
 * Sets the state of the detector to its initial state.
 *
 * This means that the FU state is assumed to be what it is in powerup.
 */
void
FSAFUResourceConflictDetector::reset() {
    pimpl_->currentState_ = pimpl_->fsa_.startState();
    issueOperation(pimpl_->NOP);
}

/**
 * The FSA FU model is considered to be idle when the FSA is at start state and
 * there are no new operations issued at that state.
 */
bool
FSAFUResourceConflictDetector::isIdle() {
    return pimpl_->currentState_ == pimpl_->fsa_.startState() 
        && pimpl_->currentState_ == pimpl_->nextState_;
}

/**
 * @file FSAFUResourceConflictDetector.icc
 *
 * Inline implementations of FSAFUResourceConflictDetector class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

/**
 * Issues an operation and reports a conflict if detected.
 *
 * Inlineable optimized version for compiled simulation and benchmarking.
 * All states are assumed initialized. For lazily initialized FSA, use
 * issueOperationLazyInline(). This version has about balanced runtime for
 * conflict and no-conflict cases for sensible benchmarking with random
 * operation sequences.
 *
 * @param id The id of the operation to issue.
 * @return False in case a conflict is detected, otherwise true.
 */
bool
FSAFUResourceConflictDetector::issueOperationInline(OperationID id) {

    pimpl_->nextState_ = pimpl_->fsa_.transitions_[pimpl_->currentState_][id];
    if (pimpl_->nextState_ == FiniteStateAutomaton::ILLEGAL_STATE) {
        pimpl_->nextState_ = 0;
        return false;
    }
    return true;    
}

/**
 * Issues an operation and reports a conflict if detected.
 *
 * Inlineable optimized version for compiled simulation and benchmarking.
 * For lazily initialized FSA. Checks if a state is missing and constructs
 * it if needed.
 *
 * @param id The id of the operation to issue.
 * @return False in case a conflict is detected, otherwise true.
 */
bool
FSAFUResourceConflictDetector::issueOperationLazyInline(OperationID id) {

    pimpl_->nextState_ = pimpl_->fsa_.transitions_[pimpl_->currentState_][id];
    if (pimpl_->nextState_ == FiniteStateAutomaton::UNKNOWN_STATE) {
        pimpl_->nextState_ = pimpl_->fsa_.resolveState(pimpl_->currentState_, id);
    }    
    if (pimpl_->nextState_ == FiniteStateAutomaton::ILLEGAL_STATE) {
        pimpl_->nextState_ = 0;
        return false;
    }
    return true;
}

/**
 * Simulates a cycle advance and reports a conflict if detected.
 *
 * Inlineable optimized version for compiled simulation and benchmarking.
 *
 * @note Do not call this in case the last operation issue transfered
 * the FSA to the illegal state! That is, returned false. No checking is
 * done to get fastest possible simulation.
 *
 * @return False in case a conflict is detected, otherwise true.
 */
inline bool
FSAFUResourceConflictDetector::advanceCycleInline() {

    // assert(nextState_ != FiniteStateAutomaton::ILLEGAL_STATE);
    pimpl_->currentState_ = pimpl_->nextState_;
    // issue NOP transition at the next cycle in case there are no other
    // operation issues
    pimpl_->nextState_ = pimpl_->
        fsa_.transitions_[pimpl_->currentState_][pimpl_->NOP];
    return true;
}

/**
 * Simulates a cycle advance and reports a conflict if detected.
 *
 * Inlineable optimized version for compiled simulation and benchmarking.
 *
 * @note Do not call this in case the last operation issue transfered
 * the FSA to the illegal state! That is, returned false. No checking is
 * done to get fastest possible simulation.
 *
 * @return False in case a conflict is detected, otherwise true.
 */
inline bool
FSAFUResourceConflictDetector::advanceCycleLazyInline() {

    // assert(nextState_ != FiniteStateAutomaton::ILLEGAL_STATE);
    pimpl_->currentState_ = pimpl_->nextState_;
    // issue NOP transition at the next cycle in case there are no other
    // operation issues
    pimpl_->nextState_ = pimpl_->fsa_.transitions_[pimpl_->currentState_][pimpl_->NOP];
    if (pimpl_->nextState_ == FiniteStateAutomaton::UNKNOWN_STATE) {
        pimpl_->nextState_ = pimpl_->fsa_.resolveState(
            pimpl_->currentState_, pimpl_->NOP);
    }    
    return true;
}

