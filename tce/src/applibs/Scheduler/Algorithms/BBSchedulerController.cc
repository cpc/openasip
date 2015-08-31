/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file BBSchedulerController.cc
 *
 * Definition of BBSchedulerController class.
 *
 * @author Pekka J‰‰skel‰inen 2006-2010 (pjaaskel-no.spam-cs.tut.fi)
 * @author Heikki Kultala 2009 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <set>
#include <string>
#include <cstdlib>

#include <boost/timer.hpp>

#include "BBSchedulerController.hh"
#include "ControlFlowGraph.hh"
#include "DataDependenceGraph.hh"
#include "DataDependenceGraphBuilder.hh"
#include "SimpleResourceManager.hh"
#include "Procedure.hh"
#include "Machine.hh"
#include "Instruction.hh"
#include "BasicBlock.hh"
#include "ControlFlowGraphPass.hh"
#include "SchedulerPass.hh"
#include "SoftwareBypasser.hh"
#include "CopyingDelaySlotFiller.hh"
#include "Program.hh"
#include "TCEString.hh"
#include "Application.hh"
#include "LLVMTCECmdLineOptions.hh"
#include "SchedulerCmdLineOptions.hh"
#include "InterPassData.hh"
#include "ResourceConstraintAnalyzer.hh"
#include "BasicBlockScheduler.hh"
#include "RegisterRenamer.hh"
#include "BUBasicBlockScheduler.hh"
#include "BypassingBUBasicBlockScheduler.hh"

namespace TTAMachine {
    class UniversalMachine;
}

//#define DEBUG_OUTPUT
//#define CFG_SNAPSHOTS
//#define BIG_DDG_SNAPSHOTS
//#define SW_BYPASSING_STATISTICS

/**
 * Constructs the basic block scheduler.
 *
 * @param data Interpass data
 * @param bypasser Helper module implementing software bypassing
 * @param delaySlotFiller Helper module implementing jump delay slot filling
 */
BBSchedulerController::BBSchedulerController(
    InterPassData& data,
    SoftwareBypasser* bypasser,
    CopyingDelaySlotFiller* delaySlotFiller,
    DataDependenceGraph* bigDDG) :
    BasicBlockPass(data), ControlFlowGraphPass(data), ProcedurePass(data),
    ProgramPass(data), bigDDG_(bigDDG), 
    softwareBypasser_(bypasser), delaySlotFiller_(delaySlotFiller),
    basicBlocksScheduled_(0),
    totalBasicBlocks_(0), progressBar_(NULL) {

    CmdLineOptions *cmdLineOptions = Application::cmdLineOptions();
    options_ = dynamic_cast<LLVMTCECmdLineOptions*>(cmdLineOptions);
}

BBSchedulerController::~BBSchedulerController() {
    delete progressBar_; 
    progressBar_ = NULL;
}

/**
 * Schedules a single basic block.
 *
 * @param bb The basic block to schedule.
 * @param targetMachine The target machine.
 * @exception Exception several TCE exceptions can be thrown in case of
 *            a scheduling error.
 */
void
BBSchedulerController::handleBasicBlock(
    TTAProgram::BasicBlock& bb,
    const TTAMachine::Machine& targetMachine,
    TTAProgram::InstructionReferenceManager& irm,
    BasicBlockNode* bbn)
    throw (Exception) {

    // TODO: define them somewhere in one place.
    static const TCEString SP_DATUM = "STACK_POINTER";
    static const TCEString RV_DATUM = "RV_REGISTER";
    // high part of 64-bit return values.
    static const TCEString RV_HIGH_DATUM = "RV_HIGH_REGISTER";

    if (bb.instructionCount() == 0)
        return;

    bool bbScheduled = false;

    RegisterRenamer* rr = NULL;

    SchedulerCmdLineOptions* options =
        dynamic_cast<SchedulerCmdLineOptions*>(
            Application::cmdLineOptions());

    // create register renamer if enabled and we know the
    // reserved registers.
    if (options != NULL && options->renameRegisters() && bigDDG_ != NULL
        && BasicBlockPass::interPassData().hasDatum(SP_DATUM) &&
        BasicBlockPass::interPassData().hasDatum(RV_DATUM) &&
        BasicBlockPass::interPassData().hasDatum(RV_HIGH_DATUM)) {
        rr = new RegisterRenamer(targetMachine, bb);
    }

    std::vector<DDGPass*> bbSchedulers;

    if (options_ != NULL && options_->useRecursiveBUScheduler()) {
        bbSchedulers.push_back(
            new BypassingBUBasicBlockScheduler(
                BasicBlockPass::interPassData(), softwareBypasser_, NULL, rr));
    } else if (options_ != NULL && options_->useBUScheduler()) {
       bbSchedulers.push_back(
           new BUBasicBlockScheduler(
               BasicBlockPass::interPassData(), softwareBypasser_, NULL, rr));
    } else {   
       bbSchedulers.push_back(
           new BasicBlockScheduler(
               BasicBlockPass::interPassData(), softwareBypasser_, NULL, rr));
    }
       
    // if not scheduled yet (or loop scheduling failed)
    if (!bbScheduled) {

        executeDDGPass(
            bb, targetMachine, irm, bbSchedulers, bbn);

        if (Application::verboseLevel() > 0) {
            if (progressBar_ != NULL)
                ++(*progressBar_);
        }

        ++basicBlocksScheduled_;
    }
    if (rr != NULL) {
        delete rr;
    }

    // these are no longer needed. delete them to save memory.
    bb.liveRangeData_->regFirstDefines_.clear();
    bb.liveRangeData_->regDefines_.clear();
    bb.liveRangeData_->regLastUses_.clear();

    bb.liveRangeData_->regDefReaches_.clear();
    bb.liveRangeData_->registersUsedAfter_.clear();
    bb.liveRangeData_->regFirstUses_.clear();
    bb.liveRangeData_->regDefines_.clear();
    AssocTools::deleteAllItems(bbSchedulers);
}

