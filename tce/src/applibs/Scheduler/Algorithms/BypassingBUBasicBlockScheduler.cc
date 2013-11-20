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
 * When the original/first scheduleOperation() call finishes,
 * all operations and
 * moves scheduled directly by it or recursively by it are "fixed in place",
 * and can no longer be reverted. Then selector gives another operation or
 * individual move to be scheduled.
 *
 * The basic principle of this scheduler is greediness:
 * "try the fastest way of doing things first, and if it succeeds, good,
 *  it not, revert to slower way of doing things.
 * so 1) bypass 2) rename 3) add regcopies
 *
 * The old BB scheduler does these in exactly opposite order(
 * first addregcopies, then rename, then bypass).
 *
 * @author Heikki Kultala 2011-2012 (hkultala-no.spam-tut.fi)
 * @note rating: red
 */

/*
  TODO:
  
  * Allow reverting of regcopyadding
  * Allow bypassing also when not scheduling recursively
  * switch from DFS to BFS
  * Bypass and schedule top-down other results?
  * Allow bypassing when scheduling reg-reg moves
  * Allow bypassing over multiple nodes
  * Smarter logic for selecting registers when 1) renaming 2)tempregadding
  * switch operands on commutative operations.
  */

#include <set>
#include <string>

#include "BypassingBUBasicBlockScheduler.hh"
#include "DataDependenceGraph.hh"
#include "SimpleResourceManager.hh"
#include "BUMoveNodeSelector.hh"
#include "ProgramOperation.hh"
#include "ControlUnit.hh"
#include "Machine.hh"
#include "BasicBlock.hh"
#include "LLVMTCECmdLineOptions.hh"
#include "InterPassData.hh"
#include "MoveNodeSet.hh"
#include "Terminal.hh"
#include "RegisterRenamer.hh"
#include "HWOperation.hh"
#include "MachineConnectivityCheck.hh"
#include "Operation.hh"
#include "TerminalRegister.hh"
#include "Move.hh"
#include "RegisterCopyAdder.hh"
#include "MoveGuard.hh"
#include "DisassemblyRegister.hh"

//#define DEBUG_OUTPUT
//#define DEBUG_REG_COPY_ADDER
//#define CFG_SNAPSHOTS
//#define BIG_DDG_SNAPSHOTS
//#define DDG_SNAPSHOTS
//#define SW_BYPASSING_STATISTICS

class CopyingDelaySlotFiller;
class RegisterRenamer;

/**
 * Constructs the basic block scheduler.
 *
 * @param data Interpass data
 * @param bypasser Helper module implementing software bypassing. Not used.
 * @param delaySlotFiller Helper module implementing jump delay slot filling
 * @param registerRenamer Helper module implementing register renaming 
 */
