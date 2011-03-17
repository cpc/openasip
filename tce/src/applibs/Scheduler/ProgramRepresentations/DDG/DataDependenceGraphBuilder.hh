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
 * @file DataDependenceGraphBuilder.hh
 *
 * Declaration of data dependence graph builder class
 *
 * @author Heikki Kultala 2006-2010 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_DDG_BUILDER_HH
#define TTA_DDG_BUILDER_HH

#include <map>
#include <set>
#include <list>

#include "DataDependenceGraph.hh"
#include "ControlFlowGraph.hh"

#include "MemoryAliasAnalyzer.hh"
#include "TCEString.hh"

namespace TTAProgram {
    class Program;
    class TerminalRegister;
    class CodeSnippet;
    class Instruction;
}

namespace TTAMachine {
    class Machine;
}

class MemoryAliasAnalyzer;
class BasicBlock;
class UniversalMachine;
class InterPassData;

/**
 * DataDependenceGraphBuilder class is repsonsible for building data 
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
        bool createMemAndFUDeps = true);

    virtual DataDependenceGraph* build(
        BasicBlock& bb, 
        DataDependenceGraph::AntidependenceLevel antidependenceLevel,
        const TCEString& ddgname = "small bb",
        const UniversalMachine* um = NULL,
        bool createMemAndFUDeps = true);

protected:

    typedef ControlFlowGraph::NodeSet BBNodeSet;
    typedef DataDependenceGraph::NodeSet MNodeSet;

    /**
     * BB_UNREACHED2 means basic block we have not yet encountered.
     * BB_QUEUED means a BB which is queued to be processed
     * BB_READY2 means BB which is already processed. May however change state to
     * BB_QUEUED 
     */
    enum BBState {
        BB_UNREACHED = 0,
        BB_QUEUED = 1,
        BB_READY = 2,
        BB_STATES};
    
    enum ConstructionPhase {
        REGISTERS_AND_PROGRAM_OPERATIONS = 0,
        MEMORY_AND_SIDE_EFFECTS};

    struct MNData2 {
        MNData2() : mn_(NULL) {} // just because STL sucks. there is always = after this.
        MNData2(
            const MoveNode& mn, bool guard = false, bool ra = false, 
            bool pseudo = false) :
            mn_(&mn), guard_(guard), ra_(ra), pseudo_(pseudo) {}
        // TODO: should be deterministic - this is not!
        bool operator< (const MNData2& other) const {
            if (mn_ == NULL) return false;
            if (other.mn_ == NULL) return true;

            if (mn_->nodeID() < other.mn_->nodeID()) return true;
            if (mn_->nodeID() > other.mn_->nodeID()) return false;

            if (guard_ < other.guard_) return true;
            if (guard_ > other.guard_) return false;
            
            if (ra_ < other.ra_) return true;
            if (ra_ > other.ra_) return false;

            if (pseudo_ < other.pseudo_) return true;
            if (pseudo_ > other.pseudo_) return false;
            return false;
        }
        const MoveNode* mn_;
        bool guard_;
        bool ra_;
        bool pseudo_;
    };

    typedef std::set<MNData2 > RegisterUseSet;
    typedef std::map<std::string, RegisterUseSet > RegisterUseMapSet;
    typedef std::map<std::string, MNData2 > RegisterUseMap;

    typedef std::pair<std::string, RegisterUseSet > RegisterUseSetPair;
    typedef std::pair<std::string, MNData2> RegisterUsePair;

    /**
     * This class stores all the basic-block related information needed by
     * the data dependency graph creator.
     */
    struct BBData {

        BBData(BasicBlockNode& bb);
        virtual ~BBData();

        POList destPending_; // operations lacking operands
        POList readPending_;  // operations lacking result read

        BBState state_;
        bool constructed_;

        BasicBlockNode* bblock_;

        // dependencies out from this BB
        RegisterUseMapSet regDefines_;
        RegisterUseMapSet regLastUses_;
        RegisterUseMap regLastKills_;

        // dependencies in to this BB
        RegisterUseMap regKills_;
        RegisterUseMapSet regFirstUses_;
        RegisterUseMapSet regFirstDefines_;

        // dependencies from previous BBs.
        RegisterUseMapSet regDefReaches_;
        RegisterUseMapSet regUseReaches_;

        // all alive after this BB.
        RegisterUseMapSet regDefAfter_;
        RegisterUseMapSet regUseAfter_;

        // dependencies out from this BB
        RegisterUseMapSet memDefines_;
        RegisterUseMapSet memLastUses_;
        RegisterUseMap memLastKills_;

        // dependencies into this one
        RegisterUseMap memKills_;
        RegisterUseMapSet memFirstUses_;
        RegisterUseMapSet memFirstDefines_;

        // deps from previous BBs
        RegisterUseMapSet memDefReaches_;
        RegisterUseMapSet memUseReaches_;

        // all alive after this
        RegisterUseMapSet memDefAfter_;
        RegisterUseMapSet memUseAfter_;
        
        // fu state deps
        RegisterUseSet fuDepReaches_;
        RegisterUseSet fuDeps_;
        RegisterUseSet fuDepAfter_;
    };

    bool updateRegistersAliveAfter(BBData& bbd);
    bool updateMemAndFuAliveAfter(BBData& bbd);
    void createMemAndFUstateDeps();
    void createRegisterDeps();
    void initializeBBStates();
    BasicBlockNode* queueFirstBB();

    void iterateBBs(
        ConstructionPhase phase);

    void setSucceedingPredeps(
        BBData& bbd, bool queueAll,
        ConstructionPhase phase);
    bool appendUseMapSets(
        const RegisterUseMapSet& srcMap, RegisterUseMapSet& dstMap);
   
    void updateBB(
        BBData& bbd, 
        ConstructionPhase phase);

    void constructIndividualBB(ConstructionPhase phase);
    void constructIndividualBB(
        BBData& bbd,
        ConstructionPhase);    
    
    void constructBB(BBNodeSet& inputBlocks) 
        throw (IllegalProgram);

    void createOperationEdges(ProgramOperation& po);

    void processGuard(class MoveNode& moveNode);

    void processSource(class MoveNode& moveNode);

    void processResultRead(class MoveNode& moveNode);
    void processCall(MoveNode& mn);
    void processReturn(MoveNode& moveNode);

    void processEntryNode(MoveNode& mn);

    void processDestination(
        class MoveNode& moveNode, ConstructionPhase phase);

    void processRegUse(MNData2 mn, const std::string& reg);
    void updateRegUse(MNData2 mn, const std::string& reg);
    void updateMemUse(MNData2 mnd, const TCEString& category);

    void processRegWrite(MNData2 mn, const std::string& reg);
    void updateRegWrite(MNData2 mn, const std::string& reg);
    void updateMemWrite(MNData2 mnd, const TCEString& category);

    void processTriggerPO(
        class MoveNode& moveNode, Operation &dop) throw (IllegalProgram);

    void processTriggerRegistersAndOperations(
        class MoveNode& moveNode,
        class Operation &dop);

    void processTriggerMemoryAndFUStates(
        class MoveNode& moveNode,
        class Operation &dop);

    void createTriggerDependencies(class MoveNode& moveNode,
                                   class Operation& dop);

    void createSideEffectEdges(    
        RegisterUseSet& prevMoves, const MoveNode& mn, Operation& dop);

    void processMemWrite(MNData2 mnd);
    void processMemUse(MNData2 mnd);

    void processOperand(
        class MoveNode& moveNode,
        Operation &dop);

    MemoryAliasAnalyzer::AliasingResult analyzeMemoryAlias(
        const MoveNode* mn1, const MoveNode* mn2);

    bool addressTraceable(const MoveNode* mn);
    TCEString memoryCategory(const MNData2& mnd);

    bool checkAndCreateMemWAW(
        const MoveNode& prev, const MoveNode& mn, bool pseudo = false);

    bool checkAndCreateMemRAW(
        const MoveNode& prev, const MoveNode& mn, bool pseudo = false);

    bool checkAndCreateMemDep(
        MNData2 prev, MNData2 mnd, DataDependenceEdge::DependenceType depType);

    bool exclusingGuards(const MoveNode& mn1, const MoveNode& mn2);

    void createRegRaw(
        const MNData2& current, const MNData2& source, const TCEString& reg);
    void createRegWar(
        const MNData2& current, const MNData2& source, const TCEString& reg);
    void createRegWaw(
        const MNData2& current, const MNData2& source, const TCEString& reg);

    // functions related to iterating over basic blocks 

    void changeState(BBData& bbd, BBState newState);

    static std::string trName(TTAProgram::TerminalRegister& tr);

    // related to mem operation addresses

    MoveNode* addressOperandMove(ProgramOperation&po);
    MoveNode* addressMove(const MoveNode&mn);

    // get special register data from old frontend code

    void getStaticRegisters(
        TTAProgram::Program& prog, 
        std::map<int,std::string>& registers);

    void getStaticRegisters(
        TTAProgram::CodeSnippet& cs, 
        std::map<int,std::string>& registers);

    void getStaticRegisters(
        ControlFlowGraph& cfg, 
        std::map<int,std::string>& registers);

    void getStaticRegisters(
        TTAProgram::Instruction& ins, 
        std::map<int,std::string>& registers);

    void getStaticRegisters(
        const UniversalMachine& um, 
        std::map<int,std::string>& registers);

    // then member variables.

    std::list<BBData*> blocksByState_[BB_STATES];

    std::map <BasicBlockNode*, BBData*> bbData_;
    BasicBlockNode* currentBB_;
    BBData* currentData_;
    DataDependenceGraph* currentDDG_;

    std::vector<MemoryAliasAnalyzer*> aliasAnalyzers_;

    // contains stack pointer, RV and parameter registers.
//    typedef std::map<int, TCEString> SpecialRegisters;
//    SpecialRegisters
    std::map<int, std::string> specialRegisters_;

    static const std::string RA_NAME;
    InterPassData* interPassData_;
    ControlFlowGraph* cfg_;
    bool rvIsParamReg_;
};

#endif
