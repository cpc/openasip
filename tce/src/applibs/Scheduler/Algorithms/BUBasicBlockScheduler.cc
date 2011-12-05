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
 * @file BUBasicBlockScheduler.cc
 *
 * Definition of BUBasicBlockScheduler class.
 *
 * @author Vladimir Guzma 2011 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include <set>
#include <string>

#include "BUBasicBlockScheduler.hh"
#include "ControlFlowGraph.hh"
#include "DataDependenceGraph.hh"
#include "DataDependenceGraphBuilder.hh"
#include "SimpleResourceManager.hh"
#include "BUMoveNodeSelector.hh"
#include "POMDisassembler.hh"
#include "Procedure.hh"
#include "ProgramOperation.hh"
#include "ControlUnit.hh"
#include "Machine.hh"
#include "UniversalMachine.hh"
#include "Guard.hh"
#include "MapTools.hh"
#include "Instruction.hh"
#include "InstructionReference.hh"
#include "BasicBlock.hh"
#include "ControlFlowGraphPass.hh"
#include "RegisterCopyAdder.hh"
#include "SchedulerPass.hh"
#include "CycleLookBackSoftwareBypasser.hh"
#include "LLVMTCECmdLineOptions.hh"
#include "InterPassData.hh"
#include "MoveNodeSet.hh"
#include "Terminal.hh"
#include "RegisterRenamer.hh"
#include "HWOperation.hh"

//#define DEBUG_OUTPUT
//#define DEBUG_REG_COPY_ADDER
//#define CFG_SNAPSHOTS
//#define BIG_DDG_SNAPSHOTS
//#define DDG_SNAPSHOTS
//#define SW_BYPASSING_STATISTICS

class CopyingDelaySlotFiller;

/**
 * Constructs the basic block scheduler.
 *
 * @param data Interpass data
 * @param bypasser Helper module implementing software bypassing
 * @param delaySlotFiller Helper module implementing jump delay slot filling
 * @param registerRenamer Helper module implementing register renaming 
 */
BUBasicBlockScheduler::BUBasicBlockScheduler(
    InterPassData& data, 
    SoftwareBypasser* bypasser, 
    CopyingDelaySlotFiller* delaySlotFiller,
    RegisterRenamer* renamer) :
    BasicBlockScheduler(data, bypasser, delaySlotFiller, renamer),
    ddg_(NULL), rm_(NULL), softwareBypasser_(bypasser), renamer_(renamer),
    bypassedCount_(0), deadResults_(0), endCycle_(INT_MAX) {

    CmdLineOptions *cmdLineOptions = Application::cmdLineOptions();
    options_ = dynamic_cast<LLVMTCECmdLineOptions*>(cmdLineOptions);
}

/**
 * Destructor.
 */
