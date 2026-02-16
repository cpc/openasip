/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file DataDependenceGraphTest.hh
 *
 * A test suite for Data Dependence Graph generation.
 *
 * @author Heikki Kultala 2006 (heikki.kultala-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2010
 * @note rating: red
 */

#ifndef FU_DATA_DEPENDENCE_GRAPH_TEST_HH
#define FU_DATA_DEPENDENCE_GRAPH_TEST_HH

#include <iostream>

#include <TestSuite.h>
#include "DataDependenceGraph.hh"
#include "DataDependenceGraphBuilder.hh"
#include "BinaryStream.hh"
#include "TPEFProgramFactory.hh"
#include "Program.hh"
#include "BinaryReader.hh"
#include "UniversalMachine.hh"
#include "OperationPool.hh"
#include "ControlFlowGraph.hh"
#include "ADFSerializer.hh"
#include "Instruction.hh"
#include "Machine.hh"
#include "InterPassData.hh"
#include "InterPassDatum.hh"
#include "BasicBlock.hh"
#include "Move.hh"

using TTAProgram::Move;

int nodeCounts0[] = { 9,-1,6 };
int nodeCounts1[] = { 13,-1,28,6 };
int nodeCounts2[] = { 19,-1,9 };
int edgeCounts0[] = { 13, -1,7 };
int edgeCounts1[] = { 8, -1, 41, 7 };
int edgeCounts2[] = { 26, -1, 12 };

unsigned int usedAfterSizes1[] = {7,UINT_MAX,7,0};
unsigned int usedAfterSizes2[] = {2,UINT_MAX,0};

unsigned int splessUsedAfterSizes1[] = {6,UINT_MAX,6,0};
unsigned int splessUsedAfterSizes2[] = {1,UINT_MAX,0};
unsigned int ipdUsedAfterSizes1[] = {8,UINT_MAX,8,0};
unsigned int ipdUsedAfterSizes2[] = {2,UINT_MAX,0};
unsigned int rallocatedUsedAfterSizes1[] = {7,UINT_MAX,7,0};
unsigned int rallocatedUsedAfterSizes2[] = {1,UINT_MAX,0};

class DataDependenceGraphTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    void testBasics();

    void testProcedureDDG();

    void testBBDDG();

    void testRallocatedDDG();
    void testRallocatedBBDDG();

    void testPathCalculation();

    void testSWBypassing();

    void testUsedAfter();
 
    void testRallocatedUsedAfter();

    void testSpecialRegIPData();

    MoveNode& findMoveNodeById(DataDependenceGraph& ddg, int id);
};


void 
DataDependenceGraphTest::setUp() {
}

void 
DataDependenceGraphTest::tearDown() {
}

/**
 * Tests the very basic API of DDG.
 */
void 
DataDependenceGraphTest::testBasics() {
    DataDependenceEdge edge(
        DataDependenceEdge::EDGE_REGISTER,
        DataDependenceEdge::DEP_UNKNOWN,
        TCEString("foo"),
        false, false);

#if 0
    PRINT_VAR(sizeof(DataDependenceEdge));
    PRINT_VAR(sizeof(DataDependenceGraph));
    PRINT_VAR(sizeof(MoveNode));
    PRINT_VAR(sizeof(DataDependenceEdge::DependenceType));
    PRINT_VAR(sizeof(DataDependenceEdge::EdgeReason));
#endif

    TS_ASSERT_EQUALS(edge.edgeReason(), DataDependenceEdge::EDGE_REGISTER);
    TS_ASSERT_EQUALS(
        edge.dependenceType(), DataDependenceEdge::DEP_UNKNOWN);
    TS_ASSERT_EQUALS(edge.guardUse(), false);
    TS_ASSERT_EQUALS(edge.certainAlias(), false);
    TS_ASSERT_EQUALS(edge.tailPseudo(), false);
    TS_ASSERT_EQUALS(edge.headPseudo(), false);
    TS_ASSERT_EQUALS(edge.data(), "foo");
    TS_ASSERT_EQUALS(edge.loopDepth(), 0);


    DataDependenceEdge edge2(
        DataDependenceEdge::EDGE_RA,
        DataDependenceEdge::DEP_WAW,
        true, true, true, true, 2);
    
    edge2.setData("this is data");

    TS_ASSERT_EQUALS(edge2.edgeReason(), DataDependenceEdge::EDGE_RA);
    TS_ASSERT_EQUALS(
        edge2.dependenceType(), DataDependenceEdge::DEP_WAW);
    TS_ASSERT_EQUALS(edge2.guardUse(), true);
    TS_ASSERT_EQUALS(edge2.certainAlias(), true);
    TS_ASSERT_EQUALS(edge2.tailPseudo(), true);
    TS_ASSERT_EQUALS(edge2.headPseudo(), true);
    TS_ASSERT_EQUALS(edge2.data(), "this is data");
    TS_ASSERT_EQUALS(edge2.loopDepth(), 2);

}

MoveNode&
DataDependenceGraphTest::findMoveNodeById(DataDependenceGraph& ddg, int id) {

    for (int i = 0; i < ddg.nodeCount(); ++i) {
        if (ddg.node(i).nodeID() == id)
            return ddg.node(i);
    }
    abortWithError("Not found??" + Conversion::toString(id));
    // silence warning
    throw 1;
}

/**
 * Tests
 *
 * @todo
 */