#ifdef DEBUG_REG_COPY_ADDER
static int graphCount = 0;
#endif

/**
 * Schedules a procedure.
 *
 * The original procedure is modified during scheduling.
 *
 * @param procedure The procedure to schedule.
 * @param targetMachine The target machine.
 * @exception Exception In case of an error during scheduling. The exception
 *            type can be any subtype of Exception.
 */
void
BBSchedulerController::handleProcedure(
    TTAProgram::Procedure& procedure,
    const TTAMachine::Machine& targetMachine)
    throw (Exception) {

    ControlFlowGraph cfg(procedure, BasicBlockPass::interPassData());

    if (Application::verboseLevel() > 0) {
        totalBasicBlocks_ = cfg.nodeCount() - 3;
        Application::logStream() 
            << " -- " << totalBasicBlocks_ << " basic blocks" << std::endl;
        basicBlocksScheduled_ = 0;

        const bool enableProgressBar = false;
        if (progressBar_ == NULL) {
            if (enableProgressBar) {
                progressBar_ = 
                    new boost::progress_display(totalBasicBlocks_ + 1);
            }
        } else {
            progressBar_->restart(totalBasicBlocks_ + 1);
        }
    }

#ifdef CFG_SNAPSHOTS
    cfg.writeToDotFile(procedure.name() + "_cfg.dot");
#endif

    bigDDG_ = ddgBuilder().build(
        cfg, DataDependenceGraph::INTRA_BB_ANTIDEPS, targetMachine);
    
    if (options_ != NULL && options_->dumpDDGsDot()) {
        bigDDG_->writeToDotFile( 
            (boost::format("proc_%s_before_scheduling.dot") % 
             bigDDG_->name()).str());
    }
    
    if (options_ != NULL && options_->dumpDDGsXML()) {
        bigDDG_->writeToXMLFile( 
            (boost::format("proc_%s_before_scheduling.xml") % 
             bigDDG_->name()).str());
    }

    scheduledProcedure_ = &procedure;

    // dsf also called between scheduling.. have to update these before it.
    // delay slot filler needs refs to be into instrs in cfg, not in
    // original program
    cfg.updateReferencesFromProcToCfg();

    procedure.clear();

    handleControlFlowGraph(cfg, targetMachine);

    if (delaySlotFiller_ != NULL && bigDDG_ != NULL) {
        delaySlotFiller_->fillDelaySlots(cfg, *bigDDG_, targetMachine);
    }

    // now all basic blocks are scheduled, let's put them back to the
    // original procedure
    cfg.copyToProcedure(procedure);

    if (bigDDG_ != NULL) {

        if (options_ != NULL && options_->dumpDDGsDot()) {
            bigDDG_->writeToDotFile(
                (boost::format("proc_%s_after_scheduling.dot") % 
                 bigDDG_->name())
                .str());
        } 

        if (options_ != NULL && options_->dumpDDGsXML()) {
            bigDDG_->writeToXMLFile(
                (boost::format("proc_%s_after_scheduling.xml") % 
                 bigDDG_->name())
                .str());
        } 
        delete bigDDG_;
        bigDDG_ = NULL;
    }
    if (delaySlotFiller_ != NULL) {
        delaySlotFiller_->finalizeProcedure();
    }
    scheduledProcedure_ = NULL;
}

/**
 * Schedules all nodes in a control flow graph.
 *
 * The original control flow graph nodes are modified during scheduling.
 *
 * @param cfg The control flow graph to schedule.
 * @param targetMachine The target machine.
 * @exception Exception In case of an error during scheduling. The exception
 *            type can be any subtype of Exception.
 */
