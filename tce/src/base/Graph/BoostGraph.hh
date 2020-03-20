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
 * @file BoostGraph.hh
 *
 * Declaration of boost-based templated implementation of graph base class.
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @author Heikki Kultala 2007-2010
 * @author Pekka J‰‰skel‰inen 2009-2010
 * @note rating: red
 */


#ifndef TTA_BOOST_GRAPH_HH
#define TTA_BOOST_GRAPH_HH

#include <boost/version.hpp>

#if BOOST_VERSION < 103500
// from boost v1.35 (missing from 1.34 making findAllPaths() fail)
#include <boost/graph/detail/edge.hpp>

namespace boost {
    namespace detail {
        template <class D, class V>
        inline bool
        operator<(const detail::edge_desc_impl<D,V>& a, 
                  const detail::edge_desc_impl<D,V>& b) {
            return a.get_property() < b.get_property();
        }
    }
}

#endif

#include <map>
#include <set>

// these need to be included before Boost so we include a working
// and warning-free hash_map
#include "hash_set.hh"
#include "hash_map.hh"

#include "CompilerWarnings.hh"
IGNORE_CLANG_WARNING("-Wunused-local-typedef")
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/subgraph.hpp>
POP_CLANG_DIAGS

#include "Exception.hh"
#include "Graph.hh"

/**
 * Graph-based program representation.
 *
 * Boost::graph based implementation. 
 */
template <typename GraphNode, typename GraphEdge>
class BoostGraph : public GraphBase<GraphNode, GraphEdge> {
public:

    typedef std::set<GraphNode*, typename GraphNode::Comparator > NodeSet;
    typedef std::set<GraphEdge*, typename GraphEdge::Comparator > EdgeSet;
    /// The (base) node type managed by this graph. 
    /// @todo What's the point of these typedefs?
    typedef GraphNode Node;
    /// The (base) edge type managed by this graph.
    typedef GraphEdge Edge;

    BoostGraph(bool allowLoopEdges = true);
    BoostGraph(const TCEString& name, bool allowLoopEdges = true);
    BoostGraph(const BoostGraph& other, bool allowLoopEdges = true);
    ~BoostGraph();

    int nodeCount() const;
    int edgeCount() const;
    Node& node(const int index) const;
    Node& node(const int index, bool cacheResult) const;
    virtual Edge& edge(const int index) const;

    virtual void addNode(Node& node);

    virtual Edge& outEdge(const Node& node, const int index) const;

    virtual Edge& inEdge(const Node& node, const int index) const;

    virtual EdgeSet outEdges(const Node& node) const;
    virtual EdgeSet inEdges(const Node& node) const;

    virtual EdgeSet rootGraphOutEdges(const Node& node) const;

    virtual EdgeSet rootGraphInEdges(const Node& node) const;

    virtual Edge& rootGraphInEdge(const Node& node, const int index) const;

    virtual Edge& rootGraphOutEdge(const Node& node, const int index) const;

    virtual int rootGraphInDegree(const Node& node) const;

    virtual int rootGraphOutDegree(const Node& node) const;

    virtual int outDegree(const Node& node) const;
    virtual int inDegree(const Node& node) const;

    virtual Node& tailNode(const Edge& edge) const;
    virtual Node& headNode(const Edge& edge) const;

    virtual void connectNodes(const Node& nTail, const Node& nHead, Edge& e);

    virtual void disconnectNodes(const Node& nTail, const Node& nHead);

    virtual void moveInEdges(const Node& source, const Node& destination);
    virtual void moveOutEdges(const Node& source, const Node& destination);

    virtual void moveInEdge(const Node& source, const Node& destination,
                            Edge& edge, 
                            const Node* tail = NULL, bool childs = false);

    virtual void moveOutEdge(const Node& source, const Node& destination,
                             Edge& edge, 
                             const Node* head = NULL, bool childs = false);

    virtual void copyInEdge(const Node& destination, Edge& edge, 
                             const Node* tail = NULL);

    virtual void copyOutEdge(const Node& destination, Edge& edge, 
                             const Node* head = NULL);

