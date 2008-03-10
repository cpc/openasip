/**
 * @file FSAFUResourceConflictDetector.cc
 *
 * Definition of FSAFUResourceConflictDetector class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <fstream>

#include "FSAFUResourceConflictDetector.hh"
#include "StringTools.hh"
#include "ResourceVectorSet.hh"

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
    const TTAMachine::FunctionUnit& fu) throw (InvalidData) :
    FUResourceConflictDetector(), fsa_(fu, true),
    currentState_(fsa_.startState()),  
    operationIssued_(false), NOP(fsa_.transitionIndex("[NOP]")), 
    fuName_(fu.name()) {
    issueOperation(NOP);
}

/**
 * Destructor.
 */
FSAFUResourceConflictDetector::~FSAFUResourceConflictDetector() {
}

/**
 * Initializes all states in the state machine.
 *
 * By default, only the initial state is constructed and rest when 
 * visited the first time.
 */
void
FSAFUResourceConflictDetector::initializeAllStates() {
    fsa_.buildStateMachine();
}

/**
 * Writes the state machine to a Graphviz dot file.
 *
 * @param fileName The file name.
 */
void
FSAFUResourceConflictDetector::writeToDotFile(
    const std::string& fileName) const {

    std::ofstream dot(fileName.c_str());
    dot << fsa_.toDotString() << std::endl;
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
std::string
FSAFUResourceConflictDetector::operationName(OperationID id) const {
    return fsa_.transitionName(id);
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
    const std::string& operationName) const
    throw (KeyNotFound) {

    return fsa_.transitionIndex(StringTools::stringToUpper(operationName));
}

/**
 * Sets the state of the detector to its initial state.
 *
 * This means that the FU state is assumed to be what it is in powerup.
 */
void
FSAFUResourceConflictDetector::reset() {
    currentState_ = fsa_.startState();
    issueOperation(NOP);
}

/**
 * The FSA FU model is considered to be idle when the FSA is at start state and
 * there are no new operations issued at that state.
 */
bool
FSAFUResourceConflictDetector::isIdle() {
    return currentState_ == fsa_.startState() && currentState_ == nextState_;
}
