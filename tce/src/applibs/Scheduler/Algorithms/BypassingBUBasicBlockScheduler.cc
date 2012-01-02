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
 * @file BypassingBUBasicBlockScheduler.cc
 *
 * Implementation of BypassingBUBasicBlockScheduler class.
 *
 * This scheduler first schedules result reads of an operation, then
 * it tries to bypass the operands, and recursively schedule the 
 * operations which produce the result, bypassing the operands while
 * bypassing the results of the other operation. If it cannot schedule the
 * operation producing the value of the operand, it reverts the bypass,
 * and schedules operands from registers.
 *
 * @author Heikki Kultala 2011 (hkultala-no.spam-tut.fi)
 * @note rating: red
 */

#include <set>
#include <string>

#include "BypassingBUBasicBlockScheduler.hh"
#include "DataDependenceGraph.hh"
#include "SimpleResourceManager.hh"
#include "BUMoveNodeSelector.hh"
#include "POMDisassembler.hh"
#include "ProgramOperation.hh"
#include "ControlUnit.hh"
#include "Machine.hh"
#include "BasicBlock.hh"
#include "SchedulerPass.hh"
#include "LLVMTCECmdLineOptions.hh"
//#include "InterPassData.hh"
#include "MoveNodeSet.hh"
#include "Terminal.hh"
#include "RegisterRenamer.hh"
#include "HWOperation.hh"
#include "MachineConnectivityCheck.hh"
#include "Operation.hh"
#include "FUPort.hh"
#include "TerminalImmediate.hh"
#include "MathTools.hh"
#include "SpecialRegisterPort.hh"
#include "LiveRange.hh"

//#define DEBUG_OUTPUT
//#define DEBUG_REG_COPY_ADDER
//#define CFG_SNAPSHOTS
//#define BIG_DDG_SNAPSHOTS
//#define DDG_SNAPSHOTS
//#define SW_BYPASSING_STATISTICS

class CopyingDelaySlotFiller;
class InterPassData;
class RegisterRenamer;

/**
 * Constructs the basic block scheduler.
 *
 * @param data Interpass data
 * @param bypasser Helper module implementing software bypassing
 * @param delaySlotFiller Helper module implementing jump delay slot filling
 * @param registerRenamer Helper module implementing register renaming 
 */
BypassingBUBasicBlockScheduler::BypassingBUBasicBlockScheduler(
    InterPassData& data, 
    SoftwareBypasser* bypasser, 
    CopyingDelaySlotFiller* delaySlotFiller,
    RegisterRenamer* renamer) :
    BasicBlockScheduler(data, bypasser, delaySlotFiller, renamer),
    killDeadResults_(false) {

    CmdLineOptions *cmdLineOptions = Application::cmdLineOptions();
    options_ = dynamic_cast<LLVMTCECmdLineOptions*>(cmdLineOptions);
    if (options_ != NULL) {
        killDeadResults_ = options_->killDeadResults();
    }
}

/**
 * Destructor.
 */
BypassingBUBasicBlockScheduler::~BypassingBUBasicBlockScheduler() {
}

/**
 * Schedules a piece of code in a DDG
 * @param ddg The ddg containing the code
 * @param rm Resource manager that is to be used.
 * @param targetMachine The target machine.
 * @exception Exception several TCE exceptions can be thrown in case of
 *            a scheduling error.
 */
