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
 * @file FUFiniteStateAutomaton.cc
 *
 * Definition of FUFiniteStateAutomaton class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include "FUFiniteStateAutomaton.hh"
#include "Application.hh"
#include "ResourceVectorSet.hh"
#include "CollisionMatrix.hh"
#include "StringTools.hh"
#include "AssocTools.hh"
#include "SequenceTools.hh"
#include "hash_set.hh"
#include "HWOperation.hh"

/**
 * Initializes the FSA from the given FU.
 *
 * This version is to be used in fast compiled simulation that needs to
 * minimize all function call overheads. To allow figuring out the state
 * ids statically, they are assigned according to the operation's order
 * in the FU when fetched with FunctionUnit::operation(). 
 *
 * The last state id (id FunctionUnit::operationCount()) is reserved for NOP. 
 * This way compiled simulator may access the state array with a constant
 * index when issuing an operation or checking for a conflict without
 * runtime overhead for figuring out the state id first for each operation.
 *
 * @param fu The function unit to build the FSA from.
 * @param lazyBuilding True in case states should be built lazily the
 * first time they are visited. Should improve startup time for larger FSAs.
 */
FUFiniteStateAutomaton::FUFiniteStateAutomaton(
    const TTAMachine::FunctionUnit& fu, bool lazyBuilding) :
    FiniteStateAutomaton(UNKNOWN_STATE, fu.operationCount() + 1), 
    operationCollisionMatrices_(fu) {

    // remove this loop from the benchmark
    for (int i = 0; i < fu.operationCount(); ++i) {
        const std::string opName = 
            StringTools::stringToUpper(fu.operation(i)->name());

        // each operation is a transition type in the state machine
        setTransitionName(i, opName);    
    }
    setTransitionName(fu.operationCount(), "[NOP]");
    nopTransition_ = fu.operationCount();

    // create S0
    addState();

    // the initial state's collision matrix is an all zeros conflict matrix, 
    // copy the NOP matrix
    addCollisionMatrixForState(
        0, 
        new CollisionMatrix(operationCollisionMatrices_.at(nopTransition_)));

    if (!lazyBuilding)
        buildStateMachine();
}

/**
 * Destructor.
 */
FUFiniteStateAutomaton::~FUFiniteStateAutomaton() {
    AssocTools::deleteAllValues(stateCollisionMatrices_);
}

/**
 * Initializes the given state transition entry.
 *
 * The transition target state is resolved and built if necessary.
 *
 * @return The index of the target state.
 */
FiniteStateAutomaton::FSAStateIndex
FUFiniteStateAutomaton::resolveState(
    FiniteStateAutomaton::FSAStateIndex source, 
    FiniteStateAutomaton::FSAStateTransitionIndex transition) {

    const CollisionMatrix& operationCollisionMatrix = 
        operationCollisionMatrices_.at(transition);

    const CollisionMatrix& stateMatrix = *stateCollisionMatrices_[source];
    // Check whether the transition is legal, that is, the conflict
    // matrix has a 0 in the column 1 (representing the next cycle) in
    // the row representing the operation. The column 0 is for
    // starting the operation at the same clock cycle, which cannot
    // be done in case of our TTA FU. Thus, all states are cycle
    // advancing.
    if (transition == nopTransition_ || 
        !stateMatrix.isCollision(transition, 1)) {

        // transition is legal, create the target state's collision
        // matrix (all states considered cycle advancing) by shifting
        // the source conflict matrix to left and ORing the operation's
        // conflict matrix
        CollisionMatrix* newMatrix = new CollisionMatrix(stateMatrix);
        newMatrix->shiftLeft();
        newMatrix->orWith(operationCollisionMatrix);

        // check if we already have a state with the created collision matrix
        FSAStateIndex targetState = ILLEGAL_STATE;
        CollisionMatrixStateIndex::const_iterator foundState =
            collisionMatrixStates_.find(*newMatrix);
        if (foundState != collisionMatrixStates_.end()) {

            // found an old state with the wanted collision matrix, 
            // set the transition target to the old state
            targetState = (*foundState).second;

            delete newMatrix;
            newMatrix = NULL;
        } else {
            // no state with the wanted collision matrix exists, 
            targetState = addState();
            addCollisionMatrixForState(targetState, newMatrix);
        }
        setTransition(source, targetState, transition);
        return targetState;
    } else {
        setTransition(source, ILLEGAL_STATE, transition);
        return ILLEGAL_STATE;
    }
}

