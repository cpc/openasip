/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file ControlFlowGraph.hh
 *
 * Declaration of prototype control flow graph of TTA program
 * representation.
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONTROL_FLOW_GRAPH_HH
#define TTA_CONTROL_FLOW_GRAPH_HH


#include <map>
#include <vector>

#include "CompilerWarnings.hh"
IGNORE_CLANG_WARNING("-Wunused-local-typedef")
IGNORE_COMPILER_WARNING("-Wunused-parameter")
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/depth_first_search.hpp>
POP_COMPILER_DIAGS
POP_CLANG_DIAGS

namespace llvm {
    class MCSymbol;
    class MachineInstr;
}

#include "Exception.hh"
#include "BoostGraph.hh"
#include "BasicBlockNode.hh"
#include "ControlFlowEdge.hh"
#include "Address.hh"
#include "NullAddress.hh"
#include "hash_map.hh"
#include "ProgramOperation.hh"

namespace TTAProgram {
    class Program;
    class Procedure;
    class Instruction;
    class Move;
    class InstructionReferenceManager;
    class POMRelocBookkeeper;
    class Address;
    class NullAddress;
}

namespace llvm {
    class MachineFunction;
    class MachineBasicBlock;
    class MCInstrDesc;
    class MCInstrInfo;
}

using boost::reverse_graph;

class InterPassData;
class DataDependenceGraph;
class CFGStatistics;

/**
 * Control Flow Graph.
 *
 * The basic blocks are initially in the original program order when traversed
 * with nodeCount()/node().
 */
class ControlFlowGraph : public BoostGraph<BasicBlockNode, ControlFlowEdge> {
public:
    ControlFlowGraph(
        const TCEString name,
        TTAProgram::Program* program = NULL);
    ControlFlowGraph(
        const TTAProgram::Procedure& procedure,
        InterPassData& passData);
    ControlFlowGraph(const TTAProgram::Procedure& procedure);
    virtual ~ControlFlowGraph();

    TCEString procedureName() const;
    int alignment() const;
    TTAProgram::Program* program() const;

    BasicBlockNode& entryNode() const;
    BasicBlockNode& exitNode() const;
    BasicBlockNode& firstNormalNode() const;

    TCEString printStatistics();
    const CFGStatistics& statistics();
    friend class ControlDependenceGraph;
    friend class ProgramDependenceGraph;
    
    void copyToProcedure(
        TTAProgram::Procedure& proc, 
        TTAProgram::InstructionReferenceManager* irm = NULL);

    void copyToLLVMMachineFunction(
        llvm::MachineFunction& mf,
        TTAProgram::InstructionReferenceManager* irm = NULL);

    void updateReferencesFromProcToCfg();
    void convertBBRefsToInstRefs();

    ControlFlowEdge* incomingFTEdge(const BasicBlockNode& bbn) const;
    EdgeSet incomingJumpEdges(const BasicBlockNode& bbn) const;
    bool hasIncomingExternalJumps(const BasicBlockNode& bbn) const;

    void deleteNodeAndRefs(BasicBlockNode& node);

    TTAProgram::InstructionReferenceManager& instructionReferenceManager();
    void setInstructionReferenceManager(
        TTAProgram::InstructionReferenceManager& irm) {
        irm_ = &irm;
    }

    BasicBlockNode* jumpSuccessor(BasicBlockNode& bbn);
    BasicBlockNode* fallThruSuccessor(const BasicBlockNode& bbn);

    void addExitFromSinkNodes(BasicBlockNode* exitNode);
    void detectBackEdges();

    void reverseGuardOnOutEdges(const BasicBlockNode& bbn);
    void optimizeBBOrdering(
        bool removeDeadCode, 
        TTAProgram::InstructionReferenceManager& irm,
        DataDependenceGraph* ddg);
    llvm::MachineBasicBlock& getMBB(
        llvm::MachineFunction& mf,
        const TTAProgram::BasicBlock& bb) const;

    void splitBasicBlocksWithCallsAndRefs();
    void splitBasicBlocksWithCalls();

    bool isSingleBBLoop(const BasicBlockNode& node) const;
private:
    // For temporary storage
    typedef hash_map<InstructionAddress, const TTAProgram::Instruction*>
        InstructionAddressMap;
    typedef std::vector<InstructionAddress> InstructionAddressVector;
    // Type of reversed underlying graph, needed for control dependence
    // analysis
    typedef reverse_graph<ControlFlowGraph::Graph> ReversedGraph;
    typedef BoostGraph<BasicBlockNode, ControlFlowEdge>::NodeDescriptor
        NodeDescriptor;
    typedef std::pair<InstructionAddress, ControlFlowEdge::CFGEdgePredicate>
    ReturnSource;
    typedef std::set<ReturnSource> ReturnSourceSet;
    /// DFS visitor which when finding back edge marks such edge as
    /// back edge
    class DFSBackEdgeVisitor : public boost::default_dfs_visitor    {
    public:
        DFSBackEdgeVisitor(){}
        template < typename EdgeDescriptor, typename Graph >
        void back_edge(EdgeDescriptor e, const Graph & g)const {
            g[e]->setBackEdge();
        }
    };

    void buildFrom(const TTAProgram::Procedure& procedure);
    void createBBEdges(
        const TTAProgram::Procedure& procedure,
        InstructionAddressMap& leaders,
        InstructionAddressMap& dataCodeRellocations);

