/*
    Copyright (c) 2002-2015 Tampere University.

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
 * @file BBSchedulerController.hh
 *
 * Declaration of BBSchedulerController class.
 * @author Pekka J��skel�inen 2006,2015 (pjaaskel-no.spam-cs.tut.fi)
 * @author Heikki Kultala 2009 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BB_SCHEDULER_CONTROLLER_HH
#define TTA_BB_SCHEDULER_CONTROLLER_HH

#include <boost/progress.hpp>

#include "BasicBlockPass.hh"
#include "ControlFlowGraphPass.hh"
#include "ProcedurePass.hh"
#include "ProgramPass.hh"

class BasicBlockNode;
class SoftwareBypasser;
class CopyingDelaySlotFiller;
class SchedYieldEmitter;
class LLVMTCECmdLineOptions;
class DDGPass;
class DataDependenceGraph;

namespace TTAProgram {
    class Program;
    class Procedure;
}

/**
 * A class that control operation of a basic block scheduler.
 *
 * This class handles calling other optimization passes and tranferring data
 * for the actual BB scheduler and loop scheudlers etc.
 */
class BBSchedulerController :
    public BasicBlockPass, public ControlFlowGraphPass, public ProcedurePass,
    public ProgramPass {
public:
    BBSchedulerController(
        InterPassData& data, SoftwareBypasser* bypasser=NULL, 
        CopyingDelaySlotFiller* delaySlotFiller=NULL,
        DataDependenceGraph* bigDDG = NULL);
    virtual ~BBSchedulerController();

    virtual void handleBasicBlock(
        TTAProgram::BasicBlock& bb, const TTAMachine::Machine& targetMachine,
        TTAProgram::InstructionReferenceManager& irm,
        BasicBlockNode* bbn = NULL) override;

    virtual void handleControlFlowGraph(
        ControlFlowGraph& cfg,
        const TTAMachine::Machine& targetMachine) override;

    virtual void handleProcedure(
        TTAProgram::Procedure& procedure,
        const TTAMachine::Machine& targetMachine) override;

    // is needed only for some sw bypass statistics
    virtual void handleProgram(
        TTAProgram::Program& program,
        const TTAMachine::Machine& targetMachine) override;

    virtual void executeDDGPass(
        TTAProgram::BasicBlock& bb, const TTAMachine::Machine& targetMachine,
        TTAProgram::InstructionReferenceManager& irm,
        std::vector<DDGPass*> ddgPasses, BasicBlockNode* bbn = NULL) override;

    bool executeLoopPass(
        TTAProgram::BasicBlock& bb, const TTAMachine::Machine& targetMachine,
        TTAProgram::InstructionReferenceManager& irm,
        std::vector<DDGPass*> ddgPasses, BasicBlockNode* bbn = NULL) override;

    virtual void handleCFGDDG(
        ControlFlowGraph& cfg,
        DataDependenceGraph& ddg,
        const TTAMachine::Machine& targetMachine);

    virtual std::string shortDescription() const override;
    virtual std::string longDescription() const override;

protected:
    virtual DataDependenceGraph* createDDGFromBB(
        TTAProgram::BasicBlock& bb, const TTAMachine::Machine& mach);

    std::pair<unsigned int, unsigned int> calculateII(
        const BasicBlockNode& bbn,  const TTAMachine::Machine& targetMachine);

private:
    
    /// The currently scheduled procedure.
    TTAProgram::Procedure* scheduledProcedure_;

    /// Control flow graph of the procedure.
    ControlFlowGraph* cfg_;

    /// whole-procedure DDG.
    DataDependenceGraph* bigDDG_;
    /// The software bypasser to use to bypass registers when possible.
    SoftwareBypasser* softwareBypasser_;

    CopyingDelaySlotFiller* delaySlotFiller_;

    /// Number of basic blocks scheduled so far.
    int basicBlocksScheduled_;
    /// Total basic blocks in the CFG currently being scheduled.
    int totalBasicBlocks_;
    /// Fancy progress bar. Pointer because creation outputs the initial
    /// progress bar and we want it only on verbose mode.
    boost::progress_display* progressBar_;

    LLVMTCECmdLineOptions* options_;
};

#endif