void
DataDependenceGraphTest::testProcedureDDG() {

    TPEF::BinaryStream binaryStream("data/arrmul.tpef");

    // read to TPEF Handler Module
    TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);

    assert(tpef_ != NULL);

    // convert the loaded TPEF to POM
    TTAProgram::TPEFProgramFactory factory(
        *tpef_, &UniversalMachine::instance());
    TTAProgram::Program* currentProgram = factory.build();
    {
    ControlFlowGraph cfg(currentProgram->procedure(1));

    DataDependenceGraph* ddg = NULL;

    DataDependenceGraphBuilder builder;

    ddg = builder.build(
        cfg, DataDependenceGraph::ALL_ANTIDEPS, 
        UniversalMachine::instance(),
        &UniversalMachine::instance());

    TS_ASSERT_EQUALS(ddg->programOperationCount(), 18);
    TS_ASSERT_EQUALS(ddg->nodeCount(), 48);
    TS_ASSERT_EQUALS(ddg->edgeCount(), 108);
    int first = ddg->node(1).nodeID();
    TS_ASSERT(
        ddg->hasEdge(
            findMoveNodeById(*ddg, first), 
            findMoveNodeById(*ddg, first + 2)));

    TS_ASSERT(
        ddg->hasEdge(
            findMoveNodeById(*ddg, first + 1), 
            findMoveNodeById(*ddg, first + 2)));

    TS_ASSERT(
        !ddg->hasEdge(
            findMoveNodeById(*ddg, first), 
            findMoveNodeById(*ddg, first + 1)));

    // check mem WARs
    TS_ASSERT(
        ddg->hasEdge(
            findMoveNodeById(*ddg, first + 25),
            findMoveNodeById(*ddg, first + 33)));

    TS_ASSERT(
        ddg->hasEdge(
            findMoveNodeById(*ddg, first + 27),
            findMoveNodeById(*ddg, first + 33)));


    // RARs are not dependencies, no edge
    TS_ASSERT(
        !ddg->hasEdge(
            findMoveNodeById(*ddg, first + 25),
            findMoveNodeById(*ddg, first + 27)));
    
    // write it out to a .dot file, just check it does not crash or anything
    // it's not feasible to verify the output as the node ids are the
    // object addresses, thus change, etc.
    ddg->writeToDotFile("/dev/null");
    //ddg->writeToDotFile("ddg.dot");

    ddg->findAllPaths();
    // test the path caching, this should be quite fast
    for (int i = 1000000; i > 0; --i) {
        TS_ASSERT(
            ddg->hasPath(
                findMoveNodeById(*ddg, first), 
                findMoveNodeById(*ddg, first + 2)));

        // from ra->store to the ra->jump
        TS_ASSERT(
            ddg->hasPath(
                findMoveNodeById(*ddg, 10), 
                findMoveNodeById(*ddg, 52))); 

        TS_ASSERT(
            !ddg->hasPath(
                findMoveNodeById(*ddg, first + 2), 
                findMoveNodeById(*ddg, first)));
    }

    // Then try to do something with subgraphs
    try {
        for (int i = 0; i < cfg.nodeCount(); i++) {
            BasicBlockNode& bbn = cfg.node(i);
            if (bbn.isNormalBB()) {
                
                DataDependenceGraph *sg = ddg->createSubgraph(
                    bbn.basicBlock(), true);
                DataDependenceGraph *sg2 = sg->createSubgraph(
                    bbn.basicBlock(), false);

                TS_ASSERT_EQUALS(sg2->nodeCount(), nodeCounts1[i]);
                TS_ASSERT_EQUALS(sg->nodeCount(), nodeCounts1[i]);

                // check that same nuber of edges that with only BB-ddg
                TS_ASSERT_EQUALS(sg2->edgeCount(), edgeCounts1[i]);

                // try to add some nodes to SG2

                auto move0 = 
                    bbn.basicBlock().instructionAtIndex(0).move(0).copy();
                auto move1 = 
                    bbn.basicBlock().instructionAtIndex(0).move(0).copy();
                auto move2 = 
                    bbn.basicBlock().instructionAtIndex(0).move(0).copy();

                MoveNode* mn0 = new MoveNode(move0);
                MoveNode* mn1 = new MoveNode(move1);
                MoveNode* mn2 = new MoveNode(move2);

                sg2->addNode(*mn0);
                sg2->addNode(*mn1);
                sg2->addNode(*mn2);
                TS_ASSERT_EQUALS(sg2->nodeCount(), nodeCounts1[i] +3);
                TS_ASSERT_EQUALS(sg->nodeCount(), nodeCounts1[i] +3);
                TS_ASSERT_EQUALS(ddg->nodeCount(), 48 + 3);

                // TODO: remove added nodes
                
                sg2->removeNode(*mn0);
                sg2->removeNode(*mn1);
                sg2->removeNode(*mn2);

                delete mn0; delete mn1; delete mn2;

                // check rootGraphInEdges and rootGraphOutEdges
                MoveNode& mn4 = sg->node(0);
                MoveNode& mn5 = sg->node(2);

                TS_ASSERT_EQUALS(
                    sg->rootGraphInEdges(mn4), ddg->inEdges(mn4));
                TS_ASSERT_EQUALS(
                    sg->rootGraphOutEdges(mn4), ddg->outEdges(mn4));
                TS_ASSERT_EQUALS(
                    sg->rootGraphInEdges(mn5), ddg->inEdges(mn5));
                TS_ASSERT_EQUALS(
                    sg->rootGraphOutEdges(mn5), ddg->outEdges(mn5));

                TS_ASSERT_EQUALS(sg2->nodeCount(), nodeCounts1[i]);
                TS_ASSERT_EQUALS(sg->nodeCount(), nodeCounts1[i]);
                TS_ASSERT_EQUALS(ddg->nodeCount(), 48);


                sg->detachSubgraph(*sg2);
                delete sg2;
                ddg->detachSubgraph(*sg);
                delete sg;
            }

        }
    } catch ( Exception &e ) {
        std::cerr << Exception::info(e) << std::endl;
        TS_ASSERT(0);
    }

    delete ddg;
    currentProgram = NULL;
    }
    // delete currentProgram only after cfg.
    delete currentProgram;
}

