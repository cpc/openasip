/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file DataDependenceGraphBuilder.hh
 *
 * Declaration of data dependence graph builder class
 *
 * @author Heikki Kultala 2006-2009 (heikki.kultala-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011
 * @note rating: red
 */

#ifndef TTA_DDG_BUILDER_HH
#define TTA_DDG_BUILDER_HH

#include "DataDependenceGraph.hh"
#include "ControlFlowGraph.hh"
#include "MemoryAliasAnalyzer.hh"
#include "TCEString.hh"
#include "MoveNodeUse.hh"
#include "LiveRangeData.hh"

namespace llvm {
    class AliasAnalysis;
}

namespace TTAProgram {
    class Program;
    class TerminalRegister;
    class CodeSnippet;
    class Instruction;
    class BasicBlock;
}

namespace TTAMachine {
    class Machine;
}

class UniversalMachine;
class InterPassData;


/**
 * DataDependenceGraphBuilder class is responsible for building data 
 * dependence graphs.
 */
class DataDependenceGraphBuilder {
public:
    DataDependenceGraphBuilder();
    DataDependenceGraphBuilder(InterPassData& ipd);
    
    virtual ~DataDependenceGraphBuilder();
   
    void addAliasAnalyzer(MemoryAliasAnalyzer* analyzer);
    
    virtual DataDependenceGraph* build(
        ControlFlowGraph& cGraph, 
        DataDependenceGraph::AntidependenceLevel antidependenceLevel,
        const UniversalMachine* um = NULL,
        bool createMemAndFUDeps = true, 
        bool createDeathInformation = true,
        llvm::AliasAnalysis* AA = NULL);
    virtual DataDependenceGraph* build(
        TTAProgram::BasicBlock& bb,
        DataDependenceGraph::AntidependenceLevel antidependenceLevel,
        const TCEString& ddgname = "small bb",
        const UniversalMachine* um = NULL, 
        bool createMemAndFUDeps = true,
        llvm::AliasAnalysis* AA = NULL);

protected:

    std::set<TCEString> allParamRegs_;

    typedef ControlFlowGraph::NodeSet BasicBlockNodeSet;
    typedef DataDependenceGraph::NodeSet MNodeSet;

    typedef LiveRangeData::MoveNodeUseSet MoveNodeUseSet;
    typedef LiveRangeData::MoveNodeUseMapSet MoveNodeUseMapSet;
    typedef LiveRangeData::MoveNodeUseMap MoveNodeUseMap;
    typedef LiveRangeData::MoveNodeUseSetPair MoveNodeUseSetPair;
    typedef LiveRangeData::MoveNodeUsePair MoveNodeUsePair;

    typedef std::map<int, TCEString> SpecialRegisters;
    typedef std::vector<MemoryAliasAnalyzer*> AliasAnalyzerVector;

    enum BBState {
        BB_UNREACHED = 0, /// Basic block we have not yet encountered.
        BB_QUEUED, /// BB which is queued to be processed
        BB_READY, ///BB which is already processed and is not queued.
        BB_STATES};
    
    enum ConstructionPhase {
        REGISTERS_AND_PROGRAM_OPERATIONS = 0,
        MEMORY_AND_SIDE_EFFECTS};
    /**
     * This class stores all the basic-block related information needed by
     * the data dependency graph creator.
     */
    struct BBData {

        BBData(BasicBlockNode& bb);
        virtual ~BBData();
        /// ProgramOperations lacking operands
        ProgramOperationPtr destPending_;
        /// ProgramOperations lacking result read
        ProgramOperationPtr readPending_;
	int poReadsHandled_;
        /// State of the BB.
        BBState state_;
        /// Whether the BB has been constructed or not.
        bool constructed_;
        BasicBlockNode* bblock_;
    };

    typedef std::map <BasicBlockNode*, BBData*> BBDataMap;
    typedef std::list<BBData*> BBDataList;

    void updatePreceedingRegistersUsedAfter(
        BBData& bbd, 
        bool firstTime);

    bool updateRegistersUsedInOrAfter(BBData& bbd);

    bool updateRegistersAliveAfter(BBData& bbd);

    bool updateMemAndFuAliveAfter(BBData& bbd);
    void createMemAndFUstateDeps();
    void createRegisterDeps();
    void initializeBBStates();
    BasicBlockNode* queueFirstBB();
    
    void clearUnneededBookkeeping();
    void clearUnneededBookkeeping(
        TTAProgram::BasicBlock& bb,
        bool aliveInformationNeeded = true);
    
    void iterateBBs(
        ConstructionPhase phase);
    void searchRegisterDeaths();
    void iterateRegisterDeaths();
    
    void setSucceedingPredeps(
        BBData& bbd, 
        bool queueAll, 
        ConstructionPhase phase);
    bool appendUseMapSets(
        const MoveNodeUseMapSet& srcMap, 
        MoveNodeUseMapSet& dstMap,
        bool addLoopProperty);
    void setSucceedingPredepsForBB(
        TTAProgram::BasicBlock& processedBB, 
        BasicBlockNode& successor, 
        bool queueAll,
        bool loop, 
        ConstructionPhase phase);
    void updateBB(
        BBData& bbd, 
        ConstructionPhase phase);
    void constructIndividualBB(ConstructionPhase phase);
    
    void constructIndividualBB(
        BBData& bbd, 
        ConstructionPhase);    
    void constructBB(BasicBlockNodeSet& inputBlocks);
    
    void createOperationEdges(ProgramOperationPtr po);
    void processGuard(MoveNode& moveNode);
    void processSource(MoveNode& moveNode);
    void processResultRead(MoveNode& moveNode);
    void processCall(MoveNode& mn);
    void processReturn(MoveNode& moveNode);
    void processEntryNode(MoveNode& mn);
    void processDestination(
        class MoveNode& moveNode, 
        ConstructionPhase phase);
    void processRegUse(
        MoveNodeUse mn, 
        const TCEString& reg);

    void updateMemUse(
        MoveNodeUse mnd, 
        const TCEString& category);

    void processRegWrite(
        MoveNodeUse mn, 
        const TCEString& reg);    
    void updateMemWrite(
        MoveNodeUse mnd, 
        const TCEString& category);

    void processTriggerPO(
        class MoveNode& moveNode, 
        Operation &dop)
        throw (IllegalProgram);
    void processTriggerRegistersAndOperations(
        MoveNode& moveNode, 
        Operation &dop);
    void processTriggerMemoryAndFUStates(
        MoveNode& moveNode, 
        Operation &dop);
    void createTriggerDependencies(
        class MoveNode& moveNode,
        class Operation& dop);

    void createSideEffectEdges(    
        MoveNodeUseSet& prevMoves, 
        const MoveNode& mn, 
        Operation& dop);

    void processMemWrite(MoveNodeUse mnd);
    void processMemUse(MoveNodeUse mnd);

    void processOperand(
        class MoveNode& moveNode,
        Operation &dop);

    MemoryAliasAnalyzer::AliasingResult analyzeMemoryAlias(
        const ProgramOperation& pop1, 
        const ProgramOperation& pop2);

    bool isAddressTraceable(const ProgramOperation& pop);
    TCEString memoryCategory(const MoveNodeUse& mnd);

    bool checkAndCreateMemDep(
        MoveNodeUse prev, 
        MoveNodeUse mnd, 
        DataDependenceEdge::DependenceType depType);
    void checkAndCreateMemAntideps(
        MoveNodeUse& mnd,
        std::set<MoveNodeUse>& prevNodes,
        DataDependenceEdge::DependenceType depType,
        bool traceable);
    bool hasEarlierMemWriteToSameAddressWithSameGuard(
        MoveNodeUse& mnd,
        std::set<MoveNodeUse>& defines);
    bool hasEarlierWriteWithSameGuard(
        MoveNodeUse& mnd, std::set<MoveNodeUse>& defines);    

    void createRegisterAntideps(
        const TCEString& reg,
        MoveNodeUse& mnd, 
        MoveNodeUseSet& predecessorNodes, 
        DataDependenceEdge::DependenceType depType,
        bool guardedKillFound);

    // functions related to iterating over basic blocks 

    void changeState(
        BBData& bbd,
        BBState newState,
        bool priorize = false);

    // related to mem operation addresses
//    MoveNode* addressMove(const MoveNode&mn);

    /// find special register data from old frontend code
    void findStaticRegisters(
        TTAProgram::CodeSnippet& cs, 
        std::map<int,TCEString>& registers);

    void findStaticRegisters(
        ControlFlowGraph& cfg, 
        std::map<int,TCEString>& registers);

    void findStaticRegisters(
        TTAProgram::Instruction& ins, 
        std::map<int,TCEString>& registers);

    void findStaticRegisters(
        const UniversalMachine& um, 
        std::map<int,TCEString>& registers);

    void appendMoveNodeUse(
        const LiveRangeData::MoveNodeUseSet& src,
        LiveRangeData::MoveNodeUseSet& dst,
        bool setLoopProperty);

    BBDataList blocksByState_[BB_STATES];

    BBDataMap bbData_;
    BasicBlockNode* currentBB_;
    BBData* currentData_;
    DataDependenceGraph* currentDDG_;
    AliasAnalyzerVector aliasAnalyzers_;
    /// contains stack pointer, RV and parameter registers.
    SpecialRegisters specialRegisters_;
    static const TCEString RA_NAME;
    InterPassData* interPassData_;
    ControlFlowGraph* cfg_;
    bool rvIsParamReg_;
};

#endif
