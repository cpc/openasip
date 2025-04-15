/*
    Copyright (c) 2002-2020 Tampere University.

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
 * @author Pekka Jääskeläinen 2006-2011 (pjaaskel-no.spam-cs.tut.fi)
 * @author Heikki Kultala 2009 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <set>
#include <string>
#include <cstdlib>

#include "BBSchedulerController.hh"
#include "ControlFlowGraph.hh"
#include "DataDependenceGraph.hh"
#include "DataDependenceGraphBuilder.hh"
#include "SimpleResourceManager.hh"
#include "Procedure.hh"
#include "ControlUnit.hh"
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
#include "LoopPrologAndEpilogBuilder.hh"
#include "MachineConnectivityCheck.hh"
#include "InterPassData.hh"
#include "ResourceConstraintAnalyzer.hh"
#include "BasicBlockScheduler.hh"
#include "RegisterRenamer.hh"
#include "BUBasicBlockScheduler.hh"
#include "BF2Scheduler.hh"
#include "DisassemblyRegister.hh"

#include "LoopAnalyzer.hh"
#include "ScheduleEstimator.hh"

namespace TTAMachine {
    class UniversalMachine;
}

//#define DEBUG_OUTPUT
//#define CFG_SNAPSHOTS
//#define BIG_DDG_SNAPSHOTS
//#define SW_BYPASSING_STATISTICS

// getting slow with very big II's. limit it. TODO: make this cmdline param.
static const int MAXIMUM_II = 60;
static const int DEFAULT_LOWMEM_MODE_THRESHOLD = 200000;

/**
 * Constructs the basic block scheduler.
 *
 * @param data Interpass data
 * @param bypasser Helper module implementing software bypassing
 * @param delaySlotFiller Helper module implementing jump delay slot filling
 */
BBSchedulerController::BBSchedulerController(
    const TTAMachine::Machine& targetMachine, InterPassData& data,
    SoftwareBypasser* bypasser, CopyingDelaySlotFiller* delaySlotFiller,
    DataDependenceGraph* bigDDG)
    : BasicBlockPass(data),
      ControlFlowGraphPass(data),
      ProcedurePass(data),
      ProgramPass(data),
      targetMachine_(targetMachine),
      scheduledProcedure_(NULL),
      bigDDG_(bigDDG),
      softwareBypasser_(bypasser),
      delaySlotFiller_(delaySlotFiller),
      basicBlocksScheduled_(0),
      totalBasicBlocks_(0) {
    CmdLineOptions *cmdLineOptions = Application::cmdLineOptions();
    options_ = dynamic_cast<LLVMTCECmdLineOptions*>(cmdLineOptions);
}