void
DataDependenceGraphTest::testBBDDG() {

    try {
        TPEF::BinaryStream binaryStream("data/arrmul.tpef");
        
        // read to TPEF Handler Module
        TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);
        
        assert(tpef_ != NULL);
    
        // convert the loaded TPEF to POM
        TTAProgram::TPEFProgramFactory factory(
            *tpef_, 
            &UniversalMachine::instance());
        TTAProgram::Program* currentProgram = factory.build();
        {
        ControlFlowGraph cfg0(currentProgram->procedure(0));
        ControlFlowGraph cfg1(currentProgram->procedure(1));
        ControlFlowGraph cfg2(currentProgram->procedure(2));

        DataDependenceGraph* ddg0 = NULL;
        DataDependenceGraph* ddg1 = NULL;
        DataDependenceGraph* ddg1l = NULL;
        DataDependenceGraph* ddg2 = NULL;
    
        DataDependenceGraphBuilder builder;

        for (int i = 0; i < cfg0.nodeCount(); i++) {
            BasicBlockNode& bb = cfg0.node(i);
            if (bb.isNormalBB()) {
                ddg0 = builder.build(
                    bb.basicBlock(), DataDependenceGraph::ALL_ANTIDEPS, 
                    UniversalMachine::instance(), "",
                    &UniversalMachine::instance());

                // check for edges to itself, should not be
                for( int j = 0; j < ddg0->nodeCount(); j++ ) {
                    TS_ASSERT(!(ddg0->hasEdge(ddg0->node(j),ddg0->node(j))));
                }
                TS_ASSERT_EQUALS(ddg0->nodeCount(), nodeCounts0[i]);
                TS_ASSERT_EQUALS(ddg0->edgeCount(), edgeCounts0[i]);

                delete ddg0; ddg0 = NULL;
            }
        }

        for (int i = 0; i < cfg1.nodeCount(); i++) {
            BasicBlockNode& bb = cfg1.node(i);
            if (bb.isNormalBB()) {
                ddg1 = builder.build(
                    bb.basicBlock(), DataDependenceGraph::ALL_ANTIDEPS,
                    UniversalMachine::instance(), "",
                    &UniversalMachine::instance());
                                     

                // check for edges to itself, should not be
                for( int j = 0; j < ddg1->nodeCount(); j++ ) {
                    TS_ASSERT(!(ddg1->hasEdge(ddg1->node(j),ddg1->node(j))));
                }
                TS_ASSERT_EQUALS(ddg1->nodeCount(), nodeCounts1[i]);
                TS_ASSERT_EQUALS(ddg1->edgeCount(), edgeCounts1[i]);

                if (cfg1.hasEdge(bb,bb)) { // looping bb
                    ddg1l = ddg1;
                } else {
                    delete ddg1; ddg1 = NULL;
                }
            }
        }

        for (int i = 0; i < cfg2.nodeCount(); i++) {
            BasicBlockNode& bb = cfg2.node(i);
            if (bb.isNormalBB()) {
                ddg2 = builder.build(
                    bb.basicBlock(), DataDependenceGraph::ALL_ANTIDEPS,
                    UniversalMachine::instance(), "",
                    &UniversalMachine::instance());

                // check for edges to itself, should not be
                for( int j = 0; j < ddg2->nodeCount(); j++ ) {
                    TS_ASSERT(!(ddg2->hasEdge(ddg2->node(j),ddg2->node(j))));
                }
                TS_ASSERT_EQUALS(ddg2->nodeCount(), nodeCounts2[i]);
                TS_ASSERT_EQUALS(ddg2->edgeCount(), edgeCounts2[i]);

                delete ddg2; ddg2 = NULL;
            }
        }
        
        TS_ASSERT_EQUALS(ddg1l->programOperationCount(), 11);
        TS_ASSERT_EQUALS(ddg1l->nodeCount(), 28);
/* no more addresses here - need to update numbers from somewher
        TS_ASSERT(
            ddg1l->hasEdge(
                findMoveNodeById(*ddg1l, 35), findMoveNodeById(*ddg1l, 36)));

        TS_ASSERT(
            !ddg1l->hasEdge(
                findMoveNodeById(*ddg1l, 35), findMoveNodeById(*ddg1l, 37)));
*/


        delete ddg1l;
        }
        delete currentProgram;
        currentProgram = NULL;

    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;

        assert(0);
    }
}

void
DataDependenceGraphTest::testRallocatedDDG() {
    try {
        TPEF::BinaryStream binaryStream("data/rallocated_arrmul.tpef");
        
        ADFSerializer adfSerializer;
        adfSerializer.setSourceFile("data/10_bus_full_connectivity.adf");
        
        TTAMachine::Machine* machine = adfSerializer.readMachine();
        
        // read to TPEF Handler Module
        TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);
        
        assert(tpef_ != NULL);
    

        // convert the loaded TPEF to POM
        TTAProgram::TPEFProgramFactory factory(
            *tpef_, *machine, &UniversalMachine::instance());
        TTAProgram::Program* currentProgram = factory.build();
        {
        ControlFlowGraph cfg(currentProgram->procedure(1));
        
        DataDependenceGraph* ddg = NULL;
        
        DataDependenceGraphBuilder builder;
        ddg = builder.build(
            cfg, DataDependenceGraph::ALL_ANTIDEPS, *machine, NULL, true, true);

        ddg->writeToDotFile("/dev/null");

        TS_ASSERT_EQUALS(ddg->programOperationCount(), 14);
        TS_ASSERT_EQUALS(ddg->nodeCount(), 38);
        TS_ASSERT_EQUALS(ddg->edgeCount(), 81);
        int first = ddg->node(1).nodeID();

        TS_ASSERT(
            !ddg->hasEdge(
                findMoveNodeById(*ddg, first), 
                findMoveNodeById(*ddg, first+1)));

        TS_ASSERT(
            !ddg->hasEdge(
                findMoveNodeById(*ddg, first+1), 
                findMoveNodeById(*ddg, first+2)));

        TS_ASSERT(
            !ddg->hasEdge(
                findMoveNodeById(*ddg, first+2), 
                findMoveNodeById(*ddg, first+3)));

        TS_ASSERT(
            !ddg->hasEdge(
                findMoveNodeById(*ddg, first+3), 
                findMoveNodeById(*ddg, first+4)));

        TS_ASSERT(
            ddg->hasEdge(
                findMoveNodeById(*ddg, first+4), 
                findMoveNodeById(*ddg, first+6)));
        
        TS_ASSERT(
            ddg->hasEdge(
                findMoveNodeById(*ddg, first+5), 
                findMoveNodeById(*ddg, first+6)));
        
        TS_ASSERT(
            !ddg->hasEdge(
                findMoveNodeById(*ddg, first+4), 
                findMoveNodeById(*ddg, first+5)));
        
        // check mem WARs
        TS_ASSERT(
            ddg->hasEdge(findMoveNodeById(*ddg,first+20),
                         findMoveNodeById(*ddg,first+28)));
        
        TS_ASSERT(
            ddg->hasEdge(findMoveNodeById(*ddg,first+22),
                         findMoveNodeById(*ddg,first+28)));
        
        // RARs are not dependencies, no edge
        TS_ASSERT(
            !ddg->hasEdge(findMoveNodeById(*ddg,first+20),
                     findMoveNodeById(*ddg,first+22)));
        }
        delete currentProgram;
        currentProgram = NULL;


    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;

        assert(0);
    }
}