BUBasicBlockScheduler::~BUBasicBlockScheduler() {
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
BUBasicBlockScheduler::handleDDG(
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
    endCycle_ = (ddg.nodeCount() < 1000) ? ddg.nodeCount() + 10 : 1000;
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
            scheduleOperation(moves);
        } else {
            if (firstMove.move().destination().isRA()) {
                scheduleMove(firstMove, endCycle_);
            } else {
                scheduleRRMove(firstMove);
            }
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
BUBasicBlockScheduler::scheduleOperation(MoveNodeGroup& moves)
    throw (Exception) {

    bool operandsFailed = true;
    bool resultsFailed = true;
    int resultsStartCycle = endCycle_;

    int maxResult = resultsStartCycle;
    while ((operandsFailed || resultsFailed) &&
        resultsStartCycle >= 0) {
        maxResult = scheduleResultReads(moves, resultsStartCycle);
        if (maxResult != -1) {
            debugLog("Results scheduled happily.");
            resultsFailed = false;
        } else {
            // At least one of the results did not get scheduled correctly,
            // We try with earlier starting cycle.
            // Drawback, in case of 3 results, 1 could be scheduled very late,
            // second bit earlier and third failing because of location of
            // second scheduled result, not first. 
            // Better solution would be to try to push scheduled results up 
            // individually, but that would be rather slow process.
            for (int i = 0; i < moves.nodeCount(); i++){
                MoveNode& moveNode = moves.node(i);
                if (moveNode.isScheduled() &&
                    moveNode.isSourceOperation()) {
                    unschedule(moveNode);
                }
            }
            resultsFailed = true;            
            // We will try to schedule results in earlier cycle
            resultsStartCycle--;
            continue;            
        }
        
        if (scheduleOperandWrites(moves, resultsStartCycle)) {
            debugLog("Operands scheduled happily.");
            operandsFailed = false;
        } else {
            // Scheduling some operand failed, unschedule all moves, try earlier 
            for (int i = 0; i < moves.nodeCount(); i++){
                MoveNode& moveNode = moves.node(i);
                if (moveNode.isScheduled()) {
                    unschedule(moveNode);
                }                
            }
            operandsFailed = true;
            resultsStartCycle--;
            maxResult--;
            resultsStartCycle = std::min(maxResult, resultsStartCycle);            
        }
    }
    // This fails if we reach 0 cycle. Probably means that the original scope
    // was too small? TODO: better heuristics.
    if (resultsFailed) {
        ddg_->writeToDotFile(
             (boost::format("bb_%s_2_failed_scheduling.dot")
              % ddg_->name()).str());    
        throw ModuleRunTimeError(
             __FILE__, __LINE__, __func__, 
             "Results scheduling failed for \'" + moves.toString());
    }
    
    if (operandsFailed) { 
        ddg_->writeToDotFile(
            (boost::format("bb_%s_2_failed_scheduling.dot")
             % ddg_->name()).str());    
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__, 
            "Operands scheduling failed for \'" + moves.toString());
    }

#ifdef DEBUG_REG_COPY_ADDER
    if (tempsAdded > 0) {
        ddg_->writeToDotFile(
            (boost::format("%s_after_scheduler_ddg.dot") % 
             ddg_->name()).str());
        Application::logStream()
            << "(operation fix #" << ddg_->name() << ")" << std::endl
            << std::endl;
        ++graphCount;
    }
#endif
}

/**
 * Schedules operand moves of an operation execution.
 *
 * Assumes the given MoveNodeGroup contains all moves in the operation
 * execution. Also assumes that all result moves of the MoveNodeGroup have
 * been scheduled. 
 * Assumes bottom up scheduling.
 *
 * @param moves Moves of the operation execution.
 * @param cycle latest cycle for starting scheduling of operands 
 * @return True if all operands got scheduled
 */
bool
BUBasicBlockScheduler::scheduleOperandWrites(
    MoveNodeGroup& moves, 
    int cycle)
    throw (Exception) {

    int lastOperandCycle = 0;
    int latestScheduledOperand = 0;
    // Counts operands that are not scheduled at beginning.
    int unscheduledMoves = 0;
    MoveNode* trigger = NULL;

    debugLog(
        "Scheduling: " + moves.toString() + "in cycle " +
        Conversion::toString(cycle));       
    for (int i = 0; i < moves.nodeCount(); i++) {
        if (!moves.node(i).isDestinationOperation()) {
            continue;
        }
        // count how many operand moves will need to be scheduled
        unscheduledMoves++;
        if (moves.node(i).move().destination().isTriggering()) {
            int limit = moves.node(i).latestTriggerWriteCycle();
            cycle = (limit < cycle) ? limit : cycle;
        }
    }
    // Find and schedule moveNode which has highest "latest Cycle"
    // other moveNodes could be scheduled in earlier cycle but this
    // makes sure there will be no problems with operands overwritting
    // and assignment failures. At least it reduced a count of such.
    int scheduledMoves = 0;    
    for (int i = 0; i < moves.nodeCount(); i++) {
        if (!moves.node(i).isDestinationOperation()) {
            continue;
        }

        int latestDDG = ddg_->latestCycle(moves.node(i));
        // passed argument could be larger than what DDG thinks is earliest
        latestDDG = std::min(latestDDG, cycle);
        int latest = rm_->latestCycle(latestDDG, moves.node(i));        
        if (latest == -1) {
            continue;
        }
        // This must pass, since we got latest cycle from RM.
        scheduleMove(moves.node(i), latest);      
          
        if (moves.node(i).isScheduled() && 
            moves.node(i).move().destination().isTriggering()) {
            // We schedulled trigger, this will give us latest possible cycle
            // in order not to have operands later then trigger.       
            debugLog("Found trigger move " + moves.node(i).toString());
            trigger = &moves.node(i);
            cycle = moves.node(i).cycle();
            lastOperandCycle = cycle;
            scheduledMoves++;
            break;
        } else {
            // Not trigger move, we will schedule it again after trigger is 
            // found.
            unschedule(moves.node(i));
        }
    }

    if (trigger == NULL) {   
        // Can not schedule trigger, will try again later.
        return false;
    }

    int counter = 0;

    // Loops till all moveNodes are scheduled or "timeouts"
    // remove timeout when software bypassing is tested and guaranteed to work
    // TODO: remove this kind of kludges. They just await for code that
    // breaks them.
    while (unscheduledMoves != scheduledMoves && counter < 2 && cycle >=0) {
        // try to schedule all input moveNodes, also find trigger
        for (int moveIndex = 0; moveIndex < moves.nodeCount(); ++moveIndex) {
            MoveNode& moveNode = moves.node(moveIndex);
            // skip the result reads
            if (!moveNode.isDestinationOperation()) {
                continue;
            }

            if (moveNode.isScheduled()) {
                if (moveNode.move().destination().isTriggering()) {
                    trigger = &moveNode;
                }
                continue;
            }
            debugLog("Trying to schedule " + moveNode.toString() +
                " in " + Conversion::toString(cycle));
            scheduleMove(moveNode, cycle);
            debugLog("Result: " + moveNode.toString());
            if (moveNode.isScheduled()) {
                lastOperandCycle =
                    std::max(lastOperandCycle, moveNode.cycle());
                if (moveNode.move().destination().isTriggering()) {
                    trigger = &moveNode;
                }
            } 
        }
        // Tests if all input moveNodes were scheduled
        // If trigger is earlier then some operand reschedule
        scheduledMoves = 0;
        latestScheduledOperand = 0;
        for (int moveIndex = 0; moveIndex < moves.nodeCount(); ++moveIndex) {
            MoveNode& moveNode = moves.node(moveIndex);
            // skip the result reads
            if (!moveNode.isDestinationOperation()) {
                continue;
            }
            // If trigger is too early, try to reschedule it
            if (moveNode.isScheduled() &&
                moveNode.move().destination().isTriggering()) {
                trigger = &moveNode;

                int triggerMinCycle = lastOperandCycle;
                if (ddg_->hasNode(moveNode)) {
                    // handle moving fu deps. 
                    // they may move trigger to later time.
                    ddg_->moveFUDependenciesToTrigger(*trigger);
                    int triggerEarliest = ddg_->latestCycle(*trigger);
                    triggerMinCycle = 
                        std::max(triggerEarliest, triggerMinCycle);
                }
                
                // triger is earlier than operand OR some new fu dep edges
                // make trigger's earliestcycle later.
                if (moveNode.cycle() < lastOperandCycle) {
                    unschedule(moveNode);
                    debugLog(
                        "Rescheduling " + moveNode.toString() +
                        Conversion::toString(triggerMinCycle));
                    scheduleMove(moveNode, std::min(cycle, triggerMinCycle));
                }
            }
            if (moveNode.isScheduled()) {
                lastOperandCycle =
                    std::max(lastOperandCycle, moveNode.cycle());
                latestScheduledOperand =
                    std::max(latestScheduledOperand, moveNode.cycle());
                scheduledMoves++;
            }
        }

        // Some moveNodes were not scheduled
        // unschedule all and try with higher start cycle
        if (scheduledMoves != unscheduledMoves) {
            for (int i = 0; i < moves.nodeCount(); i++){
                MoveNode& moveNode = moves.node(i);

                if (moveNode.isScheduled() &&
                    moveNode.isDestinationOperation()) {
                    debugLog("Unscheduling " + moveNode.toString());
                    unschedule(moveNode);
                }
            }
            scheduledMoves = 0;
        } else {
            // every operand is scheduled, we can return quickly
            return true;
        }        
        if (cycle > latestScheduledOperand) {
            cycle = latestScheduledOperand;
        } else {
            cycle--;
        }
        counter++;
    }
    // If loop timeouts we get here
    if (scheduledMoves != unscheduledMoves) {
        return false;
    }
    return true;
}

/**
 * Schedules the result read moves of an operation execution.
 *
 * Assumes the given MoveNodeGroup contains all moves in the operation
 * execution.
 *
 * @param moves Moves of the operation execution.
 * @return Last cycle in which any of the results was scheduled
 */
int
BUBasicBlockScheduler::scheduleResultReads(MoveNodeGroup& moves, int cycle) 
    throw (Exception) {
    
    int startingCycle = cycle;
    int maxResultCycle = cycle;
    
    for (int moveIndex = 0; moveIndex < moves.nodeCount(); ++moveIndex) {
        if (!moves.node(moveIndex).isSourceOperation()) {
            continue;
        }    
        MoveNode& moveNode = moves.node(moveIndex);

        if (!moveNode.isScheduled()) {
            if (!moveNode.isSourceOperation()) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    (boost::format("Move to schedule '%s' is not "
                    "result move!") % moveNode.toString()).str());                
            }
            int latestDDGCycle = ddg_->latestCycle(moveNode);
            startingCycle = 
                (latestDDGCycle != -1) ? 
                    std::min(latestDDGCycle, startingCycle) : 
                    startingCycle;
            int latestRMCycle = rm_->latestCycle(startingCycle, moveNode);
            // Node can not be scheduled at all, probably caused by
            // other results scheduled too late, or missing connection.
            if (latestRMCycle == -1) {
                return -1;
            }

            scheduleMove(moveNode, latestRMCycle);
            
            if (!moveNode.isScheduled()) {
                return -1;
            }
            // Find latest schedule result cycle
            maxResultCycle = 
                (moveNode.cycle() > maxResultCycle) ? 
                    moveNode.cycle() : maxResultCycle;
        }
    }
    return maxResultCycle;
}

