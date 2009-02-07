/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file BasicBlockScheduler.cc
 *
 * Definition of BasicBlockScheduler class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <set>
#include <string>

#include "BasicBlockScheduler.hh"
#include "ControlFlowGraph.hh"
#include "DataDependenceGraph.hh"
#include "DataDependenceGraphBuilder.hh"
#include "SimpleResourceManager.hh"
#include "CriticalPathBBMoveNodeSelector.hh"
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
#include "CopyingDelaySlotFiller.hh"

//#define DEBUG_OUTPUT
//#define DEBUG_REG_COPY_ADDER
//#define CFG_SNAPSHOTS
//#define BIG_DDG_SNAPSHOTS
//#define DDG_SNAPSHOTS
//#define SW_BYPASSING_STATISTICS

/**
 * Constructs the basic block scheduler.
 *
 * @param data Interpass data
 * @param bypasser Helper module implementing software bypassing
 * @param delaySlotFiller Helper module implementing jump delay slot filling 
 */
BasicBlockScheduler::BasicBlockScheduler(
    InterPassData& data, 
    SoftwareBypasser* bypasser, 
    CopyingDelaySlotFiller* delaySlotFiller) :
    BasicBlockPass(data), ControlFlowGraphPass(data), ProcedurePass(data),
    ProgramPass(data), DDGPass(data), ddg_(NULL), bigDDG_(NULL), rm_(NULL),
    softwareBypasser_(bypasser), delaySlotFiller_(delaySlotFiller),
    bypassedCount_(0), deadResults_(0) {
}

/**
 * Destructor.
 */
