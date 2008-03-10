/**
 * @file CriticalPathBBMoveNodeSelectorTest.hh
 *
 * A test suite for the basic block selector.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef FU_CRITICAL_PATH_BB_MOVE_NODE_SELECTOR_TEST_HH
#define FU_CRITICAL_PATH_BB_MOVE_NODE_SELECTOR_TEST_HH

#include <TestSuite.h>
#include "BasicBlockSelector.hh"
#include "UniversalMachine.hh"
#include "Program.hh"
#include "ControlFlowGraph.hh"
#include "POMDisassembler.hh"
#include "CriticalPathBBMoveNodeSelector.hh"
#include "MoveNode.hh"
#include "SimpleResourceManager.hh"

class CriticalPathBBMoveNodeSelectorTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testBasicFunctionality();
};


void 
CriticalPathBBMoveNodeSelectorTest::setUp() {
}

void 
CriticalPathBBMoveNodeSelectorTest::tearDown() {
}

/**
 * Tests basic functionality by "scheduling" a basic block and seeing that
 * the internal ready list updates correctly.
 */
void
CriticalPathBBMoveNodeSelectorTest::testBasicFunctionality() {

#if 0
    /// Universal Machine for the unscheduled part.
    UniversalMachine umach_;

    /// The tested input program with registers allocated.
    TTAProgram::Program* srcProgram = NULL;
    /// Target machine to schedule the program for.
    TTAMachine::Machine* targetMachine = NULL;


    CATCH_ANY(
        targetMachine = 
        TTAMachine::Machine::loadFromADF(
            "data/10_bus_full_connectivity.adf"));

    CATCH_ANY(
        srcProgram = 
        TTAProgram::Program::loadFromTPEF(
            "data/arrmul_reg_allocated_10_bus.tpef",
            *targetMachine, umach_));


    TTAProgram::Procedure& procedure = srcProgram->procedure(0);

    ControlFlowGraph cfg(procedure);

#define DEBUG_OUTPUT

    // pick the first basic block from the program for this test
    CriticalPathBBMoveNodeSelector selector(cfg.node(0));

    SimpleResourceManager rm(*targetMachine);

    // to see what's going on, print out the DDG 
    // should get the moves of the first addition operation (ids 0..2)

#define SCHEDULE(X__) \
    rm.assign(rm.earliestCycle(moves.node(X__)), moves.node(X__));\
    selector.notifyScheduled(moves.node(X__));

    MoveNodeGroup moves = selector.candidates();
    TS_ASSERT_EQUALS(moves.nodeCount(), 3);
    TS_ASSERT_LESS_THAN(moves.node(0).nodeID(), 3);
    TS_ASSERT_LESS_THAN(moves.node(1).nodeID(), 3);
    TS_ASSERT_LESS_THAN(moves.node(2).nodeID(), 3);

    SCHEDULE(0);
    SCHEDULE(1);
    SCHEDULE(2);

    moves = selector.candidates();

    // should now have STW (3..4) in the queue
    TS_ASSERT_EQUALS(moves.nodeCount(), 2);
    // the STW has only 2 moves (3..4)
    TS_ASSERT_LESS_THAN(moves.node(0).nodeID(), 5);
    TS_ASSERT_LESS_THAN(2, moves.node(0).nodeID());

    TS_ASSERT_LESS_THAN(moves.node(1).nodeID(), 5);
    TS_ASSERT_LESS_THAN(2, moves.node(1).nodeID());
    SCHEDULE(0);
    SCHEDULE(1);

    // now the second ADD (nodes 5..7)
    moves = selector.candidates();
    TS_ASSERT_EQUALS(moves.nodeCount(), 3);
    TS_ASSERT_LESS_THAN(moves.node(0).nodeID(), 8);
    TS_ASSERT_LESS_THAN(4, moves.node(0).nodeID());

    TS_ASSERT_LESS_THAN(moves.node(1).nodeID(), 8);
    TS_ASSERT_LESS_THAN(4, moves.node(1).nodeID());

    TS_ASSERT_LESS_THAN(moves.node(2).nodeID(), 8);
    TS_ASSERT_LESS_THAN(4, moves.node(1).nodeID());

    SCHEDULE(0);
    SCHEDULE(1);
    SCHEDULE(2);

    delete targetMachine;
    delete srcProgram;
#endif

}

#endif
