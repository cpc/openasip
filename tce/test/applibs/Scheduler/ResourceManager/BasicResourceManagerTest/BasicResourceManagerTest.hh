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
 * @file BasicResourceManagerTest.hh
 *
 * A test suite for the resource manager using the basic block selector.
 *
 * @author Pekka J��skel�inen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef BASIC_RESOURCE_MANAGER_TEST_HH
#define BASIC_RESOURCE_MANAGER_TEST_HH

#include <iostream>
#include <TestSuite.h>
#include "SimpleResourceManager.hh"
#include "CriticalPathBBMoveNodeSelector.hh"
#include "ControlFlowGraph.hh"
#include "UniversalMachine.hh"
#include "POMDisassembler.hh"
#include "Terminal.hh"
#include "MoveGuard.hh"
#include "Bus.hh"
#include "Procedure.hh"
#include "Instruction.hh"
#include "NullAddressSpace.hh"
#include "ProgramOperation.hh"
#include "TerminalRegister.hh"
#include "BinaryStream.hh"
#include "BinaryReader.hh"
#include "TPEFProgramFactory.hh"
#include "ADFSerializer.hh"
#include "ProgramWriter.hh"
#include "BinaryWriter.hh"
#include "TPEFWriter.hh"
#include "ProgramAnnotation.hh"
#include "InterPassData.hh"
#include "SequenceTools.hh"
#include "MoveNodeSet.hh"
#include "Program.hh"
#include "Move.hh"
#include "BasicBlock.hh"
#include "Conversion.hh"

// In case some debug info is needed, uncomment
#define DEBUG_OUTPUT

/**
 * Tests basic functionality of the SimpleResourceManager.
 */
class BasicResourceManagerTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testBuildAndDispose();
    void testBasicFunctionality();
    void testLongImmediates();
    void testMissingConnection();
    void testRestorationOfResources();
    void testWAWEarliestLatestCycle();
    void testMULConflict();
    void testLIMMPSocketReads();
    void testNoRegisterTriggerInvalidates();

};


void
BasicResourceManagerTest::setUp() {
}

void
BasicResourceManagerTest::tearDown() {
}

void
BasicResourceManagerTest::testBuildAndDispose() {

    /// The tested input program with registers allocated.
    TTAProgram::Program* srcProgram = NULL;
    // Target machine to schedule the program for.
    TTAMachine::Machine* targetMachine = NULL;
    
    CATCH_ANY(
        targetMachine =
        TTAMachine::Machine::loadFromADF(
            "data/10_bus_full_connectivity.adf"));
    
    CATCH_ANY(
        srcProgram =
        TTAProgram::Program::loadFromUnscheduledTPEF(
            "data/arrmul_reg_allocated_10_bus.tpef",
            *targetMachine));
    
    std::string empty;
    TTAProgram::Procedure& procedure = srcProgram->procedure(0);
    {
        ControlFlowGraph cfg(procedure);
        
        // pick the first basic block from the program for this test
        CriticalPathBBMoveNodeSelector* cpSelector = 
            new CriticalPathBBMoveNodeSelector(
                cfg.node(0).basicBlock(), *targetMachine);
        MoveNodeSelector& selector = *cpSelector;
        
        SimpleResourceManager* rm = 
            SimpleResourceManager::createRM(*targetMachine);
        
        empty = rm->toString();
        // to see what's going on, print out the DDG
        // should get the moves of the first addition operation (ids 0..2)
        
        MoveNodeGroup moves;
        moves = selector.candidates();
        rm->assign(0, moves.node(0));
        SimpleResourceManager::disposeRM(rm);
        rm = SimpleResourceManager::createRM(*targetMachine);
        TS_ASSERT_EQUALS(rm->toString(), empty);
//        std::cerr << rm->toString() << std::endl;
        SimpleResourceManager::disposeRM(rm, false);
    }
}



/**
 * Tests the functionality by "scheduling" a moves
 * using adf which is missing connections between them.
 */
void
BasicResourceManagerTest::testMissingConnection() {

    try{
        /// The tested input program with registers allocated.
        TTAProgram::Program* srcProgram = NULL;
        /// Target machine to schedule the program for.
        TTAMachine::Machine* targetMachine = NULL;

        CATCH_ANY(
            targetMachine =
            TTAMachine::Machine::loadFromADF(
                "data/10_bus_disconnected.adf"));

        CATCH_ANY(
            srcProgram =
            TTAProgram::Program::loadFromUnscheduledTPEF(
                "data/arrmul_reg_allocated_10_disconnected.tpef",
                *targetMachine));

        TTAProgram::Procedure& procedure = srcProgram->procedure(0);
        {
        ControlFlowGraph cfg(procedure);

        // pick the first basic block from the program for this test
        CriticalPathBBMoveNodeSelector selector(cfg.node(0).basicBlock(),
            *targetMachine);

        SimpleResourceManager* rm = 
            SimpleResourceManager::createRM(*targetMachine);

        MoveNodeGroup moves;

        moves = selector.candidates();
        assert(moves.node(0).isMove());

        TS_ASSERT(!moves.node(0).isScheduled());
        TS_ASSERT_THROWS_NOTHING(rm->earliestCycle(moves.node(0)));
        TS_ASSERT_THROWS_NOTHING(rm->earliestCycle(moves.node(1)));
        MoveNodeSet tempSet;
        for (int i = 0; i < moves.nodeCount(); i++) {
            tempSet.addMoveNode(moves.node(i));
        }
        TS_ASSERT_EQUALS(rm->earliestCycle(moves.node(2)), -1);
        SimpleResourceManager::disposeRM(rm, false);
        }
        delete srcProgram;
    } catch (const Exception& e) {
        std::cerr << e.errorMessage() << std::endl;
        std::cerr << e.fileName() << " " << e.lineNum();
        TS_FAIL("testMissingConnection");
    }
}

/**
 * Tests basic functionality by "scheduling" a basic block and seeing that
 * the resource manager works correctly.
 */