/**
 * Schedules moves in a single operation execution.
 *
 * Assumes the given MoveNodeGroup contains all moves in the operation
 * execution. Also assumes that all inputs to the MoveNodeGroup have
 * been scheduled.
 *
 * @param moveNode R-R Move to schedule.
 */
void
BUBasicBlockScheduler::scheduleRRMove(MoveNode& moveNode)
    throw (Exception) {
    
#ifdef DEBUG_REG_COPY_ADDER
    ddg_->setCycleGrouping(true);
    ddg_->writeToDotFile(
        (boost::format("%s_before_ddg.dot") % ddg_->name()).str());
#endif

    RegisterCopyAdder regCopyAdder(BasicBlockPass::interPassData(), *rm_);

#ifdef DEBUG_REG_COPY_ADDER
    const int tempsAdded =
#endif

        regCopyAdder.addRegisterCopiesToRRMove(moveNode, ddg_)
#ifdef DEBUG_REG_COPY_ADDER
        .count_
#endif    
        ;
#ifdef DEBUG_REG_COPY_ADDER
    if (tempsAdded > 0) {
        ddg_->writeToDotFile(
            (boost::format("%s_after_regcopy_ddg.dot") % ddg_->name()).str());
    }
    ddg_->sanityCheck();
#endif

    scheduleMove(moveNode, endCycle_);

}