BBSchedulerController::~BBSchedulerController() {
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
    TTAProgram::BasicBlock& bb, const TTAMachine::Machine& targetMachine,
    TTAProgram::InstructionReferenceManager& irm, BasicBlockNode* bbn) {
    // TODO: define them somewhere in one place.
    static const TCEString SP_DATUM = "STACK_POINTER";
    static const TCEString FP_DATUM = "FRAME_POINTER";
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
        BasicBlockPass::interPassData().hasDatum(FP_DATUM) &&
        BasicBlockPass::interPassData().hasDatum(RV_DATUM) &&
        BasicBlockPass::interPassData().hasDatum(RV_DATUM)) {
        rr = new RegisterRenamer(targetMachine, bbn->basicBlock());
    }

    std::vector<DDGPass*> bbSchedulers;

    if (options_ != NULL && options_->useBubbleFish2Scheduler()) {
        bbSchedulers.push_back(new BF2Scheduler(
                                   BasicBlockPass::interPassData(), rr));
    } else if (options_ != NULL && options_->useBUScheduler()) {
        bbSchedulers.push_back(new BUBasicBlockScheduler(
                BasicBlockPass::interPassData(), softwareBypasser_, rr));
    } else if (options_ != NULL && options_->useTDScheduler()) {
        bbSchedulers.push_back(new BasicBlockScheduler(
            BasicBlockPass::interPassData(), softwareBypasser_, rr));
    } else {
        bbSchedulers.push_back(new BF2Scheduler(
                                   BasicBlockPass::interPassData(), rr));
    }

    if (options_->isLoopOptDefined() &&
        cfg_->isSingleBBLoop(*bbn) && 
        bb.lastInstruction().hasJump() &&
        bigDDG_ != NULL) {

        LoopAnalyzer::LoopAnalysisResult* analysis = NULL;
        if (Application::verboseLevel() > 1) {
            Application::logStream() 
                << "CFG detects single BB loop" << std::endl
                << "tripcount: " << bb.tripCount() << std::endl;
        }
        if (bigDDG_ && bb.tripCount() == 0 && options_->useBubbleFish2Scheduler()) {
            analysis = LoopAnalyzer::analyze(*bbn, *bigDDG_);
            if (analysis != NULL) {
                bb.setTripCount(analysis->iterationCount);
                if (analysis->counterValueNode == NULL) {
                    if (Application::verboseLevel() > 1) {
                        Application::logStream() 
                            << "loop analyzis analyzed loop to have fixed trip count"
                            << analysis->iterationCount << std::endl;
                    }
                } else {
                    if (Application::verboseLevel() > 1) {
                        Application::logStream() 
                            << "loop analyzis analyzed loop to have variable trip count"
                            << analysis->counterValueNode->toString()
                            << " + "
                            << analysis->iterationCount << " divided by"
                            << analysis->counterMultiplier << std::endl;
                    }   
                }
            }
        }
        
        if ((((bb.tripCount() > 0 || (analysis && analysis->counterValueNode)) &&
              MachineConnectivityCheck::tempRegisterFiles(targetMachine).empty()) ||
             options_->useBubbleFish2Scheduler())) {
            if (analysis) {
                (static_cast<BF2Scheduler*>(bbSchedulers[0]))->
                    setLoopLimits(analysis);
            }
            // software pipeline instead of calling the flat BB scheduler
            if (Application::verboseLevel() > 1) {
                Application::logStream()
                    << "executing loop pass with trip count " << bb.tripCount()
                    << std::endl;
            }
            
            if (executeLoopPass(
                    bb, targetMachine, irm, bbSchedulers, bbn) ) {
                bbScheduled = true;
            } else {
                if (Application::verboseLevel() > 1) {
                    Application::logStream()
                        << "loop scheduler failed, using basic block "
                        << "scheduler instead" << std::endl;
                }
                bbScheduled = false;
            }
        }
        // if not scheduled yet (or loop scheduling failed)
    }
    if (!bbScheduled) {

        executeDDGPass(
            bb, targetMachine, irm, bbSchedulers, bbn);

        ++basicBlocksScheduled_;
    }
    if (rr != NULL) {
        delete rr;
    }

    // these are no longer needed. delete them to save memory.
    if (bb.liveRangeData_ != NULL) {
        bb.liveRangeData_->regFirstDefines_.clear();
        bb.liveRangeData_->regDefines_.clear();
        bb.liveRangeData_->regLastUses_.clear();
        
        bb.liveRangeData_->regDefReaches_.clear();
        bb.liveRangeData_->registersUsedAfter_.clear();
        bb.liveRangeData_->regFirstUses_.clear();
        bb.liveRangeData_->regDefines_.clear();
    } else {
        std::cerr << "Wargning: BB liverange data null for: " 
                  << bbn->toString() << std::endl;
    }
    for (unsigned int i = 0; i < bbSchedulers.size(); i++)
        delete bbSchedulers[i];
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
 * @todo remove the targetMachine argument. The machine is given in the
 * constructor and assumed to be the same for whole lifetime of the
 * scheduler instance.
 * @exception Exception In case of an error during scheduling. The exception
 *            type can be any subtype of Exception.
 */
