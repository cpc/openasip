/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @author Fabio Garzia 2010 (fabio.garzia-no.spam-tut.fi)
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
#include "LLVMTCECmdLineOptions.hh"
#include "InterPassData.hh"
#include "MoveNodeSet.hh"
#include "Terminal.hh"
#include "RegisterRenamer.hh"
#include "Operation.hh"
#include "FUPort.hh"
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
 */
BasicBlockScheduler::BasicBlockScheduler(
    InterPassData& data, 
    SoftwareBypasser* bypasser, 
    CopyingDelaySlotFiller* delaySlotFiller,
    RegisterRenamer* renamer) :
    BBSchedulerController(data, bypasser, delaySlotFiller, NULL),
    DDGPass(data), ddg_(NULL), rm_(NULL), softwareBypasser_(bypasser),
    renamer_(renamer),
    bypassedCount_(0), deadResults_(0) {

    CmdLineOptions *cmdLineOptions = Application::cmdLineOptions();
    options_ = dynamic_cast<LLVMTCECmdLineOptions*>(cmdLineOptions);
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
int
BasicBlockScheduler::handleDDG(
    DataDependenceGraph& ddg, SimpleResourceManager& rm,
    const TTAMachine::Machine& targetMachine, bool testOnly) {
    assert(!testOnly);
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
    
    scheduledTempMoves_.clear();

    // empty need not to be scheduled
    if (ddg.nodeCount() == 0 || 
        (ddg.nodeCount() == 1 && !ddg.node(0).isMove())) {
        return 0;
    }

    CriticalPathBBMoveNodeSelector selector(ddg, targetMachine);

    // register selector to bypasser for notfications.
    if (softwareBypasser_ != NULL) {
        softwareBypasser_->setSelector(&selector);
    }

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
                scheduleMove(firstMove, 0);
            } else {
                scheduleRRMove(firstMove);
                int lastOperandFake = 0;
                if (softwareBypasser_ != NULL) {
                    int rv = 
                        softwareBypasser_->bypassNode(
                            firstMove, lastOperandFake, ddg, rm);
                    if (rv < 0) {
                        softwareBypasser_->removeBypass(
                            firstMove, ddg, rm, true);
                        scheduleRRMove(firstMove);
                    } 
                    
                    // did the bypass cause a reg-to-itself copy?
                    // if it did, let's kill it.
                    if (firstMove.move().source().equals(
                            firstMove.move().destination())) {
                        movesRemoved = true;
                    }
                    
                    if (rv > 0) {
                        std::set<std::pair<TTAProgram::Move*, int> >
                            removedMoves;                        
                        softwareBypasser_->removeDeadResults(
                            moves, ddg, rm);
                    }
                }
            }
        }
        if (!movesRemoved) {
            if (!moves.isScheduled()) {
                std::string message = " Move(s) did not get scheduled: ";
                for (int i = 0; i < moves.nodeCount(); i++) {
                    message += moves.node(i).toString() + " ";
                }
                throw ModuleRunTimeError(__FILE__,__LINE__,__func__,message);
            }

            // notifies successors of the scheduled moves.
            notifyScheduled(moves, selector);
        }

        moves = selector.candidates();
    }

    if (softwareBypasser_ != NULL) {
        softwareBypasser_->clearCaches(ddg, true);
    }

    if (ddg.nodeCount() !=
        ddg.scheduledNodeCount()) {
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
    int size = rm.largestCycle();
    return size;
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
BasicBlockScheduler::scheduleOperation(MoveNodeGroup& moves) {
    ProgramOperation& po =
        (moves.node(0).isSourceOperation())?
        (moves.node(0).sourceOperation()):
        (moves.node(0).destinationOperation());

    // may switch operands of commutative operations. Do it before
    // regcopyadder because it's easier here.
    // cooperation with regcopyadder might make it perform better though.
    tryToSwitchInputs(po);

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
    bool bypassTrigger = true;

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
            bypassedMoves = softwareBypasser_->bypass(moves, *ddg_, *rm_, bypassTrigger);
            if (bypassedMoves == -1){
                // bypassing failed try again without attempt to bypass
                // this restores the original schedule of operands
                // and disable bypassing
                operandsFailed = true;
                if (bypassTrigger == false) {
                    tryBypassing = false;
                } else {
                    bypassTrigger = false;
                }
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
                try {
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
    
    if (operandsFailed) {
	ddg_->writeToDotFile("operands_fail.dot");
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__, 
            "Operands scheduling failed for \'" + moves.toString());
    }
    if (resultsFailed) {
	ddg_->writeToDotFile("result_fail.dot");
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
BasicBlockScheduler::scheduleOperandWrites(int& cycle, MoveNodeGroup& moves) {
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

        int limit = ddg_->earliestCycle(moves.node(i)) - 
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
        scheduleInputOperandTempMoves(moves.node(i), moves.node(i));
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

    if (firstToSchedule != NULL) {
        // start cycle could be null if there was problem scheduling
        // all input temp operands
        scheduleInputOperandTempMoves(*firstToSchedule, *firstToSchedule);
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


    // try to schedule all input moveNodes, except trigger
    for (int moveIndex = 0; moveIndex < moves.nodeCount(); ++moveIndex) {
        MoveNode& moveNode = moves.node(moveIndex);
        // skip the result reads
        if (!moveNode.isDestinationOperation()) {
            continue;
        }
        
        if (moveNode.isScheduled()) {
            earliestScheduledOperand =
                std::min(earliestScheduledOperand, moveNode.cycle());
            if (moveNode.move().destination().isTriggering()) {
                trigger = &moveNode;
            }
            continue;
        }
        
        // in case the operand move requires register copies due to
        // missing connectivity, schedule them first
        scheduleInputOperandTempMoves(moveNode, moveNode);
        scheduleMove(moveNode, cycle);
        
        if (moveNode.isScheduled()) {
            earliestScheduledOperand =
                std::min(earliestScheduledOperand, moveNode.cycle());
            if (moveNode.move().destination().isTriggering()) {
                trigger = &moveNode;
                earliestScheduledOperand =
                    std::min(earliestScheduledOperand, moveNode.cycle());
            } else {
                lastOperandCycle =
                    std::max(lastOperandCycle, moveNode.cycle());
            }
        } else {
            // Movenode scheduling failed.
            unscheduleInputOperandTempMoves(moveNode);
            // try to unschedule trigger and then schedule this operand.
            if (trigger != NULL && trigger->isScheduled()) {
                unschedule(*trigger);
                unscheduleInputOperandTempMoves(*trigger);
                scheduleInputOperandTempMoves(moveNode, moveNode);
                scheduleMove(moveNode, cycle);
                if (!moveNode.isScheduled()) {
                // if still failed return -1
                    unscheduleInputOperandTempMoves(moveNode);
                    // but make sure high-level loop advances some more cycles
                    if (earliestScheduledOperand != INT_MAX) {
                        cycle = earliestScheduledOperand;
                    }
                    return -1;
                }
                earliestScheduledOperand =
                    std::min(earliestScheduledOperand, moveNode.cycle());
                lastOperandCycle =
                    std::max(lastOperandCycle, moveNode.cycle());
            } else {
                // failed even though no trigger scheduled.
                // but make sure high-level loop advances some more cycles
                if (earliestScheduledOperand != INT_MAX) {
                    cycle = earliestScheduledOperand;
                }
                return -1;
            }
        }
    }

    // if trigger unscheduled, schedule it again.
    assert(trigger != NULL);
    if (!trigger->isScheduled()) {
        scheduleInputOperandTempMoves(*trigger, *trigger);
        scheduleMove(*trigger, lastOperandCycle);
        
        if (!trigger->isScheduled()) {
            // trigger scheduling failed.
            unscheduleInputOperandTempMoves(*trigger);
            return -1;
        }
        earliestScheduledOperand =
            std::min(earliestScheduledOperand, trigger->cycle());
    }
    
    return earliestScheduledOperand;
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
BasicBlockScheduler::scheduleResultReads(MoveNodeGroup& moves) {
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
            scheduleResultReadTempMoves(moveNode, moveNode, 0);
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
 * Schedules moves in a single operation execution.
 *
 * Assumes the given MoveNodeGroup contains all moves in the operation
 * execution. Also assumes that all inputs to the MoveNodeGroup have
 * been scheduled.
 *
 * @param moveNode R-R Move to schedule.
 */
void
BasicBlockScheduler::scheduleRRMove(MoveNode& moveNode) {
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

    scheduleMove(moveNode, 0);
    scheduleRRTempMoves(moveNode, moveNode, 0); //Fabio: 0 or more?!?
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
BasicBlockScheduler::scheduleMove(MoveNode& moveNode, int earliestCycle) {
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
        // only the branch move(s) at hand unscheduled
        if (ddg_->nodeCount() != ddg_->scheduledNodeCount() + 1) {    
        // in rare case the branch moves can have 2 parameters (SPU)
        // and therefore 2 nodes unscheduled. Check if the unscheduled
        // moves are all control flow moves.
            DataDependenceGraph::NodeSet unscheduledMoves =
                ddg_->unscheduledMoves();	  
            for (DataDependenceGraph::NodeSet::iterator i = unscheduledMoves.begin(); 
                i != unscheduledMoves.end(); ++i) {	
                if (!(*i)->move().isControlFlowMove()) {
                    std::string msg =
                    "Control Flow Move is not last of the unscheduled moves! ";
                    msg += "Scheduled count=" + 
                        Conversion::toString(ddg_->scheduledNodeCount());
                    msg += " Node count=" +
                        Conversion::toString(ddg_->nodeCount());		  
                    throw InvalidData(__FILE__, __LINE__, __func__, msg);            
                }
            }
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
    } else { // not a control flow move:
        ddgCycle = ddg_->earliestCycle(moveNode);
        if (renamer_ != NULL) {
            int minRenamedEC = std::max(
                sourceReadyCycle, ddg_->earliestCycle(
                    moveNode, INT_MAX, true, true)); // TODO: 0 or INT_MAX

            // rename if can and may alow scheuduling earlier.
            if (renamer_ != NULL && minRenamedEC < ddgCycle) {
                minRenamedEC =  rm_->earliestCycle(minRenamedEC, moveNode);
                if (minRenamedEC < ddgCycle) {
                    
                    if (renamer_->renameDestinationRegister(
                            moveNode, false, true, true, minRenamedEC)) {
                        ddgCycle = ddg_->earliestCycle(moveNode);
                    }
#ifdef THIS_IS_BUGGY_WITH_REGCOPY_ADDER                    
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
                                        *limitingAdep, false, true, true)) {
                                    ddgCycle = ddg_->earliestCycle(
                                        moveNode);
                                }
                            }
                        }
                    }
#endif
                }
            }
        }
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
 * connectivity) succeeding the given RR move. 
 *
 * The function recursively goes through all the temporary moves added to 
 * the given RR move.
 *
 * @param regToRegMove  A temp move whose successor has to be scheduled.
 * @param firstMove     The original move of which temp moves to schedule.
 * @param lastUse Recursive function parameter, it should be set as 0 
 * for the first function call.
 */
void
BasicBlockScheduler::scheduleRRTempMoves(
    MoveNode& regToRegMove, MoveNode& firstMove, int lastUse) {
    /* Because temporary register moves do not have WaR/WaW dependency edges
       between the other possible uses of the same temp register in
       the same operation, we have to limit the scheduling of the new
       temp reg move to the last use of the same temp register so
       we don't overwrite the temp registers of the other operands. */

    // find all unscheduled succeeding moves of the result read move
    // there should be only only one with the only dependence edge (RaW) to
    // the result move

    MoveNode* tempMove1 = succeedingTempMove(regToRegMove);
    if (tempMove1 == NULL)
        return; // no temp moves found
    
    MoveNode* tempMove2 = succeedingTempMove(*tempMove1);
    if (tempMove2 != NULL) {
        MoveNode* lastRead =
            ddg_->lastScheduledRegisterRead(
                tempMove1->move().destination().registerFile(),
                tempMove1->move().destination().index());
        if (lastRead != NULL)
            lastUse = lastRead->cycle();
    }

    scheduleMove(*tempMove1, lastUse);
    assert(tempMove1->isScheduled());
    scheduledTempMoves_[&firstMove].insert(tempMove1);

    // the temp move should have maximum of one unscheduled
    // succeeding additional reg to reg copy (in case of two temp reg copies),
    // schedule it also if found
    scheduleResultReadTempMoves(*tempMove1, firstMove, lastUse+1);
}

/**
 * Schedules the (possible) temporary register copy moves (due to missing
 * connectivity) preceeding the given input move. 
 *
 * The function recursively goes through all the temporary moves added to 
 * the given input move.
 *
 * @param operandMove  A temp move whose predecessor has to be scheduled.
 * @param operandWrite The original move of which temp moves to schedule.
 */
void
BasicBlockScheduler::scheduleInputOperandTempMoves(
    MoveNode& operandMove, MoveNode& operandWrite) {
    /* Because temporary register moves do not have WaR dependency edges
       between the other possible uses of the same temp register in
       the same operation, we have to limit the scheduling of the new
       temp reg move to the last use of the same temp register so
       we don't overwrite the temp registers of other operands. */
    int lastUse = 0;
    
    // find all unscheduled preceeding temp moves of the operand move
    DataDependenceGraph::EdgeSet inEdges = ddg_->inEdges(operandMove);
    MoveNode* tempMove = NULL;
    for (DataDependenceGraph::EdgeSet::iterator i = inEdges.begin();
         i != inEdges.end(); ++i) {

        if ((**i).edgeReason() != DataDependenceEdge::EDGE_REGISTER ||
            (**i).guardUse() ||
            (**i).dependenceType() != DataDependenceEdge::DEP_RAW) {
            continue;
        }

        MoveNode& m = ddg_->tailNode(**i);
        if (m.isScheduled() ||
            !m.move().hasAnnotations(
                TTAProgram::ProgramAnnotation::ANN_CONNECTIVITY_MOVE)) {
            continue;
        }

        assert(tempMove == NULL &&
               "Multiple unscheduled moves for the operand move, should have "
               "max. one (the temporary move)!");
        tempMove = &m;
        MoveNode* lastRead =
            ddg_->lastScheduledRegisterRead(
                tempMove->move().destination().registerFile(),
                tempMove->move().destination().index());
        if (lastRead != NULL)
            lastUse = lastRead->cycle();
    }

    if (tempMove == NULL)
        return; // no temp moves found

    // the temp move should have maximum of one unscheduled
    // preceeding reg to reg copy (in case of an additional temp reg copy),
    // schedule it first if found, calling recursively this function
    scheduleInputOperandTempMoves(*tempMove, operandWrite);
    
    scheduleMove(*tempMove, lastUse);
    assert(tempMove->isScheduled());
    scheduledTempMoves_[&operandWrite].insert(tempMove);
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
BasicBlockScheduler::ddgSnapshot(
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
 * The function recursively goes through all the temporary moves added to
 * the given input move.
 *
 * @param resultMove A temp move whose successor has to be scheduled.
 * @param resultRead The original move of which temp moves to schedule.
 * @param lastUse Recursive function parameter, it should be set as 0 
 * for the first function call.
 */
void
BasicBlockScheduler::scheduleResultReadTempMoves(
    MoveNode& resultMove, MoveNode& resultRead, int lastUse) {
    /* Because temporary register moves do not have WaR/WaW dependency edges
       between the other possible uses of the same temp register in
       the same operation, we have to limit the scheduling of the new
       temp reg move to the last use of the same temp register so
       we don't overwrite the temp registers of the other operands. */


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
            lastUse = lastRead->cycle();
    }

    scheduleMove(*tempMove1, lastUse);
    assert(tempMove1->isScheduled());
    scheduledTempMoves_[&resultRead].insert(tempMove1);

    // the temp move should have maximum of one unscheduled
    // succeeding additional reg to reg copy (in case of two temp reg copies),
    // schedule it also if found
    scheduleResultReadTempMoves(*tempMove1, resultRead, lastUse+1);
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
            DataDependenceGraph::NodeSet& tempMoves = tmIter->second;
            for (DataDependenceGraph::NodeSet::iterator i = 
                     tempMoves.begin(); i != tempMoves.end(); i++) {
                selector.notifyScheduled(**i);
            }
        }
    }
}

// find which movenode is the trigger of an operation.
MoveNode*
BasicBlockScheduler::findTrigger(
    const ProgramOperation& po, const TTAMachine::Machine& mach) {
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
    mach.functionUnitNavigator();
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
    
    if (mach.controlUnit()->hasOperation(po.operation().name())) {
        return findTriggerFromUnit(po, *mach.controlUnit());
    }
    return candidate;
    
}

MoveNode* 
BasicBlockScheduler::findTriggerFromUnit(
    const ProgramOperation& po, const TTAMachine::Unit& unit) {
    const TTAMachine::FunctionUnit* fu =
    dynamic_cast<const TTAMachine::FunctionUnit*>(&unit);
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


/**
 *
 * Returns the operand which is triggering in all FU's which support
 * the operation. If operation not found, is not gcu or 
 * multiple FU's have different triggers, returns 0
 */
int
BasicBlockScheduler::getTriggerOperand(
    const Operation& operation,
    const TTAMachine::Machine& machine) {
 
    const TCEString& opName = operation.name();
    TTAMachine::Machine::FunctionUnitNavigator fuNav = 
        machine.functionUnitNavigator();

    int trigger = 0;
    for (int i = 0; i < fuNav.count(); i++) {
        TTAMachine::FunctionUnit& fu = *fuNav.item(i);
        if (fu.hasOperation(opName)) {
            TTAMachine::HWOperation& hwop = *fu.operation(opName);
            for (int j = 1; j <= operation.numberOfInputs(); j++) {
                TTAMachine::FUPort& port = *hwop.port(j);
                if (port.isTriggering()) {
                    if (trigger == 0) {
                        trigger = j;
                    } else {
                        if (trigger != j) {
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return trigger;
}

/**
 * If the operation is commutative, tries to change the operands so that
 * the scheduler can schedule it better. 
 *
 * If the operation is not commutative, does nothing.
 *
 * Which is better depends lots on the code being compiled and architecture 
 * which we are compiling for. 
 * 
 * Current implementation tries to make constants triggers,
 * and if there are no constant inputs, try to also change inputs so
 * that last ready operand becomes trigger if it's near,
 * but first ready operand becomes trigger if it's further
 * (allows better bypassing of other operands)
 * This seems to work in practice
 *
 * @param po programoperation whose inputs to switch
 * @return true if changed inputs, false if not.
 */
bool 
BasicBlockScheduler::tryToSwitchInputs(ProgramOperation& po) {
    const Operation& op = po.operation();
    if (op.numberOfInputs() == 2 && op.canSwap(1, 2)) {
        
        int triggerOperand = getTriggerOperand(op, *targetMachine_);

        if (triggerOperand != 0) {
            MoveNode* latest = NULL;
            int latestMinCycle = -1;
            int firstMinCycle = INT_MAX;

            //check all input moves.
            for (int i = 0; i < po.inputMoveCount(); i++) {
                MoveNode& node = po.inputMove(i);
                // always make constants triggers.
                // todo: shoudl do this only with short imms?
                if (node.isSourceConstant()) {
                    int operandIndex = 
                        node.move().destination().operationIndex();
                    if (operandIndex != triggerOperand) {
                        po.switchInputs();
                        return true;
                    } else {
                        return false;
                    }
                }

                int minCycle = ddg_->earliestCycle(
                    node, rm_->initiationInterval(), false, false, true, true);
                if (minCycle > latestMinCycle) {
                    latest = &node;
                    latestMinCycle = minCycle;
                }
                if (minCycle < firstMinCycle) {
                    firstMinCycle = minCycle;
                }
            }

            int lastOperand = latest->move().destination().operationIndex();

            // don't change if min cycles of first and alst are equal.
            if (latestMinCycle == firstMinCycle) {
                return false;
            }
            if (latestMinCycle - firstMinCycle > 1) { // reverse logic if far.
                if (lastOperand == triggerOperand) {
                    po.switchInputs();
                    return true;
                }
            } else {
                if (lastOperand != triggerOperand) {
                    po.switchInputs();
                    return true;
                }
            }
        }
    }
    return false;
}
