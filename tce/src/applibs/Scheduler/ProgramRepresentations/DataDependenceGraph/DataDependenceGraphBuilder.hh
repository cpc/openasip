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

#include "MNData.hh"

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

    DataDependenceGraph* build(
        BasicBlock& bb, const UniversalMachine* um = NULL) 
        throw (IllegalProgram);    

private:

    typedef ControlFlowGraph::NodeSet BBNodeSet;
    typedef DataDependenceGraph::NodeSet MNodeSet;

    /**
     * BB_UNREACHABLE means basic block we have not yet encountered.
     * BB_NOT_READY means basic block which we have processed for at least
     * one time, but have to be handled multiple times and input BB for
     * later block is not yet processed.
     * BB_REACHABLE means basic block which has some of it's input BB's
     * processed but some not.
     * BB_QUEUED means basic block whose all predecessors have been
     * processed and the BB is ready to be processed.
     * BB_PROCESSSED means basic block which has been processed for the
     * last time, ie. it is completely ready.
     * Only values under this may need some processing.
     * BB_FORGOTTEN means basic block whose successors are ready,
     * so that all extra data about this basic block have been deleted 
     * as it's not needed anymore.
     */
    enum BBState { BB_UNREACHABLE    = 0,
                   BB_NOT_READY      = 1,
                   BB_REACHABLE      = 2,
                   BB_QUEUED         = 3, // all pred const.
                   BB_PROCESSED      = 4,
                   BB_FORGOTTEN      = 5,
                   BB_STATES}; ///< the count of basic block states

    typedef std::set<class MNData> MNDList;
    typedef MNDList::iterator MNDIter;

    /**
     * This class stores all the basic-block related information needed by
     * the data dependency graph creator.
     */
    struct BBData {

        BBData(BasicBlockNode& bb);
        virtual ~BBData();

        void clear();
        POList destPending_; // operations lacking operands
        POList readPending_;  // operations lacking result read

        MNDList ownRegReads_; // these two are for antidep tracking
        MNDList preRegReads_;

        MNDList ownRegWrites_;
        MNDList preRegWrites_;
        MNDList extDepRegList_;

        MNDList memWrites_; // own mem writes
        MNDList memReads_; // own mem reads
        MNDList preMemWrites_; // prev block mem writes

        /* to be added for antidependencies and between BB deps */

        MNDList preMemReads_; // if last ops of prev BB's are reads
        MNDList extDepMemReads_; // first if set of reads
        MNDList extDepMemWrites_; // first if write


        MNDList fuStateWrites_;
        MNDList preFuStates_;

        MNData* raWrite_;
        MNDList raReads_;
        BBState state_;

        int processedCount_;
        int processOrder_;
        int loopDepth_;
        int maxLoopDepth_;
        int predCount_;

        BasicBlockNode* bblock_;
    };

    void setPreDependencies(BBNodeSet& inputBlocks);

    void updateBB(BBNodeSet& inputBlocks);

    void constructIndividualBB() throw (IllegalProgram);
    
    void constructBB(BBNodeSet& inputBlocks) 
        throw (IllegalProgram);

    void createOperationEdges(ProgramOperation& po);

    void processGuard(const class MNData& moveNode);

    void processSource(const class MNData& moveNode);

    void processResultRead(const class MNData& moveNode);

    void processRARead(const class MNData& moveNode);
    void processRegRead(const class MNData& moveNode);

    void processDestination(const class MNData& moveNode);

    void processRAWrite(const class MNData& moveNode);
    void processRegWrite(const class MNData& moveNode);

    void processTrigger(const class MNData& moveNode,
                        class Operation &dop) throw (IllegalProgram);

    void createTriggerDependencies(const class MNData& moveNode,
                                   class Operation& dop);

    void processMemRead(const class MNData& moveNode, bool pseudo = false)
        throw (Exception);

    void processMemWrite(const class MNData& moveNode, bool pseudo = false)
        throw (Exception);

    void processOperand(
        const class MNData& moveNode,
        Operation &dop);

    MemoryAliasAnalyzer::AliasingResult analyzeMemoryAlias(
        MoveNode& mn1, MoveNode& mn2);

    bool addressTraceable(MoveNode& mn);
    
    bool checkAndCreateMemWAW(
        MoveNode& prev, MoveNode& mn, bool pseudo);

    bool checkAndCreateMemRAW(
        MoveNode& prev, MoveNode& mn, bool pseudo);

    bool checkAndCreateGuardRAW(const MNData& prev, const MNData& current);

    void createRegRaw(
        MoveNode& current, MoveNode& source, 
        bool tailPseudo = false, bool headPseudo = false);

    void createRegWaw(
        MoveNode& current, MoveNode& source, 
        bool tailPseudo = false, bool headPseudo = false);

    void createRegWar(
        MoveNode& current, MoveNode& source, 
        bool tailPseudo = false, bool headPseudo = false);

    // functions related to iterating over basic blocks 

    void processParameters(BBData& bbd);

    void changeState(
        std::list<BBData*> (&blocksByState)[BB_STATES],
        BBData* bbd, BBState newState);

    void copyMNDList(MNDList& dst, MNDList& src);

    void getStaticRegisters(
        TTAProgram::Program& prog, 
        std::map<int,TTAProgram::TerminalRegister*>& registers);

    void getStaticRegisters(
        TTAProgram::CodeSnippet& cs, 
        std::map<int,TTAProgram::TerminalRegister*>& registers);

    void getStaticRegisters(
        ControlFlowGraph& cfg, 
        std::map<int,TTAProgram::TerminalRegister*>& registers);

    void getStaticRegisters(
        TTAProgram::Instruction& ins, 
        std::map<int,TTAProgram::TerminalRegister*>& registers);

    void getStaticRegisters(
        const UniversalMachine& um, 
        std::map<int,TTAProgram::TerminalRegister*>& registers);
    
    std::map <BasicBlockNode*, BBData*> bbData_;
    BasicBlockNode* currentBB_;
    BBData* currentData_;
    DataDependenceGraph* currentDDG_;

    int processOrder_;

    std::vector<MemoryAliasAnalyzer*> aliasAnalyzers_;

    bool singleBBMode_;
    MoveNode* entryNode_;
    MNData* entryData_;

    // contains stack pointer, RV and parameter registers.
    std::map<int, TTAProgram::TerminalRegister*> specialRegisters_;
};

#endif