BasicBlockScheduler::~BasicBlockScheduler() {
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
BasicBlockScheduler::handleDDG(
    DataDependenceGraph& ddg,
    SimpleResourceManager& rm,
    const TTAMachine::Machine& targetMachine)
    throw (Exception) {

    ddg_ = &ddg;
    targetMachine_ = &targetMachine;

#ifdef DDG_SNAPSHOTS
    static int bbCounter = 0;
    ddg_->writeToDotFile(
        (boost::format("bb_%s_0_before_scheduling.dot") % ddg_->name()).str());
    Application::logStream() << "\nBB " << bbCounter << std::endl;
#endif
    
    scheduledTempMoves_.clear();

    // empty need not to be scheduled
    if ( ddg.nodeCount() == 0 || 
         (ddg.nodeCount() == 1 && !ddg.node(0).isMove())) {
        return;
    }

    CriticalPathBBMoveNodeSelector selector(ddg, targetMachine);

    // register selector to bypasser for notfications.
    if (softwareBypasser_ != NULL) {
        softwareBypasser_->setSelector(&selector);
    }

    rm_ = &rm;

    MoveNodeGroup moves = selector.candidates();
    while (moves.nodeCount() > 0) {

        MoveNode& firstMove = moves.node(0);
        if (firstMove.isOperationMove()) {
            scheduleOperation(moves);
        } else {
            scheduleMove(firstMove, 0);
        }

        if (!moves.isScheduled()) {
            std::string message = " Move(s) did not get scheduled: ";
            for (int i = 0; i < moves.nodeCount(); i++) {
                message += moves.node(i).toString() + " ";
            }
            throw ModuleRunTimeError(__FILE__, __LINE__, __func__, message);
        }

        // notifies successors of the scheduled moves.
        notifyScheduled(moves, selector);

        moves = selector.candidates();
    }

    if (softwareBypasser_ != NULL) {
        softwareBypasser_->clearCaches();
    }

    if (ddg.nodeCount() !=
        ddg.scheduledNodeCount()) {
        debugLog("All moves in the DDG didn't get scheduled.");
//        debugLog("Disassembly of the situation:");
//        Application::logStream() << bb.disassemble() << std::endl;
        ddg_->writeToDotFile("failed_bb.dot");
        abortWithError("Should not happen!");
    }
#ifdef DDG_SNAPSHOTS
    ddg_->writeToDotFile(
        (boost::format("bb_%s_0_after_scheduling.dot") % ddg_->name()).str());
    Application::logStream() << "\nBB " << bbCounter << std::endl;
    bbCounter++;
#endif
    
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
BasicBlockScheduler::handleBasicBlock(
    BasicBlock& bb, 
    const TTAMachine::Machine& targetMachine)
    throw (Exception) {

    if (bb.instructionCount() == 0)
        return;


    BasicBlockPass::executeDDGPass(bb, targetMachine, *this);
}

#ifdef DEBUG_REG_COPY_ADDER
static int graphCount = 0;
#endif

/**
 * Schedules moves in a single operation execution.
 *
 * Assumes the given MoveNodeGroup contains all moves in the operation
 * execution. Also assumes that all inputs to the MoveNodeGroup have
 * been scheduled.
 *
 * @param moves Moves of the operation execution.
 */
void
BasicBlockScheduler::scheduleOperation(MoveNodeGroup& moves)
    throw (Exception) {

    ProgramOperation& po =
        (moves.node(0).isSourceOperation())?
        (moves.node(0).sourceOperation()):
        (moves.node(0).destinationOperation());

#ifdef DEBUG_REG_COPY_ADDER
    ddg_->setCycleGrouping(true);
    ddg_->writeToDotFile(
        (boost::format("%s_before_ddg.dot") % ddg_->name()).str());
#endif

    RegisterCopyAdder regCopyAdder(BasicBlockPass::interPassData(), *rm_);

#ifdef DEBUG_REG_COPY_ADDER
    const int tempsAdded =
#endif

        regCopyAdder.addMinimumRegisterCopies(po, *targetMachine_, ddg_)
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

    MoveNodeSet tempSet;
    for (int i = 0; i < moves.nodeCount(); i++) {
        // MoveNodeGroup relates to DDG so we copy it to more
        // simple MoveNodeSet container
        tempSet.addMoveNode(moves.node(i));
    }

    bool operandsFailed = true;
    bool resultsFailed = true;
    int operandsStartCycle = 0;

    int maxFromRm = rm_->largestCycle();

    // Magic number defining how many times we should try to schedule
    // operands and results. Should not be needed in final version of
    // scheduler
    const int retryCount = 20;
    int minOperand = operandsStartCycle;
    bool tryBypassing = softwareBypasser_ != NULL;

    while ((operandsFailed || resultsFailed) &&
        operandsStartCycle < maxFromRm + retryCount) {

        minOperand = scheduleOperandWrites(operandsStartCycle, moves);
        if (minOperand != -1) {
            operandsFailed = false;
        } else {
            // Scheduling some operand failed, unschedule and try later cycle
            for (int i = 0; i < moves.nodeCount(); i++){
                MoveNode& moveNode = moves.node(i);
                if (moveNode.isScheduled() &&
                    moveNode.isDestinationOperation()) {
                    unschedule(moveNode);
                    unscheduleInputOperandTempMoves(moveNode);
                    }
            }
            operandsFailed = true;
            operandsStartCycle++;
            continue;
        }

        int bypassedMoves = -1;
        if (tryBypassing) {
            bypassedMoves = softwareBypasser_->bypass(moves, *ddg_, *rm_);
            if (bypassedMoves == -1){
                // bypassing failed try again without attempt to bypass
                // this restores the original schedule of operands
                // and disable bypassing
                tryBypassing = false;
                operandsFailed = true;
                softwareBypasser_->removeBypass(moves, *ddg_, *rm_);
                continue;
            }
            // bypass was success
            bypassedCount_ += bypassedMoves;
        }

        if (scheduleResultReads(moves)) {
            resultsFailed = false;
            // Results were scheduled, we are not going to change schedule
            // of this program operation. Lets check if some of the
            // bypassed operands did not produce dead result moves
            if (tryBypassing) {
                try{
                    deadResults_ +=
                        softwareBypasser_->removeDeadResults(
                            moves, *ddg_, *rm_);
                } catch (const Exception& e) {
                    throw ModuleRunTimeError(
                        __FILE__, __LINE__, __func__, e.errorMessageStack());
                }
            }
        } else {
            // Scheduling results failed, most likely due to large distance
            // between trigger and earliest result cycle
            // Some other operation is overwritting result on same FU
            for (int i = 0; i < moves.nodeCount(); i++){
                MoveNode& moveNode = moves.node(i);
                if (moveNode.isScheduled()) {
                    // Operands were scheduled successfully but result can
                    // not be, we unschedule everything
                    unschedule(moveNode);
                    if (moveNode.isDestinationOperation()) {
                        unscheduleInputOperandTempMoves(moveNode);
                    }
                    if (moveNode.isSourceOperation()) {
                        unscheduleResultReadTempMoves(moveNode);
                    }
                }
            }
            resultsFailed = true;
            // If some operands were bypassed, we need to remove bypassing
            if (tryBypassing) {
                softwareBypasser_->removeBypass(moves, *ddg_, *rm_);
                tryBypassing = false;
                bypassedCount_ -= bypassedMoves;
                continue;
            }
            // We will try to schedule operands in later cycle
            // taking larger strides
            operandsStartCycle++;
            minOperand++;
            operandsStartCycle = std::max(minOperand, operandsStartCycle);
        }
    }
    // This fails if there is "timeout" on retry count.
    // Should not be needed in final version.
    
    if (operandsFailed == true) {
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__, 
            "Operands scheduling failed for \'" + moves.toString());
    }
    if (resultsFailed == true) {
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__, 
            "Results scheduling failed for \'" + moves.toString());
    }

#ifdef DDG_SNAPSHOTS
//    static int failedCounter = 0;
    if (resultsFailed || operandsFailed) {
        ddg_->writeToDotFile(
            (boost::format("bb_%s_2_failed_scheduling.dot")
             % ddg_->name()).str());        
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__, 
            (boost::format("Bad BB %s") % ddg_->name()).str());
    }
