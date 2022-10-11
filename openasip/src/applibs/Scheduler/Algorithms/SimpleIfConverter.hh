/**
 * @file SimpleIfConverter.hh
 *
 * Declaration of simple if converter optimizer class
 *
 * @author Heikki Kultala 2008 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMPLE_IF_CONVERTER_HH
#define TTA_SIMPLE_IF_CONVERTER_HH

#include "ControlFlowGraphPass.hh"
#include "ProcedurePass.hh"
#include "ProgramPass.hh"
#include "ProgramOperation.hh"

class ControlFlowGraph;
class BasicBlockNode;
class BasicBlock;
class ControlFlowEdge;
class DataDependenceGraph;
class MoveNode;

namespace TTAMachine {
    class RegisterFile;
}

namespace TTAProgram {
    class Instruction;
    class MoveGuard;
    class Move;
    class InstructionReferenceManager;
    class CodeGenerator;
    class CodeSnippet;
    class BasicBlock;
    class TerminalFUPort;
}

class SimpleIfConverter : public ControlFlowGraphPass, ProcedurePass,
    ProgramPass {

public:
    SimpleIfConverter(
        InterPassData& data, const TTAMachine::Machine& targetMachine);

    virtual void handleControlFlowGraph(
        ControlFlowGraph& cfg, const TTAMachine::Machine& targetMachine);

    virtual void handleProcedure(
        TTAProgram::Procedure& procedure,
        const TTAMachine::Machine& targetMachine);

    virtual void handleProgram(
        TTAProgram::Program& program, const TTAMachine::Machine& targetMachine);

    virtual std::string shortDescription() const;

    static bool removeJump(TTAProgram::BasicBlock& bb);
    static ProgramOperationPtr fixTerminalPO(
        TTAProgram::TerminalFUPort& terminal,
        std::map<ProgramOperationPtr,ProgramOperationPtr>& poMapping);

private:

    /**
     * This struct holds data about one area being if-converted,
     * all the nodes and their order and the form of the area.
     */
    struct CandidateBlocks {

        CandidateBlocks(
            BasicBlockNode& firstNode, BasicBlockNode& fallThruNode,
            BasicBlockNode& jumpNode, BasicBlockNode& lastNode,
            BasicBlockNode* joinNode, BasicBlockNode* succNode1,
            BasicBlockNode* succNode2, bool removeJoin, bool removeFT,
            bool removeJd);

        ~CandidateBlocks();

        BasicBlockNode& firstNode_;
        BasicBlockNode& fallThruNode_;
        BasicBlockNode& jumpDestNode_;
        BasicBlockNode& lastNode_;
        BasicBlockNode* joinNode_;
        BasicBlockNode* succNode1_;
        BasicBlockNode* succNode2_;

        TTAProgram::BasicBlock& firstBB_;
        TTAProgram::BasicBlock& fallThruBB_;
        TTAProgram::BasicBlock& jumpDestBB_;
        TTAProgram::BasicBlock& lastBB_;
        TTAProgram::BasicBlock* joinBB_;

        bool removeJoin_;
        bool removeFt_;
        bool removeJd_;
        bool createJump_;
        TTAProgram::MoveGuard* guard_;
        TTAProgram::MoveGuard* invg_;
        int grIndex_;
        const TTAMachine::RegisterFile* grFile_;
    };

    CandidateBlocks* detectDiamond(
        BasicBlockNode& bbn, BasicBlockNode& fallThruNode,
        BasicBlockNode& jumpDestNode, ControlFlowGraph& cfg);

    CandidateBlocks* detectTriangleViaJump(
        BasicBlockNode& bbn, BasicBlockNode& fallThruNode,
        BasicBlockNode& jumpDestNode, ControlFlowGraph& cfg);

    CandidateBlocks* detectTriangleViaFt(
        BasicBlockNode& bbn, BasicBlockNode& fallThruNode,
        BasicBlockNode& jumpDestNode, ControlFlowGraph& cfg);

    std::pair<BasicBlockNode*,BasicBlockNode*> successors(
	BasicBlockNode& node, ControlFlowGraph& cfg);

    CandidateBlocks* searchCandidate(ControlFlowGraph& cfg);

    void addJump(TTAProgram::BasicBlock& bb, BasicBlockNode& bbn);

    void appendBB(
        const TTAProgram::BasicBlock& src, TTAProgram::BasicBlock& dest, 
        TTAProgram::MoveGuard* mg, bool removeJumps);

    bool writesRegister(
        const TTAProgram::BasicBlock& bb,
        int index,
        const TTAMachine::RegisterFile& rf,
        bool ignoreLastInstruction);

    bool hasConditionals(TTAProgram::BasicBlock& bb);

    bool canConvert(
        CandidateBlocks* candidates,
        ControlFlowGraph& cfg);

    void combineBlocks(CandidateBlocks& bblocks);

    void convert(CandidateBlocks& bblocks, ControlFlowGraph& cfg);

    void updateCfg(CandidateBlocks& bblocks, ControlFlowGraph& cfg);
        
    TTAProgram::CodeGenerator* codeGenerator_;
    TTAProgram::InstructionReferenceManager* irm_;

    int diamonds_;
    int diamonds2_;
    int triangles1_;
    int triangles2_;

    int grAborts_;
    int grDefAborts_;
    int grUseAborts_;
    int loopAborts_;
    int uncondAborts_;
    int sizeAborts_;
    int succAborts_;

    int diamondSizeLimit_;
    int triangleSizeLimit1_;
    int triangleSizeLimit2_;
};

#endif
