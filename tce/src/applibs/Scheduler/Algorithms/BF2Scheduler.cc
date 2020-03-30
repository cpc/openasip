/*
    Copyright (c) 2002-2014 Tampere University.

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
 * @file BF2Scheduler.cc
 *
 * Definition of BF2Scheduler class.
 *
 * Bypassing Bottom-up Breadth-First-Search Instruction Scheduler
 * (BubblefishScheduler)
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BF2Scheduler.hh"
//#include "BFFinishFront.hh"
#include "BF2ScheduleFront.hh"

#include "DataDependenceGraph.hh"
#include "SimpleResourceManager.hh"
#include "FunctionUnit.hh"
#include "Unit.hh"
#include "Machine.hh"
#include "Terminal.hh"
#include "TerminalRegister.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "MoveNodeSet.hh"
#include "Operation.hh"
#include "Move.hh"
#include "ControlUnit.hh"
#include "BUMoveNodeSelector.hh"
#include "LLVMTCECmdLineOptions.hh"
#include "MachineConnectivityCheck.hh"
#include "InterPassData.hh"
#include "RegisterCopyAdder.hh"
#include "MoveGuard.hh"
#include "DisassemblyRegister.hh"
#include "BasicBlockNode.hh"
#include "BasicBlock.hh"
#include "BasicBlockScheduler.hh"
#include "UnboundedRegisterFile.hh"
#include "RegisterRenamer.hh"
#include "MapTools.hh"
#include "BFScheduleBU.hh"
#include "ProgramAnnotation.hh"
#include "MoveNodeDuplicator.hh"
#include "BFSwapOperands.hh"
#include "BFShareOperand.hh"
#include "BFSchedulePreLoopShared.hh"
#include "BFPostpassBypasser.hh"

//#define DEBUG_PRE_SHARE
//#define DEBUG_BUBBLEFISH_SCHEDULER

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
#define DEBUG_PRE_SHARE
#endif

//#define DEBUG_PRE_SHARE

//#define ENABLE_RENAMING
//define ENABLE_DOT_SPAM

#define REMOVE_LOOP_CHECKS_WITH_LOOPBUFFER
#define ENABLE_PRE_LOOP_SHARING

BF2Scheduler::BF2Scheduler(
    InterPassData& ipd, RegisterRenamer* renamer) :
    DDGPass(ipd), ddg_(NULL), prologDDG_(nullptr), rm_(NULL),
    prologRM_(nullptr), latestCycle_(INT_MAX/1024),
    loopSchedulingMode_(PROLOG_RM_GUARD_ALAP),
    renamer_(renamer),
    killDeadResults_(true),
    jumpNode_(NULL),
    duplicator_(NULL) {
    options_ =
        dynamic_cast<LLVMTCECmdLineOptions*>(Application::cmdLineOptions());
    if (options_ != NULL) {
        killDeadResults_ = options_->killDeadResults();
    }

}

BF2Scheduler::BF2Scheduler(
    InterPassData& ipd, RegisterRenamer* renamer, bool killDeadResults) :
    DDGPass(ipd), ddg_(NULL), rm_(NULL),
    latestCycle_(INT_MAX/1024),
    loopSchedulingMode_(PROLOG_RM_GUARD_ALAP),
    renamer_(renamer),
    killDeadResults_(killDeadResults),
    jumpNode_(NULL),
    duplicator_(NULL) {
    options_ =
        dynamic_cast<LLVMTCECmdLineOptions*>(Application::cmdLineOptions());
}

TTAMachine::Unit* BF2Scheduler::getDstUnit(MoveNode& mn) {
    if (mn.isDestinationOperation()) {
        ProgramOperation& po = mn.destinationOperation();
        for (int i = 0; i < po.inputMoveCount(); i++) {
            MoveNode& inputNode = po.inputMove(i);
            assert( inputNode.isDestinationOperation());
            if (inputNode.isScheduled()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\tFound scheduled input: "
                          << inputNode.toString() << std::endl;
#endif
                    TTAProgram::Terminal& term =
                        inputNode.move().destination();
                assert(term.isFUPort());
                return term.port().parentUnit();
            }
        }
    }
    return NULL;
}

void BF2Scheduler::scheduleDDG(
    DataDependenceGraph& ddg,
    SimpleResourceManager& rm,
    const TTAMachine::Machine& targetMachine) {

    jumpGuardWrite_ = NULL;
    loopSchedulingMode_ = NO_LOOP_SCHEDULER;

    BFOptimization::clearPrologMoves();
    prologRM_ = NULL;
    preLoopSharedOperands_.clear();

    // scheduling pipeline resources after last cycle may cause problems.
    // make RM to check for those
    latestCycle_ = (INT_MAX/1024);
    rm.setMaxCycle(latestCycle_);

    targetMachine_ = &targetMachine;
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << std::endl << "Handling new ddg: " << ddg.name() << std::endl;
#endif
    ddg_ = &ddg;
    ddg_->setMachine(targetMachine);
    if (duplicator_ != NULL) {
        delete duplicator_; duplicator_ = NULL;
    }

    BUMoveNodeSelector selector(ddg, targetMachine);
    selector_ = &selector;
    if (renamer_ != NULL) {
        renamer_->initialize(ddg);
        renamer_->setSelector(&selector);
    }

    rm_ = &rm;

    if (options_ != NULL && options_->dumpDDGsDot()) {
        ddg_->writeToDotFile(
            (boost::format("bb_%s_before_scheduling.dot") %
             ddg_->name()).str());
    }

    MoveNodeGroup moves = selector.candidates();
    while (moves.nodeCount() > 0) {
        MoveNode* mn = NULL;
        for (int i = 0; i < moves.nodeCount(); i++) {
            if (!moves.node(i).isScheduled()) {
                if (isDeadResult(moves.node(i))) {
                    if (ddg_->hasNode(moves.node(i))) {
                        ddg_->dropNode(moves.node(i));
                    }
                } else {
                    mn = &moves.node(i);
                }
            }
        }
        if (mn == NULL) {
            moves = selector.candidates();
            continue;
        }

        if (!scheduleFrontFromMove(*mn)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "Scheduling of front failed! Inducing move: "
                      << mn->toString() << std::endl;
#endif
            ddg_->writeToDotFile(
                (boost::format("%s_failed_ddg.dot") %
                 ddg_->name()).str());
            throw CompileError(__FILE__, __LINE__, __func__,
                               "Bubblefish scheduler failed"
                               "retry count exceeded. Propably broken ADF");
        }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "Whole op scheduled ok? original MN: "
                  << mn->toString() << std::endl;
#endif
        moves = selector.candidates();
    }

    if (ddg_->scheduledNodeCount() != ddg_->nodeCount()) {
        ddg_->writeToDotFile(
                (boost::format("%s_unscheduled_nodes_in_ddg.dot") %
                 ddg_->name()).str());

        assert(false && "unscheduled nodes in ddg after scheduler");
    }

    if (options_ != NULL && options_->dumpDDGsDot()) {
        ddg_->writeToDotFile(
            (boost::format("bb_%s_after_scheduler_ddg.dot") %
             ddg_->name()).str());
    }

    if (options_ != NULL && options_->dumpDDGsXML()) {
        ddg_->writeToXMLFile(
            (boost::format("bb_%s_after_scheduler_ddg.dot") %
             ddg_->name()).str());
    }
}

int
BF2Scheduler::handleDDG(
    DataDependenceGraph& ddg, SimpleResourceManager& rm,
    const TTAMachine::Machine& targetMachine, bool testOnly) {

    scheduleDDG(ddg, rm, targetMachine);

    int len = rm_->largestCycle() - rm_->smallestCycle()+1;

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "Handled ddg: " <<ddg_->name() << std::endl;
#endif

    if (testOnly) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        ddg_->writeToDotFile("tested_schedule.dot");
#endif
        unschedule();
    } else {
        finalizeSchedule();
    }
    if (duplicator_ != NULL) {
        delete duplicator_; duplicator_ = NULL;
    }

    return len;
}

int BF2Scheduler::handleLoopDDG(
    BUMoveNodeSelector& selector, bool allowPreLoopOpshare) {
    if (prologRM_ != NULL) {

        if (allowPreLoopOpshare) {
            allocateFunctionUnits();
            reservePreallocatedFUs();
        }  else {
            // TODO: should these be undone, not just cleared?
            preSharedOperandPorts_.clear();
            preLoopSharedOperands_.clear();
        }

        duplicator_ = new MoveNodeDuplicator(*ddg_, *prologDDG_);
        duplicator_->setBBN(ddg_->getBasicBlockNode(ddg_->node(0)));
    } else {
        preSharedOperandPorts_.clear();
        preLoopSharedOperands_.clear();
    }

    MoveNodeGroup moves = selector.candidates();
    while (moves.nodeCount() > 0) {
        MoveNode* mn = NULL;
        for (int i = 0; i < moves.nodeCount(); i++) {
            if (!moves.node(i).isScheduled()) {
                if (isDeadResult(moves.node(i))) {
                    if (ddg_->hasNode(moves.node(i))) {
                        ddg_->dropNode(moves.node(i));
                    }
                } else {
                    mn = &moves.node(i);
                }
            }
        }
        if (mn == NULL) {
            moves = selector.candidates();
            continue;
        }

        if (!scheduleFrontFromMove(*mn)) {
#ifndef DEBUG_BUBBLEFISH_SCHEDULER
            if (options_ != NULL && options_->dumpDDGsDot()) {
#endif
                ddg_->writeToDotFile(
                    std::string("ii_fail_") +
                    Conversion::toString(rm_->initiationInterval()) +
                    "icount_" +
                    Conversion::toString(tripCount_) +
                    std::string("ops_") +
                    Conversion::toString(allowPreLoopOpshare) +
                    std::string("_dag.dot"));
#ifndef DEBUG_BUBBLEFISH_SCHEDULER
            }
#endif
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << std::endl << std::endl
                      << "Unscheduling all due scheduling failed at around: "
                      << mn->toString() << std::endl << std::endl;
#endif
            unschedule();
            if (prologRM_ != NULL) {
                preSharedOperandPorts_.clear();
                preLoopSharedOperands_.clear();
                unreservePreallocatedFUs();
            }
            if (duplicator_ != NULL) {
                delete duplicator_; duplicator_ = NULL;
            }
            return -1;
        }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "Whole op scheduled ok? original MN: "
                  << mn->toString() << std::endl;
#endif
        moves = selector.candidates();
    }

    // Try to schedule pre-loop operand shared moves. if fail, abort.
    if (prologRM_ && allowPreLoopOpshare) {
        if (!schedulePreLoopOperandShares()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "Scheduling pre-loop opshares fail, undoing all"
                      << std::endl;
#endif
            unschedule();
            if (prologRM_ != NULL) {
                preSharedOperandPorts_.clear();
                preLoopSharedOperands_.clear();
                unreservePreallocatedFUs();
            }
            if (Application::verboseLevel() > 1) {
                std::cerr << "Scheduling pre-loop operand shares failed."
                          << std::endl;
            }
            if (duplicator_ != NULL) {
                delete duplicator_; duplicator_ = NULL;
            }
            return -1;
        }
    }

    //postpass-bypass.
    auto postBypass = new BFPostpassBypasser(*this);
    if ((*postBypass)()) {
        scheduledStack_.push_back(postBypass);
    } else {
        delete postBypass;
    }

    int overlapCount =
        (latestCycle_ - rm_->smallestCycle()) / rm_->initiationInterval();
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "inner handleLoopDDG exiting, overlapcount: "
              << overlapCount << std::endl;
#endif
    return overlapCount;
}


void BF2Scheduler::checkPrologGuardsAllowed() {
    // cannot use predication if same predicate reg use for other things
    if (guardPrologMoves()) {
        auto inEdges = ddg_->inEdges(*jumpGuardWrite_);
        for (auto e : inEdges) {
            if (e->isWAW() && !e->isBackEdge()) {
#ifdef DEBUG_LOOP_SCHEDULER
                std::cerr << "Multiple writes to the guard reg "
                          << "forbid predication!" << std::endl;
#endif
                loopSchedulingMode_ =
                    (LoopSchedulingMode)(loopSchedulingMode_ &
                                         (~USE_PREDICATION_FOR_PROLOG_MOVES));
            }
        }
    }
}


BF2Scheduler::LoopSchedulingMode BF2Scheduler::selectLoopSchedulingMode() {

    if (prologRM_ == NULL) {
        return EXTERNAL_BUILDER_WITH_EPILOG;
    }
    return NO_LOOPBUF_PREDICATE_ALAP;
}

int
BF2Scheduler::handleLoopDDG(
    DataDependenceGraph& ddg, SimpleResourceManager& rm,
    const TTAMachine::Machine& targetMachine, int tripCount,
    SimpleResourceManager* prologRM, bool testOnly) {
#ifndef DEBUG_BUBBLEFISH_SCHEDULER
    if (options_ != NULL && options_->dumpDDGsDot()) {
#endif
        ddg.writeToDotFile(
            std::string("ii_begin_") +
            Conversion::toString(rm.initiationInterval()) +
            std::string("_dag.dot"));
#ifndef DEBUG_BUBBLEFISH_SCHEDULER
    }
#endif

    rm.setDDG(&ddg);
    BFOptimization::clearPrologMoves();
#ifdef ENABLE_DOT_SPAM
    ddg.writeToDotFile(
        std::string("before_loop_ddg.dot"));
#endif

    targetMachine_ = &targetMachine;
    tripCount_ = tripCount;
    rm_ = &rm;
    prologRM_ = prologRM;
    loopSchedulingMode_ = selectLoopSchedulingMode();

    // scheduling pipeline resources after last cycle may cause problems.
    // make RM to check for those
    latestCycle_ = 2*rm.initiationInterval()-1;
    rm.setMaxCycle(latestCycle_);

    if (Application::verboseLevel() > 1) {
        std::cerr << std::endl << "Handling new loop ddg: " << ddg.name()
                  << std::endl;
    }
    ddg_ = &ddg;
    ddg_->setMachine(targetMachine);

    if (duplicator_ != NULL) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "Duplicator not null in handleloopddg,"
            " deleting duplicator" << std::endl;
#endif
        delete duplicator_; duplicator_ = NULL;
    }

    if (!findJump()) {
        return -1;
    }

    jumpGuardWrite_ = ddg_->onlyGuardDefOfMove(*jumpNode_);
    if (jumpGuardWrite_ == NULL) {
        return -1;
    }

    // disable guarding 1st part moves if same predicate reg used elsewhere
    checkPrologGuardsAllowed();

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "jumpguard write node is: "
              << jumpGuardWrite_->toString() << std::endl;
#endif

    if (prologRM != NULL) {
        DataDependenceGraph::NodeSet empty;
        prologDDG_ = static_cast<DataDependenceGraph*>(
            ddg_->parentGraph())->createSubgraph(empty);
        prologDDG_->setMachine(targetMachine);
        // TODO: this is kinda incorrect. should be prolog
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "prolog RM not null, pre-allocating FUs:"
                  << prologRM << std::endl;
#endif
    }

#ifndef DEBUG_BUBBLEFISH_SCHEDULER
    if (options_ != NULL && options_->dumpDDGsDot()) {
#endif
        ddg_->writeToDotFile(
            (boost::format("bb_%s_ii_%d_before_scheduling.dot") %
             ddg_->name() % rm.initiationInterval()).str());
#ifndef DEBUG_BUBBLEFISH_SCHEDULER
    }
#endif
    BUMoveNodeSelector selector(ddg, targetMachine);
    selector_ = &selector;
    if (renamer_ != NULL) {
        renamer_->setSelector(&selector);
    }

    int overlapCount = handleLoopDDG(selector, true);
    if (overlapCount == -1) {
        if (Application::verboseLevel() > 1) {
            std::cerr << "Loop Sched. fail with pre-loop opshare on with II: "
                      << rm.initiationInterval() << std::endl;
        }
        selector.initializeReadylist();
        overlapCount = handleLoopDDG(selector, false);
        if (overlapCount == -1) {
            if (Application::verboseLevel() > 1) {
                std::cerr << "Loop Sched. fail without pre-loop opshare, II: "
                          << rm.initiationInterval() << std::endl;
            }
            if (duplicator_ != NULL) {
                delete duplicator_; duplicator_ = NULL;
            }
            return -1;
        } else {
            if (Application::verboseLevel() > 1) {
                std::cerr << "Loop Sched. ok without pre-loop opshare, II: "
                          << rm.initiationInterval() << std::endl;
            }
        }
    }

    if (ddg_->scheduledNodeCount() != ddg_->nodeCount()) {
        ddg_->writeToDotFile(
            (boost::format("%s_unscheduled_nodes_in_ddg.dot") %
             ddg_->name()).str());

        assert(false && "unscheduled nodes in ddg after scheduler");
    }

    // loop schedulign did not help.
    if (testOnly) {
        if (overlapCount == 0 && Application::verboseLevel() > 1) {
            Application::logStream()
                << "No overlapping instructions, "
                << "Should decrease II"
                << std::endl;
        }
        // this have to be calculated before unscheduling.
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "Unscheduling all due loop sched too slow or testonly"
                  << std::endl;
#endif

#ifndef DEBUG_BUBBLEFISH_SCHEDULER
        if (options_ != NULL && options_->dumpDDGsDot()) {
#endif
            ddg_->writeToDotFile(
                std::string("ii_test_or_slow_") +
                Conversion::toString(rm.initiationInterval()) +
                std::string("_dag.dot"));
#ifndef DEBUG_BUBBLEFISH_SCHEDULER
    }
#endif

        unschedule();

#ifndef DEBUG_BUBBLEFISH_SCHEDULER
        if (options_ != NULL && options_->dumpDDGsDot()) {
#endif
            ddg_->writeToDotFile(
                std::string("ii_unscheduled_slow") +
                Conversion::toString(rm.initiationInterval()) +
                std::string("_dag.dot"));
#ifndef DEBUG_BUBBLEFISH_SCHEDULER
        }
#endif

        if (prologRM_ != NULL) {
            preSharedOperandPorts_.clear();
            preLoopSharedOperands_.clear();
            unreservePreallocatedFUs();
        }
        if (duplicator_ != NULL) {
            delete duplicator_; duplicator_ = NULL;
        }
        return overlapCount;
    }

    if (tripCount && overlapCount >= tripCount) {
#ifndef DEBUG_BUBBLEFISH_SCHEDULER
        if (options_ != NULL && options_->dumpDDGsDot()) {
#endif
            ddg_->writeToDotFile(
                std::string("ii_no_overlap") +
                Conversion::toString(rm.initiationInterval()) +
                std::string("_dag.dot"));
#ifndef DEBUG_BUBBLEFISH_SCHEDULER
        }
#endif
        unschedule();

#ifndef DEBUG_BUBBLEFISH_SCHEDULER
        if (options_ != NULL && options_->dumpDDGsDot()) {
#endif
            ddg_->writeToDotFile(
                std::string("ii_unscheduled_no_overlap") +
                Conversion::toString(rm.initiationInterval()) +
                std::string("_dag.dot"));
#ifndef DEBUG_BUBBLEFISH_SCHEDULER
        }
#endif
        if (prologRM_ != NULL) {
            preSharedOperandPorts_.clear();
            preLoopSharedOperands_.clear();
            unreservePreallocatedFUs();
        }

        if (duplicator_ != NULL) {
            delete duplicator_; duplicator_ = NULL;
        }
        return -1;
    }


    if (options_ != NULL && options_->dumpDDGsDot()) {
        ddg_->writeToDotFile(
            (boost::format("bb_%s_after_scheduling.dot") %
             ddg_->name()).str());
    }

    if (options_ != NULL && options_->dumpDDGsXML()) {
        ddg_->writeToXMLFile(
            (boost::format("bb_%s_after_scheduling.dot") %
             ddg_->name()).str());
    }

    finalizeSchedule();

    if (Application::verboseLevel() > 1) {
        std::cerr << "Handled loop ddg: " <<ddg_->name() << std::endl;
    }

#ifndef DEBUG_BUBBLEFISH_SCHEDULER
    if (options_ != NULL && options_->dumpDDGsDot()) {
#endif
        ddg_->writeToDotFile(
            std::string("ii_ok_") +
            Conversion::toString(rm.initiationInterval()) +
            std::string("iters_") +
            Conversion::toString(tripCount) +
            std::string("_dag.dot"));
#ifndef DEBUG_BUBBLEFISH_SCHEDULER
    }
#endif
    return overlapCount;
}

void BF2Scheduler::finalizeSchedule() {
    for (auto m: dreRemovedMoves_) {
        if (m->isScheduled()) {
            std::cerr << "cannot kill scheduled move: "
                      << m->toString() << std::endl;
            assert(false);
        }
        if (prologRM_) {
            MoveNode *prologMN = duplicator().getMoveNode(*m);
            if (prologMN) {
                if (prologMN->isScheduled()) {
                    std::cerr << "prolog MN: " << prologMN->toString()
                              << "of MN: " << m->toString()
                              << "  is scheduled!" << std::endl;
                    assert(false);
                }

                ddg_->rootGraph()->removeNode(*prologMN);
            }
        }
        ddg_->rootGraph()->removeNode(*m);
    }
    for (auto m: removedMoves_) {
        assert(!m->isScheduled());
        ddg_->rootGraph()->removeNode(*m);
    }

    // remove undo information, as cannot be undoed after this
    while(!scheduledStack_.empty()) {
        BFOptimization* bfo = scheduledStack_.back();
        scheduledStack_.pop_back();
        delete bfo;
    }
}


#ifdef ENABLE_DOT_SPAM
void BF2Scheduler::writeDotWithNameAndNodeID(
    DataDependenceGraph& ddg,
    const TCEString& namePrefix, const MoveNode& mn) {

    TCEString dotName;
    int counter = 0;
    do {
        dotName = namePrefix;
        dotName << "_" << mn.nodeID();
        dotName << "_" << counter << ".dot";
        counter++;
    } while (exists(dotName.c_str()));
    std::cerr << "\t\t\t\t\tDumping ddg: " << dotName << " mn: "
              << mn.toString() << std::endl;
    ddg.writeToDotFile(dotName);

}
#else
void BF2Scheduler::writeDotWithNameAndNodeID(
    DataDependenceGraph&, const TCEString&, const MoveNode&) {
}
#endif



void BF2Scheduler::revertBBLiveRangeBookkeepingForDestination(MoveNode* mn) {
    BasicBlockNode& bbn = ddg_->getBasicBlockNode(*mn);
    TTAProgram::Terminal& dest = mn->move().destination();
    if (!dest.isGPR()) {
        ddg().writeToDotFile("cannot_revert.dot");
        std::cerr << "Cannot revert bb live range bookkeeping as "
                  << " dest not gpr: " << mn->toString() << std::endl;
        std::cerr << "This might be caused by broken connectivity in the ADF."
                  << std::endl;

    }
    assert(dest.isGPR());
    int index = dest.index();
    const TTAMachine::RegisterFile& rf = dest.registerFile();
    TCEString reg = DisassemblyRegister::registerName(rf, index);

    eraseFromMoveNodeUseSet(
        bbn.basicBlock().liveRangeData_->regDefines_, reg, mn);

    eraseFromMoveNodeUseSet(
        bbn.basicBlock().liveRangeData_->regFirstDefines_, reg, mn);
}

void BF2Scheduler::revertBBLiveRangeBookkeepingForSource(MoveNode* mn) {
    BasicBlockNode& bbn = ddg_->getBasicBlockNode(*mn);
    TTAProgram::Terminal& src = mn->move().source();
    if (!src.isGPR()) {
        ddg().writeToDotFile("cannot_revert.dot");
        std::cerr << "Cannot revert bb live range bookkeeping as "
                  << " src not gpr: " << mn->toString() << std::endl;
        std::cerr << "This might be caused by broken connectivity in the ADF."
                  << std::endl;
    }
    assert(src.isGPR());
    int index = src.index();
    const TTAMachine::RegisterFile& rf = src.registerFile();
    TCEString reg = DisassemblyRegister::registerName(rf, index);

    eraseFromMoveNodeUseSet(
        bbn.basicBlock().liveRangeData_->regLastUses_, reg, mn);

    eraseFromMoveNodeUseSet(
        bbn.basicBlock().liveRangeData_->regFirstUses_, reg, mn);
}

void
BF2Scheduler::eraseFromMoveNodeUseSet(
    LiveRangeData::MoveNodeUseMapSet& mnuMap,
    const TCEString& reg, MoveNode* mn) {
    LiveRangeData::MoveNodeUseMapSet::iterator s = mnuMap.find(reg);
    if (s != mnuMap.end()) {
        LiveRangeData::MoveNodeUseSet& mnuSet = s->second;
        LiveRangeData::MoveNodeUseSet::iterator i =
            mnuSet.find(MoveNodeUse(*mn));
        if (i!= mnuSet.end()) {
            mnuSet.erase(i);
        }
    }
}


bool
BF2Scheduler::isSourceUniversalReg(const MoveNode& mn) {
    if (!mn.isSourceVariable()) {
        return false;
    }
    return mn.move().source().isUniversalMachineRegister();
}

bool
BF2Scheduler::isDestinationUniversalReg(const MoveNode& mn) {
    if (!mn.isDestinationVariable()) {
        return false;
    }
    return mn.move().destination().isUniversalMachineRegister();
}

void BF2Scheduler::unschedule() {
    while(!scheduledStack_.empty()) {
        BFOptimization* bfo = scheduledStack_.back();
        scheduledStack_.pop_back();
        currentFront_ = dynamic_cast<BF2ScheduleFront*>(bfo);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "unscheduling front @ " << bfo
                  << " in unschedule" << std::endl;
#endif
        bfo->undo();
        currentFront_ = NULL;
        delete bfo;
    }
}




bool BF2Scheduler::isDeadResult(MoveNode& mn) const {
    return dreRemovedMoves_.find(&mn) != dreRemovedMoves_.end() ||
        removedMoves_.find(&mn) != removedMoves_.end();
}

void BF2Scheduler::nodeAndCopyKilled(MoveNode& mn) {
    dreRemovedMoves_.insert(&mn);
}

void BF2Scheduler::nodeKilled(MoveNode& mn) {
    removedMoves_.insert(&mn);
}

void BF2Scheduler::nodeResurrected(MoveNode& mn) {
    removedMoves_.erase(&mn);
    dreRemovedMoves_.erase(&mn);
}

/**
 * Checks whether given movenode is a trigger in given FU
 */