    ReversedGraph& reversedGraph() const;

    bool hasInstructionAnotherJump(
        const TTAProgram::Instruction& ins, 
        int moveIndex);
    void computeLeadersFromRefManager(
        InstructionAddressMap& leaders,
        const TTAProgram::Procedure& procedure);
    bool computeLeadersFromJumpSuccessors(
        InstructionAddressMap& leaders,
        const TTAProgram::Procedure& procedure);
    void computeLeadersFromRelocations(
        InstructionAddressMap& leaderSet,
        InstructionAddressMap& dataCodeRellocations,
        const TTAProgram::Procedure& procedure);

    void createAllBlocks(
        InstructionAddressMap& leaders,
        const TTAProgram::Procedure& procedure);
    BasicBlockNode& createBlock(
        TTAProgram::Instruction& leader,
        const TTAProgram::Instruction& endBlock);
    ControlFlowEdge& createControlFlowEdge(
        const TTAProgram::Instruction& iTail,
        const TTAProgram::Instruction& iHead,
        ControlFlowEdge::CFGEdgePredicate edgePredicate =
            ControlFlowEdge::CFLOW_EDGE_NORMAL,
        ControlFlowEdge::CFGEdgeType edgeType =
            ControlFlowEdge::CFLOW_EDGE_JUMP);

    void directJump(
        InstructionAddressMap& leaders,
        const InstructionAddress& leaderAddr,
        int insIndex, 
        int moveIndex,
        const TTAProgram::Instruction& instructionTarget,
        const TTAProgram::Procedure& procedure);
    void indirectJump(
        InstructionAddressMap& leaders,
        const InstructionAddress& leaderAddr,
        InstructionAddressMap& dataCodeRellocations,
        int insIndex, 
        int moveIndex,
        const TTAProgram::Procedure& procedure);
    void createJumps(
        InstructionAddressMap& leaders,
        const InstructionAddress& leaderAddr,
        InstructionAddressMap& dataCodeRellocations,
        const TTAProgram::Procedure& procedure,
        int insIndex,
        int moveIndex);
    unsigned int findNextIndex(
        const TTAProgram::Procedure& proc, 
        int jumpInsIndex, int jumpMoveIndex);

    void addExit();
    void addEntryExitEdge();
    void removeEntryExitEdge();

    bool hasFallThruPredecessor(const BasicBlockNode& bbn);

    NodeSet findReachableNodes();
    NodeSet findUnreachableNodes(const NodeSet& reachableNodes);

    BasicBlockNode* splitBasicBlockAtIndex(BasicBlockNode& bbn, int index);

    void removeUnreachableNodes(
        const NodeSet& unreachableNodes, DataDependenceGraph* ddg);
    void mergeNodes(
        BasicBlockNode& node1,
        BasicBlockNode& node2,
        DataDependenceGraph* ddg);
        
    enum RemovedJumpData {
        JUMP_NOT_REMOVED = 0, /// nothing removed
        JUMP_REMOVED, /// jump removed, other things remain in BB
        LAST_ELEMENT_REMOVED /// last jump removed, no immeds in BB.
    };

    RemovedJumpData removeJumpToTarget(
        TTAProgram::CodeSnippet& cs,
        const TTAProgram::Instruction& target,
        int idx,
        DataDependenceGraph* ddg = NULL);

    const llvm::MCInstrDesc& 
    findLLVMTargetInstrDesc(TCEString name, const llvm::MCInstrInfo& tii) 
        const;

    void buildMBBFromBB(
        llvm::MachineBasicBlock& mbb,
        const TTAProgram::BasicBlock& bb) const;

    // Data saved from original procedure object
    TCEString procedureName_;
    TTAProgram::Program* program_;
    const TTAProgram::Procedure* procedure_;
    TTAProgram::Address startAddress_;
    TTAProgram::Address endAddress_;
    int alignment_;

    // Collection of all basic blocks of the control flow graph indexed by
    // the address of their start instruction (leader).
    hash_map<InstructionAddress, BasicBlockNode*> blocks_;

    // Mapping between original instructions and those in the cfg.
    typedef hash_map<TTAProgram::Instruction*,TTAProgram::Instruction*>
        InsMap;

    InsMap originalToCfgInstructions_;
    InsMap cfgToOriginalInstructions_;

    // all basic blocks which contain a return instruction.
    ReturnSourceSet returnSources_;

    // Optional interpass data to aid in the construction of the CFG.
    InterPassData* passData_;

    // IRM needs to be set explicitly if CFG is built and used without a
    // Program object.
    TTAProgram::InstructionReferenceManager* irm_;
    
    // Maps BasicBlockNode onto it's MachineBasicBlock equivalent
    mutable std::map<const TTAProgram::BasicBlock*, llvm::MachineBasicBlock*> 
    bbMap_;

    /// For LLVM conversion: the dummy label instructions for SPU should be
    /// created for with the given MCSymbol as argument after building.
    mutable std::set<std::pair<ProgramOperationPtr, llvm::MCSymbol*> > tpos_;
    /// For LLVM conversion: mapping of created MachineInstructions to TCE 
    /// ProgramOperations.
    mutable std::map<ProgramOperation*, llvm::MachineInstr*> 
    programOperationToMIMap_;
};
#endif