void
BasicResourceManagerTest::testBasicFunctionality() {

#define SCHEDULE(X__,Y__) \
    TS_ASSERT(!moves.node(X__).isScheduled());\
    cycle = rm->earliestCycle(Y__,moves.node(X__));\
    TS_ASSERT(cycle != -1);\
    TS_ASSERT(rm->canAssign(cycle, moves.node(X__)));\
    TS_ASSERT(!moves.node(X__).isScheduled());\
    rm->assign(cycle, moves.node(X__));\
    TS_ASSERT(moves.node(X__).isScheduled());\
    TS_ASSERT_EQUALS(moves.node(X__).cycle(), cycle);\
    selector.notifyScheduled(moves.node(X__));

        try {
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
            TTAProgram::Program::loadFromUnscheduledTPEF(
                "data/arrmul_reg_allocated_10_bus.tpef",
                *targetMachine));

        TTAProgram::Procedure& procedure = srcProgram->procedure(0);
        {
        ControlFlowGraph cfg(procedure);

        // pick the first basic block from the program for this test
        CriticalPathBBMoveNodeSelector* cpSelector = 
            new CriticalPathBBMoveNodeSelector(
                cfg.node(0).basicBlock(), *targetMachine);
        MoveNodeSelector& selector = *cpSelector;
        
        SimpleResourceManager* rm = 
            SimpleResourceManager::createRM(*targetMachine);

        // to see what's going on, print out the DDG
        // should get the moves of the first addition operation (ids 0..2)

        int cycle = 0;
        MoveNodeGroup moves;
        moves = selector.candidates();

        SCHEDULE(1,5);
        SCHEDULE(0,5);
        SCHEDULE(2,5);

        // at least one of the cycles has to be greater
        // than the others (the result read)
        TS_ASSERT(!(moves.node(0).cycle() == moves.node(1).cycle() &&
                    moves.node(1).cycle() == moves.node(2).cycle()));

        // Tests that canAssign does not change status of node
        // if node isScheduled it immediately returns false
        TS_ASSERT_EQUALS(moves.node(0).isScheduled(), true);
        TS_ASSERT_EQUALS(moves.node(0).isPlaced(), true);
        TS_ASSERT_EQUALS(moves.node(0).cycle(), 5);
        // Move already assigned in different cycle
        // Move already assigned in same cycle, unassign first!
        TS_ASSERT_THROWS(rm->assign(5,moves.node(0)), ModuleRunTimeError);

        TS_ASSERT_EQUALS(moves.node(1).isScheduled(), true);
        TS_ASSERT_EQUALS(rm->canAssign(1,moves.node(1)), false);
        // Move already assigned in different cycle
        TS_ASSERT_THROWS(rm->assign(1,moves.node(1)),InvalidData);
        // Move already assigned in same cycle, unassign first!
        TS_ASSERT_THROWS(rm->assign(5,moves.node(1)),ModuleRunTimeError);

        // test unassign
        TS_ASSERT_THROWS_NOTHING(rm->unassign(moves.node(0)));
        TS_ASSERT(!moves.node(0).isScheduled());
        TS_ASSERT(!moves.node(0).isPlaced());
        TS_ASSERT(rm->canAssign(1,moves.node(0)));
        TS_ASSERT(rm->canAssign(5,moves.node(0)));

        // node is already unassignd,
        TS_ASSERT_THROWS(rm->unassign(moves.node(0)), InvalidData);
        TS_ASSERT(rm->canAssign(2,moves.node(0)));

        TS_ASSERT_THROWS_NOTHING(rm->assign(2,moves.node(0)));

        TS_ASSERT(!rm->canAssign(0,moves.node(1)));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(moves.node(1)));
        TS_ASSERT_EQUALS(
            rm->canAssign(
                rm->earliestCycle(moves.node(1)),moves.node(1)), true);
        TS_ASSERT_THROWS(rm->unassign(moves.node(1)), InvalidData);
        TS_ASSERT(!moves.node(1).isScheduled());
        TS_ASSERT_THROWS_NOTHING(
            rm->assign(rm->earliestCycle(moves.node(1)),moves.node(1)));
        TS_ASSERT(moves.node(1).isScheduled());

        TS_ASSERT_EQUALS(rm->canAssign(0,moves.node(2)), false);
        TS_ASSERT_THROWS_NOTHING(rm->unassign(moves.node(2)));
        TS_ASSERT_EQUALS(rm->canAssign(3,moves.node(2)), true);
        TS_ASSERT_THROWS_NOTHING(
            rm->assign(rm->earliestCycle(1, moves.node(2)),moves.node(2)));

        moves = selector.candidates();
        SCHEDULE(1,0);
        SCHEDULE(0,0);
        TS_ASSERT(moves.node(0).isScheduled());
        TS_ASSERT(moves.node(1).isScheduled());

        // now the second ADD (nodes 5..7)
        moves = selector.candidates();
        SCHEDULE(0,0);
        SCHEDULE(1,0);
        SCHEDULE(2,1);
        // First move of this operation reads same register
        // as a move already scheduled, using same port.
        // This work in same cycle on different
        // busses
        TS_ASSERT(moves.node(2).cycle() == 1);

        // Last move - the call
        moves = selector.candidates();
        SCHEDULE(0,4);
        TS_ASSERT(moves.node(0).isScheduled());
        TS_ASSERT(moves.node(0).cycle() == 4);
        TS_ASSERT(rm->instruction(5)->moveCount() == 0);
        TS_ASSERT(rm->instruction(4)->moveCount() == 1);
        delete cpSelector;
        SimpleResourceManager::disposeRM(rm, false);
        }
        delete srcProgram;
        delete targetMachine;
    } catch (const Exception& e) {
        std::cerr << e.procedureName() << " ";
        std::cerr << e.fileName() << " ";
        std::cerr << e.errorMessage() <<" ";
        std::cerr << e.lineNum() << std::endl;
        TS_FAIL("testBasicFunctionality failed");
    }
}

/**
 * Testing the inbuild "memory" of MOM resources.
 * Move resources before and after canAssign should be same.
 * Move resources before assign, and after unassing should be same.
 */
