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
 * @file FUFiniteStateAutomaton.hh
 *
 * Declaration of FUFiniteStateAutomaton class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_FINITE_STATE_AUTOMATON_HH
#define TTA_FU_FINITE_STATE_AUTOMATON_HH

#include <string>
#include "FiniteStateAutomaton.hh"
#include "FunctionUnit.hh"
#include "Exception.hh"
#include "FUCollisionMatrixIndex.hh"

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