void
BypassingBUBasicBlockScheduler::handleDDG(
    DataDependenceGraph& ddg,
    SimpleResourceManager& rm,
    const TTAMachine::Machine& targetMachine)
    throw (Exception) {

    ddg_ = &ddg;
    targetMachine_ = &targetMachine;

    if (renamer_ != NULL) {
        renamer_->initialize(ddg);
    }

    if (options_ != NULL && options_->dumpDDGsDot()) {
        ddgSnapshot(
            ddg, std::string("0"), DataDependenceGraph::DUMP_DOT, false);
    }

    if (options_ != NULL && options_->dumpDDGsXML()) {
        ddgSnapshot(
            ddg, std::string("0"), DataDependenceGraph::DUMP_XML, false);
    }
    
    // empty need not to be scheduled
    if (ddg.nodeCount() == 0 || 
        (ddg.nodeCount() == 1 && !ddg.node(0).isMove())) {
        return;
    }
    // TODO: Magic number! Provide better heuristics.
    // INT_MAX/2 won't work on trunk due to multithreading injecting empty
    // instructions into the beginning of basic block.
    // Remove magic 1000 once the sparse implementation of RM vectors works.
//endCycle_ = (ddg.nodeCount() < 1000) ? ddg.nodeCount() + 200 : 1000;
    endCycle_ = (int)(ddg.nodeCount()*1.3 + 150);
    BUMoveNodeSelector selector(ddg, targetMachine);

    // register selector to renamer for notfications.
    if (renamer_ != NULL) {
        renamer_->setSelector(&selector);
    }

    rm_ = &rm;

    MoveNodeGroup moves = selector.candidates();
    while (moves.nodeCount() > 0) {
        bool movesRemoved = false;
        MoveNode& firstMove = moves.node(0);
        if (firstMove.isOperationMove()) {
            scheduleOperation(moves, selector);
        } else {
            scheduleMoveBU(firstMove, 0, endCycle_);
        }
        if (!movesRemoved) {
            if (!moves.isScheduled()) {
                std::string message = " Move(s) did not get scheduled: ";
                for (int i = 0; i < moves.nodeCount(); i++) {
                    message += moves.node(i).toString() + " ";
                }
                ddg.writeToDotFile("failed_bb.dot");                
                throw ModuleRunTimeError(__FILE__,__LINE__,__func__, message);
            }

            // notifies successors of the scheduled moves.
            notifyScheduled(moves, selector);
        }
        moves = selector.candidates();
    }

    clearCaches();

    if (ddg.nodeCount() != ddg.scheduledNodeCount()) {
        debugLog("All moves in the DDG didn't get scheduled.");
        ddg.writeToDotFile("failed_bb.dot");
        abortWithError("Should not happen!");
    }

    if (options_ != NULL && options_->dumpDDGsDot()) {
        std::string wtf = "0";
        ddgSnapshot(
            ddg, wtf, DataDependenceGraph::DUMP_DOT, true);
    }
    
    if (options_ != NULL && options_->dumpDDGsXML()) {
        ddgSnapshot(
            ddg, std::string("0"), DataDependenceGraph::DUMP_XML, true);
    }
}

#ifdef DEBUG_REG_COPY_ADDER
static int graphCount = 0;
#endif

/**
 * Schedules moves in a single operation execution.
 *
 * Assumes the given MoveNodeGroup contains all moves in the operation
 * execution. Also assumes that all outputs of the MoveNodeGroup have
 * been scheduled.
 *
 * @param moves Moves of the operation execution.
 */
void
BypassingBUBasicBlockScheduler::scheduleOperation(
    MoveNodeGroup& moves, MoveNodeSelector& selector) {

    std::cerr << "ScheduleOperation from selector: " << moves.toString()
              << std::endl;

    for (int i = 0; i < moves.nodeCount(); i++) {
        if (moves.node(0).isScheduled()) {
            ddg_->writeToDotFile("op_already_sched.dot");
            throw Exception(__FILE__,__LINE__,__func__,
                            "Scheduling failed for opsched, operation: "
                            + moves.toString());
        }
    }
    ProgramOperation& po =
        (moves.node(0).isSourceOperation())?
        (moves.node(0).sourceOperation()):
        (moves.node(0).destinationOperation());

    for (int lastCycle = endCycle_; lastCycle > 0; lastCycle--) {
        if (scheduleOperation(po, lastCycle)) {
//            ddg_->writeToDotFile("op_schedok.dot");

            finalizeOperation(selector);
            return;
        }
        std::cerr << "Trying to schedule op again with smaller cycle"
                  << std::endl;
    }

    ddg_->writeToDotFile("fail.dot");
    ddg_->writeToXMLFile("fail.xml");

    throw Exception(__FILE__,__LINE__,__func__,
                    "Scheduling failed for unknown reason!, operation: "
                    + po.toString());
}

/**
 * A short description of the pass, usually the optimization name,
 * such as "basic block scheduler".
 *
 * @return The description as a string.
 */
