/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file BUMoveNodeSelectorTest.hh
 *
 * A test suite for the basic block selector.
 *
 * @author Vladim’r Guzma 2011 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef FU_BU_MOVE_NODE_SELECTOR_TEST_HH
#define FU_BU_MOVE_NODE_SELECTOR_TEST_HH

#include <TestSuite.h>
#include "UniversalMachine.hh"
#include "Program.hh"
#include "ControlFlowGraph.hh"
#include "POMDisassembler.hh"
#include "BUMoveNodeSelector.hh"
#include "MoveNode.hh"
#include "SimpleResourceManager.hh"

class BUMoveNodeSelectorTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testBasicFunctionality();
};


void 
BUMoveNodeSelectorTest::setUp() {
}

void 
BUMoveNodeSelectorTest::tearDown() {
}

/**
 * Tests basic functionality by "scheduling" a basic block and seeing that
 * the internal ready list updates correctly.
 */
void
BUMoveNodeSelectorTest::testBasicFunctionality() {

#if 1

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
            *targetMachine));


    TTAProgram::Procedure& procedure = srcProgram->procedure(2);

    ControlFlowGraph cfg(procedure);

#define DEBUG_OUTPUT

    // pick the first basic block from the program for this test
    BUMoveNodeSelector selector(cfg.node(0).basicBlock(), *targetMachine);
    SimpleResourceManager *rm =
    SimpleResourceManager::createRM(*targetMachine);
    
    // to see what's going on, print out the DDG 
    // should get the moves of the first addition operation (ids 0..2)

#define SCHEDULE(X__) \
    rm->assign(rm->latestCycle(10, moves.node(X__)), moves.node(X__));\
    selector.notifyScheduled(moves.node(X__));

    MoveNodeGroup moves;
    // For now just test that nothing breaks. Will add more exact tests
    // once stabilised.
    do {
        moves = selector.candidates();
        for (int i = 0; i < moves.nodeCount(); i++) {
            TS_ASSERT_THROWS_NOTHING(
                rm->assign(rm->latestCycle(10, moves.node(i)), moves.node(i)));
            TS_ASSERT_THROWS_NOTHING(selector.notifyScheduled(moves.node(i)));
        }
    } while (moves.nodeCount() != 0);

        
/*    for (int i = 0; i <= 10; i++) {
        std::cerr << rm->instruction(i)->toString() << std::endl;    
    }*/
    delete targetMachine;
    delete srcProgram;
#endif

}

#endif