void
BasicResourceManagerTest::testRestorationOfResources() {

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
        TTAProgram::Program::loadFromUnscheduledTPEF(
            "data/arrmul_reg_allocated_10_bus.tpef",
            *targetMachine));

    TTAProgram::Procedure& procedure = srcProgram->procedure(0);
    {
    ControlFlowGraph cfg(procedure);

    // pick the first basic block from the program for this test
    CriticalPathBBMoveNodeSelector* cpSelector = 
        new CriticalPathBBMoveNodeSelector(
        cfg.node(0).basicBlock(), *targetMachine);
    MoveNodeSelector& selector = *cpSelector;

    SimpleResourceManager* rm = 
        SimpleResourceManager::createRM(*targetMachine);

    int cycle = 0;

    MoveNodeGroup moves;

    TTAProgram::Terminal* source = NULL;
    TTAProgram::Terminal* destination = NULL;
    const TTAMachine::Bus* bus = NULL;

    moves = selector.candidates();
    // Tries assign and unassign on several nodes,
    // compares resources before and after. Also,
    // resource before assign and after unassign should be same
    // relying on buid in == method of terminals.
    // Objects are eventually different
    source = moves.node(0).move().source().copy();
    destination = moves.node(0).move().destination().copy();
    bus = &moves.node(0).move().bus();

    TTAProgram::ProgramAnnotation srcCandidate1(
        TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_SRC, "fu15");
    TTAProgram::ProgramAnnotation dstCandidate1(
        TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_DST, "fu15");

    // It's an add operation (from the root of DDG). Test
    // the unit candidate set feature, by forcing the unit selection
    // to fu15.
    moves.node(0).move().addAnnotation(dstCandidate1);
    moves.node(1).move().addAnnotation(dstCandidate1);
    moves.node(2).move().addAnnotation(srcCandidate1);

    SCHEDULE(0, 0);
    SCHEDULE(1, 0);
    SCHEDULE(2, 2);

    TS_ASSERT_EQUALS(
        moves.node(0).move().destination().functionUnit().name(), "fu15");
    TS_ASSERT_EQUALS(
        moves.node(1).move().destination().functionUnit().name(), "fu15");
    TS_ASSERT_EQUALS(
        moves.node(2).move().source().functionUnit().name(), "fu15");

    rm->unassign(moves.node(0));
    rm->unassign(moves.node(1));
    rm->unassign(moves.node(2));

    SCHEDULE(0, 0);
    TS_ASSERT(&(moves.node(0).move().source()) != source);
    TS_ASSERT(&(moves.node(0).move().destination()) != destination);
    TS_ASSERT(moves.node(0).move().bus().name() != bus->name());

    rm->unassign(moves.node(0));
    TS_ASSERT((moves.node(0).move().source()) == (*source));
    TS_ASSERT((moves.node(0).move().destination()) == (*destination));
    TS_ASSERT((moves.node(0).move().bus().name()) == (*bus).name());

    SCHEDULE(0, 0);
    TS_ASSERT(&(moves.node(0).move().source()) != source);
    TS_ASSERT(&(moves.node(0).move().destination()) != destination);
    TS_ASSERT(moves.node(0).move().bus().name() != bus->name());
    delete source;
    delete destination;

    source = moves.node(2).move().source().copy();
    destination = moves.node(2).move().destination().copy();
    bus = &moves.node(2).move().bus();

    SCHEDULE(2, 4);
    TS_ASSERT(&(moves.node(2).move().source()) != source);
    TS_ASSERT(&(moves.node(2).move().destination()) != destination);
    TS_ASSERT(moves.node(2).move().bus().name() != bus->name());

    rm->unassign(moves.node(2));
    TS_ASSERT((moves.node(2).move().source()) == (*source));
    TS_ASSERT((moves.node(2).move().destination()) == (*destination));
    TS_ASSERT((moves.node(2).move().bus().name()) == (*bus).name());
    SCHEDULE(2, 1);
    TS_ASSERT(&(moves.node(2).move().source()) != source);
    TS_ASSERT(&(moves.node(2).move().destination()) != destination);
    TS_ASSERT(moves.node(2).move().bus().name() != bus->name());
    delete source;
    delete destination;

    source = moves.node(1).move().source().copy();
    destination = moves.node(1).move().destination().copy();
    bus = &moves.node(1).move().bus();

    SCHEDULE(1, 0);
    TS_ASSERT(&(moves.node(1).move().source()) != source);
    TS_ASSERT(&(moves.node(1).move().destination()) != destination);
    TS_ASSERT(moves.node(1).move().bus().name() != bus->name());

    rm->unassign(moves.node(1));
    TS_ASSERT((moves.node(1).move().source()) == (*source));
    TS_ASSERT((moves.node(1).move().destination()) == (*destination));
    TS_ASSERT((moves.node(1).move().bus().name()) == (*bus).name());

    SCHEDULE(1, 0);
    TS_ASSERT(&(moves.node(1).move().source()) != source);
    TS_ASSERT(&(moves.node(1).move().destination()) != destination);
    TS_ASSERT(moves.node(1).move().bus().name() != bus->name());

    // at least one of the cycles has to be greater
    // than the others (the result read)
    TS_ASSERT(!(moves.node(0).cycle() == moves.node(1).cycle() &&
          moves.node(1).cycle() == moves.node(2).cycle()));

    delete cpSelector; // need to be deleted first

    delete source;
    delete destination;

    SimpleResourceManager::disposeRM(rm, false);
    }
    delete targetMachine;
    delete srcProgram;

}

/**
 * Test the assignment of long immediates and instruction
 * templates.
 */