#endif

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
 * execution. Also assumes that all inputs to the MoveNodeGroup have
 * been scheduled. Exception to this are the possible temporary register
 * copies inserted before the operand move due to missing connectivity.
 * If found, the temp moves are scheduled atomically with the operand move.
 * Assumes top-down scheduling.
 *
 * @param cycle Earliest cycle for starting scheduling of operands
 * @param moves Moves of the operation execution.
 * @return The cycle the earliest of the operands got scheduled 
 */
int
BasicBlockScheduler::scheduleOperandWrites(int& cycle, MoveNodeGroup& moves)
    throw (Exception) {

    const int EARLY_OPERAND_DIFFERENCE = 15;
    
    int lastOperandCycle = 0;
    int earliestScheduledOperand = INT_MAX;
    int startCycle = 0;
    // Counts operands that are not scheduled at beginning.
    int unscheduledMoves = 0;
    MoveNode* trigger = NULL;
    MoveNode* firstToSchedule = NULL;

    // find the movenode which has highest DDG->earliestCycle and limit
    // the starting cycle of all operands close to that.
    // this should prevent trying to schedule immediate writes very early
    // and having lots of retries until the cycle has risen to the level of
    // other moves.
    // this might make SCHEDULING_WINDOW or SimpleBrokerDirector unnecessary / 
    // allow groving it much smaller without scheduler slowdown.
    for (int i = 0; i < moves.nodeCount(); i++) {
        if (!moves.node(i).isDestinationOperation()) {
            continue;
        }
        // count how many operand moves will need to be scheduled
        unscheduledMoves++;

        int limit = ddg_->earliestCycle(moves.node(i), true) - 
            EARLY_OPERAND_DIFFERENCE;
        if (limit < 0) {
            limit = 0;
        }
        if (limit > cycle) {
            cycle = limit;
        }
    }

    // Find and schedule moveNode which has highest "earliest Cycle"
    // other moveNodes could be scheduled in earlier cycle but this
    // makes sure there will be no problems with operands overwritting
    // and assignment failures. At least it reduced a count of such.
    // Also find smallest of all earliest cycles,
    // make no sense to start from 0
    for (int i = 0; i < moves.nodeCount(); i++) {
        if (!moves.node(i).isDestinationOperation()) {
            continue;
        }

        // Temporary moves needs to be scheduled so DDG can find
        // earliest cycle
        // TODO: this should also have cycle limit?
        scheduleInputOperandTempMoves(moves.node(i));
        int earliestDDG = ddg_->earliestCycle(moves.node(i));

        if (earliestDDG == INT_MAX) {
            ddg_->writeToDotFile("failed_bb.dot");
            unscheduleInputOperandTempMoves(moves.node(i));
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                (boost::format("InputTempMoves failed to schedule "
                "successfully for '%s' ") % moves.node(i).toString()).str());
        }

        // passed argument could be larger than what DDG thinks is earliest
        earliestDDG = std::max(earliestDDG, cycle);
        int earliest = rm_->earliestCycle(earliestDDG, moves.node(i));
        if (earliest == -1) {
            unscheduleInputOperandTempMoves(moves.node(i));
            continue;
        }
        if (earliest >= startCycle) {
            // Find first moveNode that will be scheduled
            startCycle = earliest;
            firstToSchedule = &moves.node(i);
        }
        unscheduleInputOperandTempMoves(moves.node(i));

        // Find also smallest of earliestCycles
        cycle = std::min(cycle, earliest);
    }

    int scheduledMoves = 0;

    if (firstToSchedule != NULL) {
        // start cycle could be null if there was problem scheduling
        // all input temp operands
        scheduleInputOperandTempMoves(*firstToSchedule);
        // earliestCycle gave us the startCycle so this must pass
        scheduleMove(*firstToSchedule, startCycle);
        if (!firstToSchedule->isScheduled()) {
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                (boost::format("Move '%s' failed to schedule") 
                % firstToSchedule->toString()).str());            
        }
        startCycle = firstToSchedule->cycle();

        if (firstToSchedule->move().destination().isTriggering()) {
            trigger = firstToSchedule;
        }
        lastOperandCycle = std::max(lastOperandCycle, startCycle);
    } else {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            (boost::format(
                "Unable to schedule '%s' is there enough resources?") 
             % moves.toString()).str());
    }

    int counter = 0;

    // Loops till all moveNodes are scheduled or "timeouts"
    // remove timeout when software bypassing is tested and guaranteed to work
    // TODO: remove this kind of kludges. They just await for code that
    // breaks them.
    while (unscheduledMoves != scheduledMoves && counter < 2) {
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

            // in case the operand move requires register copies due to
            // missing connectivity, schedule them first
            scheduleInputOperandTempMoves(moveNode);            
            scheduleMove(moveNode, cycle);

            if (moveNode.isScheduled()) {
                lastOperandCycle =
                    std::max(lastOperandCycle, moveNode.cycle());
                if (moveNode.move().destination().isTriggering()) {
                    trigger = &moveNode;
                }
            } else {
                unscheduleInputOperandTempMoves(moveNode);
            }
        }
        // Tests if all input moveNodes were scheduled
        // If trigger is earlier then some operand reschedule
        scheduledMoves = 0;
        earliestScheduledOperand = INT_MAX;
        for (int moveIndex = 0; moveIndex < moves.nodeCount(); ++moveIndex) {
            MoveNode& moveNode = moves.node(moveIndex);
            // skip the result reads
            if (!moveNode.isDestinationOperation()) {
                continue;
            }
            // If trigger is too early, try to reschedule it
            if (moveNode.isScheduled() &&
                moveNode.move().destination().isTriggering() &&
                moveNode.cycle() < lastOperandCycle) {
                unschedule(moveNode);
                unscheduleInputOperandTempMoves(moveNode);
                trigger = &moveNode;
                scheduleInputOperandTempMoves(moveNode);
                scheduleMove(moveNode, std::max(cycle, lastOperandCycle));
                if (!moveNode.isScheduled()) {
                    unscheduleInputOperandTempMoves(moveNode);
                }
            }
            if (moveNode.isScheduled()) {
                lastOperandCycle =
                    std::max(lastOperandCycle, moveNode.cycle());
                earliestScheduledOperand =
                        std::min(earliestScheduledOperand, moveNode.cycle());
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
                    unschedule(moveNode);
                    unscheduleInputOperandTempMoves(moveNode);
                }
            }
            scheduledMoves = 0;
        } else {
            // every operand is scheduled, we can return quickly
            return earliestScheduledOperand;
        }
        cycle = std::max(cycle, earliestScheduledOperand) + 1;
        counter++;
    }
    // If loop timeouts we get here
    if (scheduledMoves != unscheduledMoves) {
        return -1;
    } else {
        return earliestScheduledOperand;
    }
}