void
BBSchedulerController::handleProcedure(
    TTAProgram::Procedure& procedure,
    const TTAMachine::Machine& targetMachine) {
    ControlFlowGraph cfg(procedure, BasicBlockPass::interPassData());

    if (Application::verboseLevel() > 0) {
        totalBasicBlocks_ = cfg.nodeCount() - 3;
        Application::logStream() 
            << " -- " << totalBasicBlocks_ << " basic blocks" << std::endl;
        basicBlocksScheduled_ = 0;
    }

#ifdef CFG_SNAPSHOTS
    cfg.writeToDotFile(procedure.name() + "_cfg.dot");
#endif

    SchedulerCmdLineOptions* opts =
        dynamic_cast<SchedulerCmdLineOptions*>(Application::cmdLineOptions());
    int lowMemThreshold = DEFAULT_LOWMEM_MODE_THRESHOLD;

    if (opts != NULL && opts->lowMemModeThreshold() > -1) {
        lowMemThreshold = opts->lowMemModeThreshold();
    }
    
    if (procedure.instructionCount() < lowMemThreshold) {
        // create the procedure-wide ddg.
        bigDDG_ = ddgBuilder().build(
            cfg, DataDependenceGraph::SINGLE_BB_LOOP_ANTIDEPS, targetMachine);

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
    }

    if (delaySlotFiller_ != NULL && bigDDG_ != NULL) {
        delaySlotFiller_->initialize(cfg, *bigDDG_, targetMachine);
    }

#ifdef BIG_DDG_SNAPSHOTS
    bigDDG_->writeToDotFile(bigDDG_->name() + "_unscheduled_ddg.dot");
#endif
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

#ifdef CFG_SNAPSHOTS
    cfg.writeToDotFile(procedure.name() + "_cfg_after.dot");
#endif

    // now all basic blocks are scheduled, let's put them back to the
    // original procedure
    cfg.copyToProcedure(procedure);
    if (bigDDG_ != NULL) {

        if (options_ != NULL && options_->dumpDDGsDot()) {
            bigDDG_->writeToDotFile(
                (boost::format("proc_%s_after_scheduling.dot") % bigDDG_->name())
                .str());
        } 

        if (options_ != NULL && options_->dumpDDGsXML()) {
            bigDDG_->writeToXMLFile(
                (boost::format("proc_%s_after_scheduling.xml") % bigDDG_->name())
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
    ControlFlowGraph& cfg, const TTAMachine::Machine& targetMachine) {
    return handleCFGDDG(cfg, bigDDG_, targetMachine);
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
    TTAProgram::Program& program, const TTAMachine::Machine& targetMachine) {
    ProgramPass::executeProcedurePass(program, targetMachine, *this);
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
bb, DataDependenceGraph::INTRA_BB_ANTIDEPS, mach,
            scheduledProcedure_->name() + '_' + 
            Conversion::toString(basicBlocksScheduled_) + ".dot");
    }
}

/**
 * Creates a DDG from the given basic block and executes the set of DDG passes 
 * for that.
 */
void
BBSchedulerController::executeDDGPass(
    TTAProgram::BasicBlock& bb, const TTAMachine::Machine& targetMachine,
    TTAProgram::InstructionReferenceManager& irm,
    std::vector<DDGPass*> ddgPasses, BasicBlockNode* bbn) {
    int minCycle = 0;
    DataDependenceGraph* ddg = NULL;
    SimpleResourceManager* rm = NULL;
    // Used for live info dumping.
    static int bbNumber = 0;
    int min = INT_MAX;
    int fastest = 0;
    if (ddgPasses.size() > 1) {
        for (unsigned int i = 0; i < ddgPasses.size(); i++) {
            ddg = createDDGFromBB(bb, targetMachine);
            rm = SimpleResourceManager::createRM(targetMachine);
            rm->setDDG(ddg);
            rm->setCFG(cfg_);
            rm->setBBN(bbn);

            int size =
                ddgPasses[i]->handleDDG(*ddg, *rm, targetMachine, minCycle, true);
            if (size < min) {
                min = size;
                fastest = i;
            }
            SimpleResourceManager::disposeRM(rm);
            delete ddg;  
            ddg = NULL;
        }
    }
    ddg = createDDGFromBB(bb, targetMachine);
    rm = SimpleResourceManager::createRM(targetMachine);
    rm->setDDG(static_cast<DataDependenceGraph*>(ddg->rootGraph()));
    rm->setCFG(cfg_);
    rm->setBBN(bbn);

    if (options_ != NULL && options_->printResourceConstraints()) {
        TCEString ddgName = ddg->name();
        // Use the BBN id if possible so it's easier to find the matching BBs when
        // looking at the if-conversion CFGs.
        if (bbn != NULL)
            ddgName << bbn->nodeID(); 
        else
            ddgName << bbNumber;
        ResourceConstraintAnalyzer rcAnalyzer(*ddg, *rm, ddgName);
        rcAnalyzer.analyzePreSchedule();
    }

    
#ifdef DDG_SNAPSHOTS
    std::string name = "scheduling";
    ddgSnapshot(ddg, name, false);
#endif

    try {
        ddgPasses[fastest]->handleDDG(*ddg, *rm, targetMachine, minCycle);
        if (bbn->isHWLoop()) {
            bbn->predecessor()->updateHWloopLength(
                ddg->largestCycle() + 1 - ddg->smallestCycle());
        }
    } catch (const Exception &e) {
        debugLog(e.errorMessageStack());
        abortWithError("Scheduling failed!");
    }

#ifdef DDG_SNAPSHOTS
    std::string name = "scheduling";
    ddgSnapshot(ddg, name, true);
#endif

    copyRMToBB(*rm, bb, targetMachine, irm);

    bbNumber++;

    if (options_ != NULL && options_->printResourceConstraints()) {
        TCEString ddgName = ddg->name();
        // Use the BBN id if possible so it's easier to find the matching BBs when
        // looking at the if-conversion CFGs.
        if (bbn != NULL)
            ddgName << bbn->nodeID(); 
        else
            ddgName << bbNumber;
        ResourceConstraintAnalyzer rcAnalyzer(*ddg, *rm, ddgName);
        rcAnalyzer.analyze();
    }
    
    if (delaySlotFiller_ != NULL && bigDDG_ != NULL) {
        rm->clearOldResources();
        delaySlotFiller_->addResourceManager(bb, *rm);
    } else {
        SimpleResourceManager::disposeRM(rm);
    }

    // print some stats about the loop kernel body DDGs
    if (Application::verboseLevel() > 1 && bb.isInInnerLoop()) {
        Application::logStream()
            << "DDG height " <<  ddg->height() << std::endl;        
    }

    delete ddg;
}

/* Returns true if node count changed */
bool BBSchedulerController::handleBBNode(
    ControlFlowGraph& cfg, BasicBlockNode& bb,
    const TTAMachine::Machine& targetMachine, int nodeCount) {

    TCEString procName = cfg.procedureName();
    if (procName == "") procName = cfg.name();

    if (!bb.isNormalBB() || bb.isScheduled()) {
        return false;
    }

    handleBasicBlock(
        bb.basicBlock(), targetMachine,
        cfg.instructionReferenceManager(), &bb);
    bb.setScheduled();

    if (delaySlotFiller_ != NULL && bigDDG_ != NULL) {
        delaySlotFiller_->bbnScheduled(bb);
    }

    // if some node is removed, make sure does not skip some node and
    // then try to handle too many nodes.
    if (cfg.nodeCount() != nodeCount) {
        return true;
    }
    return false;
}
void
BBSchedulerController::handleCFGDDG(
    ControlFlowGraph& cfg,
    DataDependenceGraph* ddg,
    const TTAMachine::Machine& targetMachine) {
    cfg_ = &cfg;
    bigDDG_ = ddg;

    ScheduleEstimator est(ProcedurePass::interPassData());
    est.handleControlFlowGraph(cfg, targetMachine);

    int nodeCount = cfg.nodeCount();

    // for handle single-BB-loops
    for (int bbIndex = 0; bbIndex < nodeCount; ++bbIndex) {
        BasicBlockNode& bb = cfg.node(bbIndex);
        if (!cfg.isSingleBBLoop(bb)) {
            continue;
        }

        if (handleBBNode(cfg, bb, targetMachine, nodeCount)) {
            nodeCount = cfg.nodeCount();
            bbIndex = 0;
        }
    }

    // then handle BBs which do not have outgoing jumps.
    for (int bbIndex = 0; bbIndex < nodeCount; ++bbIndex) {
        BasicBlockNode& bb = cfg.node(bbIndex);
        auto jumpDest = cfg.jumpSuccessor(bb);
        if (jumpDest != nullptr && jumpDest->isNormalBB()) {
            continue;
        }

        if (handleBBNode(cfg, bb, targetMachine, nodeCount)) {
            nodeCount = cfg.nodeCount();
            bbIndex = 0;
        }
    }


    for (int bbIndex = cfg.nodeCount() -1; bbIndex >= 0; --bbIndex) {
        BasicBlockNode& bb = cfg.node(bbIndex);
        auto jumpDest = cfg.jumpSuccessor(bb);
        if (jumpDest == nullptr || !jumpDest->isNormalBB()) {
            continue;
        }

        if (cfg.allScheduledInBetween(bb, *jumpDest)) {
            if (handleBBNode(cfg, bb, targetMachine, nodeCount)) {
                nodeCount = cfg.nodeCount();
                bbIndex = nodeCount -1;
            }
        }
    }

    for (int bbIndex = cfg.nodeCount() -1; bbIndex >= 0; --bbIndex) {
        BasicBlockNode& bb = cfg.node(bbIndex);
        if (handleBBNode(cfg, bb, targetMachine, nodeCount)) {
            nodeCount = cfg.nodeCount();
            bbIndex = nodeCount -1;
        }
    }
}

