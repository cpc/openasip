/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
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