void
DataDependenceGraphTest::testRallocatedBBDDG() {
    
    try {
        
        TPEF::BinaryStream binaryStream("data/rallocated_arrmul.tpef");
        
        ADFSerializer adfSerializer;
        adfSerializer.setSourceFile("data/10_bus_full_connectivity.adf");
        
        TTAMachine::Machine* machine = adfSerializer.readMachine();
        
        // read to TPEF Handler Module
        TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);
        
        assert(tpef_ != NULL);
    
        // convert the loaded TPEF to POM
        TTAProgram::TPEFProgramFactory factory(
            *tpef_, *machine, &UniversalMachine::instance());
        TTAProgram::Program* currentProgram = factory.build();
        {
        ControlFlowGraph cfg0(currentProgram->procedure(0));
        ControlFlowGraph cfg1(currentProgram->procedure(1));
        ControlFlowGraph cfg2(currentProgram->procedure(2));

        DataDependenceGraph* ddg0 = NULL;
        DataDependenceGraph* ddg1 = NULL;
        DataDependenceGraph* ddg1l = NULL;

        DataDependenceGraph* ddg2 = NULL;
    
        DataDependenceGraphBuilder builder;

        for( int i = 0; i < cfg0.nodeCount(); i++ ) {
            BasicBlockNode& bb = cfg0.node(i);
            if( bb.isNormalBB()) {
                ddg0 = builder.build(
                    bb.basicBlock(), 
                    DataDependenceGraph::INTRA_BB_ANTIDEPS,
                    *machine);
                delete ddg0; ddg0 = NULL;
            }
        }

        for( int i = 0; i < cfg1.nodeCount(); i++ ) {
            BasicBlockNode& bb = cfg1.node(i);
            if( bb.isNormalBB()) {
                ddg1 = builder.build(
                    bb.basicBlock(), 
                    DataDependenceGraph::INTRA_BB_ANTIDEPS,
                    *machine);
                if(cfg1.hasEdge(bb,bb)) { // looping bb
                    ddg1l = ddg1;
                } else {
                    delete ddg1; ddg1 = NULL;
                }
            }
        }

        for( int i = 0; i < cfg2.nodeCount(); i++ ) {
            BasicBlockNode& bb = cfg2.node(i);
            if( bb.isNormalBB()) {
                ddg2 = builder.build(
                    bb.basicBlock(),
                    DataDependenceGraph::INTRA_BB_ANTIDEPS,
                    *machine);
                delete ddg2; ddg2 = NULL;
            }
        }
        
        TS_ASSERT_EQUALS(ddg1l->programOperationCount(), 11);
        TS_ASSERT_EQUALS(ddg1l->nodeCount(), 28);
/* no more addresses here - need to update numbers from somewher
        TS_ASSERT(
            ddg1l->hasEdge(
                findMoveNodeById(*ddg1l, 35), findMoveNodeById(*ddg1l, 36)));

        TS_ASSERT(
            !ddg1l->hasEdge(
                findMoveNodeById(*ddg1l, 35), findMoveNodeById(*ddg1l, 37)));
*/

        if(ddg1l)
            delete ddg1l;
        }
        delete currentProgram;
        currentProgram = NULL;

    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;

        assert(0);
    }
}