void
BasicResourceManagerTest::testLongImmediates() {

    try{

        /// The tested input program with registers allocated.
        TTAProgram::Program* srcProgram = NULL;
        /// Target machine to schedule the program for.
        TTAMachine::Machine* targetMachine = NULL;

        CATCH_ANY(
            targetMachine =
            TTAMachine::Machine::loadFromADF(
                "data/10_bus_full_connectivity_short_imms.adf"));

        CATCH_ANY(
            srcProgram =
            TTAProgram::Program::loadFromUnscheduledTPEF(
                "data/arrmul_reg_allocated_10_bus.tpef",
                *targetMachine));

        TTAProgram::Procedure& procedure = srcProgram->procedure(0);
        {
        ControlFlowGraph cfg(procedure);

        // pick the first basic block from the program for this test
        CriticalPathBBMoveNodeSelector *selector =
            new CriticalPathBBMoveNodeSelector(
            cfg.node(0).basicBlock(), *targetMachine);

        SimpleResourceManager* rm = 
            SimpleResourceManager::createRM(*targetMachine);
        MoveNodeGroup moves;

        moves = selector->candidates();

        TS_ASSERT_EQUALS(moves.node(1).isPlaced(), false);
        TS_ASSERT(rm->canAssign(1, moves.node(1)) == true);
        TS_ASSERT_THROWS_NOTHING(rm->assign(1,moves.node(1)));
        TS_ASSERT_THROWS_NOTHING(selector->notifyScheduled(moves.node(1)));

        TS_ASSERT_EQUALS(moves.node(0).isPlaced(), false);

        TS_ASSERT(rm->canAssign(1, moves.node(0)) == true);
        TS_ASSERT_EQUALS(moves.node(0).isPlaced(), false);
        TS_ASSERT_THROWS_NOTHING(rm->assign(1,moves.node(0)));
        TS_ASSERT_EQUALS(moves.node(0).isPlaced(), true);
        TS_ASSERT_THROWS_NOTHING(selector->notifyScheduled(moves.node(0)));

        TS_ASSERT_THROWS_NOTHING(rm->assign(2,moves.node(2)));
        TS_ASSERT_THROWS_NOTHING(selector->notifyScheduled(moves.node(2)));
        MoveNode* node = moves.node(2).copy();
        delete node;

        moves = selector->candidates();
        TS_ASSERT_THROWS_NOTHING(rm->assign(2,moves.node(0)));
        TS_ASSERT_THROWS_NOTHING(selector->notifyScheduled(moves.node(0)));
        TS_ASSERT_THROWS_NOTHING(rm->assign(2,moves.node(1)));
        TS_ASSERT_THROWS_NOTHING(selector->notifyScheduled(moves.node(1)));
        node = moves.node(1).copy();

        delete node;
        moves = selector->candidates();
        TS_ASSERT_THROWS_NOTHING(rm->assign(3,moves.node(0)));
        TS_ASSERT_THROWS_NOTHING(selector->notifyScheduled(moves.node(0)));

        TS_ASSERT_EQUALS(rm->canAssign(3,moves.node(1)), true);
        TS_ASSERT_THROWS_NOTHING(rm->assign(3,moves.node(1)));
        TS_ASSERT_EQUALS(moves.node(1).isPlaced(), true);
        TS_ASSERT_EQUALS(rm->instruction(3)->moveCount(), 2);
        TS_ASSERT_THROWS_NOTHING(rm->unassign(moves.node(1)));
        TS_ASSERT_EQUALS(moves.node(1).isPlaced(), false);
        TS_ASSERT_THROWS_NOTHING(rm->assign(3, moves.node(1)));
        TS_ASSERT_THROWS_NOTHING(selector->notifyScheduled(moves.node(1)));
        TS_ASSERT_EQUALS(rm->canAssign(4,moves.node(2)), true);
        TS_ASSERT_THROWS_NOTHING(rm->assign(4,moves.node(2)));
        TS_ASSERT_THROWS_NOTHING(selector->notifyScheduled(moves.node(2)));

        TS_ASSERT_EQUALS(rm->instruction(0)->immediateCount(), 2);
        TS_ASSERT_EQUALS(rm->instruction(1)->immediateCount(), 0);
        TS_ASSERT_EQUALS(rm->instruction(1)->moveCount(), 2);
        TS_ASSERT_EQUALS(rm->instruction(2)->immediateCount(), 0);
        TS_ASSERT_EQUALS(rm->instruction(2)->moveCount(), 3);
        TS_ASSERT_EQUALS(rm->instruction(3)->moveCount(), 2);
        TS_ASSERT_EQUALS(rm->instruction(4)->moveCount(), 1);
        delete selector;
        SimpleResourceManager::disposeRM(rm, false);
        }
        delete targetMachine;
        delete srcProgram;
    } catch (const Exception& e) {
        std::cout << std::endl;
        std::cout << e.fileName() << " " << e.procedureName() << " ";
        std::cout << "Line: "<< e.lineNum() << std::endl;
        std::cout << e.errorMessage() << std::endl;
        TS_FAIL("testLongImmediates FAILED!");
        abort();
    } catch (...) {
        std::cerr << "Caught something else" << std::endl;
    }
}
/**
 * Test the WAW checking and latest cycle.
 */
