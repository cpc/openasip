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
 * @file BoostGraphTest.hh
 * 
 * A test suite for BoostGraphTest.hh
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 */

#ifndef BOOST_GRAPH_TEST_HH
#define BOOST_GRAPH_TEST_HH

#include <TestSuite.h>
#include "BoostGraph.hh"
#include "GraphNode.hh"
#include "GraphEdge.hh"
#include "AssocTools.hh"

/**
 * Class for testing BoostGraph.
 */
class BoostGraphTest : public CxxTest::TestSuite {
public:
    BoostGraphTest();
    virtual ~BoostGraphTest();

    void setUp();
    void tearDown();
    
    void testRootNodeFinding();
    void testEdgeMoving();

private:
    typedef BoostGraph<GraphNode, GraphEdge> TestGraph;
    TestGraph testGraph_;
    TestGraph::NodeSet nodes_;
    TestGraph::EdgeSet edges_;
    TestGraph::NodeSet rootNodes_;
    GraphNode* node0_;
    GraphNode* node1_;
    GraphNode* node2_;
    GraphNode* node3_;
};

/**
 * Initialize the test graphs.
 */
BoostGraphTest::BoostGraphTest() {

    GraphNode* nodeA = new GraphNode(0);
    node0_ = nodeA;
    GraphNode* nodeB = new GraphNode(1);
    node1_ = nodeB;
    GraphNode* nodeC = new GraphNode(2);
    node2_ = nodeC;
    GraphNode* nodeD = new GraphNode(3);
    node3_ = nodeD;   

    nodes_.insert(nodeA);
    nodes_.insert(nodeB);
    nodes_.insert(nodeC);
    nodes_.insert(nodeD);

    GraphEdge* edgeAB = new GraphEdge;
    GraphEdge* edgeAC = new GraphEdge;
    GraphEdge* edgeBC = new GraphEdge;
    GraphEdge* edgeDC = new GraphEdge;

    edges_.insert(edgeAB);
    edges_.insert(edgeAC);
    edges_.insert(edgeBC);
    edges_.insert(edgeDC);

    testGraph_.addNode(*nodeA);
    testGraph_.addNode(*nodeB);
    testGraph_.addNode(*nodeC);
    testGraph_.addNode(*nodeD);

    testGraph_.connectNodes(*nodeA, *nodeB, *edgeAB);
    testGraph_.connectNodes(*nodeA, *nodeC, *edgeAC);
    testGraph_.connectNodes(*nodeB, *nodeC, *edgeBC);
    testGraph_.connectNodes(*nodeD, *nodeC, *edgeDC);

    rootNodes_.insert(nodeA);
    rootNodes_.insert(nodeD);

#if 0
    testGraph_.writeToDotFile("testGraph.dot");
#endif
}

/**
 * Delete the test graphs to avoid polluting valgrind output.
 */
BoostGraphTest::~BoostGraphTest() {
    AssocTools::deleteAllItems(nodes_);
    AssocTools::deleteAllItems(edges_);
}

/**
 * Called before each test.
 */
void
BoostGraphTest::setUp() {
}


/**
 * Called after each test.
 */
void
BoostGraphTest::tearDown() {
}

/**
 * Test the root node finding.
 */
void
BoostGraphTest::testRootNodeFinding() {

    TestGraph::NodeSet rootNodes = testGraph_.rootNodes();

    TS_ASSERT_EQUALS(rootNodes, rootNodes_);
}
/**
 * Test the helper methods for moving edges between nodes.
 */
void
BoostGraphTest::testEdgeMoving() {
    
    TS_ASSERT_EQUALS(testGraph_.inDegree(*node3_), 0);

    testGraph_.moveInEdges(*node1_, *node3_);

    TS_ASSERT_EQUALS(testGraph_.inDegree(*node1_), 0);
    TS_ASSERT_EQUALS(testGraph_.inDegree(*node3_), 1);

    TS_ASSERT_EQUALS(testGraph_.inDegree(*node2_), 3);
    TS_ASSERT_EQUALS(testGraph_.outDegree(*node3_), 1);
    TS_ASSERT_EQUALS(testGraph_.outDegree(*node0_), 2);
    
    testGraph_.moveOutEdges(*node3_, *node0_);

    TS_ASSERT_EQUALS(testGraph_.inDegree(*node2_), 3);
    TS_ASSERT_EQUALS(testGraph_.outDegree(*node3_), 0);
    TS_ASSERT_EQUALS(testGraph_.outDegree(*node0_), 3);
}

#endif