/**
 * Schedules a single move to the earliest possible cycle, taking in
 * account the DDG, resource constraints, and latencies in producing
 * source values.
 *
 * This method assumes the move is possible to schedule with regards to
 * connectivity and resources. Short immediates are converted to long
 * immediates when needed.
 *
 * @param move The move to schedule.
 * @param earliestCycle The earliest cycle to try.
 */
void
BUBasicBlockScheduler::scheduleMove(
    MoveNode& moveNode,
    int latestCycle)
    throw (Exception) {

    if (moveNode.isScheduled()) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            (boost::format("Move '%s' is already scheduled!") 
            % moveNode.toString()).str());                
    }            
#if 0
    int sourceReadyCycle = endCycle_;
#endif    
    int ddgCycle = endCycle_;
    debugLog("Scheduling " + moveNode.toString() + " in cycle " +
        Conversion::toString(latestCycle));
    if (moveNode.move().isControlFlowMove()) {
        ddgCycle = endCycle_ - targetMachine_->controlUnit()->delaySlots();
#if 0        
        assert(
            latestCycle == ddgCycle && 
            "Trying to schedule CFG move ignoring delay slots.");
#endif            
    } else { // not a control flow move:
        ddgCycle = ddg_->latestCycle(moveNode);
#if 0        
        if (renamer_ != NULL) {
            int minRenamedEC = std::max(
                sourceReadyCycle, ddg_->earliestCycle(
                    moveNode, INT_MAX, true, true)); // TODO: 0 or INT_MAX

            // rename if can and may alow scheuduling earlier.
            if (renamer_ != NULL && minRenamedEC > ddgCycle) {
                minRenamedEC =  rm_->latestCycle(minRenamedEC, moveNode);
                if (minRenamedEC < ddgCycle) {
                    
                    if (renamer_->renameDestinationRegister(
                            moveNode, false, minRenamedEC)) {
                        ddgCycle = ddg_->latestCycle(moveNode);
                    }
                    else {
                        MoveNode *limitingAdep =
                            ddg_->findLimitingAntidependenceSource(
                                moveNode);
                        if (limitingAdep != NULL) {
                            // don't try to rename is same operation.
                            // as it would not help at all.
                            if (!moveNode.isSourceOperation() ||
                                !limitingAdep->isDestinationOperation() ||
                                &moveNode.sourceOperation() !=
                                &limitingAdep->destinationOperation()) {
                                    if (renamer_->renameSourceRegister(
                                        *limitingAdep, false)) {
                                        ddgCycle = ddg_->latestCycle(
                                        moveNode);
                                    }
                            }
                        }
                    }
                }
            }
        }
#endif        
    }

    // if it's a conditional move then we have to be sure that the guard
    // is defined before executing the move.
    // this is already handled by DDGs earliestCycle, except cases
    // where the guard is defined in a previous BB. 
    // So this prevents scheduling conditional moves at the beginning
    // of a BB.
   
    if (!moveNode.move().isUnconditional()) {
        ddgCycle = std::max(ddgCycle, moveNode.guardLatency()-1);
    }

    // Earliest cycle from which to start, could depend on result ready
    // for result moves.
    debugLog(
        "LatestCycle = " + Conversion::toString(latestCycle) +
        " DDGCycle = " + Conversion::toString(ddgCycle));
    int minCycle = 
        (ddgCycle != -1) ? std::min(latestCycle, ddgCycle) : latestCycle;
    if (moveNode.isSourceConstant() && 
        !moveNode.move().hasAnnotations(
            TTAProgram::ProgramAnnotation::ANN_REQUIRES_LIMM)) {
        // If source is constant and node does not have annotation already,
        // we add it if constant can not be transported so IU broker and 
        // OutputPSocket brokers will add Immediate
        // Example : 999999 -> integer0.2
        if (!rm_->canTransportImmediate(moveNode)){
            TTAProgram::ProgramAnnotation annotation(
                TTAProgram::ProgramAnnotation::ANN_REQUIRES_LIMM);
            moveNode.move().setAnnotation(annotation); 
            
        } else if (!moveNode.isDestinationOperation() &&
                   rm_->latestCycle(endCycle_, moveNode) == -1) {
            // If source is constant and node does not have annotation already,
            // we add it if node has no connection, so IU broker and 
            // OutputPSocket brokers will add Immediate
            // Example: 27 -> integer0.2
            // With bus capable of transporting 27 as short immediate but
            // no connection from that bus to integer0 unit
            TTAProgram::ProgramAnnotation annotation(
                TTAProgram::ProgramAnnotation::ANN_REQUIRES_LIMM);
            moveNode.move().setAnnotation(annotation);    
        }
    } 
    // annotate the return move otherwise it might get undetected in the
    // simulator after the short to long immediate conversion and thus
    // stopping simulation automatically might not work
    if (moveNode.isSourceConstant() &&
        moveNode.move().isReturn() && 
        !rm_->canTransportImmediate(moveNode)) {
        TTAProgram::ProgramAnnotation annotation(
            TTAProgram::ProgramAnnotation::ANN_STACKFRAME_PROCEDURE_RETURN);
        moveNode.move().setAnnotation(annotation);
    }
    
    minCycle = std::min(rm_->latestCycle(minCycle, moveNode), minCycle);
    debugLog("RM latestCycle =" + Conversion::toString(minCycle));
    if (minCycle == -1 || minCycle == INT_MAX) {
        if (moveNode.isSourceConstant() &&
            !moveNode.isDestinationOperation() &&
            moveNode.move().hasAnnotations(
                TTAProgram::ProgramAnnotation::ANN_REQUIRES_LIMM)) {
            // If earliest cycle returns -1 and source is constant and moveNode
            // needs long immediate there is most likely missing long immediate
            // unit            
            std::string msg = "Assignment of MoveNode " + moveNode.toString();
            msg += " failed! Most likely missing Long Immediate Unit";
            msg += " or Instruction Template!";
            throw IllegalMachine(
                __FILE__, __LINE__, __func__, msg);            
        }
        return;
    }
    rm_->assign(minCycle,  moveNode);
    if (!moveNode.isScheduled()) {
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__, 
            (boost::format("Assignment of MoveNode '%s' failed!") 
            % moveNode.toString()).str());
    }
    // Find out the cycle when execution of operation actually ends.
    // Only needed for ProgramOperations without result reads, like store.
