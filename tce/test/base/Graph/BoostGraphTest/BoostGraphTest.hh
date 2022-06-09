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
 * @file BoostGraphTest.hh
 * 
 * A test suite for BoostGraphTest.hh
 *
 * @author Pekka Jääskeläinen 2007 (pjaaskel-no.spam-cs.tut.fi)
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