/**
 * Schedules the result read moves of an operation execution.
 *
 * Assumes the given MoveNodeGroup contains all moves in the operation
 * execution. Also assumes that all operand moves have been scheduled.
 *
 * @param moves Moves of the operation execution.
 */
bool
BasicBlockScheduler::scheduleResultReads(MoveNodeGroup& moves) 
    throw (Exception) {
    int tempRegLimitCycle = 0;

    for (int moveIndex = 0; moveIndex < moves.nodeCount(); ++moveIndex) {
        MoveNode& moveNode = moves.node(moveIndex);

        if (!moveNode.isScheduled()) {
            if (!moveNode.isSourceOperation()) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    (boost::format("Move to schedule '%s' is not "
                    "result move!") % moveNode.toString()).str());                
            }
            if (succeedingTempMove(moveNode) != NULL) {
                
                MoveNode* lastRead =
                    ddg_->lastScheduledRegisterRead(
                        moveNode.move().destination().registerFile(),
                        moveNode.move().destination().index());
                if (lastRead != NULL) {
                    tempRegLimitCycle = lastRead->cycle();
                }
            }
            
            scheduleMove(moveNode, tempRegLimitCycle);
            if (!moveNode.isScheduled()) {
                // Result can not be scheduled even when operands were,
                // usually caused by operands too early (data dependencies
                // ok) and result forced to be scheduled much later
                // because of data dependencies (WA{R,W} dependence with
                // target register in most cases)
                return false;
            }
            scheduleResultReadTempMoves(moveNode);
            if (!moveNode.isScheduled()) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    (boost::format("Move '%s' did not get scheduled!") 
                    % moveNode.toString()).str());                
            }            
        }
    }
    return true;
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
BasicBlockScheduler::scheduleMove(
    MoveNode& moveNode,
    int earliestCycle)
    throw (Exception) {

    if (moveNode.isScheduled()) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            (boost::format("Move '%s' is already scheduled!") 
            % moveNode.toString()).str());                
    }            

    int sourceReadyCycle = 0;
    if (moveNode.isSourceOperation()) {
        sourceReadyCycle = moveNode.earliestResultReadCycle();
    }

    int ddgCycle = 0;

    if (moveNode.move().isControlFlowMove()) {

        // the branch should get scheduled last, so the DDG should have
        // only the branch move at hand unscheduled
        if (ddg_->nodeCount() != ddg_->scheduledNodeCount() + 1) {
            std::string msg =
                "Control Flow Move is not last of the unscheduled moves! ";
            msg += "Scheduled count=" + 
                Conversion::toString(ddg_->scheduledNodeCount());
            msg += " Node count=" + Conversion::toString(ddg_->nodeCount());
            throw InvalidData(__FILE__, __LINE__, __func__, msg);            
        }

        // try to fill the delay slots with moves within the same basic
        // block
        // @TODO: ignore argument/rv register deps in case of call as they
        // are not really used by the call instruction but by the called
        // procedure, thus we can schedule them at the delay slots

        // largest cycle of DDG is the highest cycle where something
        // was scheduled, however such a move can have pipeline
        // that spans more cycles and we should not change
        // the control flow while something is still in pipeline
        int largest = std::max(rm_->largestCycle(), ddg_->largestCycle());
        ddgCycle =
            std::max(
                ddg_->earliestCycle(moveNode),
                largest - targetMachine_->controlUnit()->delaySlots());
    } else {
        ddgCycle = ddg_->earliestCycle(moveNode);
    }

    // if it's a conditional move then we have to be sure that the guard
    // is defined before executing the move.
    // this is already handled by DDGs earliestCycle, except cases
    // where the guard is defined in a previous BB. 
    // So this prevents scheduling unconditional moves at the beginning
    // of a BB.
    
    if (!moveNode.move().isUnconditional()) {
        ddgCycle = std::max(ddgCycle, moveNode.guardLatency()-1);
    }

    // Earliest cycle from which to start, could depend on result ready
    // for result moves.
    int minCycle = std::max(earliestCycle, ddgCycle);
    minCycle = std::max(minCycle, sourceReadyCycle);

    // RM hasConnection takes MoveNodeSet, however is called only for one
    // moveNode here.
    MoveNodeSet tempSet;
    tempSet.addMoveNode(moveNode);
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
                   rm_->earliestCycle(rm_->largestCycle()+1, moveNode) == -1) {
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
    
    minCycle = rm_->earliestCycle(minCycle, moveNode);
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
}

