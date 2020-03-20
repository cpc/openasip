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
 * @file DataDependenceGraph.hh
 *
 * Declaration of data dependence graph class
 *
 * @author Heikki Kultala 2006-2009 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_DATA_DEPENDENCE_GRAPH_HH
#define TTA_DATA_DEPENDENCE_GRAPH_HH

#include <map>
#include <set>
#include <list>
#include <utility>
#include <vector>
#include <utility>

#include "BoostGraph.hh"
#include "DataDependenceEdge.hh"
#include "MoveNode.hh"
#include "ProgramOperation.hh"

typedef std::vector<ProgramOperationPtr> POList;
typedef POList::iterator POLIter;

class BasicBlockNode;
class DataGraphBuilder;
class ControlFlowGraph;
class MoveNodeUse;
struct LiveRange;

namespace TTAMachine {
    class BaseRegisterFile;
    class Machine;
}

namespace TTAProgram {
    class CodeSnippet;
    class BasicBlock;
}

class DataDependenceGraph : 
    public BoostGraph<MoveNode, DataDependenceEdge> {
public:

    struct UndoData {
        EdgeSet newEdges;
        RemovedEdgeMap removedEdges;
        std::map<DataDependenceEdge*,
                 TCEString, DataDependenceEdge::Comparator> changedDataEdges;
    };

    enum AntidependenceLevel {
        NO_ANTIDEPS,
        INTRA_BB_ANTIDEPS,
        SINGLE_BB_LOOP_ANTIDEPS,
        ALL_ANTIDEPS
    };

    enum DumpFileFormat {
        DUMP_DOT,
        DUMP_XML
    };

    enum EdgeWeightHeuristics {        
        EWH_HEURISTIC,   ///< Weights memory dependencies more, etc.
        EWH_REAL,        ///< Height returns the minimum cycle count for the
                         ///< schedule given enough resources.
        EWH_DEFAULT = EWH_HEURISTIC
    };

    DataDependenceGraph(
        std::set<TCEString> allParamRegs,
        const TCEString& name="", 
        AntidependenceLevel antidependenceLevel = ALL_ANTIDEPS,
        BasicBlockNode* ownedBBN = NULL, 
        bool containsProcedure=false, bool noLoopEdges=false);

    virtual ~DataDependenceGraph();    

    /* Gets the BB in which this move belongs. This should be moved to
       a better place: DDG API should not deal with basic blocks. Correct
       place BasicBlockNode? 

       This information is needed for PDG construction, and putting it to
       BBN does not work. I know this is ugly, but currently these is no
       better place for this.
    */
    const BasicBlockNode& getBasicBlockNode(const MoveNode& mn) const;
    BasicBlockNode& getBasicBlockNode(MoveNode& mn);
    void setBasicBlockNode(const MoveNode& mn, BasicBlockNode& bbn);

    int programOperationCount() const;
    ProgramOperation& programOperation(int index);
    const ProgramOperation& programOperationConst(int index) const;

    DataDependenceEdge* onlyRegisterEdgeIn(MoveNode& mn) const;
    DataDependenceEdge* onlyRegisterEdgeOut(MoveNode& mn) const;

    // should not be called by the user, only by a DataDependenceGraphBuilder
    void addNode(MoveNode& moveNode);
    void addNode(MoveNode& moveNode, MoveNode& relatedNode);
    void addNode(MoveNode& moveNode, BasicBlockNode& bblock);
    void addProgramOperation(ProgramOperationPtr po);

    using BoostGraph<MoveNode, DataDependenceEdge>::addNode;

    int edgeLatency(const DataDependenceEdge& edge, int ii,
                    const MoveNode* tail, const MoveNode* head) const;

    int earliestCycle(
        const MoveNode& moveNode, unsigned int ii = UINT_MAX, 
        bool ignoreRegWaRs = false, 
        bool ignoreRegWaWs = false, bool ignoreGuards = false,
        bool ignoreFUDeps = false,
        bool ignoreSameOperationEdges = false) const;

    int latestCycle(
        const MoveNode& moveNode, unsigned int ii = UINT_MAX,
        bool ignoreRegAntideps = false, 
        bool ignoreUnscheduledSuccessors = true,
        bool ignoreGuards = false,
        bool ignoreFUDeps = false,
        bool ignoreSameOperationEdges = false) const;
    
    int smallestCycle() const;
    int largestCycle() const;
    int scheduledNodeCount() const;

    NodeSet unscheduledMoves() const;
    NodeSet scheduledMoves() const;
    NodeSet movesAtCycle(int cycle) const;

    MoveNode* onlyGuardDefOfMove(MoveNode& moveNode);
    MoveNode* lastGuardDefMove(MoveNode& moveNode);
    NodeSet guardDefMoves(const MoveNode& moveNode);

    MoveNode*
    lastScheduledRegisterRead(
        const TTAMachine::BaseRegisterFile& rf,
        int registerIndex,
        int lastCycleToTest = INT_MAX) const;

    NodeSet
    lastScheduledRegisterReads(
        const TTAMachine::BaseRegisterFile& rf, int registerIndex) const;

    NodeSet
    lastScheduledRegisterGuardReads(
        const TTAMachine::BaseRegisterFile& rf, int registerIndex) const;
    
    NodeSet
    lastScheduledRegisterWrites(
        const TTAMachine::BaseRegisterFile& rf, int registerIndex) const;

    MoveNode*
    firstScheduledRegisterRead(
        const TTAMachine::BaseRegisterFile& rf, 
        int registerIndex, 
        int firstCycleToTest = 0) const;

    MoveNode*
    firstScheduledRegisterWrite(
        const TTAMachine::BaseRegisterFile& rf, int registerIndex) const;

    NodeSet
    firstScheduledRegisterWrites(
        const TTAMachine::BaseRegisterFile& rf, int registerIndex) const;

    MoveNode*
    lastScheduledRegisterKill(
        const TTAMachine::BaseRegisterFile& rf, int registerIndex) const;

    MoveNode*
    firstScheduledRegisterKill(
        const TTAMachine::BaseRegisterFile& rf, int registerIndex) const;

    int lastRegisterCycle(
        const TTAMachine::BaseRegisterFile& rf, int registerIndex) const;

    int firstRegisterCycle(
        const TTAMachine::BaseRegisterFile& rf, int registerIndex) const;

    void sanityCheck() const;

    /// Dot printing related methods
    virtual TCEString dotString() const;
    virtual void setCycleGrouping(bool flag);
    virtual void setProgramOperationNodes(bool flag) {
        dotProgramOperationNodes_ = flag;
    }
    // XML dumping
    void writeToXMLFile(std::string fileName) const;

    bool mergeAndKeepAllowed(MoveNode& sourceNode, MoveNode& userNode);
    bool isLoopBypass(MoveNode& sourceNode, MoveNode& userNode);

    bool mergeAndKeep(MoveNode& resultNode, MoveNode& userNode);
    void unMerge(MoveNode& resultNode, MoveNode& mergedNode);

    bool resultUsed(MoveNode& resultNode);

    void removeNode(MoveNode& node);
    void deleteNode(MoveNode& node);

    void setEdgeWeightHeuristics(EdgeWeightHeuristics ewh) {
        if (edgeWeightHeuristics_ != ewh) {
            // force path recalculation
            height_ = -1; 
            sourceDistances_.clear();
            sinkDistances_.clear();
        }
        edgeWeightHeuristics_ = ewh;
    }

    int edgeWeight(DataDependenceEdge& e, const MoveNode& hNode) const;
    bool predecessorsReady(MoveNode& node) const;
    bool successorsReady(MoveNode& node) const;    
    void setMachine(const TTAMachine::Machine& machine);
    const TTAMachine::Machine& machine() const { return *machine_; } 

    bool connectOrDeleteEdge(        
        const MoveNode& tailNode, const MoveNode& headNode, 
        DataDependenceEdge* edge);

    DataDependenceGraph* createSubgraph(
        NodeSet& nodes, bool includeLoops = false);

    DataDependenceGraph* createSubgraph(
        TTAProgram::CodeSnippet& cs, bool includeLoops = false);

    DataDependenceGraph* createSubgraph(
        std::list<TTAProgram::CodeSnippet*>& codeSnippets,
        bool includeLoops = false);

    DataDependenceGraph* trueDependenceGraph(
        bool removeMemAntideps=true, bool ignoreMemDeps=false);
    DataDependenceGraph* criticalPathGraph();
    DataDependenceGraph* memoryDependenceGraph();

    MoveNode& nodeOfMove(const TTAProgram::Move& move);

    void dropBackEdges();

    void fixInterBBAntiEdges(
        BasicBlockNode& bbn1, BasicBlockNode& bbn2, bool loopEdges);

    // Duplicates all in- and outgoing edges in dst to src
    void copyDependencies(
        const MoveNode& src, MoveNode& dst, bool ignoreSameBBBackedges,
        bool moveOverLoopEdge = true);

    void copyIncomingGuardEdges(const MoveNode& src, MoveNode& dst);
    void copyOutgoingGuardWarEdges(const MoveNode& src, MoveNode& dst);
    
    NodeSet guardRawPredecessors(const MoveNode& node) const;

    DataDependenceEdge* onlyIncomingGuard(const MoveNode& mn) const;
    MoveNode* onlyRegisterRawSource(
        const MoveNode& mn, int allowGuardEdges = 2, int backEdges = 0) const;
    
    NodeSet onlyRegisterRawDestinations(
        const MoveNode& mn,
        bool allowGuardEdges = false,
        bool allowBackEdges = false) const;

    std::map<DataDependenceEdge*, MoveNode*, DataDependenceEdge::Comparator>
    onlyRegisterRawDestinationsWithEdges(
        const MoveNode& mn, bool allowBackEdges) const;

    NodeSet regWarSuccessors(const MoveNode& node) const;
    NodeSet regRawSuccessors(const MoveNode& node) const;
    NodeSet regWawSuccessors(const MoveNode& node) const;
    NodeSet regDepSiblings(const MoveNode& mn) const;

    void createRegisterAntiDependenciesBetweenNodes(NodeSet& nodes);

    bool exclusingGuards(const MoveNode& mn1, const MoveNode& mn2) const;
    bool sameGuards(const MoveNode& mn1, const MoveNode& mn2) const;

    int rWarEdgesOut(MoveNode& mn);
    int regRawSuccessorCount(const MoveNode& mn, bool onlyScheduled);

    bool guardsAllowBypass(const MoveNode& defNode,
                           const MoveNode& useNode,
                           bool loopBypass = false);

    const MoveNode* onlyRegisterRawAncestor(
        const MoveNode& node, const std::string& sp) const;

    EdgeSet copyDepsOver(MoveNode& node, bool anti, bool raw);

    void copyDepsOver(MoveNode& node1, MoveNode& node2, bool anti, bool raw);

    void combineNodes(
        MoveNode& node1, MoveNode& node2, MoveNode& destination);

    TTAProgram::Move* findLoopLimit(MoveNode& jumpMove);

    bool writesJumpGuard(const MoveNode& moveNode);

    void moveFUDependenciesToTrigger(MoveNode& trigger);

    LiveRange* findLiveRange(
        MoveNode& lrNode, bool writingNode, bool guardUseNode) const;

    MoveNode* findLimitingAntidependenceSource(MoveNode& mn);
    MoveNode* findLimitingAntidependenceDestination(MoveNode& mn);

    DataDependenceGraph::UndoData sourceRenamed(MoveNode& mn);
    DataDependenceGraph::UndoData guardRenamed(MoveNode& mn);
    DataDependenceGraph::UndoData destRenamed(MoveNode& mn);

    void renamedSimpleLiveRange(
        MoveNode& src, MoveNode& dest, MoveNode& antidepPoint,
        DataDependenceEdge& lrEdge,
        const TCEString& oldReg, const TCEString& newReg);

    void copyExternalInEdges(MoveNode& nodeCopy, const MoveNode& source);
    void copyExternalOutEdges(MoveNode& nodeCopy, const MoveNode& source);

    void updateRegWrite(
        const MoveNodeUse& mnd, const TCEString& reg, 
        TTAProgram::BasicBlock& bb);

    void updateRegUse(
        const MoveNodeUse& mnd, const TCEString& reg, 
        TTAProgram::BasicBlock& bb);

    void removeIncomingGuardEdges(MoveNode& node);
    void removeOutgoingGuardWarEdges(MoveNode& node);

    inline bool hasAllRegisterAntidependencies() {
        return registerAntidependenceLevel_ >= ALL_ANTIDEPS;
    }

    inline bool hasSingleBBLoopRegisterAntidependencies() {
        return registerAntidependenceLevel_ >= SINGLE_BB_LOOP_ANTIDEPS;
    }

    inline bool hasIntraBBRegisterAntidependencies() {
        return registerAntidependenceLevel_ >= INTRA_BB_ANTIDEPS;
    }

    bool isNotAvoidable(const DataDependenceEdge& edge) const;

    bool isLoopInvariant(const MoveNode& mn) const;

    bool hasOtherRegWawSources(const MoveNode& mn) const;

    void undo(UndoData& undoData);

    EdgeSet operationInEdges(const MoveNode& node) const;
private:

    bool queueRawPredecessors(
        NodeSet& queue, NodeSet& finalDest, NodeSet& predQueue,
        NodeSet& predFinalDest, bool guard) const;

    bool rWawRawEdgesOutUncond(MoveNode& mn);
    int rAntiEdgesIn(MoveNode& mn);

    void setNodeBB(
        MoveNode& mn, BasicBlockNode& bblock, DataDependenceGraph* updater);

    bool isRootGraphProcedureDDG();
    
    bool hasEqualEdge(
        const MoveNode& tailNode, const MoveNode& headNode, 
        const DataDependenceEdge& edge) const;
    
    int getOperationLatency(const TCEString& name) const;

    std::set<TCEString> allParamRegs_;

    // cache to make things faster
    // may not be used with iterator.
    std::map<const TTAProgram::Move*, MoveNode*> nodesOfMoves_;

    // own all the programoperations
    POList programOperations_;
    std::map<const MoveNode*, BasicBlockNode*> moveNodeBlocks_;

    /// Dot printing related variables.
    /// Group the printed MoveNodes according to their cycles.
    bool cycleGrouping_;
    /// The moves belonging to the same program operation
    /// are merged to a single node. Reduces the complexity of the graph.
    bool dotProgramOperationNodes_;

    // Machine related variables. 
    const TTAMachine::Machine* machine_;
    int delaySlots_;
    std::map<TCEString, int> operationLatencies_;

    BasicBlockNode* ownedBBN_;
    bool procedureDDG_;
    AntidependenceLevel registerAntidependenceLevel_;

    /// The heuristics to use to weight edges in the longest path computation.
    EdgeWeightHeuristics edgeWeightHeuristics_;
};

#endif