void
BasicResourceManagerTest::testWAWEarliestLatestCycle() {

    try{
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
                TTAProgram::Program::loadFromUnscheduledTPEF(
                    "data/arrmul_reg_allocated_10_bus.tpef",
                    *targetMachine));
        TTAProgram::Procedure& procedure = srcProgram->procedure(0);
        ControlFlowGraph* cfg = new ControlFlowGraph(procedure);

        SimpleResourceManager* rm = 
            SimpleResourceManager::createRM(*targetMachine);
        MoveNode* node1 =
            new MoveNode(cfg->node(0).basicBlock().instructionAt(0).movePtr(0));
        MoveNode* node2 =
            new MoveNode(cfg->node(0).basicBlock().instructionAt(1).movePtr(0));
        MoveNode* node3 =
            new MoveNode(cfg->node(0).basicBlock().instructionAt(2).movePtr(0));
        ProgramOperationPtr po1 = 
            ProgramOperationPtr(
                new ProgramOperation(
                    node1->move().destination().hintOperation()));
        po1->addInputNode(*node1);
        node1->addDestinationOperationPtr(po1);
        po1->addInputNode(*node2);
        node2->addDestinationOperationPtr(po1);
        po1->addOutputNode(*node3);
        node3->setSourceOperationPtr(po1);

        MoveNode* node5 =
            new MoveNode(cfg->node(0).basicBlock().instructionAt(5).movePtr(0));
        MoveNode* node6 =
            new MoveNode(cfg->node(0).basicBlock().instructionAt(6).movePtr(0));
        MoveNode* node7 =
            new MoveNode(cfg->node(0).basicBlock().instructionAt(7).movePtr(0));
        ProgramOperationPtr po2 = 
            ProgramOperationPtr(
                new ProgramOperation(
                    node5->move().destination().hintOperation()));
        po2->addInputNode(*node5);
        node5->addDestinationOperationPtr(po2);
        po2->addInputNode(*node6);
        node6->addDestinationOperationPtr(po2);
        po2->addOutputNode(*node7);
        node7->setSourceOperationPtr(po2);
        auto newMove = std::make_shared<TTAProgram::Move>(
            node3->move().source().copy(),
            node6->move().destination().copy(),
            node6->move().bus());
        TTAProgram::TerminalRegister* newDst =
            new TTAProgram::TerminalRegister(
                node3->move().destination().port(),
                node3->move().destination().index()+1);

        auto modifyMove = std::make_shared<TTAProgram::Move>(
            node3->move().source().copy(), newDst, node3->move().bus());

        MoveNode* bypassed = new MoveNode(newMove);
        MoveNode* duplicate = new MoveNode(node3->move().copy());
        MoveNode* otherTarget = new MoveNode(modifyMove);

        bypassed->setSourceOperationPtr(po1);
        bypassed->addDestinationOperationPtr(po2);
        po1->addOutputNode(*bypassed);
        po2->addInputNode(*bypassed);
        // bypass is now triggering, we remove original triggering move
        //po2->removeInputNode(*node6);
        //node6->unsetDestinationOperation();
        //TS_ASSERT_EQUALS(node6->isSourceOperation(), false);
        //TS_ASSERT_EQUALS(node6->isDestinationOperation(), false);

        duplicate->setSourceOperationPtr(po1);
        po1->addOutputNode(*duplicate);
        otherTarget->setSourceOperationPtr(po1);
        po1->addOutputNode(*otherTarget);

        // fu selection logic changed. use annotation to force original
        // fu selection.
        TTAProgram::ProgramAnnotation dstCandidate15(
            TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_DST, 
            "fu15");
        node1->move().addAnnotation(dstCandidate15);
        node2->move().addAnnotation(dstCandidate15);
        node3->move().addAnnotation(dstCandidate15);

        TS_ASSERT_EQUALS(bypassed->isSourceOperation(), true);
        TS_ASSERT_EQUALS(bypassed->isDestinationOperation(), true);

        TS_ASSERT_EQUALS(rm->earliestCycle(*node1), 0);
        TS_ASSERT_EQUALS(rm->earliestCycle(*node2), 0);
        TS_ASSERT_EQUALS(rm->earliestCycle(*node3), 1);
        TS_ASSERT_EQUALS(rm->latestCycle(*node2), INT_MAX);
        TS_ASSERT_EQUALS(rm->latestCycle(*node3), INT_MAX);

        TS_ASSERT_THROWS_NOTHING(rm->assign(4,*node1));
        TS_ASSERT_EQUALS(rm->earliestCycle(*node2), 4);
        TS_ASSERT_THROWS_NOTHING(rm->assign(5,*node2));
        TS_ASSERT_THROWS_NOTHING(rm->assign(8,*node3));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node1));
        TS_ASSERT_EQUALS(rm->earliestCycle(*node1), 0);
        TS_ASSERT_EQUALS(rm->latestCycle(*node1), 5);
        TS_ASSERT_THROWS_NOTHING(rm->assign(3, *node1));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node2));
        TS_ASSERT_EQUALS(rm->earliestCycle(2, *node2), 3);
        TS_ASSERT_EQUALS(rm->latestCycle(10, *node2), 7);
        TS_ASSERT_THROWS_NOTHING(rm->assign(rm->latestCycle(*node2), *node2));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node3));
        TS_ASSERT_THROWS_NOTHING(rm->assign(rm->earliestCycle(*node3), *node3));

        TS_ASSERT_THROWS_NOTHING(rm->assign(2, *node5));
        TS_ASSERT_EQUALS(rm->earliestCycle(*node6), 2);
        TS_ASSERT_EQUALS(rm->latestCycle(*node6), INT_MAX);
        TS_ASSERT_THROWS_NOTHING(rm->assign(2, *node6));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node5));
        TS_ASSERT_EQUALS(rm->earliestCycle(*node5), 0);
        TS_ASSERT_EQUALS(rm->latestCycle(*node5), 2);
        TS_ASSERT_THROWS_NOTHING(rm->assign(1,*node5));
        
        TS_ASSERT_EQUALS(rm->earliestCycle(*node7), 3);
        TS_ASSERT_EQUALS(rm->earliestCycle(4,*node7), 4);
        TS_ASSERT_EQUALS(rm->earliestCycle(7,*node7), 7);
        TS_ASSERT_EQUALS(rm->earliestCycle(8,*node7), 9);
        TS_ASSERT_EQUALS(rm->canAssign(8,*node7), false);
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node3));
        TS_ASSERT_THROWS_NOTHING(rm->assign(9,*node3));
        TS_ASSERT_EQUALS(rm->earliestCycle(8,*node7), 8);       
        TS_ASSERT_EQUALS(rm->earliestCycle(4,*node7), 4);
        TS_ASSERT_EQUALS(rm->earliestCycle(9,*node7), 10);
        TS_ASSERT_EQUALS(rm->earliestCycle(10,*node7), 10);

        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node5));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node6));
        TS_ASSERT_EQUALS(rm->earliestCycle(*node6), 0);
        TS_ASSERT_EQUALS(rm->canAssign(6, *node6), true);
        TS_ASSERT_EQUALS(rm->earliestCycle(*node5), 0);
        TS_ASSERT_THROWS_NOTHING(rm->assign(9, *node6));
        TS_ASSERT_EQUALS(rm->earliestCycle(*node5), 0);
        TS_ASSERT_EQUALS(rm->latestCycle(*node5), 9);
        TS_ASSERT_THROWS_NOTHING(rm->assign(8, *node5));
        
        TS_ASSERT_EQUALS(rm->earliestCycle(9, *node7), 10);
        TS_ASSERT_EQUALS(rm->earliestCycle(11, *node7), 11);
        TS_ASSERT_EQUALS(rm->earliestCycle(12, *node7), 12);
        TS_ASSERT_THROWS_NOTHING(rm->assign(11, *node7));


        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node5));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node6));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node7));
        TS_ASSERT_THROWS_NOTHING(rm->assign(0, *node5));
        TS_ASSERT_THROWS_NOTHING(rm->assign(0, *node6));
        
        TS_ASSERT_EQUALS(rm->earliestCycle(9, *node7), 10)
        TS_ASSERT_EQUALS(rm->earliestCycle(0, *node7), 1)

        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node5));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node6));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node1));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node2));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node3));


        TS_ASSERT_THROWS_NOTHING(rm->assign(5,*node2));
        TS_ASSERT_THROWS_NOTHING(rm->assign(4,*node1));
        TS_ASSERT_EQUALS(rm->earliestCycle(5,*node3), 6);
        TS_ASSERT_EQUALS(rm->latestCycle(5,*node3), -1);
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node1));
        TS_ASSERT_EQUALS(rm->earliestCycle(*node1), 0);
        TS_ASSERT_EQUALS(rm->latestCycle(*node1), 5);
        TS_ASSERT_THROWS_NOTHING(rm->assign(7,*node3));
        TS_ASSERT_THROWS_NOTHING(rm->assign(rm->latestCycle(
            *node1),*node1));
        TS_ASSERT_EQUALS(node1->isScheduled(), true);
        TS_ASSERT_EQUALS(node2->isScheduled(), true);
        TS_ASSERT_EQUALS(node3->isScheduled(), true);

        TS_ASSERT_THROWS_NOTHING(rm->assign(5,*node5));
        TS_ASSERT_THROWS_NOTHING(
            rm->assign(rm->earliestCycle(*bypassed),*bypassed));

        TS_ASSERT_EQUALS(rm->earliestCycle(3,*node7), 8);
        TS_ASSERT_EQUALS(rm->earliestCycle(11,*node7), 11);
        // can not schedule result before trigger
        TS_ASSERT_EQUALS(rm->latestCycle(3,*node7), -1);
        TS_ASSERT_EQUALS(rm->latestCycle(10,*node7), 10);
        TS_ASSERT_EQUALS(rm->latestCycle(9,*otherTarget), 9);
        TS_ASSERT_THROWS_NOTHING(
            rm->assign(rm->earliestCycle(*otherTarget),*otherTarget));

        TS_ASSERT_EQUALS(rm->latestCycle(7,*node7), -1);
        TS_ASSERT_EQUALS(rm->latestCycle(11,*node7), 11);
        TS_ASSERT_THROWS_NOTHING(rm->assign(11,*node7));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node7));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node5));
        TS_ASSERT_THROWS_NOTHING(rm->assign(rm->earliestCycle(*node7),*node7));
        TS_ASSERT_EQUALS(rm->earliestCycle(*node5), 0);
        TS_ASSERT_THROWS_NOTHING(rm->assign(6, *node5));
        TS_ASSERT(rm->latestCycle(*node5) >= rm->earliestCycle(*node5));
        TS_ASSERT_EQUALS(node1->isScheduled(), true);
        TS_ASSERT_EQUALS(node2->isScheduled(), true);
        TS_ASSERT_EQUALS(node3->isScheduled(), true);
        TS_ASSERT_EQUALS(node5->isScheduled(), true);
        TS_ASSERT_EQUALS(node7->isScheduled(), true);
        TS_ASSERT_EQUALS(bypassed->isScheduled(), true);
        TS_ASSERT_EQUALS(otherTarget->isScheduled(), true);
        TS_ASSERT_THROWS_NOTHING(
            rm->assign(rm->earliestCycle(*duplicate),*duplicate));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*duplicate));
        TS_ASSERT_THROWS_NOTHING(
            rm->assign(rm->earliestCycle(*duplicate),*duplicate));

        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node2));
        TS_ASSERT_EQUALS(rm->earliestCycle(*node2), rm->latestCycle(*node2));

        TS_ASSERT_THROWS_NOTHING(rm->assign(5,*node2));
        //TS_ASSERT_THROWS_NOTHING(rm->assign(rm->earliestCycle(*node6),
        //*node6));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*bypassed));
        /*TS_ASSERT_THROWS_NOTHING(rm->unassign(*node5));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node6));
        TS_ASSERT_THROWS_NOTHING(rm->unassign(*node7));*/

        try{
            int cycle = rm->earliestCycle(*bypassed);
            rm->assign(cycle,*bypassed);
        } catch (const Exception& e) {
            std::cerr << e.errorMessageStack() << std::endl;
        }

        TS_ASSERT_EQUALS(po1->isAssigned(), true);
        
        delete node1;
        delete node2;
        delete node3;
        delete node5;
        delete node6;
        delete node7;
        delete duplicate;
        delete otherTarget;
        delete bypassed;
        delete cfg;
        delete srcProgram;
        delete targetMachine;
        SimpleResourceManager::disposeRM(rm, false);

    } catch (const Exception& e) {
        std::cout << e.fileName() << " " << e.procedureName() << " ";
        std::cout << "Line: "<< e.lineNum() << std::endl;
        std::cout << e.errorMessage() << std::endl;
        TS_FAIL("testWAW FAILED!");
    }
}

