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