/**
 * Connects a state and a collision matrix.
 *
 * @param state The state index.
 * @param matrix The collision matrix (becomes owned by the index).
 */
void
FUFiniteStateAutomaton::addCollisionMatrixForState(
    FSAStateIndex state, CollisionMatrix* matrix) {

    stateCollisionMatrices_[state] = matrix;
    collisionMatrixStates_[*matrix] = state;
}

/**
 * Builds all states and state transitions.
 *
 * This might take very long time if there are lots of states to build. That
 * is, if the FU pipeline resource usage patterns are long and complicated.
 */
void
FUFiniteStateAutomaton::buildStateMachine() {

    hash_set<FSAStateIndex> handledStates;
    hash_set<FSAStateIndex> unfinishedStatesList;
    unfinishedStatesList.insert(0); // start from the start state

    while (!unfinishedStatesList.empty()) {
        const FSAStateIndex state = *unfinishedStatesList.begin();
        unfinishedStatesList.erase(state);
        // go through all operations (transitions), the NOP is included
        // and resolve their target state from the current state
        for (int i = 0; i < operationCollisionMatrices_.size(); ++i) {
            FSAStateIndex resolvedState = resolveState(state, i);
            if (resolvedState != ILLEGAL_STATE && 
                handledStates.find(resolvedState) == handledStates.end() &&
                resolvedState != state)
                unfinishedStatesList.insert(resolvedState);            
        }
        handledStates.insert(state);
    }
}

/**
 * Returns the collision matrix for the given operation.
 *
 * @param operationName Name of the operation.
 * @return The collision matrix.
 */
CollisionMatrix& 
FUFiniteStateAutomaton::operationCollisionMatrix(
    const std::string operationName) {
    return operationCollisionMatrices_.at(
        transitionIndex(StringTools::stringToUpper(operationName)));
}

/**
 * Returns the index of the state resulting from the given transition from 
 * the given source state.
 *
 * This method is called often, thus should be as fast as possible. 
 * Therefore, all range checking etc. is disabled. This implementation
 * supports lazy initialization, i.e., it is capable of building missing
 * states when requested.
 *
 * @param source The source state.
 * @param transition The transition.
 * @return The index of the destination state.
 */
FUFiniteStateAutomaton::FSAStateIndex
FUFiniteStateAutomaton::destinationState(
    FSAStateIndex source, FSAStateTransitionIndex transition) {
    if (transitions_[source][transition] == UNKNOWN_STATE)
        return resolveState(source, transition);
    return transitions_[source][transition];
}

/**
 * Returns true in case the given transition is legal (accepted by the
 * state machine) from the given state.
 *
 * This method is called often, thus should be as fast as possible. 
 * Therefore, all range checking etc. is disabled. This implementation
 * supports lazy initialization, i.e., it is capable of building missing
 * states when requested.
 * 
 * @param source The source state.
 * @param transition The transition.
 * @return True in case the transition is legal.
 */
bool 
FUFiniteStateAutomaton::isLegalTransition(
    FSAStateIndex source,
    FSAStateTransitionIndex transition) {
    return destinationState(source, transition) != ILLEGAL_STATE;
}

/**
 * Returns a join state which is created by ORin the conflict vectors of the
 * given source states.
 *
 * If the state already exists, returns it, creates a new state otherwise. 
 *
 * @param sourceStates The set of source states which are joined.
 * @return Index to the join state.
 */
FiniteStateAutomaton::FSAStateIndex
FUFiniteStateAutomaton::joinState(FSAStateIndexSet) {
    abortWithError("Not implemented.");
    return -1;
}

/**
 * Returns the textual description of the state at the given index.
 *
 * @param state The index of the state of which name to return.
 * @return The name of the state.
 */
std::string
FUFiniteStateAutomaton::stateName(FSAStateIndex state) const
    throw (OutOfRange) {

    StateCollisionMatrixIndex::const_iterator i = 
        stateCollisionMatrices_.find(state);

    if (i == stateCollisionMatrices_.end())
        throw OutOfRange(
            __FILE__, __LINE__, __func__, "No such state.");

    return (*i).second->toDotString();
}