void
BasicResourceManagerTest::testMULConflict() {

    /// The tested input program with registers allocated.
    TTAProgram::Program* srcProgram = NULL;
    /// Target machine to schedule the program for.
    TTAMachine::Machine* targetMachine = NULL;
    
    CATCH_ANY(
        targetMachine =
        TTAMachine::Machine::loadFromADF(
            "data/machine2.adf"));
    
    CATCH_ANY(
        srcProgram =
        TTAProgram::Program::loadFromUnscheduledTPEF(
            "data/arrmul_reg_allocated_10_bus.tpef",
             *targetMachine));
    {
    TTAProgram::Procedure& procedure = srcProgram->procedure(1);
    ControlFlowGraph cfg(procedure);
    SimpleResourceManager* rm = 
        SimpleResourceManager::createRM(*targetMachine);
    //std::cerr << POMDisassembler::disassemble(procedure,1);
    MoveNode* node1 = new MoveNode(procedure.instructionAt(44).movePtr(0));
    MoveNode* node2 = new MoveNode(procedure.instructionAt(45).movePtr(0));
    MoveNode* node3 = new MoveNode(procedure.instructionAt(46).movePtr(0));
    ProgramOperationPtr po1 = 
        ProgramOperationPtr(
            new ProgramOperation(
                node2->move().destination().operation()));
    po1->addNode(*node1);
    po1->addNode(*node2);
    po1->addNode(*node3);
    node1->addDestinationOperationPtr(po1);
    node2->addDestinationOperationPtr(po1);
    node3->setSourceOperationPtr(po1);

    MoveNode* node4 = node1->copy();
    MoveNode* node5 = node2->copy();
    MoveNode* node6 = node3->copy();
    po1->removeInputNode(*node4);
    po1->removeInputNode(*node5);
    po1->removeOutputNode(*node6);
    ProgramOperationPtr po2 = 
        ProgramOperationPtr(
            new ProgramOperation(
                node5->move().destination().operation()));

    node4->clearDestinationOperation();
    node5->clearDestinationOperation();
    po2->addNode(*node4);
    po2->addNode(*node5);
    po2->addNode(*node6);
    node4->addDestinationOperationPtr(po2);
    node5->addDestinationOperationPtr(po2);
    node6->setSourceOperationPtr(po2);
    
    TS_ASSERT_THROWS_NOTHING(rm->assign(0,*node1));
    TS_ASSERT_THROWS_NOTHING(rm->assign(0,*node2));

    TS_ASSERT_EQUALS(rm->canAssign(0,*node3), false);
    TS_ASSERT_EQUALS(rm->canAssign(1,*node3), false);
    TS_ASSERT_EQUALS(rm->canAssign(2,*node3), false);
    TS_ASSERT_EQUALS(rm->canAssign(3,*node3), true);

    TS_ASSERT_THROWS_NOTHING(rm->assign(3,*node3));
    TS_ASSERT_EQUALS(po1->isReady(), true);
    TS_ASSERT_EQUALS(po1->areInputsAssigned(), true);
    TS_ASSERT_EQUALS(po1->isComplete(), true);
    TS_ASSERT_EQUALS(po1->isAssigned(), true);
    TS_ASSERT_EQUALS(node1->isAssigned(), true);
    TS_ASSERT_EQUALS(node2->isAssigned(), true);
    TS_ASSERT_EQUALS(node3->isAssigned(), true);
    
    TS_ASSERT_EQUALS(rm->canAssign(0,*node4), false);
    TS_ASSERT_EQUALS(rm->canAssign(1,*node4), true);
    TS_ASSERT_EQUALS(rm->canAssign(1,*node5), false);
    TS_ASSERT_EQUALS(rm->canAssign(2,*node5), true);
    TS_ASSERT_THROWS_NOTHING(rm->assign(2,*node5));
    TS_ASSERT_THROWS_NOTHING(rm->assign(1,*node4));
    TS_ASSERT_THROWS_NOTHING(rm->assign(5,*node6));
    SimpleResourceManager::disposeRM(rm, false);
    }
    delete srcProgram;
    delete targetMachine;
}