void
DataDependenceGraphTest::testPathCalculation() {
    int longestPathLengths0[] = { 9 ,-1,12 };
    int longestPathLengths0M[] = { 8 ,-1,22 };
    int longestPathLengths1[] = { 9, -1,24 ,6};
    int longestPathLengths1M[] = { 17, -1,32 ,17};
    int longestPathLengths2[] = { 15, -1,9};
    int longestPathLengths2M[] = { 13, -1,17};
    try {
        TPEF::BinaryStream binaryStream("data/arrmul.tpef");
        
        // read to TPEF Handler Module
        TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);
       
        ADFSerializer adfSerializer;
        adfSerializer.setSourceFile("data/10_bus_full_connectivity.adf");
        
        TTAMachine::Machine* machine = adfSerializer.readMachine();

        assert(tpef_ != NULL);
    
        // convert the loaded TPEF to POM
        TTAProgram::TPEFProgramFactory factory(
            *tpef_, &UniversalMachine::instance());
        TTAProgram::Program* currentProgram = factory.build();
        {
        ControlFlowGraph cfg0(currentProgram->procedure(0));
        ControlFlowGraph cfg1(currentProgram->procedure(1));
        ControlFlowGraph cfg2(currentProgram->procedure(2));

        DataDependenceGraph* ddg = NULL;
        DataDependenceGraphBuilder builder;

        for (int i = 0; i < cfg0.nodeCount(); i++) {

            BasicBlockNode& bb = cfg0.node(i);
            if (bb.isNormalBB()) {
                auto move0 = 
                    bb.basicBlock().instructionAtIndex(0).move(0).copy();
                auto move1 = 
                    bb.basicBlock().instructionAtIndex(0).move(0).copy();
                auto move2 = 
                    bb.basicBlock().instructionAtIndex(0).move(0).copy();

                ddg = builder.build(
                    bb.basicBlock(), DataDependenceGraph::INTRA_BB_ANTIDEPS,
                    UniversalMachine::instance(), "test ddg", 
                    &UniversalMachine::instance());
                TS_ASSERT_EQUALS(ddg->height(), longestPathLengths0[i]);

                // test adding a new node into graph which forces a 
                // recalculation.
                bool sinkFound = false;
                for (int j = 0; j < ddg->nodeCount(); j++ ) {
                    MoveNode &node = ddg->node(j);
                    if (ddg->maxSourceDistance(node) == ddg->height()) {
                        
                        if (ddg->outDegree(node) == 0) {
                        
                        MoveNode *mn0 = new MoveNode(move0);
                        MoveNode *mn1 = new MoveNode(move1);
                        MoveNode *mn2 = new MoveNode(move2);
                        ddg->addNode(*mn0,bb);
                        ddg->addNode(*mn1,bb);
                        ddg->addNode(*mn2,bb);

                        DataDependenceEdge *e0 = new DataDependenceEdge(
                            DataDependenceEdge::EDGE_REGISTER,
                            DataDependenceEdge::DEP_UNKNOWN,TCEString("foo"),false, false);

                        DataDependenceEdge *e1 = new DataDependenceEdge(
                            DataDependenceEdge::EDGE_REGISTER,
                            DataDependenceEdge::DEP_UNKNOWN,TCEString("foo"),false, false);

                        DataDependenceEdge *e2 = new DataDependenceEdge(
                            DataDependenceEdge::EDGE_REGISTER,
                            DataDependenceEdge::DEP_UNKNOWN,TCEString("foo"),false, false);

                        // add node to end.  should increase height
                        ddg->connectNodes(node, *mn0,*e0);
                        TS_ASSERT_EQUALS(ddg->height(),
                                         longestPathLengths0[i]+3);

                        // add another node to end. should increase height
                        ddg->connectNodes(*mn0, *mn1,*e1);

                        TS_ASSERT_EQUALS(ddg->height(),
                                         longestPathLengths0[i]+6);

                        // add node which has edge to last
                        // should NOT increase height
                        ddg->connectNodes(*mn2, *mn1,*e2);

                        TS_ASSERT_EQUALS(ddg->height(),
                                         longestPathLengths0[i]+6);

                        TS_ASSERT_EQUALS(ddg->maxSinkDistance(*mn2),3);

                        sinkFound = true;
                        break;
                        }
                    }
                }
                TS_ASSERT(sinkFound);

                // check how path lengths change when we set a machine
                ddg->setMachine(*machine);
                
                TS_ASSERT_EQUALS(ddg->height(),
                                 longestPathLengths0M[i]+6);

                delete ddg; ddg = NULL;
            }
        }

        for (int i = 0; i < cfg1.nodeCount(); i++) {
            BasicBlockNode& bb = cfg1.node(i);
            if (bb.isNormalBB()) {
                auto move0 = bb.basicBlock().instructionAtIndex(0).move(0).copy();
                auto move1 = bb.basicBlock().instructionAtIndex(0).move(0).copy();
                auto move2 = bb.basicBlock().instructionAtIndex(0).move(0).copy();
                ddg = builder.build(
                    bb.basicBlock(), 
                    DataDependenceGraph::INTRA_BB_ANTIDEPS,
                    UniversalMachine::instance());
                TS_ASSERT_EQUALS(ddg->height(), longestPathLengths1[i]);


                // test adding a new node into graph which forces a 
                // recalculation.
                bool sinkFound = false;
                for (int j = 0; j < ddg->nodeCount(); j++ ) {
                    MoveNode &node = ddg->node(j);
                    if (ddg->maxSourceDistance(node) == ddg->height()) {
                        
                        TS_ASSERT_EQUALS(ddg->outDegree(node), 0);
                        
                        MoveNode *mn0 = new MoveNode(move0);
                        MoveNode *mn1 = new MoveNode(move1);
                        MoveNode *mn2 = new MoveNode(move2);
                        ddg->addNode(*mn0,bb);
                        ddg->addNode(*mn1,bb);
                        ddg->addNode(*mn2,bb);

                        DataDependenceEdge *e0 = new DataDependenceEdge(
                            DataDependenceEdge::EDGE_REGISTER,
                            DataDependenceEdge::DEP_UNKNOWN,TCEString("foo"),false, false);

                        DataDependenceEdge *e1 = new DataDependenceEdge(
                            DataDependenceEdge::EDGE_REGISTER,
                            DataDependenceEdge::DEP_UNKNOWN,TCEString("foo"),false, false);

                        DataDependenceEdge *e2 = new DataDependenceEdge(
                            DataDependenceEdge::EDGE_REGISTER,
                            DataDependenceEdge::DEP_UNKNOWN,TCEString("foo"),false, false);

                        // add node to end. should increase height
                        ddg->connectNodes(node, *mn0,*e0);
                        TS_ASSERT_EQUALS(ddg->height(),
                                         longestPathLengths1[i]+3);

                        // add another node to end. should increase height
                        ddg->connectNodes(*mn0, *mn1,*e1);

                        TS_ASSERT_EQUALS(ddg->height(),
                                         longestPathLengths1[i]+6);

                        // add node which has edge to last
                        // should NOT increase height
                        ddg->connectNodes(*mn2, *mn1,*e2);

                        TS_ASSERT_EQUALS(ddg->height(),
                                         longestPathLengths1[i]+6);

                        TS_ASSERT_EQUALS(ddg->maxSinkDistance(*mn2),3);

                        sinkFound = true;
                        break;
                    }
                }
                TS_ASSERT(sinkFound);

                // check how path lengths change when we set a machine
                ddg->setMachine(*machine);
                
                TS_ASSERT_EQUALS(ddg->height(),
                                 longestPathLengths1M[i]+6);

                delete ddg; ddg = NULL;
            }
        }
        
        for (int i = 0; i < cfg2.nodeCount(); i++) {
            BasicBlockNode& bb = cfg2.node(i);
            if (bb.isNormalBB()) {
                auto move0 = 
                    bb.basicBlock().instructionAtIndex(0).move(0).copy();
                auto move1 = 
                    bb.basicBlock().instructionAtIndex(0).move(0).copy();
                auto move2 = 
                    bb.basicBlock().instructionAtIndex(0).move(0).copy();
                ddg = builder.build(
                    bb.basicBlock(),
                    DataDependenceGraph::INTRA_BB_ANTIDEPS,
                    UniversalMachine::instance());
                TS_ASSERT_EQUALS(ddg->height(), longestPathLengths2[i]);

                // test adding a new node into graph which forces a 
                // recalculation.
                bool sinkFound = false;
                for (int j = 0; j < ddg->nodeCount(); j++ ) {
                    MoveNode &node = ddg->node(j);
                    if (ddg->maxSourceDistance(node) == ddg->height()) {
                        
                        if (ddg->outDegree(node) == 0) {
                        
                        MoveNode *mn0 = new MoveNode(move0);
                        MoveNode *mn1 = new MoveNode(move1);
                        MoveNode *mn2 = new MoveNode(move2);

                        ddg->addNode(*mn0, bb);
                        ddg->addNode(*mn1, bb);
                        ddg->addNode(*mn2, bb);

                        DataDependenceEdge *e0 = new DataDependenceEdge(
                            DataDependenceEdge::EDGE_REGISTER,
                            DataDependenceEdge::DEP_UNKNOWN,TCEString("foo"),false, false);

                        DataDependenceEdge *e1 = new DataDependenceEdge(
                            DataDependenceEdge::EDGE_REGISTER,
                            DataDependenceEdge::DEP_UNKNOWN,TCEString("foo"),false, false);

                        DataDependenceEdge *e2 = new DataDependenceEdge(
                            DataDependenceEdge::EDGE_REGISTER,
                            DataDependenceEdge::DEP_UNKNOWN,TCEString("foo"),false, false);

                        // add node to end. should increase height
                        ddg->connectNodes(node, *mn0, *e0);
                        TS_ASSERT_EQUALS(ddg->height(),
                                         longestPathLengths2[i]+3);

                        // add another node to end. should increase height
                        ddg->connectNodes(*mn0, *mn1, *e1);

                        TS_ASSERT_EQUALS(ddg->height(),
                                         longestPathLengths2[i]+6);

                        // add node which has edge to last
                        // should NOT increase height.
                        // added node should have sink distance of 1
                        ddg->connectNodes(*mn2, *mn1,*e2);

                        TS_ASSERT_EQUALS(ddg->height(),
                                         longestPathLengths2[i]+6);

                        TS_ASSERT_EQUALS(ddg->maxSinkDistance(*mn2),3);

                        sinkFound = true;
                        break;
                            }
                    }
                }
                TS_ASSERT(sinkFound);

                // check how path lengths change when we set a machine
                ddg->setMachine(*machine);
                
                TS_ASSERT_EQUALS(ddg->height(),
                                 longestPathLengths2M[i]+6);

                delete ddg; ddg = NULL;
            }
        }
        }
        delete currentProgram;
        currentProgram = NULL;

    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;

        assert(0);
    }
}