void
BBSchedulerController::handleControlFlowGraph(
    ControlFlowGraph& cfg,
    const TTAMachine::Machine& targetMachine)
    throw (Exception) {

    return handleCFGDDG(cfg, *bigDDG_, targetMachine);
}

/**
 * Schedules a program.
 *
 * The original program is modified during scheduling.
 *
 * @param program The program to schedule.
 * @param targetMachine The target machine.
 * @exception Exception In case of an error during scheduling. The exception
 *            type can be any subtype of Exception.
 */
void
BBSchedulerController::handleProgram(
    TTAProgram::Program& program,
    const TTAMachine::Machine& targetMachine)
    throw (Exception) {

    ProgramPass::executeProcedurePass(program, targetMachine, *this);
#ifdef SW_BYPASSING_STATISTICS
    if (softwareBypasser != NULL) {
        Application::logStream() << softwareBypasser_->bypassedCount() << 
            " moves were bypassed and " <<
            softwareBypassed_->deadResults() << 
            " dead results were removed." << std::endl;
#endif
}

/**
 * A short description of the pass, usually the optimization name,
 * such as "basic block scheduler".
 *
 * @return The description as a string.
 */
std::string
BBSchedulerController::shortDescription() const {
    return "Instruction scheduler with a basic block scope.";
}

/**
 * Optional longer description of the pass.
 *
 * This description can include usage instructions, details of choice of
 * algorithmic details, etc.
 *
 * @return The description as a string.
 */
std::string
BBSchedulerController::longDescription() const {
    return
        "Basic block scheduler that uses the longest path information of "
        "data dependency graph to prioritize the ready list. Assumes that "
        "the input has registers allocated and no connectivity missing.";
}


/**
 * Helper function used to create DDG for BBPass.
 *
 * Overrided version in order to use subgraphs.
 *
 * @param bb BasicBlock where DDG is to be created from
 */
DataDependenceGraph*
    BBSchedulerController::createDDGFromBB(
        TTAProgram::BasicBlock& bb, const TTAMachine::Machine& mach) {
    if (bigDDG_ != NULL) {
        return bigDDG_->createSubgraph(bb);
    } else {
        return this->ddgBuilder().build(
            bb, DataDependenceGraph::INTRA_BB_ANTIDEPS, mach);
    }
}

/**
 * Creates a DDG from the given basic block and executes a DDG pass for that.
 */
void
BBSchedulerController::executeDDGPass(
    TTAProgram::BasicBlock& bb,
    const TTAMachine::Machine& targetMachine, 
    TTAProgram::InstructionReferenceManager& irm, 
    std::vector<DDGPass*> ddgPasses, BasicBlockNode*)
    throw (Exception) {

    static int bbNumber = 0;
    DataDependenceGraph* ddg = createDDGFromBB(bb, targetMachine);
    SimpleResourceManager* rm = SimpleResourceManager::createRM(targetMachine);

    assert (ddgPasses.size() == 1);
    ddgPasses.at(0)->handleDDG(*ddg, *rm, targetMachine);
    copyRMToBB(*rm, bb, targetMachine, irm);
    
    if (delaySlotFiller_ != NULL && bigDDG_ != NULL) {
        rm->clearOldResources();
        delaySlotFiller_->addResourceManager(bb, *rm);
    } else {
        SimpleResourceManager::disposeRM(rm);
    }

    if (options_ != NULL && options_->printResourceConstraints()) {
        TCEString ddgName = ddg->name();
        ddgName << bbNumber;
        ResourceConstraintAnalyzer rcAnalyzer(*ddg, *rm, ddgName);
        rcAnalyzer.analyze();
    }
    ++bbNumber;

    delete ddg;
}

void
BBSchedulerController::handleCFGDDG(
    ControlFlowGraph& cfg,
    DataDependenceGraph& ddg,
    const TTAMachine::Machine& targetMachine) {
    bigDDG_ = &ddg;

    TCEString procName = cfg.procedureName();

    if (procName == "") procName = cfg.name();

    int nodeCount = cfg.nodeCount();
    for (int bbIndex = 0; bbIndex < nodeCount; ++bbIndex) {
        BasicBlockNode& bb = dynamic_cast<BasicBlockNode&>(cfg.node(bbIndex));
        if (!bb.isNormalBB())
            continue;
        if (bb.isScheduled()) {
            continue;
        }

        handleBasicBlock(
            bb.basicBlock(), targetMachine, cfg.instructionReferenceManager());
        bb.setScheduled();

        if (delaySlotFiller_ != NULL && bigDDG_ != NULL) {
            delaySlotFiller_->bbnScheduled(bb);
        }

        // if some node is removed, make sure does not skip some node and
        // then try to handle too many nodes.
        if (cfg.nodeCount() != nodeCount) {
            nodeCount = cfg.nodeCount();
            bbIndex = 0;
        }
    }
}