void
BasicResourceManagerTest::testLIMMPSocketReads() {

    /// The tested input program with registers allocated.
    TTAProgram::Program* srcProgram = NULL;
    /// Target machine to schedule the program for.
    TTAMachine::Machine* targetMachine = NULL;
    
    CATCH_ANY(
        targetMachine =
        TTAMachine::Machine::loadFromADF(
            "data/limmpsocktest.adf"));
    
    CATCH_ANY(
        srcProgram =
        TTAProgram::Program::loadFromUnscheduledTPEF(
            "data/arrmul_reg_allocated_10_bus.tpef",
            *targetMachine));
    
    TTAProgram::Procedure& procedure = srcProgram->procedure(0);
    ControlFlowGraph cfg(procedure);

    SimpleResourceManager* rm = 
        SimpleResourceManager::createRM(*targetMachine);

    //std::cerr << POMDisassembler::disassemble(procedure,1);
    MoveNode* node1 = new MoveNode(procedure.instructionAt(0).movePtr(0));
    MoveNode* node2 = new MoveNode(procedure.instructionAt(1).movePtr(0));
    MoveNode* node3 = new MoveNode(procedure.instructionAt(2).movePtr(0));
    ProgramOperationPtr po1 = 
        ProgramOperationPtr(
            new ProgramOperation(
                node2->move().destination().operation()));
    po1->addNode(*node1);
    po1->addNode(*node2);
    po1->addNode(*node3);
    node1->addDestinationOperationPtr(po1);
    node2->addDestinationOperationPtr(po1);
    node3->setSourceOperationPtr(po1);

    TS_ASSERT_EQUALS(rm->earliestCycle(*node1), 1);
    TS_ASSERT_EQUALS(rm->earliestCycle(*node2), 1);

    TS_ASSERT_THROWS_NOTHING(rm->assign(2,*node1));
    TS_ASSERT_EQUALS(rm->earliestCycle(*node2), 3);

    SimpleResourceManager::disposeRM(rm, false);
    delete srcProgram;
    delete targetMachine;
}

