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

    std::string name() const;

    /* Gets the BB in which this move belongs. This should be moved to
       a better place: DDG API should not deal with basic blocks. Correct
       place BasicBlockNode? */
    BasicBlockNode& getBasicBlockNode(MoveNode& mn);

    int programOperationCount();
    ProgramOperation& programOperation(int index);

    DataDependenceEdge* onlyRegisterEdgeIn(MoveNode& mn);
    DataDependenceEdge* onlyRegisterEdgeOut(MoveNode& mn);

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

    int edgeWeight(DataDependenceEdge& e, MoveNode& hNode) const;
    bool predecessorsReady(MoveNode& node) const;
    void setMachine(const TTAMachine::Machine& machine);

    void connectOrDeleteEdge(        
        MoveNode& tailNode, MoveNode& headNode, 
        DataDependenceEdge* edge);

    DataDependenceGraph* createSubgraph(
        NodeSet& nodes, bool includeLoops = false)
        throw (InstanceNotFound);

    DataDependenceGraph* createSubgraph(
        TTAProgram::CodeSnippet& cs, bool includeLoops = false)
        throw (InstanceNotFound);

    DataDependenceGraph* createSubgraph(
        std::set<TTAProgram::CodeSnippet*> codeSnippets,
        bool includeLoops)
        throw (InstanceNotFound);

    MoveNode& nodeOfMove(TTAProgram::Move& move) throw (InstanceNotFound);

    void dropBackEdges();

    void fixInterBBAntiEdges(BasicBlockNode& bbn1, BasicBlockNode& bbn2)
        throw (Exception);

    // Duplicates all in- and outgoing edges in dst to src
    void copyDependencies(MoveNode& src, MoveNode& dst) 
        throw (InstanceNotFound); 

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
    
    bool hasEqualEdge(
        MoveNode& tailNode, MoveNode& headNode, 
        const DataDependenceEdge& edge) const;
    
    int getOperationLatency(Operation& op) const;

    std::string name_;

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