void
DataDependenceGraphTest::testSWBypassing() {

    TPEF::BinaryStream binaryStream("data/arrmul.tpef");

    // read to TPEF Handler Module
    TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);

    assert(tpef_ != NULL);

    // convert the loaded TPEF to POM
    TTAProgram::TPEFProgramFactory factory(
        *tpef_, &UniversalMachine::instance());
    TTAProgram::Program* currentProgram = factory.build();
    {
    ControlFlowGraph cfg(currentProgram->procedure(2));

    DataDependenceGraph* ddg = NULL;

    DataDependenceGraphBuilder builder;

    ddg = builder.build(
        cfg, DataDependenceGraph::ALL_ANTIDEPS,
        UniversalMachine::instance(), &UniversalMachine::instance(),
        true, true);
    // these do not exist at the beginning..
    TS_ASSERT(!ddg->hasEdge(ddg->node(6), ddg->node(27)));
    TS_ASSERT(!ddg->hasEdge(ddg->node(6), ddg->node(26)));

    TS_ASSERT(!ddg->hasEdge(ddg->node(20), ddg->node(27)));
    TS_ASSERT(!ddg->hasEdge(ddg->node(20), ddg->node(26)));

    // do the bypassing...
    MoveNode& res = ddg->node(24);
    MoveNode& user = ddg->node(25);
    MoveNode& warDest = ddg->node(27);

    TS_ASSERT(ddg->hasEdge(user, warDest));

    TS_ASSERT(ddg->resultUsed(res));
    ddg->mergeAndKeepUser(res, user);

    // new op edges
    TS_ASSERT(ddg->hasEdge(ddg->node(22), user));
    TS_ASSERT(ddg->hasEdge(ddg->node(23), user));
    // and missing original edge
    TS_ASSERT(!ddg->hasEdge(res, user));
    // new WaW edge
    TS_ASSERT(ddg->hasEdge(res, warDest));


    // do DRE
    TS_ASSERT(!ddg->resultUsed(res));

    ddg->copyDepsOver(res, true, false);

    // updated WaW edge due copyantidepsover
    TS_ASSERT(ddg->hasEdge(ddg->node(6), warDest));
    // updated WaR edge due copyantidepsover
    TS_ASSERT(ddg->hasEdge(ddg->node(20), warDest)); //ddg->node(26))); 

    ddg->removeNode(res); // should fix WaW edge 
/*
    TTAProgram::Instruction& ins =  res.move().parent();
    ins.removeMove(res.move());
    if (ins.moveCount() == 0) {
        ins.parent().remove(ins);
    }
    delete &res;
*/
    // updated WaW edge shoudl still be there
    TS_ASSERT(ddg->hasEdge(ddg->node(6), warDest));
    // updated WaR edge should stll be there
    TS_ASSERT(ddg->hasEdge(ddg->node(20), warDest)); //ddg->node(26))); 

    // try to do another bypass.. that cannot ne DRE'd.
    MoveNode& res2 = ddg->node(6);
    MoveNode& user2 = ddg->node(7);

    TS_ASSERT(ddg->resultUsed(res2));
    ddg->mergeAndKeepUser(res2, user2);
    // result still alive
    TS_ASSERT(ddg->resultUsed(res2));

    // new op edges
    TS_ASSERT(ddg->hasEdge(ddg->node(4), user2));
    TS_ASSERT(ddg->hasEdge(ddg->node(5), user2));
    // and missing original edge
    TS_ASSERT(!ddg->hasEdge(res2, user2));

    ddg->unMergeUser(res2,user2);

    // new op edges should be removed 
    TS_ASSERT(!ddg->hasEdge(ddg->node(4),user2));
    TS_ASSERT(!ddg->hasEdge(ddg->node(5),user2));
    // original edge should have returned
    TS_ASSERT(ddg->hasEdge(res2,user2));

    delete ddg;
    }
    delete currentProgram;
    currentProgram = NULL;
    
}


/**
 * Tests if the bookkeeping of registers used in later BB's work.
 *
 * @todo
 */
