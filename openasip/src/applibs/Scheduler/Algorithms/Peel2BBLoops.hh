/**
 * @file Peel2BBLoops.hh
 *
 * This optimizer optimizes some 2-bb loops into
 * 1-BB loop be peeling out the 1st iteration which jumps into the middle
 * of the jump. The resulting 1-BB loop can then be loop-scheduled.
 *
 * @author Heikki Kultala 2016 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_PEEL_2_BB_LOOPS_HH
#define TTA_PEEL_2_BB_LOOPS_HH

#include "ProcedurePass.hh"
#include "ControlFlowGraphPass.hh"
#include "ProgramPass.hh"
#include "Application.hh"
#include <vector>


class Peel2BBLoops : public ControlFlowGraphPass {

public:
    Peel2BBLoops(
        InterPassData& data, const TTAMachine::Machine& targetMachine);

    void handleControlFlowGraph(
        ControlFlowGraph& cfg,
        const TTAMachine::Machine& targetMachine) override;

    virtual std::string shortDescription() const override {
        return "optimizes two-BB inner loops into single-bb inner loops";
    }
private:
    struct BBNodes {
        BBNodes(bool ok) : BBNodes() {
            assert(!ok && "only call this on fail to allow return false"); }
        BBNodes() : preLoop(NULL), beginLoop(NULL),
                    endLoop(NULL), postLoop(NULL) {}
        BBNodes(BasicBlockNode* prel, BasicBlockNode* bl,
                BasicBlockNode* el, BasicBlockNode* pol)
            : preLoop(prel), beginLoop(bl), endLoop(el), postLoop(pol) {}
        BasicBlockNode* preLoop;
        BasicBlockNode* beginLoop;
        BasicBlockNode* endLoop;
        BasicBlockNode* postLoop;
        operator bool() const { return preLoop != NULL; }
    };

    bool negateOp(ProgramOperationPtr po);

    BBNodes testIf2BBLoop(ControlFlowGraph& cfg, BasicBlockNode& bbn);
    void peel2BBLoop(ControlFlowGraph& cfg, BBNodes& bbns);
    void updateCFG(ControlFlowGraph& cfg, BBNodes& bbns);
    void performCodeMotion(BBNodes& bbns);
    void appendBB(
        const TTAProgram::BasicBlock& src,
        TTAProgram::BasicBlock& dest,
        BasicBlockNode* newJumpDest);

    TTAProgram::CodeGenerator* codeGenerator_;
    TTAProgram::InstructionReferenceManager* irm_;
    const TTAMachine::Machine& mach_;
};

#endif
