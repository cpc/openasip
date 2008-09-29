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
 * @file DataGraph.hh
 *
 * Declaration of data dependence graph class
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_LOCAL_DATA_DEPENDENCE_GRAPH_HH
#define TTA_LOCAL_DATA_DEPENDENCE_GRAPH_HH

#include "BoostGraph.hh"
#include "DataDependenceEdge.hh"

#include <map>
#include <set>
#include <list>
#include <utility>
#include <vector>
#include <utility>

#include "MoveNode.hh"

typedef std::vector<class ProgramOperation*> POList;
typedef POList::iterator POLIter;

class BasicBlockNode;
class DataGraphBuilder;
class ControlFlowGraph;

namespace TTAMachine {
    class BaseRegisterFile;
    class Machine;
}

namespace TTAProgram {
    class CodeSnippet;
}

class DataDependenceGraph : 
    public BoostGraph<MoveNode,DataDependenceEdge> {
public:

    DataDependenceGraph(
        const std::string& name="", bool containsProcedure=false);
    virtual ~DataDependenceGraph();    

    /* Gets the BB in which this move belongs. This should be moved to
       a better place: DDG API should not deal with basic blocks. Correct
       place BasicBlockNode? */
    BasicBlockNode& getBasicBlockNode(MoveNode& mn);

    int programOperationCount();
    ProgramOperation& programOperation(int index);

    DataDependenceEdge* onlyRegisterEdgeIn(MoveNode& mn);
    DataDependenceEdge* onlyRegisterEdgeOut(MoveNode& mn);

    // should not be called by the user
    void addNode(MoveNode& moveNode) throw (ObjectAlreadyExists);
    void addNode(MoveNode& moveNode, MoveNode& relatedNode);
    void addNode(MoveNode& moveNode, BasicBlockNode& bblock);
    void addProgramOperation(ProgramOperation* po);

    using BoostGraph<MoveNode, DataDependenceEdge>::addNode;

    int earliestCycle(const MoveNode& moveNode) const;
    int latestCycle(const MoveNode& moveNode) const;
    
    int smallestCycle() const;
    int largestCycle() const;
    int scheduledNodeCount() const;

    NodeSet unscheduledMoves() const;
    NodeSet movesAtCycle(int cycle) const;

    MoveNode* guardDefMove(MoveNode& moveNode);

    MoveNode*
    lastScheduledRegisterRead(
        const TTAMachine::BaseRegisterFile& rf, int registerIndex) const;

    void sanityCheck() const
        throw (Exception);

    /// Dot printing related methods
    virtual std::string dotString() const;
    virtual void setCycleGrouping(bool flag);

    void mergeAndKeep(MoveNode& resultNode, MoveNode& userNode);
    void unMerge(MoveNode& resultNode, MoveNode& mergedNode);

    bool resultUsed(MoveNode& resultNode);
    
    void removeNode(MoveNode& node) throw (InstanceNotFound);
    void restoreNode(MoveNode& node);
    void deleteNode(MoveNode& node);

    int edgeWeight(DataDependenceEdge& e, const MoveNode& hNode) const;
    bool predecessorsReady(MoveNode& node) const;
    void setMachine(const TTAMachine::Machine& machine);

    bool connectOrDeleteEdge(        
        const MoveNode& tailNode, const MoveNode& headNode, 
        DataDependenceEdge* edge);

    DataDependenceGraph* createSubgraph(
        NodeSet& nodes, bool includeLoops = false)
        throw (InstanceNotFound);

    DataDependenceGraph* createSubgraph(
        TTAProgram::CodeSnippet& cs, bool includeLoops = false)
        throw (InstanceNotFound);

    DataDependenceGraph* createSubgraph(
        std::list<TTAProgram::CodeSnippet*>& codeSnippets,
        bool includeLoops)
        throw (InstanceNotFound);

    MoveNode& nodeOfMove(TTAProgram::Move& move) throw (InstanceNotFound);

    void dropBackEdges();

    void fixInterBBAntiEdges(BasicBlockNode& bbn1, BasicBlockNode& bbn2)
        throw (Exception);

    // Duplicates all in- and outgoing edges in dst to src
    void copyDependencies(MoveNode& src, MoveNode& dst) 
        throw (InstanceNotFound); 

    DataDependenceEdge* onlyIncomingGuard(const MoveNode& mn);
    
private:
    int rWarEdgesOut(MoveNode& mn);
    int rWawRawEdgesOut(MoveNode& mn);
    int rAntiEdgesIn(MoveNode& mn);

    void setNodeBB(
        MoveNode& mn, BasicBlockNode& bblock, DataDependenceGraph* updater);

    bool isRootGraphProcedureDDG();

    struct RemovedNodeData {
        std::vector<std::pair<DataDependenceEdge*,MoveNode*> > inEdges;
        std::vector<std::pair<DataDependenceEdge*,MoveNode*> > outEdges;
        ~RemovedNodeData();
    };
    std::map<MoveNode*,RemovedNodeData*> removedNodes_;

    // cache to make things faster
    // may not be ised with iterator.
    std::map<TTAProgram::Move*, MoveNode*> nodesOfMoves_;
    
    bool hasEqualEdge(
        const MoveNode& tailNode, const MoveNode& headNode, 
        const DataDependenceEdge& edge) const;
    
    int getOperationLatency(Operation& op) const;

    // own all the programoperations
    POList programOperations_;
    std::map<MoveNode*, BasicBlockNode*> moveNodeBlocks_;

    /// Dot printing related variables.
    /// Group the printed MoveNodes according to their cycles.
    bool cycleGrouping_;

    // Machine related variables. 
    const TTAMachine::Machine* machine_;
    int delaySlots_;
    std::map<std::string, int> operationLatencies_;

    bool procedureDDG_;
};

#endif