std::string
BypassingBUBasicBlockScheduler::shortDescription() const {
    return "Bypassing Bottom-up list scheduler with a basic block scope.";
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
BypassingBUBasicBlockScheduler::longDescription() const {
    return
        "Bottom-up list basic block scheduler that first tries to bypass"
        " and only then assign moves.";
}

void
BypassingBUBasicBlockScheduler::finalizeOperation(MoveNodeSelector& selector) {
    for (std::set<MoveNode*, MoveNode::Comparator>::iterator i =
             pendingBypassSources_.begin(); i != pendingBypassSources_.end();
         i++) {
        MoveNode* node = *i;
        
        // this may lead to extra raw edges.
        static_cast<DataDependenceGraph*>(ddg_->rootGraph())->
            copyDepsOver(*node, true, true);

        ddg_->dropNode(*node);
        removedNodes_.insert(node);
    }
    pendingBypassSources_.clear();

    for (std::set<MoveNode*, MoveNode::Comparator>::iterator i =
             scheduledMoves_.begin(); i != scheduledMoves_.end();
         i++) {
        selector.notifyScheduled(**i);
    }
    scheduledMoves_.clear();
}


bool
BypassingBUBasicBlockScheduler::scheduleOperation(
    ProgramOperation& po, int& latestCycle) {

    int lc = latestCycle;
    std::cerr << "Scheduling operation: " << po.toString() 
              << " lc: " << latestCycle << std::endl;
    if (po.isAnyNodeAssigned()) {
        ddg_->writeToDotFile("po_already_assigned.dot");
        throw Exception(__FILE__,__LINE__,__func__,
                        "po already scheduled!" +
                        po.toString());
    }
    if (!scheduleResults(po, latestCycle)) {
        std::cerr << "results fail for: " << po.toString() << std::endl;
        return false;
    }

    // update latestcycle
    int lastRes = -1;
    for (int i = 0; i < po.outputMoveCount(); i++) {
        MoveNode& mn = po.outputMove(i);
        if (mn.isScheduled()) {
            if (mn.cycle() > lastRes) {
                lastRes = mn.cycle();
            }
            lc = endCycle_;
            // TODO: why does this make the schedule worse?
//            if (mn.cycle() < lc) {
//                lc = mn.cycle() -1;
//            }
        }
    }
    if (lastRes > -1) {
        std::cerr << "Last result cycle: " << lastRes << std::endl;
        assert(lastRes <= latestCycle);
        latestCycle = lastRes;

    }

    MoveNode* trigger = findTrigger(po);

    // this occurs if operation without retvals
    // has different trigger selections?
    if (trigger == NULL) {
        std::cerr << "Trigger was null for: " << po.toString() << std::endl;
        return false;
    }

    if (!bypassAndScheduleNode(*trigger, trigger, lc)) {
        unscheduleResults(po);
        std::cerr << "trigger bypass or shed fail for: " << 
            po.toString() << std::endl;
        return false;
    }

    // if trigger bypass jamms fu, this may fail.
    if (!bypassAndScheduleOperands(po, trigger, lc)) {
        unscheduleOperands(po);

        // try to remove bypass of trigger and then re-schedule this
        if (!scheduleMoveBU(*trigger, 0, latestCycle)) {
            unscheduleResults(po);
            return false;
        }
        
        // try now operands, with bypass.
        if (!bypassAndScheduleOperands(po, trigger, lc)) {
            // advance latest cycle retry counter
            if (po.outputMoveCount() == 0) {
                latestCycle = trigger->cycle();
            }
            unscheduleOperands(po);
            unscheduleResults(po);

            std::cerr << "Operands scheduling fail for: " <<
                po.toString()<< std::endl;
            return false;
        }
    }
    std::cerr << "Operation scheduled ok!" << po.toString() << std::endl;
//    ddg_->writeToDotFile("op_schedok.dot");
//    if (!po.isScheduled()) {

    return true;
}

/**
 * Schedules result writes to an operation. 
 *
 * There result writes may be bypasses.
 */
bool
BypassingBUBasicBlockScheduler::scheduleResults(
    ProgramOperation& po, int latestCycle) {

    TTAMachine::HWOperation* hwop = NULL;
    int ectrig = -1;
    // TODO: do bypasses first? then put other close to them?
    for (int i = 0; i < po.outputMoveCount(); i++) {
        MoveNode& mn = po.outputMove(i);
        if (mn.isDestinationOperation()) {
            std::pair<int,int> cycleLimits = operandCycleLimits(mn, NULL);
            if (!scheduleMoveBU(
                    mn, cycleLimits.first, std::min(
                        cycleLimits.second, latestCycle))) {
                unscheduleResults(po);
                return false;
            } else {
                TTAProgram::Terminal& src = mn.move().source();
                TTAMachine::FunctionUnit* fu = 
                    dynamic_cast<TTAMachine::FunctionUnit*>(
                        src.port().parentUnit());
                assert(fu != NULL);
                hwop = fu->operation(po.operation().name());
                ectrig = mn.cycle() - hwop->latency(src.operationIndex());
            }
        }
    }

    // then non-bypasseds.
    for (int i = 0; i < po.outputMoveCount(); i++) {
        MoveNode& mn = po.outputMove(i);
        if (!mn.isDestinationOperation()) {
            // if this is 
            if (pendingBypassSources_.find(&mn) != 
                pendingBypassSources_.end()) {
                std::cerr << "This node is to be DRE'd: " << mn.toString()
                          << std::endl;
                continue;
            }
            if (hwop != NULL) {
                TTAProgram::Terminal& src = mn.move().source();

                // try to put as close to other results as possible.
                // so use top-down-scheduling here.

                // todo: make sure successors not scheduled?
                if (ddg_->successorsReady(mn)) {
                    if (scheduleMoveUB(
                            mn, ectrig + hwop->latency(src.operationIndex()),
                            latestCycle)) {
                        return true;
                    }
                } else {
                    unscheduleResults(po);
                    return false;
                }
            }
            if (!scheduleMoveBU(mn, 0, latestCycle)) {
                unscheduleResults(po);
                return false;
            }
        }
    }
    

    return true;
}

/** 
 * Schedule a single move.
 */
bool
BypassingBUBasicBlockScheduler::scheduleMoveUB(
    MoveNode& mn, int earliestCycle, int latestCycle) {
    std::cerr << "\t\tSchedulign moveUB: " << mn.toString() << 
        " Cycle limits: " << earliestCycle << " , " << latestCycle
              << std::endl;
    // TODO: unscheduld ones
    latestCycle = std::min(latestCycle, (int)endCycle_);
    int ddgLC = ddg_->latestCycle(mn);
    latestCycle = std::min(latestCycle, ddgLC);
    int ddgEC = ddg_->earliestCycle(mn);
    if (ddgEC == -1 || ddgEC == INT_MAX) {
        return false;
    }
    int minCycle = std::max(earliestCycle, ddgEC);
    int rmCycle = rm_->earliestCycle(minCycle, mn);
    if (rmCycle != -1 && rmCycle <= latestCycle) {
        rm_->assign(rmCycle, mn);
        scheduledMoves_.insert(&mn);

        std::cerr << "\t\tScheduled to cycle: " << rmCycle << std::endl;
        return true;
    } else {
        return false;
    }
}

/** 
 * Schedule a single move.
 */
bool
BypassingBUBasicBlockScheduler::scheduleMoveBU(
    MoveNode& mn, int earliestCycle, int latestCycle) {
    int endCycle = endCycle_;
    // if control flow move, limit delay slots before end
    if (mn.move().isControlFlowMove()) {
        endCycle -= targetMachine_->controlUnit()->delaySlots();
    }

    std::cerr << "\t\tSchedulign moveBU: " << mn.toString() << 
        " Cycle limits: " << earliestCycle << " , " << latestCycle
              << std::endl;
    latestCycle = std::min(latestCycle, endCycle);
    int ddgCycle = ddg_->latestCycle(mn,UINT_MAX,false, false);
    if (ddgCycle == -1) {
        std::cerr << "ddg cycle -1, ddg prevents scheduling!" << std::endl;
        return false;
    }
    int maxCycle = std::min(latestCycle, ddgCycle);
    std::cerr << "\t\t\tmaxCycle after ddg: " << maxCycle << std::endl;
    int rmCycle = rm_->latestCycle(maxCycle, mn);
    std::cerr << "\t\t\trmCycle: " << rmCycle << std::endl;
    if (rmCycle != -1 && rmCycle >= earliestCycle) {
        rm_->assign(rmCycle, mn);
        scheduledMoves_.insert(&mn);
        std::cerr << "\t\tScheduled to cycle: " << rmCycle << std::endl;
        return true;
    } else {
        std::cerr << "\t\t\trmcycle is -1, cannot schedule move" << std::endl;
        return false;
    }
}

/**
 *
 * @return the amount of moves saved by this bypass
 */
int
BypassingBUBasicBlockScheduler::bypassNode(MoveNode& node, int maxHopCount) {
    std::cerr << "\t\tIncoming hopcount: " << maxHopCount << std::endl;
    std::pair<MoveNode*,int> bypassSource = 
        findBypassSource(node, maxHopCount);
    if (bypassSource.second == 0) {
        std::cerr << "\t\thopcount 0" << std::endl;
        return 0;
    }

    // do not bypass from already scheduled moves.
    if (bypassSource.first->isScheduled()) {
        std::cerr << "\t\tbypassource scheduled" << std::endl;
        return 0;
    }
/*
        ddg_->writeToDotFile("bypass_source_scheduled.dot");
        throw Exception(__FILE__, __LINE__,__func__,
                        "Bypass souce already scheduled! " +
                        bypassSource.first->toString());
    }
*/  
                     
    if (!ddg_->guardsAllowBypass(*bypassSource.first, node)) {
        std::cerr << "\t\tguardsnotallowbypass" << std::endl;
        return 0;
    }
    
//    std::cerr << "\t\tMerging." << std::endl;
//    ddg_->writeToDotFile("merging.dot");
    if (!ddg_->mergeAndKeep(*bypassSource.first,node)) {

        std::cerr << "Merge fail." << std::endl;
        return 0;
    }
    
    std::cerr << "Merged." << std::endl;

    bypassSources_[&node] = bypassSource.first;

    // TODO: DRE seems to slow things down!
    if (killDeadResults_) {
        if (!ddg_->resultUsed(*bypassSource.first)) {
            std::cerr << "Could remove source:" << 
                bypassSource.first->toString() << std::endl;
            pendingBypassSources_.insert(bypassSource.first);
        }
    }
    
    return bypassSource.second;
}

/**
 * Finds a source where from to bypass. 
 *
 * Goes ddg backwards and checks for connectivity.
 */
std::pair<MoveNode*, int>
BypassingBUBasicBlockScheduler::findBypassSource(
    MoveNode& node, int maxHopCount) {
    std::set<const TTAMachine::Port*>
        destinationPorts = 
        MachineConnectivityCheck::findPossibleDestinationPorts(
            *targetMachine_, node);

    MoveNode* n = &node;
    MoveNode* okSource = NULL;
    int hops = 0;
    for (int i = 0; i < maxHopCount; i++) {
        MoveNode* rrSource = ddg_->onlyRegisterRawSource(*n);
        if (rrSource == NULL) {
            break;
        }
        n = rrSource;
        if (MachineConnectivityCheck::canSourceWriteToAnyDestinationPort(
                *n, destinationPorts)) {
            okSource = n;
            hops = i+1;
        }
    }
    return std::pair<MoveNode*,int>(okSource, hops);
}

bool
BypassingBUBasicBlockScheduler::bypassAndScheduleNode(
    MoveNode& node, MoveNode* trigger, int latestCycle) {
    // TODO: DDG does not yet support bypass over multiply nodes
    int bypassHopCount = 1; //INT_MAX;
    while (bypassHopCount >= 0) {
        bypassHopCount = bypassNode(node, bypassHopCount);
        if (bypassHopCount > 0) {
            std::cerr << "\tBypassed node: " << node.toString() << std::endl;
            if (node.isSourceOperation()) {
                // take a copy of latestCycle because scheduleOperation
                // may modify it
                int lc = latestCycle;
                if (scheduleOperation(node.sourceOperation(), lc)) {
                    return true; 
                } else {
                    // scheduling bypassed op failed. 
                    // revert bypass, and try with smaller bypass ho count.
                    undoBypass(node);
                    bypassHopCount--;
                    continue;
                }
            } else {
                // schedule individual input, not bypassed from op.
                // bypassed from reg write?
                if (scheduleOperandOrTrigger(node, trigger, latestCycle)) {
                    return true;
                } else {
                    // scheduling bypassed op failed. 
                    // revert bypass, and try with smaller bypass ho count.
                    undoBypass(node);
                    bypassHopCount--;
                    continue;
                }                    
            }
        }
        // not bypassed.
        std::cerr << "\tDid not bypass: " << node.toString() << std::endl;
        if (scheduleOperandOrTrigger(node, trigger, latestCycle)) {
            return true;
        } else {
            return false;
        }
    }
    // for some reason went out from the loop. fail
    return false;
}

bool 
BypassingBUBasicBlockScheduler::bypassAndScheduleOperands(
    ProgramOperation& po, MoveNode* trigger, int latestCycle) {
    
    for (int i = 0; i < po.inputMoveCount(); i++) {
        MoveNode& operand = po.inputMove(i);
        if (trigger != &operand) {
            if (!bypassAndScheduleNode(operand, trigger, latestCycle)) {
                for (; i >= 0; i++) {
                    undoBypassAndUnschedule(po.inputMove(i));
                    return false;
                }
            }
        } else {
            assert(operand.isScheduled());
        }
    }
    return true;
}

// Called when cannot bypass.
bool 
BypassingBUBasicBlockScheduler::scheduleOperandOrTrigger(
    MoveNode& operand, MoveNode* trigger, int latestCycle) {
    std::pair<int,int> cycleLimits = 
        operandCycleLimits(operand, trigger);

    // if not connected and cannot rename fail here
    if (!renameSourceIfNotConnected(
            operand, cycleLimits.first, std::min(
                cycleLimits.second, latestCycle))) {
        std::cerr << "\t\t\tNo connctivity and cannot rename for: "
                  << operand.toString();

        // TODO: revert to adding regcopies.
        return false;
    }

    return scheduleMoveBU(operand, cycleLimits.first, 
                          std::min(latestCycle,cycleLimits.second));
}

/* 
 * Calculates the range where operand can be scheduled, based on
 * results and other operands.
 */
std::pair<int,int>
BypassingBUBasicBlockScheduler::operandCycleLimits(
    MoveNode& mn, MoveNode* trigger) {
    int minn = 0;
    int maxx = endCycle_;
    assert (mn.isDestinationOperation());
    ProgramOperation& po = mn.destinationOperation();
    if (po.areOutputsAssigned()) {
        maxx = std::min(mn.latestTriggerWriteCycle(), (int)endCycle_);
    } else {
        maxx = -1;
    }
    if (po.isAnyInputAssigned()) {
        // if trigger now known, find it.
        if (trigger == NULL) {
            trigger = findTrigger(po);
        }
        if (trigger != NULL) {
            if (trigger == &mn) {
                // this is trigger. limit min to last operand.
                minn = std::max(lastOperandCycle(po), 0);
            } else {
                // this is not trigger.
                if (trigger->isScheduled()) {
                    maxx = trigger->cycle();
                }
            }
        } else {
            // TODO: what to do if cannot find trigger? fail?
            return std::pair<int,int>(INT_MAX, -1);
        }
    }
    return std::pair<int,int>(minn, maxx);
}

// find which movenode is the trigger of an operation.
MoveNode*
BypassingBUBasicBlockScheduler::findTrigger(ProgramOperation& po) {
    for (int i = 0; i < po.inputMoveCount(); i++) {
        MoveNode& mn = po.inputMove(i);
        if (mn.isScheduled()) {
            TTAProgram::Terminal& term = mn.move().destination();
            if (term.isTriggering()) {
                return &mn;
            }
            return findTriggerFromUnit(po, *term.port().parentUnit());
        }
    }

    for (int i = 0; i < po.outputMoveCount(); i++) {
        MoveNode& mn = po.outputMove(i);
        if (mn.isScheduled()) {
            TTAProgram::Terminal& term = mn.move().source();
            return findTriggerFromUnit(po, *term.port().parentUnit());
        }
    }

    // no scheduled moves. getting harder.

    TTAMachine::Machine::FunctionUnitNavigator nav = 
        targetMachine_->functionUnitNavigator();
    MoveNode* candidate = NULL;
    for (int i = 0; i < nav.count(); i++) {
        TTAMachine::FunctionUnit* fu = nav.item(i);
        if (fu->hasOperation(po.operation().name())) {
            if (candidate == NULL) {
                candidate = findTriggerFromUnit(po, *fu);
            } else {
                // make sure two different FU's don't have different
                // trigger ports.
                if (findTriggerFromUnit(po, *fu) != candidate) {
                    return NULL;
                }
            }
        }
    }
    
    if (targetMachine_->controlUnit()->hasOperation(po.operation().name())) {
        return findTriggerFromUnit(po, *targetMachine_->controlUnit());
    }
    return candidate;

}

//int lastOperandCycle(const ProgramOperation& po)

MoveNode* BypassingBUBasicBlockScheduler::findTriggerFromUnit(
    ProgramOperation& po, TTAMachine::Unit& unit) {
    TTAMachine::FunctionUnit* fu = 
        dynamic_cast<TTAMachine::FunctionUnit*>(&unit);
    int ioIndex = -1;
    for (int i = 0; i < fu->operationPortCount(); i++) {
        TTAMachine::FUPort* port = fu->operationPort(i);
        if (port->isTriggering()) {
            TTAMachine::HWOperation* hwop = 
                fu->operation(po.operation().name());
            ioIndex = hwop->io(*port);
            return &po.inputNode(ioIndex).at(0);
        }
    }
    return NULL;
}

int
BypassingBUBasicBlockScheduler::lastOperandCycle(
    const ProgramOperation& po) {
    int res = -1;
    for (int i = 0; i < po.inputMoveCount(); i++) {
        const MoveNode& mn = po.inputMove(i);
        if (mn.isScheduled()) {
            if (mn.cycle() > res) {
                res = mn.cycle();
            }
        }
    }
    return res;
}


void
BypassingBUBasicBlockScheduler::undoBypass(MoveNode& mn) {
    std::cerr << "\tundoing bypass: " << mn.toString() << std::endl;
    MoveNode* source = bypassSources_[&mn];
    if (source != NULL) {
        if (pendingBypassSources_.erase(source)) {
            std::cerr << "undid dre: " << source->toString()
                      << std::endl;
        }
        ddg_->unMerge(*source, mn);
        bypassSources_[&mn] = NULL;
    }
    std::cerr << "\tundid bypass: " << mn.toString() << std::endl;
}

void
BypassingBUBasicBlockScheduler::undoBypassAndUnschedule(MoveNode& mn) {
    if (mn.isBypass()) {
        unscheduleOperation(mn.sourceOperation());
    }
    if (mn.isScheduled()) {
        rm_->unassign(mn);
        scheduledMoves_.erase(&mn);
    }
    undoBypass(mn);
}

void 
BypassingBUBasicBlockScheduler::unscheduleResults(ProgramOperation& po) {
    std::cerr << "\tUnschedulign results" << std::endl;
    for (int i = 0; i < po.outputMoveCount(); i++) {
        MoveNode& mn = po.outputMove(i);
        if (mn.isScheduled()) {
            rm_->unassign(mn);
            scheduledMoves_.erase(&mn);
        }
    }
}
    
void
BypassingBUBasicBlockScheduler::unscheduleOperands(ProgramOperation& po) {
    for (int i = 0; i < po.inputMoveCount(); i++) {
        undoBypassAndUnschedule(po.inputMove(i));
    }
}

void
BypassingBUBasicBlockScheduler::unscheduleOperation(ProgramOperation& po) {
    unscheduleOperands(po);
    unscheduleResults(po);
}

void
BypassingBUBasicBlockScheduler::clearCaches() {
    bypassSources_.clear();
    for (DataDependenceGraph::NodeSet::iterator i = removedNodes_.begin();
         i != removedNodes_.end(); i++) {
        if (ddg_->rootGraph() != ddg_) {
            ddg_->rootGraph()->removeNode(**i);
        }
        delete *i;
    }
    removedNodes_.clear();
    pendingBypassSources_.clear();
}                                     

bool BypassingBUBasicBlockScheduler::renameSourceIfNotConnected(
    MoveNode& moveNode, int earliestCycle, int latestCycle) {
    
    std::set<const TTAMachine::Port*>
        destinationPorts = 
        MachineConnectivityCheck::findPossibleDestinationPorts(
            *targetMachine_,moveNode);

    if (MachineConnectivityCheck::canSourceWriteToAnyDestinationPort(
            moveNode, destinationPorts)) {
        std::cerr << "Already connected." << std::endl;
        return true;
    }

    
    return renamer_->renameSourceRegister(moveNode, false, latestCycle);

#if 0
    LiveRange* lr = ddg_->findLiveRange(moveNode, false);
    // todo: should rename here
    if (lr == NULL) {
        std::cerr << "Could not find liverange for:" << moveNode.toString() << std::endl;
        return false;
    }

    // for now limit to simple cases.
    if (!(lr->writes.size() == 1  && lr->reads.size() > 0)) {
        std::cerr << "Liverange too complex to rename: " << lr->toString()
                  <<" for: " << moveNode.toString() << std::endl;
        return false;
    } 
    
    std::cerr << "(Not really yeat)Trying to rename lr: " << lr->toString() << std::endl;

    // TODO: find the register.

    return renamer_->renameLiveRange(*lr, "", false, false, false);
#endif
}