    virtual void removeNode(Node& node);
    virtual void removeEdge(Edge& e);

    virtual void dropNode(Node& node);

    virtual void dropEdge(Edge& edge);

    virtual bool hasEdge(
        const Node& nTail,
        const Node& nHead) const;

    /// useful utility functions 
    virtual NodeSet rootNodes() const;
    virtual NodeSet sinkNodes() const;

    virtual NodeSet successors(
        const Node& node, bool ignoreBackEdges=false, 
        bool ignoreForwardEdges=false) const;
    virtual NodeSet predecessors(
        const Node& node, bool ignoreBackEdges=false,
        bool ignoreForwardEdges=false) const;

    // critical path-related functions
    int maxPathLength(const GraphNode& node) const;
    int maxSinkDistance(const GraphNode& node) const;
    int maxSourceDistance(const GraphNode& node) const;
    bool isInCriticalPath(const GraphNode& node) const {
        return maxSinkDistance(node) + maxSourceDistance(node) == height();
    }
    int height() const;
    void findAllPaths() const;

    void detachSubgraph(BoostGraph& subGraph);

    BoostGraph* parentGraph();
    BoostGraph* rootGraph();

    const BoostGraph* rootGraph() const;

    bool hasNode(const Node&) const;

    virtual const TCEString& name() const;

    bool hasPath(GraphNode& src, const GraphNode& dest) const;

    void restoreNodeFromParent(GraphNode& node);
    bool detectIllegalCycles() const;

    EdgeSet connectingEdges(
        const Node& nTail, const Node& nHead) const;

private:
    /// Assignment forbidden.
    BoostGraph& operator=(const BoostGraph&);

protected:   

    class GraphHashFunctions {
    public:
        size_t operator()(const GraphNode* node) const {
            int tmp = reinterpret_cast<size_t>(node);
            return tmp ^ (tmp >> 16);
        }
        size_t operator()(const GraphEdge* edge) const {
            int tmp = reinterpret_cast<size_t>(edge);
            return tmp ^ (tmp >> 16);
        }
    };

    struct RemovedEdgeDatum {
        GraphNode& nTail;
        GraphNode& nHead;
        GraphEdge& edge;

        RemovedEdgeDatum(GraphNode& tail, GraphNode& head, GraphEdge& e) :
            nTail(tail), nHead(head), edge(e) {}

        bool operator< (const RemovedEdgeDatum& other) const {
            return edge.edgeID() < other.edge.edgeID();
        }
    };

    typedef std::set<RemovedEdgeDatum> RemovedEdgeMap;

    /// Internal graph type, providing actual graph-like operations.
    /// This type definition relies on bundled properties of boost library,
    /// which need the host compiler to support partial template
    /// specialisation.
    
    typedef typename boost::adjacency_list<
        boost::listS, boost::vecS,
        boost::bidirectionalS, Node*, Edge*> Graph;

    /// Traits characterising the internal graph type.
    typedef typename boost::graph_traits<Graph> GraphTraits;
    
    /// Output edge iterator type.
    typedef typename GraphTraits::out_edge_iterator OutEdgeIter;
    /// Input edge iterator type.
    typedef typename GraphTraits::in_edge_iterator InEdgeIter;
    /// Iterator type for the list of all edges in the graph.
    typedef typename GraphTraits::edge_iterator EdgeIter;
    /// Iterator type for the list of all nodes in the graph.
    typedef typename GraphTraits::vertex_iterator NodeIter;
    
    /// Type with which edges of the graph are seen internally.
    typedef typename GraphTraits::edge_descriptor EdgeDescriptor;
    /// Type with which nodes of the graph are seen internally.
    typedef typename GraphTraits::vertex_descriptor NodeDescriptor;
    
    // private helper methods

