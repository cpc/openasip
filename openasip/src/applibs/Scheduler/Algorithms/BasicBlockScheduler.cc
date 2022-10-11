/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @author Pekka Jääskeläinen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @author Fabio Garzia 2010 (fabio.garzia-no.spam-tut.fi)
 * @note rating: red
 */

#include <set>
#include <string>
#include <cstdlib>

#include <boost/timer.hpp>

#include "BasicBlockScheduler.hh"
#include "DataDependenceGraph.hh"
#include "SimpleResourceManager.hh"
#include "DDGMoveNodeSelector.hh"
#include "POMDisassembler.hh"
#include "ProgramOperation.hh"
#include "ControlUnit.hh"
#include "Machine.hh"
#include "UniversalMachine.hh"
#include "Guard.hh"
#include "MapTools.hh"
#include "Instruction.hh"
#include "InstructionReference.hh"
#include "BasicBlock.hh"
#include "RegisterCopyAdder.hh"
#include "CycleLookBackSoftwareBypasser.hh"
#include "TCEString.hh"
#include "Application.hh"
#include "LLVMTCECmdLineOptions.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "CodeGenerator.hh"
#include "TerminalImmediate.hh"
#include "InterPassData.hh"
#include "MoveNodeSet.hh"
#include "RegisterRenamer.hh"
#include "Operation.hh"
#include "FUPort.hh"
#include "HWOperation.hh"

//#define DEBUG_OUTPUT
//#define DEBUG_REG_COPY_ADDER

#define RESCHEDULE_NEXT_CYCLE_AFTER_DRE


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
    SoftwareBypasser* bypasser, RegisterRenamer* renamer) :
    DDGPass(data), BasicBlockPass(data), ddg_(NULL), rm_(NULL),
    softwareBypasser_(bypasser), renamer_(renamer), minCycle_(0), 
    jumpNode_(NULL) {
    CmdLineOptions *cmdLineOptions = Application::cmdLineOptions();
    options_ = dynamic_cast<LLVMTCECmdLineOptions*>(cmdLineOptions);
}

BasicBlockScheduler::~BasicBlockScheduler() {
}

/**
 * Schedules a piece of code in a DDG
 *
 * @param ddg The ddg containing the code
 * @param rm Resource manager that is to be used.
 * @param targetMachine The target machine.
 * @return size of the produces schedule.
 * @exception Exception several TCE exceptions can be thrown in case of
 *            a scheduling error.
 */
int
BasicBlockScheduler::handleDDG(
    DataDependenceGraph& ddg, SimpleResourceManager& rm,
    const TTAMachine::Machine& targetMachine, int minCycle, bool test) {
    tripCount_ = 0;
    ddg_ = &ddg;
    targetMachine_ = &targetMachine;
    minCycle_ = minCycle;
    schedulingTime_.restart();

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

    // empty DDGs can be ignored
    if (ddg.nodeCount() == 0 ||
        (ddg.nodeCount() == 1 && !ddg.node(0).isMove())) {
        return 0;
    }

    CriticalPathBBMoveNodeSelector selector(ddg, targetMachine);
    selector_ = &selector;

    rm_ = &rm;


    // register selector to bypasser for notfications.
    if (softwareBypasser_ != NULL) {
        softwareBypasser_->setSelector(&selector);
    }

    if (renamer_ != NULL) {
        renamer_->setSelector(&selector);
    }

    MoveNodeGroup moves = selector.candidates();
    while (moves.nodeCount() > 0) {
        bool movesRemoved = false;
        MoveNode& firstMove = moves.node(0);
        if (firstMove.isOperationMove()) {
            scheduleOperation(moves);
        } else {
            if (firstMove.move().destination().isRA()) {
                scheduleMove(firstMove,0, true);
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
                            moves, ddg, rm, removedMoves);
                        // TODO: disabled becaused may be problematic
                        // when rescheduled to different buses
//                    handleRemovedResultMoves(removedMoves);
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
                
                unscheduleAllNodes();
                
                throw ModuleRunTimeError(__FILE__,__LINE__,__func__,message);
            }

            // notifies successors of the scheduled moves.
            notifyScheduled(moves, selector);
        }

        moves = selector.candidates();
    }
    int size = rm.largestCycle();    
    if (test) {
        unscheduleAllNodes();
        return size;
    }
    if (softwareBypasser_ != NULL) {
        softwareBypasser_->clearCaches(ddg, true);
    }

    if (ddg.nodeCount() !=
        ddg.scheduledNodeCount()) {
        ddg.writeToDotFile("failed_bb.dot");
        std::cerr << ddg.nodeCount() <<  ", " << ddg.scheduledNodeCount()
        << std::endl;
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__,
            "All moves in the DDG didn't get scheduled.");
    }

    if (options_ != NULL && options_->dumpDDGsDot()) {
        ddgSnapshot(ddg, std::string("0"), DataDependenceGraph::DUMP_DOT, true);
    }
    
    if (options_ != NULL && options_->dumpDDGsXML()) {
        ddgSnapshot(ddg, std::string("0"), DataDependenceGraph::DUMP_XML, true);
    }
    return size;
}