bool BF2Scheduler::isTrigger(const TTAMachine::Unit& unit, MoveNode& mn) {
    const TTAMachine::FunctionUnit& fu =
        dynamic_cast<const TTAMachine::FunctionUnit&>(unit);

    TTAProgram::Terminal& term = mn.move().destination();
    int operandNum =  term.operationIndex();

    const Operation& op = mn.destinationOperation().operation();

    const TTAMachine::HWOperation* hwop =
        fu.operation(op.name());

    const TTAMachine::FUPort* port = hwop->port(operandNum);
    return port->isTriggering();
}

/**
 * Finds the source where to bypass from.
 */
DataDependenceEdge* BF2Scheduler::findBypassEdge(const MoveNode& mn) {

    DataDependenceGraph::EdgeSet edges = ddg_->inEdges(mn);
    DataDependenceGraph::EdgeSet::iterator edgeIter = edges.begin();
    DataDependenceEdge* bypassEdge = NULL;

    // find one incoming raw edge. if multiple, cannot bypass.
    while (edgeIter != edges.end()) {

        DataDependenceEdge& edge = *(*edgeIter);
        // if the edge is not a real reg/ra raw edge, skip to next edge
        if (edge.edgeReason() != DataDependenceEdge::EDGE_REGISTER ||
            edge.dependenceType() != DataDependenceEdge::DEP_RAW ||
            edge.guardUse() || edge.headPseudo()) {
            edgeIter++;
            continue;
        }

        if (bypassEdge == NULL) {
            bypassEdge = &edge;
        } else {
            // cannot bypass if multiple inputs
            return NULL;
        }
        edgeIter++;
    }

    // if no bypassable edge found, cannot bypass
    if (bypassEdge == NULL) {
        return 0;
    }

    if (bypassEdge->isBackEdge()) {
       if (!rm().initiationInterval()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tbackedge without without loop sched!!"
                      << " not allowed." << std::endl;
#endif
            return NULL;
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        } else {
            std::cerr << "\t\tback edge bypass ok in loop" << std::endl;
#endif
        }
    }
    return bypassEdge;
}


