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
 * @file DataDependenceGraph.hh
 *
 * Declaration of data dependence graph class
 *
 * @author Heikki Kultala 2006 (heikki.kultala-no.spam-tut.fi)
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

    int earliestCycle(
        const MoveNode& moveNode, 
        bool ignoreUnscheduledPredecessors=false) const;

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
    
    void moveFUDependenciesToTrigger(MoveNode& trigger);

private:
    bool rWarEdgesOutUncond(MoveNode& mn);
    bool rWawRawEdgesOutUncond(MoveNode& mn);
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