#if 0    
    if (moveNode.isDestinationOperation()) {
    
        const Operation& op = moveNode.destinationOperation().operation();
        const TTAMachine::HWOperation& hwop = 
            *moveNode.move().destination().functionUnit().operation(op.name());
            
        unsigned int epEndCycle = moveNode.cycle() + hwop.latency();
        // The pipeline will end after current the final cycle, 
        // have to scheduler earlier.
        if (epEndCycle > endCycle_) {
            debugLog("Unassigning move " + moveNode.toString() + " " +
                Conversion::toString(moveNode.cycle()) + " " +
                Conversion::toString(hwop.latency()) + " " +
                Conversion::toString(endCycle_));
            rm_->unassign(moveNode);
        }
    }
#endif    
}


/** 
 * Prints DDG to a dot file before and after scheudling
 * 
 * @param ddg to print
 * @param name operation name for ddg
 * @param final specify if ddg is after scheduling
 * @param resetCounter
 * @param format format of the file
 * @return number of cycles/instructions copied.
 */
void
BUBasicBlockScheduler::ddgSnapshot(
        DataDependenceGraph& ddg,
        const std::string& name,
        DataDependenceGraph::DumpFileFormat format,
        bool final,
        bool resetCounter) const {

    static int bbCounter = 0;

    if (resetCounter) {
        bbCounter = 0;
    }

    if (final) {
        if (format == DataDependenceGraph::DUMP_DOT) {
            ddg.writeToDotFile(
                (boost::format("bb_%s_%s_after_scheduling.dot") % 
                 ddg_->name() % name).str());
        } else {
            ddg.writeToXMLFile(
                (boost::format("bb_%s_%s_after_scheduling.xml") %
                 ddg_->name() % name).str());
        }
        
    } else {
        if (format == DataDependenceGraph::DUMP_DOT) {
            ddg.writeToDotFile(
                (boost::format("bb_%s_%d_%s_before_scheduling.dot")
                 % ddg.name() % bbCounter % name).str());
        } else {
            ddg.writeToXMLFile(
                (boost::format("bb_%s_%d_%s_before_scheduling.xml")
                 % ddg.name() % bbCounter % name).str());
        }
    }
    ++bbCounter;
}

