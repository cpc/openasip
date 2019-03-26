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
 * @file FUFiniteStateAutomaton.hh
 *
 * Declaration of FUFiniteStateAutomaton class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_FINITE_STATE_AUTOMATON_HH
#define TTA_FU_FINITE_STATE_AUTOMATON_HH

#include <string>
#include "FiniteStateAutomaton.hh"
#include "Exception.hh"
#include "FUCollisionMatrixIndex.hh"

namespace TTAMachine {
    class FunctionUnit;
}

class ResourceVectorSet;
class CollisionMatrix;
class ReservationTable;

/**
 * Finite state automaton (FSA) used to model function unit pipeline
 * resources.
 *
 * Includes support for lazily initializing the states when needed.
 */
class FUFiniteStateAutomaton : public FiniteStateAutomaton {
public:
    /// Friend due to highly optimized compiled simulation versions of
    /// the conflict detection functions.
    friend class FSAFUResourceConflictDetector;

    typedef FSAStateTransitionIndex OperationID;

    FUFiniteStateAutomaton(
        const TTAMachine::FunctionUnit& fu, 
        bool lazyBuilding = true);
    virtual ~FUFiniteStateAutomaton();

    FSAStateIndex joinState(FSAStateIndexSet sourceStates);

    CollisionMatrix& operationCollisionMatrix(
        const std::string operationName);

    virtual FSAStateIndex destinationState(
        FSAStateIndex source, 
        FSAStateTransitionIndex transition);

    virtual bool isLegalTransition(
        FSAStateIndex source,
        FSAStateTransitionIndex transition);

    virtual std::string stateName(FSAStateIndex state)
        const throw (OutOfRange);

    /// Inline functions for fast access in the compiled simulator.
    bool conflictsWith(OperationID operation) const;
    void issueOperation(OperationID operation);
    void advanceCycle();

    void buildStateMachine();

private:
    void addCollisionMatrixForState(
        FSAStateIndex state, CollisionMatrix* matrix);

    FiniteStateAutomaton::FSAStateIndex resolveState(
        FiniteStateAutomaton::FSAStateIndex source, 
        FiniteStateAutomaton::FSAStateTransitionIndex transition);

    /// Index for collision matrices of states (key is the state index).
    typedef std::map<FSAStateIndex, CollisionMatrix*>
    StateCollisionMatrixIndex;   

    /// Index for finding the state for a collision matrix.
    typedef std::map<CollisionMatrix, FSAStateIndex>
    CollisionMatrixStateIndex;

    /// Collision matrices of operations are stored here.
    FUCollisionMatrixIndex operationCollisionMatrices_;
    /// The collision matrices of each state.
    StateCollisionMatrixIndex stateCollisionMatrices_;    
    /// An index for quickly finding the state of a collision matrix.
    CollisionMatrixStateIndex collisionMatrixStates_;
    /// The number of the NOP transition.
    FSAStateTransitionIndex nopTransition_;
};

#endif