/**
 * Schedules the (possible) temporary register copy moves (due to missing
 * connectivity) preceeding the given input move.
 *
 * @param operandMove The move of which temp moves to schedule.
 */
void
BasicBlockScheduler::scheduleInputOperandTempMoves(MoveNode& operandMove)
    throw (Exception) {

    /* Because temporary register moves do not have WaR dependency edges
       between the other possible uses of the same temp register in
       the same operation, we have to limit the scheduling of the new
       temp reg move to the last use of the same temp register so
       we don't overwrite the temp registers of other operands. */
    int lastUse1 = 0, lastUse2 = 0;

    // find all unscheduled preceeding temp moves of the operand move
    DataDependenceGraph::NodeSet pred = ddg_->predecessors(operandMove);
    MoveNode* tempMove = NULL;
    for (DataDependenceGraph::NodeSet::iterator i = pred.begin(); 
         i != pred.end(); ++i) {
        MoveNode& m = **i;
        if (m.isScheduled() ||
            !m.move().hasAnnotations(
                TTAProgram::ProgramAnnotation::ANN_CONNECTIVITY_MOVE))
            continue;

        assert(tempMove == NULL &&
               "Multiple unscheduled moves for the operand move, should have "
               "max. one (the temporary move)!");
        tempMove = &m;
        MoveNode* lastRead =
            ddg_->lastScheduledRegisterRead(
                tempMove->move().destination().registerFile(),
                tempMove->move().destination().index());
        if (lastRead != NULL)
            lastUse1 = lastRead->cycle();
    }

    if (tempMove == NULL)
        return; // no temp moves found

    // the temp move should have maximum of one unscheduled
    // preceeding reg to reg copy (in case of two temp reg copies),
    // schedule it first if found
    pred = ddg_->predecessors(*tempMove);
    MoveNode* tempMove2 = NULL;
    for (DataDependenceGraph::NodeSet::iterator i = pred.begin(); 
         i != pred.end(); ++i) {
        MoveNode& m = **i;
        if (m.isScheduled() ||
            !m.move().hasAnnotations(
                TTAProgram::ProgramAnnotation::ANN_CONNECTIVITY_MOVE))
            continue;

        assert(tempMove2 == NULL &&
               "Multiple unscheduled moves for the operand move, should have "
               "max. one (the temporary move)!");
        tempMove2 = &m;
        MoveNode* lastRead2 =
            ddg_->lastScheduledRegisterRead(
                tempMove2->move().destination().registerFile(),
                tempMove2->move().destination().index());
        if (lastRead2 != NULL)
            lastUse2 = lastRead2->cycle();
    }

    if (tempMove2 != NULL) {
        scheduleMove(*tempMove2, lastUse2);
        scheduledTempMoves_[&operandMove].insert(tempMove2);

    }
    scheduleMove(*tempMove, lastUse1);
    scheduledTempMoves_[&operandMove].insert(tempMove);
}