/**
 * Unschedules the given move.
 *
 * Also restores a possible short immediate source in case it was converted
 * to a long immediate register read during scheduling.
 *
 * @param moveNode Move to unschedule.
 */
void
BUBasicBlockScheduler::unschedule(MoveNode& moveNode) {    
    if (!moveNode.isScheduled()) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            (boost::format("Trying to unschedule move '%s' which "
            "is not scheduled!") % moveNode.toString()).str());
    }
    rm_->unassign(moveNode);
    if (moveNode.move().hasAnnotations(
        TTAProgram::ProgramAnnotation::ANN_REQUIRES_LIMM)) {
        // If we added annotation during scheduleMove delete it
        moveNode.move().removeAnnotations(
        TTAProgram::ProgramAnnotation::ANN_REQUIRES_LIMM);
    }    
    if (moveNode.isScheduled() || moveNode.isPlaced()) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            (boost::format("Unscheduling of move '%s' failed!") 
            % moveNode.toString()).str());
    }
}

/**
 * A short description of the pass, usually the optimization name,
 * such as "basic block scheduler".
 *
 * @return The description as a string.
 */
std::string
BUBasicBlockScheduler::shortDescription() const {
    return "Bottom-up list scheduler with a basic block scope.";
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
BUBasicBlockScheduler::longDescription() const {
    return
        "Bottom-up list basic block scheduler that uses the longest path "
        "information of data dependency graph to prioritize the ready list."
        "Assumes that the input has registers allocated and no connectivity "
        "missing.";
}

/**
 * Notifies to the selector that given nodes and their temp reg copies are
 * scheduled .
 * 
 * @param nodes nodes which are scheduled.
 * @param selector selector which to notify.
 */
void BUBasicBlockScheduler::notifyScheduled(
    MoveNodeGroup& moves, MoveNodeSelector& selector) {
    
    for (int moveIndex = 0; moveIndex < moves.nodeCount(); ++moveIndex) {
        MoveNode& moveNode = moves.node(moveIndex);
        selector.notifyScheduled(moveNode);
    }
}