void
DataDependenceGraphTest::testUsedAfter() {

    UniversalMachine* umach = &UniversalMachine::instance();
    TPEF::BinaryStream binaryStream("data/arrmul.tpef");

    // read to TPEF Handler Module
    TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);

    assert(tpef_ != NULL);

    // convert the loaded TPEF to POM
    TTAProgram::TPEFProgramFactory factory(*tpef_, umach);
    TTAProgram::Program* currentProgram = factory.build();
    DataDependenceGraphBuilder builder;
    {
    ControlFlowGraph cfg1(currentProgram->procedure(1));
    DataDependenceGraph* ddg1 = NULL;
    ddg1 = builder.build(
        cfg1, DataDependenceGraph::ALL_ANTIDEPS, *umach, umach, true, true);

    for (int i = 0; i < cfg1.nodeCount(); i++) {
        BasicBlockNode& bbn = cfg1.node(i);
        if (bbn.isNormalBB()) {
            TTAProgram::BasicBlock& bb = bbn.basicBlock();
            std::set<TCEString> usedAfter = bb.liveRangeData_->usedAfter();
            TS_ASSERT_EQUALS(usedAfter.size(), usedAfterSizes1[i]);   

            // due very simple loop, the usedafter is same for both
            // BB0 and BB2.
            if (i != 3) {

                // r1=sp is used after
                TS_ASSERT(usedAfter.find("universal_integer_rf.1") != 
                          usedAfter.end());

                // r2 = iarg1 is not used after
                TS_ASSERT(usedAfter.find("universal_integer_rf.2") == 
                          usedAfter.end());

                // r3 = iarg2 is not used after
                TS_ASSERT(usedAfter.find("universal_integer_rf.3") == 
                          usedAfter.end());

                // r4 = iarg3 is not used after
                TS_ASSERT(usedAfter.find("universal_integer_rf.4") == 
                          usedAfter.end());

                // r5 = iarg4 is used after
                TS_ASSERT(usedAfter.find("universal_integer_rf.5") != 
                          usedAfter.end());

                // r6 is used after
                TS_ASSERT(usedAfter.find("universal_integer_rf.6") != 
                          usedAfter.end());

                // r7 is used after
                TS_ASSERT(usedAfter.find("universal_integer_rf.7") != 
                          usedAfter.end());

                // r8 is used after
                TS_ASSERT(usedAfter.find("universal_integer_rf.8") != 
                          usedAfter.end());

                // r9 is used after
                TS_ASSERT(usedAfter.find("universal_integer_rf.9") != 
                          usedAfter.end());

                // rv is used after as we don't know it's void.
                TS_ASSERT(usedAfter.find("universal_integer_rf.0") != 
                          usedAfter.end());

                // RA is loaded before return so the old value not used
                TS_ASSERT(usedAfter.find("RA") == 
                          usedAfter.end());

            }
        }
    }
    delete ddg1;


    ControlFlowGraph cfg2(currentProgram->procedure(2));
    DataDependenceGraph* ddg2 = NULL;
    ddg2 = builder.build(
        cfg2, DataDependenceGraph::ALL_ANTIDEPS, *umach, umach, true, true);

    for (int i = 0; i < cfg2.nodeCount(); i++) {
        BasicBlockNode& bbn = cfg2.node(i);
        if (bbn.isNormalBB()) {
            TTAProgram::BasicBlock& bb = bbn.basicBlock();
            std::set<TCEString> usedAfter = bb.liveRangeData_->usedAfter();
            TS_ASSERT_EQUALS(usedAfter.size(), usedAfterSizes2[i]);   

            // due very simple loop, the usedafter is same for both
            // BB0 and BB2.
            if (i == 0) {

                // rv is used after as we don't know it's void.
                TS_ASSERT(usedAfter.find("universal_integer_rf.0") != 
                          usedAfter.end());

                // r1=sp is used after
                TS_ASSERT(usedAfter.find("universal_integer_rf.1") != 
                          usedAfter.end());

                // r2 = iarg1 is not used after
                TS_ASSERT(usedAfter.find("universal_integer_rf.2") == 
                          usedAfter.end());

                // r3 = iarg2 is not used after
                TS_ASSERT(usedAfter.find("universal_integer_rf.3") == 
                          usedAfter.end());

                // r4 = iarg3 is not used after
                TS_ASSERT(usedAfter.find("universal_integer_rf.4") == 
                          usedAfter.end());

                // r5 = iarg4 is used after
                TS_ASSERT(usedAfter.find("universal_integer_rf.5") == 
                          usedAfter.end());

                // RA is loaded before return so the old value not used
                TS_ASSERT(usedAfter.find("RA") == 
                          usedAfter.end());
                
            }
        }
    }
    delete ddg2;
    }
    delete currentProgram;
}

void DataDependenceGraphTest::testRallocatedUsedAfter() {

    UniversalMachine* umach = &UniversalMachine::instance();
    TPEF::BinaryStream binaryStream("data/rallocated_arrmul.tpef");
    
    ADFSerializer adfSerializer;
    adfSerializer.setSourceFile("data/10_bus_full_connectivity.adf");
    
    TTAMachine::Machine* machine = adfSerializer.readMachine();
    
    // read to TPEF Handler Module
    TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);
    
    assert(tpef_ != NULL);
    
    
    // convert the loaded TPEF to POM
    TTAProgram::TPEFProgramFactory factory(*tpef_, *machine, umach);
    TTAProgram::Program* currentProgram = factory.build();
    {
    DataDependenceGraphBuilder builder;
    ControlFlowGraph cfg0(currentProgram->procedure(0));
    ControlFlowGraph cfg(currentProgram->procedure(1));
    ControlFlowGraph cfg2(currentProgram->procedure(2));
    
    DataDependenceGraph* ddg = NULL;
    
    // DDg builder reads sp and rv from there.
    ddg = builder.build(
        cfg0, DataDependenceGraph::SINGLE_BB_LOOP_ANTIDEPS, *machine);
    delete ddg;

    ddg = builder.build(
        cfg, DataDependenceGraph::SINGLE_BB_LOOP_ANTIDEPS, *machine);
    
    for (int i = 0; i < cfg.nodeCount(); i++) {
        BasicBlockNode& bbn = cfg.node(i);
        if (bbn.isNormalBB()) {
            TTAProgram::BasicBlock& bb = bbn.basicBlock();
            std::set<TCEString> usedAfter = bb.liveRangeData_->usedAfter();
            TS_ASSERT_EQUALS(
                usedAfter.size(), rallocatedUsedAfterSizes1[i]);   

            if (i != 3) {

                // rv is never used so it's not needed
                TS_ASSERT(usedAfter.find("integer0.0") == 
                          usedAfter.end());

                // r1=sp is used after
                TS_ASSERT(usedAfter.find("integer0.1") != 
                          usedAfter.end());

                // r2 = iarg1 is not used after
                TS_ASSERT(usedAfter.find("integer0.2") == 
                          usedAfter.end());

                // r3 = iarg2 is not used after
                TS_ASSERT(usedAfter.find("integer0.3") == 
                          usedAfter.end());

                // r4 = iarg3 is not used after
                TS_ASSERT(usedAfter.find("integer0.4") == 
                          usedAfter.end());

                // r5 = iarg4 is used after
                TS_ASSERT(usedAfter.find("integer0.5") != 
                          usedAfter.end());

                // r6 is used after
                TS_ASSERT(usedAfter.find("integer0.6") != 
                          usedAfter.end());

                // r7 is used after
                TS_ASSERT(usedAfter.find("integer0.7") != 
                          usedAfter.end());

                // r8 is not used after
                TS_ASSERT(usedAfter.find("integer0.8") == 
                          usedAfter.end());

                // r1.1 is used after
                TS_ASSERT(usedAfter.find("integer1.1") != 
                          usedAfter.end());

                // r1.1 is used after
                TS_ASSERT(usedAfter.find("integer1.0") != 
                          usedAfter.end());


                // RA is not loaded before return so the old value is used
                TS_ASSERT(usedAfter.find("RA") != 
                          usedAfter.end());

            }
        }
    }
    delete ddg;

    ddg = builder.build(
        cfg2, DataDependenceGraph::SINGLE_BB_LOOP_ANTIDEPS, *machine);
    
    for (int i = 0; i < cfg2.nodeCount(); i++) {
        BasicBlockNode& bbn = cfg2.node(i);
        if (bbn.isNormalBB()) {
            TTAProgram::BasicBlock& bb = bbn.basicBlock();
            std::set<TCEString> usedAfter = bb.liveRangeData_->usedAfter();
            TS_ASSERT_EQUALS(
                usedAfter.size(), rallocatedUsedAfterSizes2[i]);   

            if (i == 0) {

                // rv is never used so it's not needed
                TS_ASSERT(usedAfter.find("integer0.0") == 
                          usedAfter.end());

                // r1=sp is used after
                TS_ASSERT(usedAfter.find("integer0.1") != 
                          usedAfter.end());

                // RV is loaded from mem so old value not used.
                TS_ASSERT(usedAfter.find("RA") ==
                          usedAfter.end());

            }
        }
    }
    }
    delete currentProgram;
}

