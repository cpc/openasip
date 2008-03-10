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

    GraphEdge* edgeAB = new GraphEdge(0);
    GraphEdge* edgeAC = new GraphEdge(1);
    GraphEdge* edgeBC = new GraphEdge(2);
    GraphEdge* edgeDC = new GraphEdge(3);

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