/**
 * Schedules loop in a DDG
 *
 * @param ddg The ddg containing the loop
 * @param rm Resource manager that is to be used.
 * @param targetMachine The target machine.
 * @exception Exception several TCE exceptions can be thrown in case of
 *            a scheduling error.
 * @return negative if failed, else overlapcount.
 */
int
BasicBlockScheduler::handleLoopDDG(
    DataDependenceGraph& ddg, SimpleResourceManager& rm,
    const TTAMachine::Machine& targetMachine, int tripCount,
    SimpleResourceManager*, bool testOnly) {
    tripCount_ = tripCount;
    ddg_ = &ddg;
    targetMachine_ = &targetMachine;
    minCycle_ = 0;
    schedulingTime_.restart();

    if (renamer_ != NULL) {
        renamer_->initialize(ddg);
    }

    if (options_ != NULL && options_->dumpDDGsDot()) {
        ddgSnapshot(
            ddg, std::string("0"), DataDependenceGraph::DUMP_DOT, false, true);
    }

    if (options_ != NULL && options_->dumpDDGsXML()) {
        ddgSnapshot(
            ddg, std::string("0"), DataDependenceGraph::DUMP_XML, false, true);
    }

    scheduledTempMoves_.clear();

    // empty DDGs can be ignored
    if (ddg.nodeCount() == 0 ||
        (ddg.nodeCount() == 1 && !ddg.node(0).isMove())) {
        return 0;
    }

    CriticalPathBBMoveNodeSelector selector(ddg, targetMachine);
 
    rm_ = &rm;

    // register selector to bypasser for notfications.
    if (softwareBypasser_ != NULL) {
        softwareBypasser_->setSelector(&selector);
    }

    if (renamer_ != NULL) {
        renamer_->setSelector(&selector);
    }
    
    for (int i = ddg.nodeCount()-1; i>= 0 ; i--) {
        MoveNode& node = ddg.node(i);
        if (node.move().isControlFlowMove()) {
            jumpNode_ = &node;
            MoveNodeGroup mng;
            mng.addNode(node);
            scheduleOperation(mng);
        }
    }

    MoveNodeGroup moves = selector.candidates();
    while (moves.nodeCount() > 0) {

        MoveNode& firstMove = moves.node(0);
        if (firstMove.isOperationMove()) {
            scheduleOperation(moves);
        } else {
            if (firstMove.move().destination().isRA()) {
                scheduleMove(firstMove,0, true);
            } else {
                scheduleRRMove(firstMove);
            }
        }

        if (!moves.isScheduled()) {
            ddg_->writeToDotFile(
                std::string("ii_") + 
                Conversion::toString(rm.initiationInterval()) + 
                std::string("_dag.dot"));

            unscheduleAllNodes();

            return -1;
        }

        // notifies successors of the scheduled moves.
        notifyScheduled(moves, selector);

        moves = selector.candidates();
    }
    
    if (ddg.nodeCount() !=
        ddg.scheduledNodeCount()) {
        debugLog("All moves in the DDG didn't get scheduled.");
//        debugLog("Disassembly of the situation:");
//        Application::logStream() << bb.disassemble() << std::endl;
        ddg.writeToDotFile("failed_bb.dot");
        abortWithError("Should not happen!");
    }

    // loop schedulign did not help.
    if (static_cast<unsigned>(ddg.largestCycle()) < rm.initiationInterval()
        || testOnly) {
        if (static_cast<unsigned>(ddg.largestCycle()) < 
            rm.initiationInterval()) {
            Application::logStream() 
                << "No overlapping instructions." 
                << "Should Revert to ordinary scheduler."
                << std::endl;
        } 
        // this have to be calculated before unscheduling.
        int rv = ddg.largestCycle() / rm.initiationInterval();
        unscheduleAllNodes();
        return rv;
    }

    // test that ext-cond jump not in prolog (where it is skipped)
    int overlap_count = ddg.largestCycle() / rm.initiationInterval();
    if (overlap_count >= tripCount) {
        unscheduleAllNodes();
        return -1;
    }
    
    if (softwareBypasser_ != NULL) {
        softwareBypasser_->clearCaches(ddg, true);
    }

    if (options_ != NULL && options_->dumpDDGsDot()) {
        ddgSnapshot(ddg, std::string("0"), DataDependenceGraph::DUMP_DOT, true);
    }

    if (options_ != NULL && options_->dumpDDGsXML()) {
        ddgSnapshot(ddg, std::string("0"), DataDependenceGraph::DUMP_XML, true);
    }

    return ddg.largestCycle() / rm.initiationInterval();
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

    RegisterCopyAdder regCopyAdder(
        DDGPass::interPassData(), *rm_, *selector_);
    RegisterCopyAdder::AddedRegisterCopies copies = 
        regCopyAdder.addMinimumRegisterCopies(po, *targetMachine_, ddg_);
#ifdef DEBUG_REG_COPY_ADDER
    const int tempsAdded = copies.count_;
#endif

#ifdef DEBUG_REG_COPY_ADDER
    if (tempsAdded > 0) {
        ddg_->writeToDotFile(
            (boost::format("%s_after_regcopy_ddg.dot") % ddg_->name()).str());
    }
    ddg_->sanityCheck();
#endif

    MoveNodeSet tempSet;
    for (int i = 0; i < moves.nodeCount(); i++) {
        // MoveNodeGroup relates to DDG so we copy it to
        // a simpler MoveNodeSet container
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

        regCopyAdder.operandsScheduled(copies, *ddg_);

        int bypassedMoves = -1;
        if (tryBypassing) {
            bypassedMoves = softwareBypasser_->bypass(
                moves, *ddg_, *rm_, bypassTrigger);
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
        }

	// TODO: disabled because may cause fail if rescheduled
	// to different bus.
//        tryToDelayOperands(moves);

        if (scheduleResultReads(moves)) {
            resultsFailed = false;
            // Results were scheduled, we are not going to change schedule
            // of this program operation. Lets check if some of the
            // bypassed operands did not produce dead result moves
            if (tryBypassing) {
                try {
                    std::set<std::pair<TTAProgram::Move*, int> >
                        removedMoves;                        
                    softwareBypasser_->removeDeadResults(
                        moves, *ddg_, *rm_, removedMoves);
                    handleRemovedResultMoves(removedMoves);
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
        ddg_->writeToDotFile(
            std::string("ii_") + 
            Conversion::toString(rm_->initiationInterval()) + 
            std::string("_dag.dot"));

        unscheduleAllNodes();
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__,
            "Operands scheduling failed for \'" + moves.toString());
    }
    if (resultsFailed) {
        ddg_->writeToDotFile(
            std::string("ii_") + 
            Conversion::toString(rm_->initiationInterval()) + 
            std::string("_dag.dot"));

        unscheduleAllNodes();
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__,
            "Results scheduling failed for \'" + moves.toString());
    }

    if (options_ != NULL &&
        (options_->dumpDDGsDot() || options_->dumpDDGsDot()) &&
        (resultsFailed || operandsFailed)) {

        //static int failedCounter = 0;
	if (options_->dumpDDGsDot()) {
	    ddgSnapshot(*ddg_, std::string("2_failed.dot"),
			DataDependenceGraph::DUMP_DOT, true);
	} else {
	    ddgSnapshot(*ddg_, std::string("2_failed.xml"),
			DataDependenceGraph::DUMP_XML, true);
	}

        unscheduleAllNodes();
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__,
            (boost::format("Bad BB %s") % ddg_->name()).str());
    }

    regCopyAdder.resultsScheduled(copies, *ddg_);

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

// #define DEBUG_SCHEDULE_OPERAND_WRITES

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
 * @param cycle Earliest cycle for starting scheduling of operands. This
 *              parameter is modified to the highest cycle one
 *              should try next in case scheduling starting from the
 *              given cycle failed.
 * @param moves Moves of the operation execution.
 * @return The cycle the earliest of the operands got scheduled
 */
int
BasicBlockScheduler::scheduleOperandWrites(int& cycle, MoveNodeGroup& moves) {
    const int MAX_OPERAND_CYCLE_DIFFERENCE = 15;

    const int MAX_OPERATION_START_BEFORE_EARLIEST_READ = 50;
    
    int lastOperandCycle = 0;
    int earliestScheduledOperand = INT_MAX;
    int startCycle = 0;
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
        int limit = 0;
        if (moves.node(i).isDestinationOperation()) {
            limit = ddg_->earliestCycle(
                moves.node(i), rm_->initiationInterval()) - 
                MAX_OPERAND_CYCLE_DIFFERENCE;
        } else if (moves.node(i).isSourceOperation()) {
            limit = ddg_->earliestCycle(
                moves.node(i), rm_->initiationInterval()) - 
                MAX_OPERATION_START_BEFORE_EARLIEST_READ;
        } else {
            continue;
        }
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
        int earliestDDG = ddg_->earliestCycle(
            moves.node(i), rm_->initiationInterval());

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
        scheduleMove(*firstToSchedule, startCycle, true);
        if (!firstToSchedule->isScheduled()) {
            ddg_->writeToDotFile(
                std::string("ii_") + 
                Conversion::toString(rm_->initiationInterval()) + 
                std::string("_dag.dot"));

            unscheduleAllNodes();
            throw ModuleRunTimeError(
                __FILE__, __LINE__, __func__,
                (boost::format("Move '%s' failed to schedule")
                % firstToSchedule->toString()).str());
        }
        startCycle = firstToSchedule->cycle();
        if (firstToSchedule->move().destination().isTriggering()) {
            trigger = firstToSchedule;
        } else {
            lastOperandCycle = std::max(lastOperandCycle, startCycle);
        }
    } else {        
        ddg_->writeToDotFile(
            std::string("ii_") + 
            Conversion::toString(rm_->initiationInterval()) + 
            std::string("_dag.dot"));
        
        unscheduleAllNodes();
        throw ModuleRunTimeError(
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
        scheduleMove(moveNode, cycle, true);
        
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
                scheduleMove(moveNode, cycle, true);
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
        scheduleMove(*trigger, lastOperandCycle, true);
        
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

            scheduleMove(moveNode, tempRegLimitCycle, true);
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

    RegisterCopyAdder regCopyAdder(
        DDGPass::interPassData(), *rm_, *selector_);

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

    scheduleMove(moveNode, 0, true);
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
 * @param allowPredicationAndRenaming Whether allowed to remove guard from 
 *        the move being scheduled. Checks again side-effects are still made, 
 *        this can be done only to operand moves and triggers without mem 
 *        write or side-effects.
 */
void
BasicBlockScheduler::scheduleMove(
    MoveNode& moveNode, int earliestCycle, bool allowPredicationAndRenaming) {
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

    unsigned int ii = rm_->initiationInterval();

    // schedule jumps and calls...
    if (moveNode.move().isControlFlowMove()) {

        // the branch should get scheduled last, so the DDG should have
        // only the branch move(s) at hand unscheduled. For software
         // pipelining (ii > 0), branches might be scheduled earlier.
        if (ii == 0 && ddg_->nodeCount() != ddg_->scheduledNodeCount() + 1) {    
        // in rare case the branch moves can have 2 parameters (SPU)
        // and therefore 2 nodes unscheduled. Check if the unscheduled
        // moves are all control flow moves.
            DataDependenceGraph::NodeSet unscheduledMoves =
                ddg_->unscheduledMoves();	  
            for (DataDependenceGraph::NodeSet::iterator i = unscheduledMoves.begin(); 
                i != unscheduledMoves.end(); ++i) {	
                if (!(*i)->move().isControlFlowMove()) {
                    TCEString msg =
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
        if (ii == 0) {
            ddgCycle =
                std::max(
                    ddg_->earliestCycle(moveNode),
                    largest - targetMachine_->controlUnit()->delaySlots());
        } else {
            unsigned int delaySlots = 
                targetMachine_->controlUnit()->delaySlots();
            // is the jump in first or later iteration?
	    MoveNode* jumpLimit = ddg_->findLoopLimitAndIndex(moveNode).first;

            ddgCycle = (((ddg_->earliestCycle(moveNode, ii) + 
                          delaySlots) / ii) + 1)*ii  - 1 - delaySlots;
            if ((unsigned)ddgCycle >= ii - delaySlots) {
                
                int jumpOverlapCount = (ddgCycle + delaySlots) / ii;
                if (jumpLimit == NULL) {
                    // allow jump only in first iteration if we could
                    // not find the limit
                    return;
                } else {
                    // update loop limit counter.
                    TTAProgram::TerminalImmediate& ti = dynamic_cast<
			TTAProgram::TerminalImmediate&>(jumpLimit->move().source());
                    int jumpLimitValue = ti.value().unsignedValue();
                    int loopCounterStep = 1;
                    if (jumpLimitValue != tripCount_) {
                        if (jumpLimitValue == 2 * tripCount_) {
                            loopCounterStep = 2;
                        } else {
                            if (jumpLimitValue == 4 * tripCount_) {
                                loopCounterStep = 4;
                            } else {
                                // don't know how much to decr. counter.
                                return;
                            }
                        }
                    }
                        
                    if (tripCount_ > jumpOverlapCount) {
			jumpLimit->move().setSource(
                            new TTAProgram::TerminalImmediate(
                                SimValue(
                                    jumpLimitValue - 
                                    (jumpOverlapCount * loopCounterStep), 
                                    ti.value().width())));
                    } else {
                        // loop limit is too short. 
                        // would be always executed too many times.
                        return;
                    }
                }
            }
        }
    } else { // not a control flow move:
        ddgCycle = ddg_->earliestCycle(moveNode, ii);

        // <pekka> This is now always called even though renaming is disabled?
        int minRenamedEC = std::max(
            sourceReadyCycle, ddg_->earliestCycle(moveNode, ii, true, true));
        
        // rename if can and may alow scheuduling earlier.
        if (renamer_ != NULL && minRenamedEC < ddgCycle && 
            allowPredicationAndRenaming) {
            minRenamedEC =  rm_->earliestCycle(minRenamedEC, moveNode);
            if (minRenamedEC < ddgCycle) {
                
                if (renamer_->renameDestinationRegister(
                        moveNode, ii != 0, true, true, minRenamedEC)) {
                    ddgCycle = ddg_->earliestCycle(moveNode, ii);
                } else {
#ifdef THIS_IS_BUGGY_WITH_REGCOPY_ADDER                    
                    // renaming already scheduled ones may fail if
                    // loop scheudling.
                    if (rm_->initiationInterval() == 0) {
                        MoveNode *limitingAdep =
                            ddg_->findLimitingAntidependenceSource(moveNode);
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

        if (allowPredicationAndRenaming) {
            // try to get rid of the guard if it gives earlier earliestcycle.
            if (ddg_->earliestCycle(moveNode, ii, false, false, true) 
                < ddgCycle) {
                bool guardNeeded = false;
                if (moveNode.move().destination().isGPR() || 
                    moveNode.move().isControlFlowMove()) {
                    guardNeeded = true;
                } else {
                    // this would be needed only for trigger,
                    // but trigger may change during scheduling.
                    // so lets just limit all operands, it seems 
                    // this does not make the schedule any worse.
                    if (moveNode.isDestinationOperation()) {
                        const Operation& o = 
                            moveNode.destinationOperation().operation();
                        if (o.writesMemory() || o.hasSideEffects() ||
                            o.affectsCount() != 0) {
                            guardNeeded = true;
                        }
                    }
                }
                if (!guardNeeded) {
                    moveNode.move().setGuard(NULL);
                    ddg_->removeIncomingGuardEdges(moveNode);
                    ddgCycle = ddg_->earliestCycle(moveNode, ii);
                    assert(moveNode.move().isUnconditional());
                }
            }
            
            if (ii == 0 && tryToOptimizeWaw(moveNode)) {
                ddgCycle = std::max(ddg_->earliestCycle(moveNode, ii),
                                    moveNode.guardLatency()-1);
            }

        }
    }

    // Earliest cycle from which to start, could depend on result ready
    // for result moves.
    int minCycle = std::max(std::max(earliestCycle, ddgCycle), minCycle_);
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
                   rm_->earliestCycle(rm_->largestCycle() + 1, moveNode) == -1
                   && rm_->initiationInterval() == 0) {

            // If source is constant and node does not have annotation already,
            // we add it if node has no connection, so IU broker and
            // OutputPSocket brokers will add Immediate
            // Example: 27 -> integer0.2
            // With bus capable of transporting 27 as short immediate but
            // no connection from that bus to integer0 unit
            // this may mess up modulo scheduling.
            // TODO: do this more cleanly, this is a kludge.
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
            if (rm_->initiationInterval() == 0) {
                ddg_->writeToDotFile("illegalmach.dot");
                throw IllegalMachine(
                    __FILE__, __LINE__, __func__, msg);
            } else {
                ddg_->writeToDotFile(
                    std::string("ii_") + 
                    Conversion::toString(rm_->initiationInterval()) + 
                    std::string("_dag.dot"));

                unscheduleAllNodes();
                throw ModuleRunTimeError(
                    __FILE__, __LINE__, __func__, msg);
            }   
        }
        return;
    }

    int latestDDG = ddg_->latestCycle(moveNode, ii);

    if (ii != 0 && (minCycle > latestDDG)) {
        
        if (ddg_->latestCycle(moveNode, ii, true) > latestDDG) {

            if (renamer_ != NULL && allowPredicationAndRenaming) {
                // todo: do also otherway
                if (renamer_->renameSourceRegister(moveNode,true, true, true)) {
                    latestDDG = ddg_->latestCycle(moveNode, ii);
                } 
            }
        }
    }

    // test again after renaming? 

    if (ii != 0 && (minCycle > latestDDG)) {

        // if we pre-scheduled jump, unschedule it and try to schdule
        // the node again.
        if (jumpNode_ != NULL) {
            ddg_->writeToDotFile("unschedJump.dot");
            unschedule(*jumpNode_);
            jumpNode_ = NULL;
            scheduleMove(moveNode, earliestCycle, allowPredicationAndRenaming);
            return;
        }

        ddg_->writeToDotFile(
            std::string("ii_") + Conversion::toString(ii) + 
            std::string("_dag.dot"));
        
        unscheduleAllNodes();
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__, "Schedule failed try bigger ii.");
    }

    try {
        rm_->assign(minCycle,  moveNode);
    } catch (const Exception& e) {
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__, e.errorMessageStack());
    }

    if (!moveNode.isScheduled()) {
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__,
            (boost::format("Assignment of MoveNode '%s' failed!")
            % moveNode.toString()).str());
    }
    assert(moveNode.cycle() >= minCycle_);
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

    scheduleMove(*tempMove1, lastUse, true);
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

    /* <pekka> TODO: this could be improved by allowing to schedule also
     *before* the previous use, in the "holes" where it's unused. Now in effect
     the copies serialize the code quite badly. */
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
    
    // TODO: is the true correct here?
    scheduleMove(*tempMove, lastUse, true);
    assert(tempMove->isScheduled());
    scheduledTempMoves_[&operandWrite].insert(tempMove);
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

    scheduleMove(*tempMove1, lastUse, true);
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
    if (moveNode.isScheduled()) {
        rm_->unassign(moveNode);
    }

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
 * Calls unschedule() for all ddg nodes, which are scheduled.
 */
void
BasicBlockScheduler::unscheduleAllNodes() {
    DataDependenceGraph::NodeSet scheduled = ddg_->scheduledMoves();
    for (DataDependenceGraph::NodeSet::iterator i = scheduled.begin();
         i != scheduled.end(); ++i) {
        if (softwareBypasser_ != NULL) {
            softwareBypasser_->removeBypass(**i, *ddg_, *rm_, false);
        }
        unschedule(**i);
    }
    if (softwareBypasser_ != NULL) {
        softwareBypasser_->clearCaches(*ddg_, false);
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
void 
BasicBlockScheduler::notifyScheduled(
    MoveNodeGroup& moves, MoveNodeSelector& selector) {
    
    for (int moveIndex = 0; moveIndex < moves.nodeCount(); ++moveIndex) {
        MoveNode& moveNode = moves.node(moveIndex);
        selector.notifyScheduled(moveNode);
        std::map<const MoveNode*, DataDependenceGraph::NodeSet>::
            iterator tmIter = scheduledTempMoves_.find(&moveNode);
        if (tmIter != scheduledTempMoves_.end()) {
            DataDependenceGraph::NodeSet& tempMoves = tmIter->second;
            for (DataDependenceGraph::NodeSet::iterator i = 
                     tempMoves.begin(); i != tempMoves.end(); i++) {
                if ((**i).isScheduled()) {
                    selector.notifyScheduled(**i);
                }
            }
        }
    }
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
            DataDependenceGraph* criticalPath = ddg.criticalPathGraph();
            criticalPath->writeToDotFile(
                (boost::format(
                    "bb_%s_%d_%s_before_scheduling_critical_path.dot")
                 % ddg.name() % bbCounter % name).str());
            delete criticalPath;
        } else {
            ddg.writeToXMLFile(
                (boost::format("bb_%s_%d_%s_before_scheduling.xml")
                 % ddg.name() % bbCounter % name).str());
        }
    }
    ++bbCounter;
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

/**
 * Checks if the result moves that were removed might remove some
 * resource bottleneck of already scheduled moves.
 *
 * This situation happens at least when a result move used the last RF write
 * port for a cycle and limited the earliest cycle of a previously scheduled 
 * move. Thus, after the last use of that result was scheduled and bypassed,
 * the RF write port is freed as the result move becomes unnecessary, thus
 * the move that was scheduled before could be scheduled earlier. Same
 * thing can happen for bus (move slot) contrained moves and also moves
 * that were restricted by a WaR edge due to the result move, etc.
 */
void
BasicBlockScheduler::handleRemovedResultMoves(
    std::set<std::pair<TTAProgram::Move*, int> > removedMoves) {

    if (removedMoves.size() == 0)
        return;

#ifndef RESCHEDULE_NEXT_CYCLE_AFTER_DRE
    return;
#endif

    const bool DEBUG_PRINT = false; //Application::verboseLevel() > 0;

    if (DEBUG_PRINT) {
        Application::logStream()
            << removedMoves.size() << " dead results eliminated: " 
            << std::endl;
    }

    for (std::set<std::pair<TTAProgram::Move*, int> >::
             const_iterator i = removedMoves.begin(); 
         i != removedMoves.end(); ++i) {

        TTAProgram::Move& eliminatedMove = *(*i).first;
        int cycle = (*i).second;

        if (DEBUG_PRINT) {
            Application::logStream()
                << eliminatedMove.toString() << " (cycle " << cycle << ") ";
        }

        int oldCycle = cycle + 1;
        // look for already scheduled moves that were scheduled after 
        // the cycle of the result move, thus potentially were limited by 
        // the result move earlier
        DataDependenceGraph::NodeSet nextCycleMoves = 
            ddg_->movesAtCycle(oldCycle);

        // if true, try to reschedule all moves at the next cycle (slower,
        // but handles the case when the cycle was bus constrained previously),
        // if false, tries to schedule only potentially RF write port
        // constrained moves (faster)
        bool rescheduleAllSucceedingMoves = false;
        for (DataDependenceGraph::NodeSet::const_iterator m = 
                 nextCycleMoves.begin(); m != nextCycleMoves.end(); ++m) {
            MoveNode& moveNode = **m;

            if (rescheduleAllSucceedingMoves ||
                (moveNode.move().destination().isGPR() &&
                 &eliminatedMove.destination().registerFile() ==
                 &moveNode.move().destination().registerFile())) {

                if (DEBUG_PRINT) {
                    Application::logStream() 
                        << "Trying to reschedule "
                        << moveNode.toString() << " " << std::endl;
                }

                assert(moveNode.isScheduled() && moveNode.cycle() == oldCycle);

                unschedule(moveNode);
                // because a pontially removed WaR, we might be able to
                // schedule this move (write to the same reg) even earlier
                // than the dead result move cycle
                scheduleMove(moveNode, std::max(oldCycle - 10, 0), false);

                if (!moveNode.isScheduled()) {
                    for (int c = 4; c >= 0; --c) {
                        Application::logStream()
                            << "\t\t" << oldCycle - c << ": " 
                            << rm_->instruction(
                                std::max(oldCycle - c, 0))->toString() 
                            << std::endl;
                    }
                    assert(moveNode.isScheduled());
                }

                if (moveNode.cycle() < oldCycle) {
                    if (DEBUG_PRINT) {
                        Application::logStream() 
                            << " OK at cycle " << moveNode.cycle() << ". " << std::endl;
                    }
                } else {
                    // should not get worse schedule!
                    assert(moveNode.cycle() == oldCycle);
                    if (DEBUG_PRINT) {
                        Application::logStream() << " NO change. " << std::endl;
                    }
                }
                
            }
            /// @todo: reschedule also the moves dependent from the 
            /// rescheduled ones that got earlier
        }

        if (DEBUG_PRINT) {
            Application::logStream() << std::endl;
        }
    }
}

void
BasicBlockScheduler::printStats() const {
    if (Application::verboseLevel() > 0) {
    }
}

/**
 * Tries to get rid of WaW dependence from unconditional to conditional.
 *
 * if the conditional move's result is overwritten by the cond for
 * all users, make the unconditional move conditional, with opposite guard.
 */
bool 
BasicBlockScheduler::tryToOptimizeWaw(const MoveNode& moveNode) {

    if (ddg_->earliestCycle(moveNode, 0 , false, true) >=
        ddg_->earliestCycle(moveNode)) {
        return false;
    }
    
    MoveNode* wawPred = NULL;
    DataDependenceEdge* wawEdge = NULL;

    DataDependenceGraph::EdgeSet inEdges = ddg_->inEdges(moveNode);
    for (DataDependenceGraph::EdgeSet::iterator i = inEdges.begin();
         i != inEdges.end(); i++) {
        DataDependenceEdge* e = *i;

        if (e->dependenceType() == DataDependenceEdge::DEP_WAW &&
            e->edgeReason() == DataDependenceEdge::EDGE_REGISTER &&
            !e->tailPseudo()) {
            if (wawPred == NULL) {
                wawPred = &ddg_->tailNode(**i);
                wawEdge = e;
            } else {
                return false;
            }
        }
    }
    if (wawPred == NULL) {
        return false;
    }

    assert(wawPred->isScheduled());
    int wawPredCycle = wawPred->cycle();
    
    if (!wawPred->move().isUnconditional()) {
        return false;
    }

    DataDependenceGraph::NodeSet gdMoves = ddg_->guardDefMoves(moveNode);
    for (DataDependenceGraph::NodeSet::iterator i = gdMoves.begin();
         i != gdMoves.end(); i++) {
        MoveNode& mn = **i;
        assert(mn.isScheduled());
        if (mn.cycle() + moveNode.guardLatency() > wawPredCycle) {
            return false;
        }
    }

    // check that no other usage of the data.
    DataDependenceGraph::NodeSet consumers = ddg_->regRawSuccessors(*wawPred);
    DataDependenceGraph::NodeSet consumers2 = ddg_->regRawSuccessors(moveNode);
    for (DataDependenceGraph::NodeSet::iterator i = consumers.begin();
         i != consumers.end(); i++) {
        MoveNode* mn = *i;
        if (consumers2.find(mn) == consumers2.end() &&
            (mn->move().isUnconditional() ||
             !ddg_->exclusingGuards(*mn, moveNode))) {
            return false;
        }
    }

    unschedule(*wawPred);

    TTAProgram::CodeGenerator cg(*targetMachine_);
    wawPred->move().setGuard(cg.createInverseGuard(moveNode.move().guard()));

    ddg_->copyDepsOver(*wawPred, true, false);
    
    ddg_->copyIncomingGuardEdges(moveNode, *wawPred);
    ddg_->copyOutgoingGuardWarEdges(moveNode, *wawPred);
    
    ddg_->removeEdge(*wawEdge);
    
    bool revert = false;

    try {
        scheduleMove(*wawPred, wawPredCycle, false);

        if (!wawPred->isScheduled()) {
            revert = true;
        }
        if (wawPred->isScheduled() && wawPred->cycle() > wawPredCycle) {
            unschedule(*wawPred);
            revert = true;
        }
    } catch (Exception&e) {
        revert = true;
    }

    if (revert) {
        wawPred->move().setGuard(NULL);
        ddg_->removeIncomingGuardEdges(*wawPred);
        ddg_->removeOutgoingGuardWarEdges(*wawPred);
        scheduleMove(*wawPred, wawPredCycle, false);
        ddg_->connectNodes(*wawPred, moveNode, *wawEdge);
        return false;
    }

    return true;
}

void 
BasicBlockScheduler::tryToDelayOperands(MoveNodeGroup& moves) {
    
    // try to then push non-triggers back
    // to as close as possible to tirggers.
    // this frees up the FU to be used fr other operations before
    // this operation.
    // first search first cycle when this FU is used.

    int ec = INT_MAX;
    MoveNode* trigger = NULL;
    for (int i = 0; i < moves.nodeCount(); i++) {
        MoveNode& moveNode = moves.node(i);
        if (!moveNode.isDestinationOperation()) {
            continue;
        }
        if (moveNode.move().isTriggering()) {
            trigger = &moveNode;
        } else {
            int oldCycle = moveNode.cycle();
            if (oldCycle < ec) {
                ec = oldCycle;
            }
        }
    }

    int triggerCycle = trigger->cycle();
    
    bool failed = false;
    while (ec < triggerCycle && !failed) {
        for (int i = 0; i < moves.nodeCount(); i++) {
            MoveNode& moveNode = moves.node(i);
            if (!moveNode.isDestinationOperation()) {
                continue;
            }
            if (&moveNode != trigger) {
                int oldCycle = moveNode.cycle();
                if (oldCycle == ec) {
                    int latest = ddg_->latestCycle(moveNode);
                    if (latest > oldCycle) {
                        latest = std::min(latest, triggerCycle);
                        assert(latest >= ec);
                        rm_->unassign(moveNode);
                        int latestrm = rm_->latestCycle(latest, moveNode);
                        if (latestrm == ec) {
                            failed = true;
                        }
                        assert(latestrm >= ec);
                        rm_->assign(latestrm, moveNode);
                    } else {
                        failed = true;
                    }
                }
            }
        }
        ec = INT_MAX;
        // first earliest scheduled operands.
        for (int i = 0; i < moves.nodeCount(); i++) {
            MoveNode& moveNode = moves.node(i);
            if (!moveNode.isDestinationOperation()) {
                continue;
            }
            if (&moveNode != trigger) {
                int oldCycle = moveNode.cycle();
                if (oldCycle < ec) {
                    ec = oldCycle;
                }
            }
        }
    }
}

// find which movenode is the trigger of an operation.
MoveNode*
BasicBlockScheduler::findTrigger(
    const ProgramOperation& po, const TTAMachine::Machine& mach) {

    MoveNode* triggerFromPO = po.triggeringMove();
    if (triggerFromPO) {
        return triggerFromPO;
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

    int portC = fu->portCount();
    for (int i = 0; i < portC; i++) {
        auto p = fu->port(i);
        const TTAMachine::FUPort* port = dynamic_cast<const TTAMachine::FUPort*>(p);
        if (port != nullptr && port->isTriggering()) {
            const TTAMachine::HWOperation* hwop =
            fu->operation(po.operation().name());
            ioIndex = hwop->io(*port);
            return &po.inputNode(ioIndex).at(0);
        }
    }
    return NULL;
}