std::string BF2Scheduler::shortDescription() const {
    return "BubbleFish instruction Scheduler";
}


/**
 * Find possible temp reg RF's for connectivity of given register.
 *
 * This only gives the register files that for the "next register in the
 * temp reg chain", not the whole chain
 */
std::set<const TTAMachine::RegisterFile*, TTAMachine::MachinePart::Comparator>
BF2Scheduler::possibleTempRegRFs(
    const MoveNode& mn, bool tempRegAfter,
    const TTAMachine::RegisterFile* forbiddenRF) {

    std::set<const TTAMachine::RegisterFile*,
        TTAMachine::MachinePart::Comparator>
        result;

    std::map<int, int> rfDistanceFromSource;
    std::map<int, int> rfDistanceFromDestination;

    typedef SimpleInterPassDatum<
    std::vector<std::pair<const TTAMachine::RegisterFile*, int> > >
        TempRegData;

    std::string srDatumName = "SCRATCH_REGISTERS";
    if (!DDGPass::interPassData().hasDatum(srDatumName) ||
        (dynamic_cast<TempRegData&>(
             DDGPass::interPassData().datum(srDatumName))).size() == 0) {
        TCEString msg("No scratch registers available for "
                      "temporary moves for move: ");
        msg << mn.toString();
        throw IllegalProgram(__FILE__, __LINE__, __func__, msg);
    }
    const TempRegData& tempRegs =
        dynamic_cast<TempRegData&>(
            DDGPass::interPassData().datum(srDatumName));


    for (unsigned int i = 0; i < tempRegs.size(); i++) {
        rfDistanceFromSource[i] = INT_MAX;
        rfDistanceFromDestination[i] = INT_MAX;
    }


    for (unsigned int i = 0; i < tempRegs.size(); i++) {
        const TTAMachine::RegisterFile* rf = tempRegs[i].first;
        MachineConnectivityCheck::PortSet readPorts =
            MachineConnectivityCheck::findReadPorts(*rf);
        MachineConnectivityCheck::PortSet writePorts =
            MachineConnectivityCheck::findWritePorts(*rf);
        bool srcOk = false;

        if (rf == forbiddenRF) {
            continue;
        }

        if (MachineConnectivityCheck::canSourceWriteToAnyDestinationPort(
                mn, writePorts)) {
            rfDistanceFromSource[i] = 1;
            srcOk = true;
        }

        if (MachineConnectivityCheck::canAnyPortWriteToDestination(
                readPorts, mn)) {
            rfDistanceFromDestination[i] = 1;
            if (srcOk) {
                // this RF does it!
                result.insert(rf);
            }
        }
    }

    // modified check to avoid 4ever loop in case of broken machine
    bool modified = true;
    if (!tempRegAfter) {
    while (result.empty() && modified) {
        int shortest = INT_MAX;
        modified = false;
        for (unsigned int i = 0; i < tempRegs.size(); i++) {
            int srcDist = rfDistanceFromSource[i];
            if (srcDist != INT_MAX) {
                const TTAMachine::RegisterFile* rfSrc = tempRegs[i].first;
                for (unsigned int j = 0; j < tempRegs.size(); j++) {
                    if (rfDistanceFromSource[j] > srcDist + 1) {
                        const TTAMachine::RegisterFile* rfDest =
                            tempRegs[j].first;
                        // ignore rf's which are not wide enough
                        if (MachineConnectivityCheck::isConnected(
                                *rfSrc, *rfDest,
                                (mn.move().isUnconditional() ? NULL :
                                 &mn.move().guard().guard()))) {
                            rfDistanceFromSource[j] = srcDist + 1;
                            modified = true;
                            if (rfDistanceFromDestination[j] == 1) {
                                int dist = srcDist + 2;
                                if (dist < shortest) {
                                    result.clear();
                                    shortest = dist;
                                }
                                if (dist == shortest) {
                                    result.insert(rfDest);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
    } else {
        while (result.empty() && modified) {
        int shortest = INT_MAX;
        modified = false;
        for (unsigned int i = 0; i < tempRegs.size(); i++) {
            int dstDist = rfDistanceFromDestination[i];
            if (dstDist != INT_MAX) {
                const TTAMachine::RegisterFile* rfDst = tempRegs[i].first;
                for (unsigned int j = 0; j < tempRegs.size(); j++) {
                    if (rfDistanceFromDestination[j] > dstDist + 1) {
                        const TTAMachine::RegisterFile* rfSrc =
                            tempRegs[j].first;
                        if (MachineConnectivityCheck::isConnected(
                                *rfDst, *rfSrc,
                                (mn.move().isUnconditional() ? NULL :
                                 &mn.move().guard().guard()))) {
                            rfDistanceFromDestination[j] = dstDist + 1;
                            modified = true;
                            if (rfDistanceFromSource[j] == 1) {
                                int dst = dstDist + 2;
                                if (dst < shortest) {
                                    result.clear();
                                    shortest = dst;
                                }
                                if (dst == shortest) {
                                    result.insert(rfSrc);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
    }
}

int BF2Scheduler::scheduleFrontFromMove(MoveNode& mn) {
    BF2ScheduleFront* bfsf = new BF2ScheduleFront(*this, mn, latestCycle_);
    currentFront_ = bfsf;
    if ((*bfsf)()) {
        scheduledStack_.push_back(bfsf);
        currentFront_ = NULL;
        return true;
    } else {
        delete bfsf;
        currentFront_ = NULL;
        return false;
    }
}



bool BF2Scheduler::hasUnscheduledSuccessors(MoveNode& mn) const {

    DataDependenceGraph::NodeSet succ = ddg_->successors(mn, true);
    for (auto m: succ) {
        if (!m->isScheduled()) {
            if (!isDeadResult(*m) && m != &mn) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
		std::cerr << "\t\t\tunsched succ: " << m->toString()
			  << std::endl;
#endif
                return true;
            }
        }
    }
    return false;
}

/**
 * Finds the jump from the bb.
 *
 * @return if found a guarded jump.
 */
bool BF2Scheduler::findJump() {
    for (int i = 0; i < ddg_->nodeCount(); i++) {
        MoveNode& mn = ddg_->node(i);
        if (mn.isMove() && mn.move().isJump()) {
            jumpNode_ = &mn;
            return !mn.move().isUnconditional();
        }
    }
    return false;
}

TTAProgram::MoveGuard* BF2Scheduler::jumpGuard() {
    if (jumpNode_->move().isUnconditional()) {
        std::cerr << "jump is unconditional: " << jumpNode_->toString()
                  << std::endl;
    }
    assert (!jumpNode_->move().isUnconditional());
    return &jumpNode_->move().guard();
}

/**
 * @return new operand index of the operand. 0 if not swapped.
 */
int BF2Scheduler::swapToUntrigger(
    ProgramOperationPtr po, const Operation& op,
    int operandIndex, MoveNode& trig) {
    for (int i = 1; i <= op.numberOfInputs(); i++) {
        if (i == operandIndex) continue;
        if (op.canSwap(i, operandIndex)) {
            MoveNodeSet& inputNodeSet = po->inputNode(i);
            if (inputNodeSet.count() == 1) {
                BFSwapOperands* swapper =
                    new BFSwapOperands(*this, po, trig,
                                       inputNodeSet.at(0));
                if ((*swapper)()) {
                    scheduledStack_.push_back(swapper);
                    return i;
                    break;
                } else {
                    delete swapper;
                }
            }
        }
    }
    return 0;
}

bool BF2Scheduler::mustBeTrigger(
    const MoveNode& mn, const ProgramOperation& po) {
    const Operation& op = po.operation();
    int inputCount = op.numberOfInputs();
    if (inputCount < 2) {
        return true;
    }
    MoveNode* trig =
        BasicBlockScheduler::findTrigger(po, *targetMachine_);
    if (trig != &mn) {
        return false;
    }
    int myIdx = mn.move().destination().operationIndex();
    for (int i = 1; i <= inputCount; i++) {
        if (i != myIdx && op.canSwap(myIdx, i)) {
            return false;
        }
    }
    return true;
}

/**
 * Revert last optimization from the optimization stack.
 */
void BF2Scheduler::revertTopOpt() {
    assert(!scheduledStack_.empty());
    BFOptimization* bfo = scheduledStack_.back();
    scheduledStack_.pop_back();
    bfo->undo();
    delete bfo;
}

void BF2Scheduler::countLoopInvariantValueUsages() {
    std::map<TCEString, int> invariantCounts;
    std::map<TCEString, int> variableCounts;

    invariants_.clear();
    invariantsOfCount_.clear();

    static int iaCounter= 0;
    for (int i = 0; i < ddg().programOperationCount(); i++) {
        ProgramOperation& po = ddg().programOperation(i);
        const Operation& op = po.operation();
        if (op.numberOfInputs() == 1) {
            continue; // must be trigger
        }
        for (int k = 1; k <= op.numberOfInputs(); k++) {
            MoveNodeSet& inputNodeSet = po.inputNode(k);
            assert(inputNodeSet.count() == 1);
            MoveNode& inputNode = inputNodeSet.at(0);
            TCEString inputVal;
            if (mustBeTrigger(inputNode, po)) {
                continue;
            }

            if (inputNode.isSourceVariable()) {
                inputVal = DisassemblyRegister::registerName(
                    inputNode.move().source());
            } else {
                if (inputNode.isSourceConstant()) {
                    if (inputNode.move().source().isInstructionAddress()) {
                        inputVal << "IADDR" << iaCounter++;
                    } else {
                        // todo: imms longer than 32 bits?
                        inputVal <<
                            inputNode.move().source().value().intValue();
                    }
                } else {
                    // unknown?
                    return;
                }
            }
            if (ddg().isLoopInvariant(inputNode)) {
                invariantCounts[inputVal]++;
                invariants_.insert(std::make_pair(inputVal, &inputNode));
            } else {
                if (inputVal == "0") {
                    std::cerr << "zero not loop invariant: "
                              << inputNode.toString() << std::endl;
                }
                variableCounts[inputVal]++;
            }

        }
    }

    for (auto p: invariantCounts) {
#ifdef DEBUG_PRE_SHARE
        std::cerr << "usage count of invariant value: " << p.first
                  << " is " << p.second << std::endl;
#endif
        invariantsOfCount_.insert(std::make_pair(p.second, p.first));
    }

#ifdef DEBUG_PRE_SHARE
    for (auto p: invariants_) {
        std::cerr << "Invariant value: " << p.first << " used by mn: "
                  << p.second->toString() << " of po: "
                  << p.second->destinationOperation().toString() << std::endl;
    }
    for (auto i = invariantsOfCount_.rbegin();
         i != invariantsOfCount_.rend(); i++) {
        std::cerr << "Count: " << i->first << " for invariant value: "
                  << i->second << std::endl;
    }
#endif
}

/**
 * Allocate function units for pre-loop operand sharing.
 */
void BF2Scheduler::allocateFunctionUnits() {

    countLoopInvariantValueUsages();

    preSharedOperandPorts_.clear();
    preLoopSharedOperands_.clear();

    for (auto i = invariantsOfCount_.rbegin();
         i != invariantsOfCount_.rend(); i++) {
#ifdef DEBUG_PRE_SHARE
        std::cerr << "got invariant: " << i->second << " with usage count: "
                  << i->first << std::endl;
#endif
        for (auto it = invariants_.lower_bound(i->second),
                 end = invariants_.upper_bound(i->second); it != end; ++it) {
#ifdef DEBUG_PRE_SHARE
            std::cerr << "\tgot MN: " << it->second->toString() << " of PO: "
                      << it->second->destinationOperation().toString()
                      << std::endl;
#endif
            preAllocateFunctionUnits(it->second->destinationOperationPtr());
        }
    }

#ifdef DEBUG_PRE_SHARE
    std::cerr << "Operand bindings to ports: " << std::endl;
#endif
    for (auto p : preSharedOperandPorts_) {
        auto fup = p.first;
        if (p.second != NULL) {
            MoveNode* mn = p.second;
            preLoopSharedOperands_[mn] = fup;
#ifdef DEBUG_PRE_SHARE
            TTAMachine::FunctionUnit* fu = fup->parentUnit();
            ProgramOperation& po = p.second->destinationOperation(0);
            std::cerr << "\tPort: " << fu->name() << "." << fup->name() <<
                " move: " << p.second->toString() << " of PO: " <<
                po.toString() << std::endl;
#endif
        } else {
#ifdef DEBUG_PRE_SHARE
            TTAMachine::FunctionUnit* fu = fup->parentUnit();
            std::cerr << "\tPort: " << fu->name() << "." << fup->name() <<
                        " shared with multiple ops" << std::endl;
#endif
        }
    }
}

void BF2Scheduler::preAllocateFunctionUnits(ProgramOperationPtr po) {
    const Operation& op = po->operation();
#ifdef DEBUG_PRE_SHARE
    std::cerr << "Trying to preallocate for: " << po->toString()
              << std::endl;
#endif

    // first phase only share port which is already
    // shared with same value.
    PreLoopShareInfo rv = preAllocateFunctionUnitsInner(po, op, true);
    if (rv.state_ == NOT_SHARED) {
#ifdef DEBUG_PRE_SHARE
        std::cerr << "\t\tDid not find pre-shared, trying again.."
                  << std::endl;
#endif
        // second phase, can reserve a new port.
        rv = preAllocateFunctionUnitsInner(po, op, false);
    }

    // could not allocate even with shared ones. must steal from someone
    // just take first FU capable of executing the op.
    // find the port with least use.
    if (rv.state_ == NO_PORT) {
        releasePortForOp(op);
    }
}

void BF2Scheduler::releasePortForOp(const Operation& op) {
#ifdef DEBUG_PRE_SHARE
    std::cerr << "\tNo free fu for op: " << op.name() <<
            " have to revert earlier share" << std::endl;
#endif
    TTAMachine::Machine::FunctionUnitNavigator fuNav =
        targetMachine().functionUnitNavigator();
    TCEString opName = op.name();
    TTAMachine::FUPort* bestPort = NULL;

    int shareOpCount = INT_MAX;
    for (int j = 0; j < fuNav.count(); j++) {
        const TTAMachine::FunctionUnit& fu = *(fuNav.item(j));
        if (!fu.hasOperation(opName)) {
            continue;
        }
        const TTAMachine::HWOperation& hwop = *fu.operation(opName);
        for (int k = 1; k <= op.numberOfInputs(); k++) {
            TTAMachine::FUPort* port = hwop.port(k);
            if (port->isTriggering()) {
                continue;
            } else {
                std::map<TTAMachine::FUPort*, MoveNode*>::iterator i =
                    preSharedOperandPorts_.find(port);
                if (i != preSharedOperandPorts_.end() && i->second != NULL) {
                    int destOpCount = preSharedOperandPorts_.count(port);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                    std::cerr << "\t\t\tReserved port: " <<
                        port->parentUnit()->name() << "." <<
                        port->name() << " has  " << destOpCount <<
                        " shared operations using it" << std::endl;
#endif
                    if (destOpCount < shareOpCount) {
                        bestPort = port;
                        shareOpCount = destOpCount;
                    }
                }
            }
        }
    }

    assert(bestPort != NULL);
#ifdef DEBUG_PRE_SHARE
    std::cerr << "\t\tPass 3: UnReserved port: " <<
        bestPort->parentUnit()->name() << "." <<
        bestPort->name() << " because of " <<
        op.name() << std::endl;
#endif
    preSharedOperandPorts_.erase(bestPort);
    preSharedOperandPorts_.insert(std::make_pair(bestPort, (MoveNode*)NULL));
}



BF2Scheduler::PreLoopShareInfo BF2Scheduler::preAllocateFunctionUnitsInner(
    ProgramOperationPtr po, const Operation& op,
    bool onlySharedWithAnother) {

    TCEString opName = op.name();
    TTAMachine::Machine::FunctionUnitNavigator fuNav =
        targetMachine().functionUnitNavigator();

    bool fuFound = false;
    // we have a loop invariant.
    // Check which FUs can execute this operation.
    for (int j = 0; j < fuNav.count(); j++) {
        const TTAMachine::FunctionUnit& fu = *(fuNav.item(j));
        if (!fu.hasOperation(opName)) {
            continue;
        }
        const TTAMachine::HWOperation& hwop = *fu.operation(opName);
        PreLoopShareInfo rv = preAllocateFunctionUnits(
            po, op, hwop, onlySharedWithAnother);
        switch(rv.state_) {
        case SHARED:
        case NO_LOOP_INVARIANT:
            return rv;
        case NOT_SHARED:
#ifdef DEBUG_PRE_SHARE
            std::cerr << "\t\tfu found but cannot share: " << fu.name()
                      << std::endl;
#endif
            fuFound = true;
        default:
            break;
        }
    }
    if (targetMachine().controlUnit()->hasOperation(opName)) {
        fuFound = true;
    }
    return fuFound ?
        PreLoopShareInfo(NOT_SHARED) :
        PreLoopShareInfo(NO_PORT);
}

BF2Scheduler::PreLoopShareInfo BF2Scheduler::preAllocateFunctionUnits(
    ProgramOperationPtr po, const Operation& op,
    const TTAMachine::HWOperation& hwop, bool onlySharedWithAnother) {

    bool hasLoopInvariant = false;
    for (int k = 1; k <= op.numberOfInputs(); k++) {
        MoveNodeSet& inputNodeSet = po->inputNode(k);
        assert(inputNodeSet.count() == 1);
        MoveNode& inputNode = inputNodeSet.at(0);
        if (!ddg().isLoopInvariant(inputNode)) {
            // still need to check if the port is free,
            // to not use all FU ports for shared operands.
            TTAMachine::FUPort* port = hwop.port(k);
            std::multimap<TTAMachine::FUPort*, MoveNode*>::iterator pi=
                preSharedOperandPorts_.find(port);
            if (pi != preSharedOperandPorts_.end() && pi->second != NULL) {
                return PreLoopShareInfo(NO_PORT);
            }
            continue;
        }

        // we have a loop invariant.
        hasLoopInvariant = true;
        PreLoopShareInfo rv = preAllocateFunctionUnits(
            po, op, k, hwop, onlySharedWithAnother);
        if (rv.state_ == SHARED) {
            // Allocated to this FU now.
#ifdef DEBUG_PRE_SHARE
            std::cerr << "Allocating port for pre-loop opshare: "
                      << rv.sharedPort_->parentUnit()->name() << "."
                      << rv.sharedPort_->name() << " move: "
                      << rv.sharedMN_->toString() << " of po: "
                      << rv.sharedMN_->destinationOperation().toString()
                      << std::endl;
#endif
            preSharedOperandPorts_.insert(
                std::make_pair(rv.sharedPort_,rv.sharedMN_));
            return rv;
        }
        if (rv.state_ == NO_PORT) {
            // cannot allocate this op to this FU
            return rv;
        }
    }
    return hasLoopInvariant ?
        PreLoopShareInfo(NOT_SHARED) :
        PreLoopShareInfo(NO_LOOP_INVARIANT);

}

BF2Scheduler::PreLoopShareInfo BF2Scheduler::preAllocateFunctionUnits(
    ProgramOperationPtr po, const Operation& op, int operandIndex,
    const TTAMachine::HWOperation& hwop,
    bool onlySharedWithAnother) {

    MoveNodeSet& inputNodeSet = po->inputNode(operandIndex);
    assert(inputNodeSet.count() == 1);
    MoveNode& inputNode = inputNodeSet.at(0);

    TTAMachine::FUPort* port = hwop.port(operandIndex);
    bool isTrigger = port->isTriggering();
    int swappedOperandIndex = 0;
    if (isTrigger) {
        swappedOperandIndex = swapToUntrigger(po, op, operandIndex,inputNode);
        // still trigger? then try another FU?
        if (swappedOperandIndex) {
            port = hwop.port(swappedOperandIndex);
        } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\tswap failed" << std::endl;
#endif
            return PreLoopShareInfo(NOT_SHARED);
        }
    }

    std::map<TTAMachine::FUPort*, MoveNode*>::iterator pi=
        preSharedOperandPorts_.find(port);
    if (pi == preSharedOperandPorts_.end()) {
        // if need to share with another use, do not allow reserving new
        // port.
        if (onlySharedWithAnother) {
            if (swappedOperandIndex) {
                revertTopOpt();
            }
            return PreLoopShareInfo(NOT_SHARED);
        } else {
            // create new sharing, use first free
            assert(!port->isTriggering());
            return PreLoopShareInfo(inputNode, *port);
        }
    } else {
        MoveNode* prev = pi->second;
        if (prev == NULL) {
            // NULL means reserved for everybody. cannot own it.
            if (swappedOperandIndex) {
                revertTopOpt();
            }
            return PreLoopShareInfo(NOT_SHARED);
        }
        // port already used for some operand share..
        // but is it same?
        if (prev->move().source().equals(
                inputNode.move().source())) {
            assert(!port->isTriggering());
            return PreLoopShareInfo(inputNode, *port);
        } else {
            // different input value than previous, cannot share or use this fu
            if (swappedOperandIndex) {
                revertTopOpt();
            }
#ifdef DEBUG_PRE_SHARE
            std::cerr << "\t\t\tport used by another sharing, cannot use"
                      << std::endl;
#endif
            return PreLoopShareInfo(NO_PORT);
        }
    }
}

void annotateAllInputs(ProgramOperation& po,
                       TTAProgram::ProgramAnnotation::Id id,
                       const std::string& payload) {
    for (int i = 0; i < po.inputMoveCount(); i++) {
        MoveNode& inputNode = po.inputMove(i);
        TTAProgram::Move& m = inputNode.move();
        m.addAnnotation(
            TTAProgram::ProgramAnnotation(id, payload));
    }
}

void annotateAllOutputs(ProgramOperation& po,
                         TTAProgram::ProgramAnnotation::Id id,
                         const std::string& payload) {
    for (int i = 0; i < po.outputMoveCount(); i++) {
        MoveNode& outputNode = po.outputMove(i);
        TTAProgram::Move& m = outputNode.move();
        m.addAnnotation(
            TTAProgram::ProgramAnnotation(id, payload));
    }
}

void BF2Scheduler::reservePreallocatedFUs() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "Reserving preallocated fus" << std::endl;
#endif
    for (int i = 0; i < ddg().programOperationCount(); i++) {
        ProgramOperation& po = ddg().programOperation(i);
        const Operation& op = po.operation();
        TCEString opName = op.name();

        const TTAMachine::FunctionUnit* preAllocatedFU = NULL;
        for (int j = 1; j <= op.numberOfInputs(); j++) {
            MoveNodeSet& inputNodes = po.inputNode(j);
            if (inputNodes.count() != 1) {
                assert(0);
            }
            MoveNode& inputNode = inputNodes.at(0);
            TTAMachine::FUPort* fup = isPreLoopSharedOperand(inputNode);
            if (fup) {
                preAllocatedFU = fup->parentUnit();
                break;
            }
        }
        if (preAllocatedFU) {
            std::string fuName = preAllocatedFU->name();
            annotateAllInputs(
                po,
                TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_DST,
                fuName);

            annotateAllOutputs(po,
                TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_SRC,
                fuName);
        } else {
            const TTAMachine::FunctionUnit* prevFU = nullptr;
            for (auto p: preSharedOperandPorts_) {
                if (p.second == NULL) {
                    continue;
                }
                const TTAMachine::FunctionUnit* fu = p.first->parentUnit();
                if (fu == prevFU) {
                    continue;
                }
                prevFU = fu;
                std::string fuName= fu->name();

                annotateAllInputs(po,
                    TTAProgram::ProgramAnnotation::ANN_REJECTED_UNIT_DST,
                    fuName);

                annotateAllOutputs(po,
                    TTAProgram::ProgramAnnotation::ANN_REJECTED_UNIT_SRC,
                    fuName);
            }
        }
    }
}

void BF2Scheduler::unreservePreallocatedFUs() {
    for (int i = 0; i < ddg().nodeCount(); i++) {
        MoveNode& mn = ddg().node(i);
        TTAProgram::Move& m = mn.move();

        if (m.hasAnnotations(
                TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_SRC) ||
            m.hasAnnotations(
                TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_DST)) {

            m.removeAnnotations(
                TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_SRC);
            m.removeAnnotations(
                TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_DST);
        }
        m.removeAnnotations(
            TTAProgram::ProgramAnnotation::ANN_REJECTED_UNIT_SRC);
        m.removeAnnotations(
            TTAProgram::ProgramAnnotation::ANN_REJECTED_UNIT_DST);

    }
}

TTAMachine::FUPort* BF2Scheduler::isPreLoopSharedOperand(MoveNode& mn) const {
    auto i = preLoopSharedOperands_.find(&mn);
    if (i == preLoopSharedOperands_.end()) {
        return NULL;
    }
    return i->second;
}

void BF2Scheduler::mergePreLoopOperandShares() {
    for (auto i = preLoopSharedOperands_.begin();
         i != preLoopSharedOperands_.end(); i++) {
        auto j = i; j++;
        while(j != preLoopSharedOperands_.end()) {
            if (i->second == j->second) {
#ifdef DEBUG_PRE_SHARE
                std::cerr << "Should merge pre loop operand shares: "
                          << i->first->toString() << " and "
                          << j->first->toString() << std::endl;
#endif
                BFShareOperand* share =
                    new BFShareOperand(*this, *(j->first), *(i->first));
                if ((*share)()) {
                    scheduledStack_.push_back(share);
                    preLoopSharedOperands_.erase(j++);
#ifdef DEBUG_PRE_SHARE
                    std::cerr << "merging pre-shared ok!" << std::endl;
#endif
                } else {
                    delete share;
#ifdef DEBUG_PRE_SHARE
                    std::cerr << "merging pre-shared failed!" << std::endl;
#endif
                    j++;
                }
            } else {
                j++;
            }
        }
    }
}


bool BF2Scheduler::schedulePreLoopOperandShares() {

    // merge here multiple uses of same port
    mergePreLoopOperandShares();

    for (auto i : preLoopSharedOperands_) {
#ifdef DEBUG_PRE_SHARE
        std::cerr << "should after-schedule: " << i.first->toString() <<
            " to port: " << i.second->name() << std::endl;
#endif
        BFSchedulePreLoopShared* sbu =
            new BFSchedulePreLoopShared(*this, *(i.first));
        if ((*sbu)()) {
            scheduledStack_.push_back(sbu);
        } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "Failed scheduling of pre-loop-opshare: "
                      << i.first->toString()
                      << " to port: " << i.second->parentUnit()->name() << "."
                      << i.second->name() << std::endl;
            ddg_->writeToDotFile("pre_loop_share_fail.dot");
#endif
            delete sbu;
            return false;
        }
    }
    preLoopSharedOperands_.clear();
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "Scheduled all pre-loop opshares ok!" << std::endl;
#endif
    return true;
}

void BF2Scheduler::deletingNode(MoveNode* deletedNode) {
    currentFront_->deletingNode(deletedNode);
}


BF2Scheduler::~BF2Scheduler() {
    while(!scheduledStack_.empty()) {
        BFOptimization* bfo = scheduledStack_.back();
        scheduledStack_.pop_back();
        delete bfo;
    }
}

BF2Scheduler::MoveNodeMap BF2Scheduler::bypassNodes() {
    MoveNodeMap bypasses;
    for (auto o : scheduledStack_) {
	auto f = dynamic_cast<BF2ScheduleFront*>(o);
	if (f != NULL) {
	    f->appendBypassSources(bypasses);
	}
    }
    return bypasses;
}
