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
