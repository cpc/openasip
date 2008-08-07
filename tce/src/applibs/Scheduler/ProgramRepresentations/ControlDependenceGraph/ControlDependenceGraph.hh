/**
 * @file ControlDependenceGraph.hh
 *
 * Declaration of prototype control dependence graph of TTA program
 * representation.
 *
 * @author Andrea Cilio 2005 (cilio@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */


#ifndef TTA_CONTROL_DEPENDENCE_GRAPH_HH
#define TTA_CONTROL_DEPENDENCE_GRAPH_HH


#include <map>
#include <boost/graph/reverse_graph.hpp>

#include "BaseType.hh"
#include "Exception.hh"
#include "NullAddress.hh"
#include "BoostGraph.hh"
#include "ControlFlowGraph.hh"
#include "ControlDependenceEdge.hh"
#include "ControlDependenceNode.hh"

/**
 * Graph-based program representation.
 */
class ControlDependenceGraph : public
    BoostGraph<ControlDependenceNode, ControlDependenceEdge> {

public:
    ControlDependenceGraph(const ControlFlowGraph& cGraph);
    virtual ~ControlDependenceGraph();

    int alignment() const;
    TTAProgram::Program* program() const;
    ControlDependenceNode& entryNode();

private:
    typedef std::map<ControlFlowGraph::NodeDescriptor, int> PostOrderMap;
    typedef boost::associative_property_map<PostOrderMap> PostOrder;
    typedef std::pair<
        ControlDependenceNode*, ControlDependenceEdge::CDGEdgeType> 
        SourceType;
    typedef std::vector<SourceType> DependentOn;
    typedef std::vector<BasicBlockNode*> BlockVector;
    typedef std::map<ControlDependenceNode*, DependentOn*,
                     ControlDependenceNode::Comparator> DependenceMap;


    void computeDependence();
    void createPostDominanceTree(
        BlockVector& nodes,
        PostOrder& postOrder);
    void detectControlDependencies(
        BlockVector& nodes,
        std::vector<ControlDependenceNode*>& cdNodes,
        PostOrder& postOrder,
        DependenceMap& dependencies);
    void eliminateMultipleOutputs();
    bool findSubset(DependentOn*, DependentOn*, ControlDependenceNode*);
    int nearestCommonDom(std::vector<int>& iDom, int node1, int node2) const;

    ControlDependenceEdge& createControlDependenceEdge(
        ControlDependenceNode& bTail,
        ControlDependenceNode& bHead,
        ControlDependenceEdge::CDGEdgeType edgeValue =
            ControlDependenceEdge::CDEP_EDGE_NORMAL);

    // Data saved from original procedure object
    TTAProgram::Program* program_;
    TTAProgram::Address startAddress_;
    int alignment_;

    ControlFlowGraph* cGraph_;
    std::vector<int> iDomTree;
};

#endif