void
BasicResourceManagerTest::testNoRegisterTriggerInvalidates() {

    /// The tested input program with registers allocated.
    TTAProgram::Program* srcProgram = NULL;
    /// Target machine to schedule the program for.
    TTAMachine::Machine* targetMachine = NULL;
    
    CATCH_ANY(
        targetMachine =
        TTAMachine::Machine::loadFromADF(
            "data/machineNoRegister.adf"));
   
    CATCH_ANY(
        srcProgram =
        TTAProgram::Program::loadFromUnscheduledTPEF(
            "data/arrmul_reg_allocated_10_bus.tpef",
             *targetMachine));
    
    TTAProgram::Procedure& procedure = srcProgram->procedure(0);
    ControlFlowGraph cfg(procedure);
    SimpleResourceManager* rm =
        SimpleResourceManager::createRM(*targetMachine);
    //Application::logStream() << 
    //    POMDisassembler::disassemble(procedure,1);
    MoveNode* node1 = new MoveNode(procedure.instructionAt(0).movePtr(0));
    MoveNode* node2 = new MoveNode(procedure.instructionAt(1).movePtr(0));
    MoveNode* node3 = new MoveNode(procedure.instructionAt(2).movePtr(0));
    ProgramOperationPtr po1 = 
        ProgramOperationPtr(
            new ProgramOperation(node2->move().destination().operation()));
    po1->addNode(*node1);
    po1->addNode(*node2);
    po1->addNode(*node3);
    node1->addDestinationOperationPtr(po1);
    node2->addDestinationOperationPtr(po1);
    node3->setSourceOperationPtr(po1);
    TS_ASSERT_EQUALS(rm->earliestCycle(*node1), 0);
    TS_ASSERT_EQUALS(rm->earliestCycle(*node2), 0);

    TS_ASSERT_THROWS_NOTHING(rm->assign(0,*node1));
    // no-register property allows node2
    // only in same cycle as node1
    TS_ASSERT_EQUALS(rm->earliestCycle(*node2), 0);
    TS_ASSERT(rm->canAssign(1,*node2) == false);
    TS_ASSERT_THROWS_NOTHING(rm->assign(0,*node2));
    TS_ASSERT_EQUALS(rm->earliestCycle(*node3), 2);
    // no-register do not allow result to be later then
    // cycle when it is written
    TS_ASSERT_EQUALS(rm->canAssign(5, *node3), false);
    TS_ASSERT_THROWS_NOTHING(
	rm->assign(rm->earliestCycle(*node3), *node3));

    // second operation add on second added in same cycles
    MoveNode* node4 = new MoveNode(procedure.instructionAt(5).movePtr(0));
    MoveNode* node5 = new MoveNode(procedure.instructionAt(6).movePtr(0));
    MoveNode* node6 = new MoveNode(procedure.instructionAt(7).movePtr(0));
    ProgramOperationPtr po2 = 
        ProgramOperationPtr(new ProgramOperation(node5->move().destination().operation()));
    po2->addNode(*node4);
    po2->addNode(*node5);
    po2->addNode(*node6);
    node4->addDestinationOperationPtr(po2);
    node5->addDestinationOperationPtr(po2);
    node6->setSourceOperationPtr(po2);
    TS_ASSERT_EQUALS(rm->earliestCycle(*node4), 0);
    TS_ASSERT_EQUALS(rm->earliestCycle(*node5), 0);
    TS_ASSERT_THROWS_NOTHING(rm->assign(1,*node4));
    TS_ASSERT_EQUALS(rm->canAssign(2,*node5), false);
    TS_ASSERT_THROWS_NOTHING(rm->assign(1,*node5));
    TS_ASSERT_EQUALS(rm->earliestCycle(*node6), 3);
    // no-register do not allow result to be later then
    // cycle when it is written
    TS_ASSERT_EQUALS(rm->canAssign(5, *node6), false);
    TS_ASSERT_THROWS_NOTHING(
	rm->assign(rm->earliestCycle(*node6), *node6));


#ifdef SUPPORT_TRIGGER_INVALIDATES_RESULT_IN_RM
    // Both adders are busy and write same result register.
    // in different cycles. Lets try to squeze 3rd add with same
    // result register and check trigger-invalidates-results
    MoveNode* node7 = new MoveNode(procedure.instructionAt(9).movePtr(0));
    MoveNode* node8 = new MoveNode(procedure.instructionAt(10).movePtr(0));
    MoveNode* node9 = new MoveNode(procedure.instructionAt(11).movePtr(0));
    ProgramOperationPtr po3 = 
        ProgramOperationPtr(
            new ProgramOperation(node8->move().destination().operation()));
    po3->addNode(*node7);
    po3->addNode(*node8);
    po3->addNode(*node9);
    node7->setDestinationOperationPtr(po3);
    node8->setDestinationOperationPtr(po3);
    node9->setSourceOperationPtr(po3);
    // in cycle 0 this add will fit onto alu2
    TS_ASSERT_EQUALS(rm->earliestCycle(*node7), 0);
    TS_ASSERT_EQUALS(rm->earliestCycle(*node8), 0);

#ifdef TRIGGER_INVALIDATES_IN_RM

    targetMachine->setTriggerInvalidatesResults(true);
    // trigger-invalidates-results is now true
    // schedule in cycle 1 should be imposible.
    TS_ASSERT_EQUALS(rm->earliestCycle(*node7), 0);
    // in cycle 1, add could be allocated on alu1
    // but will conflict with another add on alu1
    // starting in cycle 0
    TS_ASSERT_EQUALS(rm->canAssign(0,*node8), false);
    TS_ASSERT_EQUALS(rm->canAssign(1,*node8), false);
    TS_ASSERT_EQUALS(rm->canAssign(2,*node8), false);
    // non triggering operand can be still scheduled
    // anywhere
    TS_ASSERT_EQUALS(rm->canAssign(0,*node7), true);
    TS_ASSERT_EQUALS(rm->canAssign(1,*node7), true);
    TS_ASSERT_EQUALS(rm->canAssign(2,*node7), true);    
    TS_ASSERT_THROWS_NOTHING(rm->assign(0,*node7));
    // trigger is impossible to schedule, no-register
    // forces it into cycle 0, but trigger-invalidates-results
    // do not allow it there
    TS_ASSERT_EQUALS(rm->earliestCycle(*node8), -1);
    TS_ASSERT_EQUALS(rm->earliestCycle(*node9), -1);
    // Unschedule non-triggering operand
    TS_ASSERT_THROWS_NOTHING(rm->unassign(*node7));

    // Try to schedule earliest cycle possible
    TS_ASSERT_THROWS_NOTHING(rm->assign(
		rm->earliestCycle(*node8),*node8));
    TS_ASSERT_THROWS_NOTHING(rm->assign(
		rm->earliestCycle(*node7),*node7));
    TS_ASSERT_THROWS_NOTHING(rm->assign(
		rm->earliestCycle(*node9),*node9));
    // Should get scheduled on alu1 after first add finished
    TS_ASSERT_EQUALS(node7->cycle(), 3);
    TS_ASSERT_EQUALS(node8->cycle(), 3);
    TS_ASSERT_EQUALS(node9->cycle(), 5);
    targetMachine->setTriggerInvalidatesResults(false);
    // unassign third add and try without trigger-invalidate-results
    // now trigger can go to cycle 0 and result to cycle 2
    TS_ASSERT_THROWS_NOTHING(rm->unassign(*node7));
    TS_ASSERT_THROWS_NOTHING(rm->unassign(*node8));
    TS_ASSERT_THROWS_NOTHING(rm->unassign(*node9));

    TS_ASSERT_EQUALS(rm->canAssign(0,*node8), true);
    TS_ASSERT_THROWS_NOTHING(rm->assign(0,*node8));
    TS_ASSERT_EQUALS(rm->canAssign(0,*node7), true);
    TS_ASSERT_EQUALS(rm->canAssign(1,*node7), false);
    TS_ASSERT_THROWS_NOTHING(rm->assign(0, *node7));
    TS_ASSERT_EQUALS(rm->earliestCycle(*node9), 2);
    TS_ASSERT_THROWS_NOTHING(
	rm->assign(rm->earliestCycle(*node9), *node9));

#endif

#if 0    
    Application::logStream() << 
	POMDisassembler::disassemble(*rm->instruction(0)) << std::endl;
    Application::logStream() << 
	POMDisassembler::disassemble(*rm->instruction(1)) << std::endl;
    Application::logStream() << 
	POMDisassembler::disassemble(*rm->instruction(2)) << std::endl;
    Application::logStream() << 
	POMDisassembler::disassemble(*rm->instruction(3)) << std::endl;
    Application::logStream() << 
	POMDisassembler::disassemble(*rm->instruction(4)) << std::endl;
    Application::logStream() << 
	POMDisassembler::disassemble(*rm->instruction(5)) << std::endl;
#endif
#endif
    SimpleResourceManager::disposeRM(rm, false);
    delete srcProgram;
    delete targetMachine;

}
#endif
