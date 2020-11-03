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
 * @file ControlDependenceGraph.hh
 *
 * Declaration of prototype control dependence graph of TTA program
 * representation.
 *
 * Known limitations are:
 * - can not create CDG for procedure with indirect jump - multiple out edges
 *   to basic blocks in CFG without edge predicates
 * - can not create CDG for procedure with unreachable basic block - crt0 with
 *   missing fall through edge after calling __exit
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */


#ifndef TTA_CONTROL_DEPENDENCE_GRAPH_HH
#define TTA_CONTROL_DEPENDENCE_GRAPH_HH


#include <map>

#include "CompilerWarnings.hh"
IGNORE_CLANG_WARNING("-Wunused-local-typedef")
#include <boost/graph/reverse_graph.hpp>
POP_CLANG_DIAGS

#include "BaseType.hh"
#include "Exception.hh"
#include "NullAddress.hh"
#include "BoostGraph.hh"
#include "ControlFlowGraph.hh"
#include "ControlDependenceEdge.hh"
#include "ControlDependenceNode.hh"

#include "hash_set.hh"
/**
 * Graph-based program representation.
 */
class ControlDependenceGraph : public
    BoostGraph<ControlDependenceNode, ControlDependenceEdge> {

public:
    enum CompareResult {
        A_BEFORE_B, // Subgraph A must be scheduled before subgraph B
        B_BEFORE_A, // vice versa
        ANY_ORDER,  // Any order is acceptable
        UNORDERABLE, // Can not be ordered, needs additional predicate
        ERROR      // Try again with reordered nodes TODO: remove this!
    };

    ControlDependenceGraph(const ControlFlowGraph& cGraph);
    virtual ~ControlDependenceGraph();

    int alignment() const;
    TTAProgram::Program* program() const;
    ControlDependenceNode& entryNode();
    void analyzeCDG();
    bool analyzed() const { return analyzed_; }
    int componentCount() const { return strongComponents_.size(); }
private:
    /// Stores data to compute post order relation on CFG
    typedef std::map<ControlFlowGraph::NodeDescriptor, int> PostOrderMap;
    typedef boost::associative_property_map<PostOrderMap> PostOrder;
    /// Stores data to compute post order relation on CDG and strong components
    typedef std::map<NodeDescriptor, int> CDGOrderMap;
    typedef boost::associative_property_map<CDGOrderMap> CDGOrder;
    /// Storage for relations between nodes
    typedef std::map<NodeDescriptor, NodeDescriptor> DescriptorMap;
    typedef boost::associative_property_map<DescriptorMap> Descriptors;
    /// Storage for color property used by dfs
    typedef std::map <NodeDescriptor, boost::default_color_type > ColorMap;
    typedef boost::associative_property_map<ColorMap> Color;

    typedef std::pair<Node*, Edge::CDGEdgeType> SourceType;
    typedef std::vector<SourceType> DependentOn;
    typedef std::vector<BasicBlockNode*> BlockVector;

    typedef std::map<Node*, DependentOn*, Node::Comparator> DependenceMap;

    void computeDependence();
    void createPostDominanceTree(
        BlockVector& nodes,
        PostOrder& postOrder);
    void detectControlDependencies(
        BlockVector& nodes,
        std::vector<Node*>& cdNodes,
        PostOrder& postOrder,
        DependenceMap& dependencies);
    void eliminateMultipleOutputs();
    bool findSubset(DependentOn*, DependentOn*, Node*);
    int nearestCommonDom(std::vector<int>& iDom, int node1, int node2) const;

    int detectStrongComponents(
        CDGOrderMap& components,
        DescriptorMap& roots);
    void computeRegionInfo(const CDGOrderMap& orderMap);
    void computeEECInfo(const CDGOrderMap& orderMap);
    CompareResult compareSiblings(Node* a, Node* b) const;
    void regionHelper(Node*, Node::NodesInfo&);
    void computeRelations(const CDGOrderMap& orderMap);
    void processRegion(Node* region);
    ControlDependenceEdge& createControlDependenceEdge(
        Node& bTail,
        Node& bHead,
        Edge::CDGEdgeType edgeValue = Edge::CDEP_EDGE_NORMAL);

    // Data saved from original procedure object
    TTAProgram::Program* program_;
    TTAProgram::Address startAddress_;
    int alignment_;

    ControlFlowGraph* cGraph_;
    std::vector<int> iDomTree_;
    std::vector<std::set<Node*> > strongComponents_;
    /// Indicates that CDG already has data required for serialization
    bool analyzed_;
    /// Stores reference to entryNode of the graph
    Node* entryNode_;
    bool componentsDetected_;
};

#endif