void DataDependenceGraphTest::testSpecialRegIPData() {

    UniversalMachine* umach = &UniversalMachine::instance();
    TPEF::BinaryStream binaryStream("data/rallocated_arrmul.tpef");
    
    ADFSerializer adfSerializer;
    adfSerializer.setSourceFile("data/10_bus_full_connectivity.adf");
    
    TTAMachine::Machine* machine = adfSerializer.readMachine();
    
    // read to TPEF Handler Module
    TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);
    
    assert(tpef_ != NULL);
    
    
    // convert the loaded TPEF to POM
    TTAProgram::TPEFProgramFactory factory(*tpef_, *machine, umach);
    TTAProgram::Program* currentProgram = factory.build();
    {
    DataDependenceGraphBuilder builder;
    ControlFlowGraph cfg0(currentProgram->procedure(0));
    ControlFlowGraph cfg(currentProgram->procedure(1));
    ControlFlowGraph cfg2(currentProgram->procedure(2));

    ControlFlowGraph cfg21(currentProgram->procedure(1));
    ControlFlowGraph cfg22(currentProgram->procedure(2));
    DataDependenceGraph* ddg = NULL;
    
    // we do not know SP and RV here, so those are not alive
    ddg = builder.build(
        cfg, DataDependenceGraph::SINGLE_BB_LOOP_ANTIDEPS, *machine);
    
    for (int i = 0; i < cfg.nodeCount(); i++) {
        BasicBlockNode& bbn = cfg.node(i);
        if (bbn.isNormalBB()) {
            TTAProgram::BasicBlock& bb = bbn.basicBlock();
            std::set<TCEString> usedAfter = bb.liveRangeData_->usedAfter();
            TS_ASSERT_EQUALS(
                usedAfter.size(), splessUsedAfterSizes1[i]);

            if (i != 3) {

                // rv is never used so it's not needed
                TS_ASSERT(usedAfter.find("integer0.0") == 
                          usedAfter.end());

                // SP not known here so not found.
                TS_ASSERT(usedAfter.find("integer0.1") == 
                          usedAfter.end());

            }
        }
    }
    delete ddg;

    // we do not know SP and RV here
    ddg = builder.build(
        cfg2, DataDependenceGraph::SINGLE_BB_LOOP_ANTIDEPS, *machine);
    
    for (int i = 0; i < cfg2.nodeCount(); i++) {
        BasicBlockNode& bbn = cfg2.node(i);
        if (bbn.isNormalBB()) {
            TTAProgram::BasicBlock& bb = bbn.basicBlock();
            std::set<TCEString> usedAfter = bb.liveRangeData_->usedAfter();
            TS_ASSERT_EQUALS(
                usedAfter.size(), splessUsedAfterSizes2[i]);   

            if (i == 0) {

                // rv is never used so it's not needed
                TS_ASSERT(usedAfter.find("integer0.0") == 
                          usedAfter.end());

                // SP known here, gets from annotations.x
                TS_ASSERT(usedAfter.find("integer0.1") != 
                          usedAfter.end());
                
            }
        }
    }
    delete ddg;
        
    typedef SimpleInterPassDatum<
        std::pair<TCEString, unsigned int> > RegDatum;

    // no nice constructor available..
    RegDatum* sp = new RegDatum; sp->first = "integer0"; sp->second = 1;
    RegDatum* rv = new RegDatum; rv->first = "integer0"; rv->second = 0;

    InterPassData ipd;

    ipd.setDatum("STACK_POINTER", sp);
    ipd.setDatum("RV_REGISTER", rv);
    
    DataDependenceGraphBuilder builder2(ipd);
    
    ddg = builder2.build(
        cfg21, DataDependenceGraph::SINGLE_BB_LOOP_ANTIDEPS, *machine);
    
    for (int i = 0; i < cfg21.nodeCount(); i++) {
        BasicBlockNode& bbn = cfg21.node(i);
        if (bbn.isNormalBB()) {
            TTAProgram::BasicBlock& bb = bbn.basicBlock();
            std::set<TCEString> usedAfter = bb.liveRangeData_->usedAfter();
            TS_ASSERT_EQUALS(
                usedAfter.size(), ipdUsedAfterSizes1[i]);   

            if (i == 0) {

                // rv is used by return.
                TS_ASSERT(usedAfter.find("integer0.0") != 
                          usedAfter.end());

                // SP not known here so not found.
                TS_ASSERT(usedAfter.find("integer0.1") != 
                          usedAfter.end());

            }
        }
    }
    delete ddg;

    ddg = builder2.build(
        cfg22, DataDependenceGraph::SINGLE_BB_LOOP_ANTIDEPS, *machine);
    
    for (int i = 0; i < cfg22.nodeCount(); i++) {
        BasicBlockNode& bbn = cfg22.node(i);
        if (bbn.isNormalBB()) {
            TTAProgram::BasicBlock& bb = bbn.basicBlock();
            std::set<TCEString> usedAfter = bb.liveRangeData_->usedAfter();
            TS_ASSERT_EQUALS(
                usedAfter.size(), ipdUsedAfterSizes2[i]);   

            if (i == 0) {

                // rv known so should be in use?
                TS_ASSERT(usedAfter.find("integer0.0") != 
                          usedAfter.end());

                // SP not known here so not found.
                TS_ASSERT(usedAfter.find("integer0.1") != 
                          usedAfter.end());

            }
        }
    }
    delete ddg;
    }
    delete currentProgram;
}

#endif