/**
 * Finds the temp move succeeding the given movenode.
 *
 * If it does not have one , returns null.
 * 
 * @param current MoveNode whose tempmoves we are searching
 * @return tempMove succeeding given node, or null if does not exist.
 */
MoveNode* 
BasicBlockScheduler::succeedingTempMove(MoveNode& current) {
    
    DataDependenceGraph::NodeSet succ = ddg_->successors(current);
    MoveNode* result = NULL;
    for (DataDependenceGraph::NodeSet::iterator i = succ.begin(); 
         i != succ.end(); ++i) {
        MoveNode& m = **i;
        if (m.isScheduled() ||
            !m.move().hasAnnotations(
                TTAProgram::ProgramAnnotation::ANN_CONNECTIVITY_MOVE))
            continue;

        assert(result == NULL &&
               "Multiple candidates for the temp move of result read.");
        result = &m;
    }
    return result;
}


/**
 * Schedules the (possible) temporary register copy moves (due to missing
 * connectivity) succeeding the given result read move.
 *
 * @param resultMove The move of which temp moves to schedule.
 */
void
BasicBlockScheduler::scheduleResultReadTempMoves(MoveNode& resultMove)
    throw (Exception) {

    /* Because temporary register moves do not have WaR/WaW dependency edges
       between the other possible uses of the same temp register in
       the same operation, we have to limit the scheduling of the new
       temp reg move to the last use of the same temp register so
       we don't overwrite the temp registers of the other operands. */
    int lastUse2 = 0;

    // find all unscheduled succeeding moves of the result read move
    // there should be only only one with the only dependence edge (RaW) to
    // the result move

    MoveNode* tempMove1 = succeedingTempMove(resultMove);
    if (tempMove1 == NULL)
        return; // no temp moves found
    
    MoveNode* tempMove2 = succeedingTempMove(*tempMove1);
    if (tempMove2 != NULL) {
        MoveNode* lastRead =
            ddg_->lastScheduledRegisterRead(
                tempMove1->move().destination().registerFile(),
                tempMove1->move().destination().index());
        if (lastRead != NULL)
            lastUse2 = lastRead->cycle();
    }

    scheduleMove(*tempMove1, lastUse2);
    scheduledTempMoves_[&resultMove].insert(tempMove1);

    // the temp move should have maximum of one unscheduled
    // succeeding additional reg to reg copy (in case of two temp reg copies),
    // schedule it also if found
    if (tempMove2 != NULL) {
        scheduleMove(*tempMove2, lastUse2+1);
        scheduledTempMoves_[&resultMove].insert(tempMove2);
    }
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
BasicBlockScheduler::unschedule(MoveNode& moveNode) {    
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
 * Unschedules the (possible) temporary register copy moves (due to missing
 * connectivity) preceeding the given move.
 *
 * @param operandMove The move of which temp moves to unschedule.
 */
void
BasicBlockScheduler::unscheduleInputOperandTempMoves(MoveNode& operandMove) {

    if (!MapTools::containsKey(scheduledTempMoves_, &operandMove))
        return; // nothing to unschedule

    DataDependenceGraph::NodeSet tempMoves = scheduledTempMoves_[&operandMove];

    for (DataDependenceGraph::NodeSet::iterator i = tempMoves.begin();
         i != tempMoves.end(); ++i) {
        unschedule(**i);
    }
    scheduledTempMoves_.erase(&operandMove);
}

/**
 * Unschedules the (possible) temporary register copy moves (due to missing
 * connectivity) succeeding the given result read move.
 *
 * @param resultMove The move of which temp moves to unschedule.
 */
void
BasicBlockScheduler::unscheduleResultReadTempMoves(MoveNode& resultMove) {

    if (!MapTools::containsKey(scheduledTempMoves_, &resultMove))
        return; // nothing to unschedule

    DataDependenceGraph::NodeSet tempMoves = scheduledTempMoves_[&resultMove];

    for (DataDependenceGraph::NodeSet::iterator i = tempMoves.begin();
         i != tempMoves.end(); ++i) {
        unschedule(**i);
    }
    scheduledTempMoves_.erase(&resultMove);
}


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
BasicBlockScheduler::handleProcedure(
    TTAProgram::Procedure& procedure,
    const TTAMachine::Machine& targetMachine)
    throw (Exception) {

    ControlFlowGraph cfg(procedure);

#ifdef CFG_SNAPSHOTS
    cfg.writeToDotFile(procedure.name() + "_cfg.dot");    
#endif
    // create the procedure-wide ddg.
    DataDependenceGraphBuilder ddgBuilder;
    bigDDG_ = ddgBuilder.build(cfg);

#ifdef BIG_DDG_SNAPSHOTS
    bigDDG_->writeToDotFile(bigDDG_->name() + "_ddg.dot");
#endif
    UniversalMachine um;

    handleControlFlowGraph(cfg, targetMachine);

    if (delaySlotFiller_ != NULL) {
        delaySlotFiller_->fillDelaySlots(
            cfg, *bigDDG_, targetMachine, um, true);
    }

    // now all basic blocks are scheduled, let's put them back to the
    // original procedure

    copyCfgToProcedure(procedure, cfg);

    delete bigDDG_;
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
BasicBlockScheduler::handleControlFlowGraph(
    ControlFlowGraph& cfg,
    const TTAMachine::Machine& targetMachine)
    throw (Exception) {

    ControlFlowGraphPass::executeBasicBlockPass(cfg, targetMachine, *this);
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
BasicBlockScheduler::handleProgram(
    TTAProgram::Program& program,
    const TTAMachine::Machine& targetMachine)
    throw (Exception) {

    ProgramPass::executeProcedurePass(program, targetMachine, *this);
#ifdef SW_BYPASSING_STATISTICS
    Application::logStream() << bypassedCount_ << " moves were bypassed and "
        << deadResults_ << " dead results were removed." << std::endl;
#endif
}

/**
 * A short description of the pass, usually the optimization name,
 * such as "basic block scheduler".
 *
 * @return The description as a string.
 */
std::string
BasicBlockScheduler::shortDescription() const {
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
BasicBlockScheduler::longDescription() const {
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
BasicBlockScheduler::createDDGFromBB(BasicBlock& bb) {
    if (bigDDG_ != NULL) {
        return bigDDG_->createSubgraph(bb);
    } else {
        DataDependenceGraphBuilder ddgb;
        return ddgb.build(bb);
    }
}

/**
 * Helper function used to delete resourcemanager.
 *
 * Does not delete the resourcemanager but gives it to 
 * delay slot filler.
 *
 * @param rm ResourceManager to delete or store for future use.
 * @param bb BasicBlock which the RM relates to.
 */
void 
BasicBlockScheduler::deleteRM(SimpleResourceManager* rm, BasicBlock& bb) {

    // TODO: nobody currently deletes those
    if (delaySlotFiller_ != NULL) {
        delaySlotFiller_->addResourceManager(bb, *rm);
    } else {
        delete rm;
    }
}

/**
 * Notifies to the selector that given nodes and their temp reg copies are
 * scheduled .
 * 
 * @param nodes nodes which are scheduled.
 * @param selector selector which to notify.
 */
void BasicBlockScheduler::notifyScheduled(
    MoveNodeGroup& moves, MoveNodeSelector& selector) {
    
    for (int moveIndex = 0; moveIndex < moves.nodeCount(); ++moveIndex) {
        MoveNode& moveNode = moves.node(moveIndex);
        selector.notifyScheduled(moveNode);
        std::map<const MoveNode*, DataDependenceGraph::NodeSet >::
            iterator tmIter = scheduledTempMoves_.find(&moveNode);
        if (tmIter != scheduledTempMoves_.end()) {
            DataDependenceGraph::NodeSet tempMoves = tmIter->second;
            for (DataDependenceGraph::NodeSet::iterator i = 
                     tempMoves.begin(); i != tempMoves.end(); i++) {
                selector.notifyScheduled(**i);
            }
        }
    }
}

