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
 * @file ProgramDependenceGraph.hh
 *
 * Declaration of prototype of graph-based program representation:
 * declaration of the program dependence graph.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROGRAM_DEPENDENCE_GRAPH_HH
#define TTA_PROGRAM_DEPENDENCE_GRAPH_HH

#include <boost/graph/filtered_graph.hpp>

#include "BoostGraph.hh"
#include "ProgramDependenceEdge.hh"
#include "ProgramDependenceNode.hh"
#include "ControlDependenceGraph.hh"
#include "AssocTools.hh"
#include "MoveNode.hh"

namespace TTAProgram {
    class Terminal;
}
class DataDependenceGraph;

class ProgramDependenceGraph :
    public BoostGraph<ProgramDependenceNode, ProgramDependenceEdge> {

public:
    enum CompareResult {
        A_BEFORE_B, // Subgraph A must be scheduled before subgraph B
        B_BEFORE_A, // vice verse
        ANY_ORDER,  // Any order is acceptable
        UNORDERABLE, // Can not be ordered, needs additional predicate
        ERROR      // Relation can not be determined! This is error
    };

    ProgramDependenceGraph(
        ControlDependenceGraph& cdg,
        DataDependenceGraph& ddg);
    virtual ~ProgramDependenceGraph();
    ProgramDependenceNode& entryNode() const;
    bool serializePDG();
    void disassemble() const;

private:
    typedef std::map<const ControlDependenceNode*, ProgramDependenceNode*,
        ControlDependenceNode::Comparator>
        ControlToProgram;
    typedef std::map<const BasicBlockNode*, ControlDependenceNode*,
        BasicBlockNode::Comparator> BBToCD;
    typedef std::map<const MoveNode*, ProgramDependenceNode*,
         MoveNode::Comparator> MoveNodeToPDGNode;
    typedef std::map<const ControlDependenceNode*, std::vector<Node*> >
        MovesInCD;

    /// Filter control dependence edges only
    template <typename GraphType>
    struct CDGFilter {
        CDGFilter() { }
        CDGFilter(GraphType graph) : graph_(graph) { }
        template <typename Edge>
        bool operator()(const Edge& e) const {
            return graph_[e]->isControlDependence()
                || graph_[e]->isArtificialControlDependence();
        }
        GraphType graph_;
    };

    /// Type of filtered graph with CD edges only
    typedef boost::filtered_graph<Graph, CDGFilter<Graph> > FilteredCDG;
    /// Few types to work with filtered graph
    typedef boost::graph_traits<FilteredCDG>::in_edge_iterator
        FilteredInEdgeIter;
    typedef boost::graph_traits<FilteredCDG>::out_edge_iterator
        FilteredOutEdgeIter;
    typedef boost::graph_traits<FilteredCDG>::vertex_descriptor
        FilteredVertexDescriptor;
    typedef std::pair<FilteredInEdgeIter, FilteredInEdgeIter>
        FilteredInEdgePair;
    typedef std::pair<FilteredOutEdgeIter, FilteredOutEdgeIter>
        FilteredOutEdgePair;
    /// Stores data to compute post order relation on CDG and strong components
    typedef std::map<NodeDescriptor, int> PDGOrderMap;
    typedef boost::associative_property_map<PDGOrderMap> PDGOrder;
    /// Storage for relations between nodes
    typedef std::map<NodeDescriptor, NodeDescriptor> DescriptorMap;
    typedef boost::associative_property_map<DescriptorMap> Descriptors;
    /// Storage for color property used by dfs
    typedef std::map <NodeDescriptor, boost::default_color_type > ColorMap;
    typedef boost::associative_property_map<ColorMap> Color;

    /// Filter nodes of subgraph only
    template <typename NodeSetType, typename GraphType>
    struct SubgraphTypeTest {
        SubgraphTypeTest() { }
        SubgraphTypeTest(Node* root, NodeSetType nodes, GraphType graph) :
            root_(root), subgraphNodes_(nodes), graph_(graph){ }
        template <typename Node>
            bool operator()(const Node& n) const {
                return (graph_[n] != root_) &&
                    AssocTools::containsKey(subgraphNodes_, graph_[n]);
        }
        Node* root_;
        NodeSetType subgraphNodes_;
        GraphType graph_;
    };

    /// Filter away back edges
    template <typename GraphType>
    struct BackFilter {
        BackFilter() { }
        BackFilter(GraphType graph) : graph_(graph) { }
        template <typename Edge>
        bool operator()(const Edge& e) const {
            return !(graph_[e]->isLoopCloseEdge() ||
                (graph_[e]->isDataDependence() &&
                graph_[e]->dataDependenceEdge().isBackEdge()));
        }
        GraphType graph_;
    };
    typedef boost::filtered_graph<
        Graph, BackFilter<Graph>, SubgraphTypeTest<NodeSet, Graph> >
        Subgraph;

    template <typename GraphType>
    struct BackCFGFilter {
        BackCFGFilter() { }
        BackCFGFilter(GraphType graph) : graph_(graph) { }
        template <typename Edge>
        bool operator()(const Edge& e) const {
            return !(graph_[e]->isBackEdge());
            }
            GraphType graph_;
    };
    typedef boost::filtered_graph<Graph,BackCFGFilter<Graph> >
        CFGSubgraph;


    void removeGuardedJump(
        ControlToProgram&,
        ProgramDependenceNode&,
        ControlDependenceNode&);

    void copyRegionEECComponent(
        ControlToProgram&,
        BBToCD&,
        MoveNodeToPDGNode&,
        MovesInCD&);

    int detectStrongComponents(
        PDGOrderMap& components,
        DescriptorMap& roots,
        FilteredCDG& filteredCDG);
    void computeRegionInfo(
        const PDGOrderMap& orderMap,
        FilteredCDG& filteredCDG);
    void computeEECInfo(
        const PDGOrderMap& orderMap,
        FilteredCDG& filteredCDG);

    void computeRelations(
        const PDGOrderMap& orderMap,
        FilteredCDG& filteredCDG);
    CompareResult compareSiblings(Node* a, Node* b);

    void regionHelper(
        Node* node,
        FilteredCDG& filteredCDG,
        Node::NodesInfo& finalNodesInfo);
    void processRegion(
        Node* region,
        FilteredCDG& filteredCDG);
    void processPredicate(
        Node* predicate,
        FilteredCDG& filteredCDG);
    void processEntry(BasicBlockNode* firstBB);
    void processLoopClose(Node* node);
    void processLoopEntry(Node* node, BasicBlockNode* bb);

    void moveDDGedges(
        Node& root,
        NodeSet& subgraphNodes,
        FilteredCDG& filteredCDG);

    void createJump(
        BasicBlockNode* from,
        BasicBlockNode* to,
        TTAProgram::Terminal* guardReg = NULL,
        ControlFlowEdge::CFGEdgePredicate predicate =
            ControlFlowEdge::CFLOW_EDGE_NORMAL);
    void addLeafEdges(std::vector<BasicBlockNode*> leafs, BasicBlockNode* bb);

    /// Stores original control dependence graph
    const ControlDependenceGraph* cdg_;
    /// Stores original data dependence graph
    const DataDependenceGraph* ddg_;
    /// Stores pointer to entry node of the PDG graph
    Node* entryNode_;
    /// Stored reference to PDG equivalent of DDG ENTRYNODE
    Node* ddgEntryNode_;
    /// stores nodes present in each of the found components
    std::vector<std::set<Node*> > strongComponents_;
    /// Newly created control flow graph
    ControlFlowGraph* newCFG_;
    /// Counts new instructions when creating basic blocks
    int insCount_;
    /// Original Program object, to get instruction reference manager
    TTAProgram::Program* program_;


    template <class Name>
    class label_writer {
        public:
        label_writer(Name _name) : name(_name) {}
        template <class VertexOrEdge>
            void operator()(std::ostream& out, const
            VertexOrEdge& v) const {
            out << "[" << name[v]->dotString() << "]";
            //"[label=\"" << name[v]->toString() << "\"]";
        }
        private:
        Name name;
    };
    int wrongCounter_;
};

#endif
