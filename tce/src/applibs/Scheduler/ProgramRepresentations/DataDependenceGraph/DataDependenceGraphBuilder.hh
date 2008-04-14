/**
 * @file DataDependenceGraphBuilder.hh
 *
 * Declaration of data dependence graph builder class
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
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

//#include "MNData.hh"

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

/**
 * DataDependenceGraphBuilder class is repsonsible for building data 
 * dependence graphs.
 */
class DataDependenceGraphBuilder {
public:
    DataDependenceGraphBuilder();
    
    virtual ~DataDependenceGraphBuilder();
   
    void addAliasAnalyzer(MemoryAliasAnalyzer* analyzer);
    
    DataDependenceGraph* build(
        ControlFlowGraph& cGraph, const UniversalMachine* um = NULL);

//    DataDependenceGraph* build2(
//        ControlFlowGraph& cGraph, const UniversalMachine* um = NULL);

    DataDependenceGraph* build(
        BasicBlock& bb, const UniversalMachine* um = NULL) 
        throw (IllegalProgram);    

private:

    typedef ControlFlowGraph::NodeSet BBNodeSet;
    typedef DataDependenceGraph::NodeSet MNodeSet;

    /**
     * BB_UNREACHED2 means basic block we have not yet encountered.
     * BB_QUEUED means a BB which is queued to be processed
     * BB_READY2 means BB which is already processed. May however change state to
     * BB_QUEUED 
     */
    enum BBState2 {
        BB_UNREACHED2 = 0,
        BB_QUEUED2 = 1,
        BB_READY2 = 2,
        BB_STATES2};
    
    struct MNData2 {
        MNData2() : mn_(NULL) {} // just because STL sucks. there is always = after this.
        MNData2(const MoveNode& mn, bool guard = false, bool ra = false, bool pseudo = false) :
            mn_(&mn), guard_(guard), ra_(ra), pseudo_(pseudo) {}
        // TODO: should be deterministic - this is not!
        bool operator< (const MNData2& other) const {
            if ((mn_ == NULL) && (other.mn_ != NULL)) return true;
            if ((mn_ != NULL) < (other.mn_ != NULL)) return true;
            if ((mn_ != NULL) > (other.mn_ != NULL)) return false;
            if (mn_ != NULL) {
                if (mn_->nodeID() < other.mn_->nodeID()) return true;
                if (mn_->nodeID() > other.mn_->nodeID()) return false;
            }
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

        BBState2 state2_;
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

        // dependencies not used in this or after this.
        // colleced afterwards, currently not yet implemented.
        std::set<std::string> regExpires_;

        // dependencies out from this BB
        RegisterUseSet memDefines_;
        RegisterUseSet memLastUses_;
        MNData2 memLastKill_;

        // dependencies into this one
        MNData2 memKill_;
        RegisterUseSet memFirstUses_;
        RegisterUseSet memFirstDefines_;

        // deps from previous BBs
        RegisterUseSet memDefReaches_;
        RegisterUseSet memUseReaches_;

        // all alive after this
        RegisterUseSet memDefAfter_;
        RegisterUseSet memUseAfter_;
        
        // fu state deps
        RegisterUseSet fuDepReaches_;
        RegisterUseSet fuDeps_;
        RegisterUseSet fuDepAfter_;
    };

    bool updateAliveAfter(BBData& bbd);

    void setSucceedingPredeps(
        BBData& bbd, ControlFlowGraph& cfg, bool queueAll);
    bool appendUseMapSets(
        const RegisterUseMapSet& srcMap, RegisterUseMapSet& dstMap);
        
    void updateBB(BBData& bbd);

    void constructIndividualBB() throw (IllegalProgram);
    void constructIndividualBB(BBData& bbd);
    
    void constructBB(BBNodeSet& inputBlocks) 
        throw (IllegalProgram);

    void createOperationEdges(ProgramOperation& po);

    void processGuard(class MoveNode& moveNode);

    void processSource(class MoveNode& moveNode);

    void processResultRead(class MoveNode& moveNode);
    void processCall(MoveNode& mn);
    void processReturn(MoveNode& moveNode);

    void processEntryNode(MoveNode& mn);

    void processDestination(class MoveNode& moveNode);

    void processRegUse(MNData2 mn, const std::string& reg);
    void updateRegUse(MNData2 mn, const std::string& reg);
    void updateMemUse(MNData2 mnd);

    void processRegWrite(MNData2 mn, const std::string& reg);
    void updateRegWrite(MNData2 mn, const std::string& reg);
    void updateMemWrite(MNData2 mnd);

    void processTrigger(class MoveNode& moveNode,
                        class Operation &dop) throw (IllegalProgram);

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
        const MoveNode& mn1, const MoveNode& mn2);

    bool addressTraceable(const MoveNode& mn);
    
    bool checkAndCreateMemWAW(
        const MoveNode& prev, const MoveNode& mn, bool pseudo = false);

    bool checkAndCreateMemRAW(
        const MoveNode& prev, const MoveNode& mn, bool pseudo = false);

    bool checkAndCreateMemDep(
        MNData2 prev, MNData2 mnd, DataDependenceEdge::DependenceType depType);

    bool exclusingGuards(const MoveNode& mn1, const MoveNode& mn2);

    void createRegRaw(const MNData2& current, const MNData2& source);
    void createRegWar(const MNData2& current, const MNData2& source);
    void createRegWaw(const MNData2& current, const MNData2& source);

    // functions related to iterating over basic blocks 

//    void processParameters(BBData& bbd);

    void changeState(BBData& bbd, BBState2 newState);
//    void queueBB(BBData& bbData);

    static std::string trName(TTAProgram::TerminalRegister& tr);

//    void copyMNDList(MNDList& dst, MNDList& src);

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


    std::list<BBData*> blocksByState_[BB_STATES2];

    
    std::map <BasicBlockNode*, BBData*> bbData_;
    BasicBlockNode* currentBB_;
    BBData* currentData_;
    DataDependenceGraph* currentDDG_;

    int processOrder_;

    std::vector<MemoryAliasAnalyzer*> aliasAnalyzers_;

    bool singleBBMode_;
    MoveNode* entryNode_;

    // contains stack pointer, RV and parameter registers.
    std::map<int, std::string> specialRegisters_;

    static const std::string RA_NAME;

            
};

#endif