BypassingBUBasicBlockScheduler::BypassingBUBasicBlockScheduler(
    InterPassData& data, 
    SoftwareBypasser* bypasser, 
    CopyingDelaySlotFiller* delaySlotFiller,
    RegisterRenamer* renamer) :
    BasicBlockScheduler(data, bypasser, delaySlotFiller, renamer),
    killDeadResults_(false), renameRegisters_(false) {

    CmdLineOptions *cmdLineOptions = Application::cmdLineOptions();
    options_ = dynamic_cast<LLVMTCECmdLineOptions*>(cmdLineOptions);
    if (options_ != NULL) {
        killDeadResults_ = options_->killDeadResults();
        renameRegisters_ = options_->renameRegisters();
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

    if (!BasicBlockPass::interPassData().hasDatum("SCRATCH_REGISTERS")) {
        RegisterCopyAdder::findTempRegisters(
            targetMachine, BasicBlockPass::interPassData());
    }

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

    // scheduling pipeline resources after last cycle may cause problems.
    // make RM to check for those
    rm.setMaxCycle(endCycle_);


    // register selector to renamer for notfications.
    if (renamer_ != NULL) {
        renamer_->setSelector(&selector);
    }

    rm_ = &rm;

    MoveNodeGroup moves = selector.candidates();
    while (moves.nodeCount() > 0) {
        MoveNode& firstMove = moves.node(0);
        if (firstMove.isOperationMove()) {
            scheduleOperation(moves, selector);
        } else {
            std::cerr << "Individual move: " << firstMove.toString()
                      << std::endl;
            scheduleMoveBU(firstMove, 0, endCycle_, TempRegBefore);
            finalizeOperation(selector);
        }
        if (!moves.isScheduled()) {
            std::string message = " Move(s) did not get scheduled: ";
            for (int i = 0; i < moves.nodeCount(); i++) {
                message += moves.node(i).toString() + " ";
            }
            ddg.writeToDotFile("failed_bb.dot");                
            throw ModuleRunTimeError(__FILE__,__LINE__,__func__, message);
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
        if (scheduleOperation(po, lastCycle , true)) {
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
    return "Bypassing recursive Bottom-up list scheduler with a basic block scope.";
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
        " and only then assign moves,and recursively schedules bypass sources";
}

void
BypassingBUBasicBlockScheduler::finalizeOperation(MoveNodeSelector& selector) {

    for (std::map<MoveNode*, MoveNode*, MoveNode::Comparator>::iterator i =
             bypassSources_.begin(); i != bypassSources_.end(); i++) {
        // only notify if no DRE'd
        if (i->second != NULL && pendingBypassSources_.find(i->second) 
            == pendingBypassSources_.end()) {
            selector.mightBeReady(*i->second);
        }
    }

    bypassSources_.clear();

    for (std::set<MoveNode*, MoveNode::Comparator>::iterator i =
             pendingBypassSources_.begin(); i != pendingBypassSources_.end();
         i++) {
        MoveNode* node = *i;
        
        // this may lead to extra raw edges.
        static_cast<DataDependenceGraph*>(ddg_->rootGraph())->
            copyDepsOver(*node, true, true);

        std::cerr << "DRE killing node: " << (**i).toString() << std::endl;
        DataDependenceGraph::NodeSet preds = ddg_->predecessors(*node);
        ddg_->dropNode(*node);
        removedNodes_.insert(node);
        for (DataDependenceGraph::NodeSet::iterator i = 
                 preds.begin(); i != preds.end(); i++) {
            selector.mightBeReady(**i);
        }
    }
    pendingBypassSources_.clear();

    for (std::set<MoveNode*, MoveNode::Comparator>::iterator i =
             scheduledMoves_.begin(); i != scheduledMoves_.end();
         i++) {
        assert((**i).isScheduled());
        selector.notifyScheduled(**i);
    }
    scheduledMoves_.clear();

    regCopiesBefore_.clear();
    regCopiesAfter_.clear();
}

/**
 * Tries to schedule an operation.
 *
 * This function is called recursively, when bypassing.
 *
 */
bool
BypassingBUBasicBlockScheduler::scheduleOperation(
    ProgramOperation& po, int& latestCycle, bool allowRegCopies) {

    int lc = latestCycle;
    std::cerr << "Scheduling operation: " << po.toString() 
              << " lc: " << latestCycle << std::endl;
    if (po.isAnyNodeAssigned()) {
        ddg_->writeToDotFile("po_already_assigned.dot");
        throw Exception(__FILE__,__LINE__,__func__,
                        "po already scheduled!" +
                        po.toString());
    }
    if (!scheduleResults(po, latestCycle, allowRegCopies)) {
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

    if (!bypassAndScheduleNode(*trigger, trigger, lc, allowRegCopies)) {
        unscheduleResults(po);
        std::cerr << "trigger bypass or shed fail for: " << 
            po.toString() << std::endl;
        return false;
    }

    // if trigger bypass jamms fu, this may fail.
    if (!bypassAndScheduleOperands(po, trigger, lc, allowRegCopies)) {
        unscheduleOperands(po);

        // try to remove bypass of trigger and then re-schedule this
        if (!scheduleMoveBU(*trigger, 0, latestCycle, 
                            allowRegCopies ? 
                            TempRegBefore : 
                            TempRegNotAllowed)) {
            unscheduleResults(po);
            std::cerr << "Unscheduled trigger fail:" << trigger->toString()
                      << std::endl;
            return false;
        }
        
        // try now operands, with bypass.
        if (!bypassAndScheduleOperands(po, trigger, lc, allowRegCopies)) {
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
    ProgramOperation& po, int latestCycle, bool allowRegCopies) {

    TTAMachine::HWOperation* hwop = NULL;
    int ectrig = -1;
    // TODO: do bypasses first? then put other close to them?
    for (int i = 0; i < po.outputMoveCount(); i++) {
        MoveNode& mn = po.outputMove(i);
        if (mn.isDestinationOperation()) {
            std::pair<int,int> cycleLimits = operandCycleLimits(mn, NULL);
            if (!scheduleMoveBU(
                    mn, cycleLimits.first, std::min(
                        cycleLimits.second, latestCycle), 
                    allowRegCopies ? TempRegAfter : TempRegNotAllowed)) {
                unscheduleResults(po);
                return false;
            } else {
                // bypassed move is now scheduled.
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
            // is some bypassed move already scheduled?
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

            // old regcopy may mess up scheduling result
            // TODO: remove those regcopies?
            if (!scheduleMoveBU(
                    mn, 0, latestCycle, 
                    allowRegCopies? 
                    TempRegAfter :
                    TempRegNotAllowed)) {
                unscheduleResults(po);
                return false;
            }
        }
    }
    

    return true;
}

/** 
 * Schedule a single move with up-down scheduling.
 *
 * Currently used only for results which also have bypassed read of the
 * result, to get this as close to the bypassed one as possible.
 */
bool
BypassingBUBasicBlockScheduler::scheduleMoveUB(
    MoveNode& mn, int earliestCycle, int latestCycle) {
    std::cerr << "\t\tSchedulign moveUB: " << mn.toString() << 
        " Cycle limits: " << earliestCycle << " , " << latestCycle
              << std::endl;
    // TODO: unscheduld ones

    // Does not try to rename as this ub is mostly used with non-dre'd result
    // when bypassing
    if (!MachineConnectivityCheck::canTransportMove(mn, *targetMachine_)) {
        return false;
    }
/*
        MoveNode* reg2RegCopy = createTempRegCopy(mn, true);
        // recursively call this first, then the generated copy.
        
        if (reg2RegCopy == NULL) {
            return false;
        }
        std::cerr << "Created regcopy: " << reg2RegCopy->toString();

        if (!scheduleMoveUB(mn, earliestCycle, latestCycle)) {
            return false;
        }

        if (!scheduleMoveUB(*reg2RegCopy, 0, endCycle_)) {
            unschedule(mn);
            return false;
        }
    }
*/

    latestCycle = std::min(latestCycle, (int)endCycle_);
    std::cerr << "\t\tlc: " << latestCycle << std::endl;
    int ddgLC = ddg_->latestCycle(mn);
    latestCycle = std::min(latestCycle, ddgLC);
    std::cerr << "\t\tddglc: " << latestCycle << std::endl;
    int ddgEC = ddg_->earliestCycle(mn);
    std::cerr << "\t\tddgec: " << ddgEC << std::endl;
    if (ddgEC == -1 || ddgEC == INT_MAX) {
        return false;
    }
    int minCycle = std::max(earliestCycle, ddgEC);
    std::cerr << "\t\tmincycle: " << minCycle << std::endl;
    int rmCycle = rm_->earliestCycle(minCycle, mn);
    std::cerr << "\t\trmcycle: " << rmCycle << std::endl;
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
 * Schedule a single move with bottom-up-scheduling.
 * 
 * If the machine does not have sufficient connectivity,
 * tries to rename the source register or add regcopies.
 *
 * @param mn MoveNode to be scheduled
 * @param earliestCycle earliestCycle allowed.
 * @param latestCycle latest allowed cycle. Tries to schedule close to this.
 * @param t whether temp register copies are allowed, and whether they
          are added after or before this move.
 */
bool
BypassingBUBasicBlockScheduler::scheduleMoveBU(
    MoveNode& mn, int earliestCycle, int latestCycle, 
    TempRegCopyLocation t) {
    int endCycle = endCycle_;

    std::cerr << "\t\tSchedulign moveBU: " << mn.toString() << 
        " Cycle limits: " << earliestCycle << " , " << latestCycle
              << std::endl;

    // if control flow move, limit delay slots before end
    if (mn.move().isControlFlowMove()) {
        endCycle -= targetMachine_->controlUnit()->delaySlots();
    }

    // if regcpy is unscheudled it must be rescheduled
    if (t == TempRegAfter) {
        MoveNode* regCopyAfter = regCopiesAfter_[&mn];
        if (regCopyAfter != NULL && 
            pendingBypassSources_.find(regCopyAfter) ==
            pendingBypassSources_.end()) {
            if (!regCopyAfter->isScheduled()) {
                std::cerr << "\t\t\tScheduling regcopyafter: " <<
                    regCopyAfter->toString() << std::endl;
                if (!(scheduleMoveBU(
                          *regCopyAfter, 0, latestCycle, t))) {
                    std::cerr << "\t\t\t\tRegcopyafter scheduling failed" << std::endl;
                    return false;
                }
            }
        }
    }

    if (!MachineConnectivityCheck::canTransportMove(
            mn, *targetMachine_)) {
        std::cerr << "\t\t\tSchedulemoveBU cannot transport move" << std::endl;

        if (t == TempRegNotAllowed) {
            return false;
        }

        if (!renameSourceIfNotConnected(mn, latestCycle)) {
            
            MoveNode* reg2RegCopy = 
                createTempRegCopy(mn, t == TempRegAfter);
            // recursively call this first, then the generated copy.
            
            if (reg2RegCopy == NULL) {
                return false;
            }
            std::cerr << "\t\t\tCreated regcopy: " << reg2RegCopy->toString()
                      << std::endl;
            if (t == TempRegBefore) {
                MoveNode* oldRC = regCopiesBefore_[&mn];
                if (oldRC != NULL) {
                    std::cerr << "Existing regcopy before: " << oldRC->toString()
                              << " setting it as regcopy of regcopy" 
                              << std::endl;
                    regCopiesBefore_[reg2RegCopy] = oldRC;
                }
                regCopiesBefore_[&mn] = reg2RegCopy;
                std::cerr << "\t\t\t\tRegcopybefore added." << std::endl;

                return scheduleMoveBU(
                    mn, earliestCycle, latestCycle, TempRegBefore);

            } else {
                MoveNode* oldRC = regCopiesAfter_[&mn];
                if (oldRC != NULL) {
                    std::cerr << "Existing regcopy after: "<<oldRC->toString()
                              << " setting it as regcopy of regcopy" 
                              << std::endl;
                    regCopiesAfter_[reg2RegCopy] = oldRC;
                }

                regCopiesAfter_[&mn] = reg2RegCopy;
                if (!scheduleMoveBU(
                        *reg2RegCopy, 0, endCycle_, TempRegAfter)) {
                    return false;
                }
                
                if (!scheduleMoveBU(
                        mn, earliestCycle, latestCycle, TempRegAfter)) {
                    unschedule(*reg2RegCopy);
                    return false;
                } else {
                    return true;
                }
            }
        } else {
            std::cerr << "\t\t\tRenamed source to get connectivity: " <<
                mn.toString() << std::endl;
        }
    } else {
        std::cerr << "\t\t\tCantransportmove ok for: " <<  mn.toString() << std::endl;
    }

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

        // if regcpy is unscheudled it must be rescheduled
        if (t == TempRegBefore) {
            MoveNode* regCopyBefore = regCopiesBefore_[&mn];
            if (regCopyBefore != NULL &&
                pendingBypassSources_.find(regCopyBefore) ==
                pendingBypassSources_.end()) {
                if (!regCopyBefore->isScheduled()) {
                    std::cerr << "scheduling regcopybefore: " <<
                        regCopyBefore->toString() << std::endl;
                    if (!(scheduleMoveBU(
                              *regCopyBefore, 0, rmCycle-1, t))) {
                        std::cerr << "regcopybefore scheduling failed" << 
                            std::endl;
                        unschedule(mn);
                        return false;
                    }
                }
            }
        }
        return true;
    } else {
        std::cerr << "\t\t\trmcycle is -1, cannot schedule move" << std::endl;
        return false;
    }
}

/**
 * Bypasses a node.
 *
 * @param maxHopCount maximum amount or registers to skip while bypassing.
 *        1 == bypass from move which produced the value, 0 == do not bypass.
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
        // bypassing own regcopy away? what if 2?
        if (regCopiesBefore_[&node] == bypassSource.first) {
            MoveNode* rgBefore = regCopiesBefore_[bypassSource.first];
            if (rgBefore != NULL) {
                regCopiesBefore_[&node] = rgBefore;
            }
        }
    }
    
    return bypassSource.second;
}

/**
 * Finds a source where from to bypass. 
 *
 * Goes ddg backwards and checks for connectivity.
 *
 * @return pair of source of bypass and amount of regs skipped by the bypass.
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

/**
 * Tries to schedule an (input) node, bypassing if possible
 */
bool
BypassingBUBasicBlockScheduler::bypassAndScheduleNode(
    MoveNode& node, MoveNode* trigger, int latestCycle, bool allowRegCopies) {
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
                if (scheduleOperation(node.sourceOperation(), lc, false)) {
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
                if (scheduleOperandOrTrigger(
                        node, trigger, latestCycle, allowRegCopies)) {
                    return true;
                } else {
                    std::cerr << "Bypassed operand failed.. trying smaller "
                              << " hop count." << 
                        node.toString() << std::endl;
                        
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
        if (scheduleOperandOrTrigger(
                node, trigger, latestCycle, allowRegCopies)) {
            return true;
        } else {
            return false;
        }
    }
    // for some reason went out from the loop. fail
    return false;
}

/** 
 * Schedule all operands of an operation, (but no trigger), trying t
 * bypass them.
 */
bool 
BypassingBUBasicBlockScheduler::bypassAndScheduleOperands(
    ProgramOperation& po, MoveNode* trigger, int latestCycle, 
    bool allowRegCopies) {
    
    for (int i = 0; i < po.inputMoveCount(); i++) {
        MoveNode& operand = po.inputMove(i);
        if (trigger != &operand) {
            if (!bypassAndScheduleNode(
                    operand, trigger, latestCycle, allowRegCopies)) {
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

/**
 * Tries to schedue operand or trigger, but not trying to bypass
 */
bool 
BypassingBUBasicBlockScheduler::scheduleOperandOrTrigger(
    MoveNode& operand, MoveNode* trigger, int latestCycle, 
    bool allowRegCopies) {
    std::pair<int,int> cycleLimits = 
        operandCycleLimits(operand, trigger);

    return scheduleMoveBU(operand, cycleLimits.first, 
                          std::min(latestCycle,cycleLimits.second), 
                          allowRegCopies ? 
                          TempRegBefore : 
                          TempRegNotAllowed);
}

/**
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

/**
 * find the highest cycle some operand of an operation is scheduled.
 *
 * @TODO: move this to ProgramOperation class?
 */
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

/**
 * undoes a bypass.
 */
void
BypassingBUBasicBlockScheduler::undoBypass(MoveNode& mn) {
    std::cerr << "\tundoing bypass: " << mn.toString() << std::endl;
    MoveNode* source = bypassSources_[&mn];
    if (source != NULL) {
        if (pendingBypassSources_.erase(source)) {
            std::cerr << "undid dre: " << source->toString()
                      << std::endl;
            if (regCopiesBefore_[source] != NULL) {
                assert(regCopiesBefore_[source] == regCopiesBefore_[&mn]);
                regCopiesBefore_[&mn] = source;
                // TODO: what if bypassed over 2 moves?
                // ddg does not support it anyway, not yet a problem
            }
        }
        ddg_->unMerge(*source, mn);
        bypassSources_[&mn] = NULL;
    }
    std::cerr << "\tundid bypass: " << mn.toString() << std::endl;
}

/**
 * Undoes bypass and unschedules a move.
 * 
 * This also recursively unschedules the source operation of the bypass.
 */
void
BypassingBUBasicBlockScheduler::undoBypassAndUnschedule(MoveNode& mn) {
    if (mn.isBypass()) {
        unscheduleOperation(mn.sourceOperation());
    }
    unschedule(mn);
    undoBypass(mn);
}

/*
void
BypassingBUBasicBlockScheduler::revertRegCopyBefore(
    MoveNode& regCopy, MoveNode& mn) {
    ddg->mergeAndKeep(regCopy, mn);
}

void
BypassingBUBasicBlockScheduler::revertRegCopyAfter(
    MoveNode& mn, MoveNode& regCopy) {
    ddg->mergeAndKeep(regCopy, mn);
}
*/

/**
 * Unschedule a move
 *
 * Also unschedules the tempregcopies of that move.
 */
void
BypassingBUBasicBlockScheduler::unschedule(MoveNode& mn) {
    std::cerr << "UnScheduling: " << mn.toString() << std::endl;
    MoveNode* regcopy = regCopiesBefore_[&mn];
    if (regcopy != NULL) {
        unschedule(*regcopy);
    }
    if (mn.isScheduled()) {
        rm_->unassign(mn);
    }
/*
    if (regcopy != NULL) {
        revertRegCopyBefore(mn);
    }
*/
    scheduledMoves_.erase(&mn);
    regcopy = regCopiesAfter_[&mn];
    if (regcopy != NULL) {
        unschedule(*regcopy);
//        revertRegCopyAfter(mn);
    }

    
}

/**
 * Unschedule all result moves of an operation.
 */
void 
BypassingBUBasicBlockScheduler::unscheduleResults(ProgramOperation& po) {
    std::cerr << "\tUnschedulign results" << std::endl;
    for (int i = 0; i < po.outputMoveCount(); i++) {
        MoveNode& mn = po.outputMove(i);
        if (mn.isScheduled()) {
            unschedule(mn);
        }
    }
}
   
/**
 * Unschedules all operands of an operation. 
 * Also reverts bypasses and unschedules the bypass sources.
 */
void
BypassingBUBasicBlockScheduler::unscheduleOperands(
    ProgramOperation& po) {
    for (int i = 0; i < po.inputMoveCount(); i++) {
        undoBypassAndUnschedule(po.inputMove(i));
    }
}

/** 
 * Unschedules an operation (and recursively the bypass sources
 */
void
BypassingBUBasicBlockScheduler::unscheduleOperation(ProgramOperation& po) {
    unscheduleOperands(po);
    unscheduleResults(po);
}

/**
 * Cleanups some bookkeeping. This should not be needed
 */
void
BypassingBUBasicBlockScheduler::clearCaches() {
    bypassSources_.clear();
    for (DataDependenceGraph::NodeSet::iterator i = removedNodes_.begin();
         i != removedNodes_.end(); i++) {
        if (ddg_->rootGraph() != ddg_) {
            if ((**i).isScheduled()) {
                std::cerr << "DRE'd move: " << (**i).toString() << 
                    " is Scheduled! " << std::endl;
            }
            assert(!(**i).isScheduled());
            ddg_->rootGraph()->removeNode(**i);
        }
        delete *i;
    }
    removedNodes_.clear();
    pendingBypassSources_.clear();
    
    regCopiesBefore_.clear();
    regCopiesAfter_.clear();
}                                     

/**
 * Tries to rename source register of unconnected move to make it connected.
 */
bool BypassingBUBasicBlockScheduler::renameSourceIfNotConnected(
    MoveNode& moveNode, int latestCycle) {

    if (!renameRegisters_) {
        return false;
    }

    if (!renamer_->renameSourceRegister(
	    moveNode, false, false, false, latestCycle)) {
        return false;
    }

    if (MachineConnectivityCheck::canTransportMove(
            moveNode, *targetMachine_)) {
        return true;
    } else {
        return false;
    }
    


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

/**
 * Creates a temp reg copy for given move.
 *
 * Does not necessarily create all required temp reg copies, 
 * so this may be needed to be called recursively to get all required copies.
 * 
 * @TODO: Smarted logic to select the register to use
 *
 * @param mn movenode to be splitted
 * @param after set to true if temp reg is the last of the nodes, false if first
 * @return the created tempregcopy
 */
MoveNode*
BypassingBUBasicBlockScheduler::createTempRegCopy(MoveNode& mn, bool after) {

    // before splitting, annotate the possible return move so we can still
    // detect a procedure return in simulator
    if (mn.move().isReturn()) {
        TTAProgram::ProgramAnnotation annotation(
            TTAProgram::ProgramAnnotation::ANN_STACKFRAME_PROCEDURE_RETURN);
        mn.move().setAnnotation(annotation);
    }

    std::set<TTAMachine::RegisterFile*, TTAMachine::MachinePart::Comparator>
        rfs = possibleTempRegRFs(mn, after);

    TTAMachine::RegisterFile* rf = *rfs.rbegin();
    
    int lastRegisterIndex = rf->size()-1;
    TTAMachine::Port* dstRFPort = rf->firstWritePort();
    TTAMachine::Port* srcRFPort = rf->firstReadPort();

    TTAProgram::TerminalRegister* tempWrite =
        new TTAProgram::TerminalRegister(*dstRFPort, lastRegisterIndex);

    TTAProgram::TerminalRegister* tempRead =
        new TTAProgram::TerminalRegister(*srcRFPort, lastRegisterIndex);
    
    TTAProgram::ProgramAnnotation connMoveAnnotation(
        TTAProgram::ProgramAnnotation::ANN_CONNECTIVITY_MOVE);

    TTAProgram::Move* copyMove = mn.move().copy();
    MoveNode* copyNode = new MoveNode(copyMove);
    BasicBlockNode& bbn = ddg_->getBasicBlockNode(mn);
    ddg_->addNode(*copyNode,bbn);
    copyMove->addAnnotation(connMoveAnnotation);

    if (after) {
        std::cerr << "Creating temp move after: " << mn.toString() << " ";
        copyMove->setSource(tempRead);
        mn.move().setDestination(tempWrite);
        RegisterCopyAdder::fixDDGEdgesInTempReg(
            *ddg_, mn, &mn, copyNode, rf, lastRegisterIndex, bbn, true);
        std::cerr << " Moves after split: " << mn.toString() << " and " <<
            copyMove->toString() << std::endl;


    } else {
        std::cerr << "Creating temp move before: " << mn.toString();
        copyMove->setDestination(tempWrite);
        mn.move().setSource(tempRead);

        RegisterCopyAdder::fixDDGEdgesInTempReg(
            *ddg_, mn, copyNode, &mn, rf, lastRegisterIndex, bbn, true);

        std::cerr << "Moves after split: " << copyMove->toString()
                  << " and "<< mn.toString() << std::endl;

        createAntidepsFromUnscheduledRegCopies(*copyNode, mn, *tempWrite);
    }

    return copyNode;
}

/**
 * Creste register antideps from unscheduled temp reg copies
 * to the just created one.
 * 
 * This is needed when temp reg copies are "created on wrong order",
 * for example if some operation is attempted to first schedule some FU,
 * later to some another
 */
void BypassingBUBasicBlockScheduler::createAntidepsFromUnscheduledRegCopies(
    MoveNode& copyNode, MoveNode& mn, 
    TTAProgram::Terminal& terminalRegister) {
    TCEString reg = DisassemblyRegister::registerName(terminalRegister);

    for (std::map<MoveNode*, MoveNode*, MoveNode::Comparator>::iterator i =
             regCopiesBefore_.begin(); i != regCopiesBefore_.end(); i++) {
        // found non-scheduled move having regcopies..
        if (!i->first->isScheduled() && i->first != &mn) {
            if (i->first->isSourceVariable()) {
                TCEString reg1 = DisassemblyRegister::registerName(
                    i->first->move().source());
                if  (reg1 == reg) {
                    DataDependenceEdge* edgeWar = 
                        new DataDependenceEdge(
                            DataDependenceEdge::EDGE_REGISTER,
                            DataDependenceEdge::DEP_WAR, reg);
                    ddg_->connectOrDeleteEdge(*i->first, copyNode, edgeWar);

                    DataDependenceEdge* edgeWaw = 
                        new DataDependenceEdge(
                            DataDependenceEdge::EDGE_REGISTER,
                            DataDependenceEdge::DEP_WAW, reg);
                    ddg_->connectOrDeleteEdge(*i->second, copyNode, edgeWaw);
                }
            }
        }
    }
}

/**
 * Find possible temp reg RF's for connectivity of given register.
 *
 * This only gives the register files that for the "next register in the
 * temp reg chain", not the whole chain
 */
std::set<TTAMachine::RegisterFile*, TTAMachine::MachinePart::Comparator> 
BypassingBUBasicBlockScheduler::possibleTempRegRFs(
    const MoveNode& mn, bool tempRegAfter) {
    
    std::set<TTAMachine::RegisterFile*, TTAMachine::MachinePart::Comparator>
        result;

    std::map<int, int> rfDistanceFromSource;
    std::map<int, int> rfDistanceFromDestination;

    typedef SimpleInterPassDatum<
    std::vector<std::pair<TTAMachine::RegisterFile*, int> > > 
        TempRegData;

    std::string srDatumName = "SCRATCH_REGISTERS";
    if (!BasicBlockPass::interPassData().hasDatum(srDatumName) ||
        (dynamic_cast<TempRegData&>(
            BasicBlockPass::interPassData().datum(srDatumName))).size() == 0)
        throw IllegalProgram(
            __FILE__, __LINE__, __func__,
            "No scratch registers available for temporary moves.");

    const TempRegData& tempRegs = 
        dynamic_cast<TempRegData&>(
            BasicBlockPass::interPassData().datum(srDatumName));


    for (unsigned int i = 0; i < tempRegs.size(); i++) {
        rfDistanceFromSource[i] = INT_MAX;
        rfDistanceFromDestination[i] = INT_MAX;
    }


    for (unsigned int i = 0; i < tempRegs.size(); i++) {
        TTAMachine::RegisterFile* rf = tempRegs[i].first;
        std::set<const TTAMachine::Port*> readPorts = 
            MachineConnectivityCheck::findReadPorts(*rf);
        std::set<const TTAMachine::Port*> writePorts = 
            MachineConnectivityCheck::findWritePorts(*rf);
        bool srcOk = false;
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
                TTAMachine::RegisterFile* rfSrc = tempRegs[i].first;
                for (unsigned int j = 0; j < tempRegs.size(); j++) {
                    if (rfDistanceFromSource[j] > srcDist + 1) {
                        TTAMachine::RegisterFile* rfDest = tempRegs[j].first;
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
                TTAMachine::RegisterFile* rfDst = tempRegs[i].first;
                for (unsigned int j = 0; j < tempRegs.size(); j++) {
                    if (rfDistanceFromDestination[j] > dstDist + 1) {
                        TTAMachine::RegisterFile* rfSrc = tempRegs[j].first;
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