    // this is a slow(linear to size of the graph) operation
    EdgeDescriptor descriptor(const Edge& e) const;
    // these two are much faster operations
    EdgeDescriptor edgeDescriptor(
        const NodeDescriptor& tailNode, const Edge& e) const;
    EdgeDescriptor edgeDescriptor(
        const Edge& e, const NodeDescriptor& headNode) const;
    NodeDescriptor descriptor(const Node& n) const;
    bool hasEdge(
        const Node& nTail,
        const Node& nHead,
        const Edge& edge) const;
    bool hasEdge(
        const Edge& edge, const Node* nTail = NULL, const Node* nHead = NULL) 
        const;
    
    EdgeDescriptor connectingEdge(
        const Node& nTail,
        const Node& nHead) const;

    // optimized but uglier versions
    Node& tailNode(const Edge& edge, const NodeDescriptor& headNode) const;
    Node& headNode(const Edge& edge, const NodeDescriptor& tailNode) const;

    // fast node removal
    void replaceNodeWithLastNode(GraphNode& dest);

    // internal implementation of path-length-related things.
    void calculatePathLengths() const;

    void calculatePathLengthsFast() const;

    void calculateSinkDistance(
        const GraphNode& node, int len, bool looping = false) const;
    
    void calculateSourceDistances(
        const GraphNode* startNode = NULL, int startingLength = 0,
        bool looping = false) const;

    void calculatePathLengthsOnConnect(
        const GraphNode& nTail, const GraphNode& nHead, GraphEdge& e);

    virtual int edgeWeight( GraphEdge& e, const GraphNode& n) const;
    
    // Calculated path lengths
    mutable std::map<const GraphNode*,int, typename GraphNode::Comparator>
    sourceDistances_;
    mutable std::map<const GraphNode*,int, typename GraphNode::Comparator> 
    sinkDistances_;

    // Calculated path lengths
    mutable std::map<const GraphNode*,int, typename GraphNode::Comparator>
    loopingSourceDistances_;
    mutable std::map<const GraphNode*,int, typename GraphNode::Comparator> 
    loopingSinkDistances_;
    
    mutable int height_;

    /// The internal graph structure.
    Graph graph_;

    // these cache data that may get cached even on ro operations,
    // so they are mutable. 
    typedef hash_map<const Edge*, EdgeDescriptor, GraphHashFunctions>
    EdgeDescMap;
    typedef hash_map<const Node*, NodeDescriptor, GraphHashFunctions>
    NodeDescMap;

    mutable EdgeDescMap edgeDescriptors_;
    mutable NodeDescMap nodeDescriptors_;

    void clearDescriptorCache(EdgeSet edges);

    // graph editing functions which tell the changes to parents and childs

    virtual void removeNode(Node& node, BoostGraph* modifierGraph);
    virtual void removeEdge(
        Edge& e, const GraphNode* tailNode, const GraphNode* headNode,
        BoostGraph* modifierGraph = NULL);

    virtual void connectNodes(
        const Node& nTail, const Node& nHead, Edge& e,
        GraphBase<GraphNode, GraphEdge>* modifier, bool creatingSG = false);

    void moveInEdges(
        const Node& source, const Node& destination, BoostGraph* modifierGraph);

    virtual void moveOutEdges(
        const Node& source, const Node& destination, BoostGraph* modifierGraph);
    void constructSubGraph(BoostGraph& subGraph, NodeSet& nodes);

    /**
     * This class is used in the pririty queue, to select which node to
     * start sink distance calculations */
    struct PathLengthHelper {
        inline PathLengthHelper(
            NodeDescriptor nd, int len, int sd, bool looping = false);
        NodeDescriptor nd_;
        int len_;
        int sd_;
        bool looping_;
        inline bool operator< (const PathLengthHelper& other) const;
    };

    // for subgraphs
    BoostGraph<GraphNode, GraphEdge>* parentGraph_;
    std::vector<BoostGraph<GraphNode,GraphEdge>*> childGraphs_;

    TCEString name_;
    int sgCounter_;

    std::set<Edge*> ownedEdges_;
    bool allowLoopEdges_;
    
    // cache to speed up hasPath(), call findAllPaths() to initialize
    typedef std::vector<std::vector<int> > PathCache;
    mutable PathCache* pathCache_;
};

#include "BoostGraph.icc"

#endif
