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
#include "MachineConnectivityCheck.hh"

//#define DEBUG_OUTPUT
//#define DEBUG_REG_COPY_ADDER
//#define CFG_SNAPSHOTS
//#define BIG_DDG_SNAPSHOTS
//#define DDG_SNAPSHOTS
//#define SW_BYPASSING_STATISTICS
//#define DEBUG_BYPASS

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
    endCycle_(INT_MAX), bypassDistance_(5) {

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

    if (options_ != NULL && options_->bypassDistance() != -1) {
        bypassDistance_ = options_->bypassDistance();
    }


    // empty need not to be scheduled
    if (ddg.nodeCount() == 0 ||
        (ddg.nodeCount() == 1 && !ddg.node(0).isMove())) {
        return;
    }

    // INT_MAX/2 won't work on trunk due to multithreading injecting empty
    // instructions into the beginning of basic block.    
    endCycle_ = INT_MAX/1000;
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
            movesRemoved = true;
        } else {
            if (firstMove.move().destination().isRA()) {
                scheduleMove(firstMove, endCycle_);
                notifyScheduled(moves, selector);                
            } else {
                scheduleRRMove(firstMove);
                notifyScheduled(moves, selector);                
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
    if (rm_->largestCycle() > (int)endCycle_) {
        ddg.writeToDotFile("largest_bigger_than_endcycle.dot");
        std::cerr << "rm largest cycle bigger than endCycle_!" <<
            std::endl << "This may break delay slot filler!" <<
            std::endl << " rm largest: " << rm_->largestCycle() <<
            " end cycle: " << endCycle_ << std::endl;
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
BUBasicBlockScheduler::scheduleOperation(
    MoveNodeGroup& moves, BUMoveNodeSelector& selector)
    throw (Exception) {

    ProgramOperation& po =
        (moves.node(0).isSourceOperation())?
        (moves.node(0).sourceOperation()):
        (moves.node(0).destinationOperation());
    bool operandsFailed = true;
    bool resultsFailed = true;
    int resultsStartCycle = endCycle_;
    int maxResult = resultsStartCycle;

#ifdef DEBUG_REG_COPY_ADDER
    ddg_->setCycleGrouping(true);
    ddg_->writeToDotFile(
        (boost::format("%s_before_ddg.dot") % ddg_->name()).str());
#endif
    RegisterCopyAdder regCopyAdder(BasicBlockPass::interPassData(), *rm_, true);
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
//    ddg_->sanityCheck();
#endif
    bool bypass = true;
    bool bypassLate = false;
    bool dre = true;
    while ((operandsFailed || resultsFailed) &&
        resultsStartCycle >= 0) {
        maxResult = scheduleResultReads(
            moves, resultsStartCycle, bypass, bypassLate, dre);
        if (maxResult != -1) {
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
                    unscheduleResultReadTempMoves(moveNode);
                    undoBypass(moveNode);
                }
            }
            resultsFailed = true;
            if (bypass) {
                bypass = false;            
                bypassLate = true;
            } else if (bypassLate) {
                bypassLate = false;
            } else {
                // We will try to schedule results in earlier cycle
                resultsStartCycle--;                
            }
                
            continue;
        }

        if (scheduleOperandWrites(moves, resultsStartCycle)) {
            operandsFailed = false;
        } else {
            // Scheduling some operand failed, unschedule all moves, try earlier
            for (int i = 0; i < moves.nodeCount(); i++){
                MoveNode& moveNode = moves.node(i);
                if (moveNode.isScheduled()) {
                    unschedule(moveNode);
                    if (moveNode.isDestinationOperation()) {
                        unscheduleInputOperandTempMoves(moveNode);
                    }
                    if (moveNode.isSourceOperation()) {
                        unscheduleResultReadTempMoves(moveNode);
                        undoBypass(moveNode);
                    }
                }
            }
            operandsFailed = true;
            maxResult--;
            if (bypass) {
                bypass = false;            
                bypassLate = false;
            } else if (bypassLate) {
                bypassLate = false;
            } else {
                resultsStartCycle--;                
                resultsStartCycle = std::min(maxResult, resultsStartCycle);                                
            }
        }
    }
    // This fails if we reach 0 cycle.
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
            (boost::format("bb_%s_2_scheduling.dot")
             % ddg_->name()).str());
        throw ModuleRunTimeError(
            __FILE__, __LINE__, __func__,
            "Operands scheduling failed for \'" + moves.toString());
    }
  
  for (int i = 0; i < moves.nodeCount(); i++) {
      MoveNode* node = &moves.node(i);
      if (node->isScheduled()) {
          selector.notifyScheduled(*node);
          std::map<const MoveNode*, DataDependenceGraph::NodeSet >::
          iterator tmIter = scheduledTempMoves_.find(node);
          if (tmIter != scheduledTempMoves_.end()) {
              DataDependenceGraph::NodeSet& tempMoves = tmIter->second;
              for (DataDependenceGraph::NodeSet::iterator i =
                   tempMoves.begin(); i != tempMoves.end(); i++) {
                  selector.notifyScheduled(**i);
              }
          }          
      } else if (MapTools::containsKey(bypassDestinations_, node) && dre) {
      
#ifdef DEBUG_BYPASS
          std::cerr << "\tDroping node " << node->toString() << std::endl;
          ddg_->writeToDotFile("before_copyDeps.dot");
#endif                
          static_cast<DataDependenceGraph*>(ddg_->rootGraph())->
              copyDepsOver(*node, true, true); 
          DataDependenceGraph::NodeSet preds = ddg_->predecessors(*node); 
          ddg_->dropNode(*node);
          if (ddg_->rootGraph() != ddg_) {
              assert(!node->isScheduled());
              ddg_->rootGraph()->removeNode(*node);
          }
#ifdef DEBUG_BYPASS
          ddg_->writeToDotFile("after_dropNode.dot");
#endif                
          
          for (DataDependenceGraph::NodeSet::iterator i = 
               preds.begin(); i != preds.end(); i++) {
               selector.mightBeReady(**i);
          }          
          std::map<const MoveNode*, DataDependenceGraph::NodeSet >::
              iterator tmIter = scheduledTempMoves_.find(node);
          if (tmIter != scheduledTempMoves_.end()) {
              DataDependenceGraph::NodeSet& tempMoves = tmIter->second;
              for (DataDependenceGraph::NodeSet::iterator i =
                   tempMoves.begin(); i != tempMoves.end(); i++) {
                  if ((*i)->isScheduled()) {
                      selector.notifyScheduled(**i);
                  } else {
#ifdef DEBUG_BYPASS
                      std::cerr << "\tDroping temp move for node " 
                      << node->toString() << ", " << (*i)->toString() << std::endl;
                      ddg_->writeToDotFile("before_temp_copyDeps.dot");
#endif                                  
                      static_cast<DataDependenceGraph*>(ddg_->rootGraph())->
                        copyDepsOver(**i, true, true);                   
                      ddg_->dropNode(**i);
                      if (ddg_->rootGraph() != ddg_) {
                          assert(!node->isScheduled());
                          ddg_->rootGraph()->removeNode(*node);
                      }                      
#ifdef DEBUG_BYPASS
                      ddg_->writeToDotFile("after_temp_dropNode.dot");
#endif                                      
                  }    
              }
          }          
          
      } else {
          std::cerr << "Node " << node->toString() << " did not get scheduled"
          << std::endl;
      }      
  }
          
  bypassDestinationsCycle_.clear();
  bypassDestinations_.clear();

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

    ProgramOperation& po =
        (moves.node(0).isSourceOperation())?
        (moves.node(0).sourceOperation()):
        (moves.node(0).destinationOperation());

    int unscheduledMoves = 0;
    int scheduledMoves = 0;
    MoveNode* trigger = NULL;

    for (int i = 0; i < moves.nodeCount(); i++) {
        if (!moves.node(i).isDestinationOperation()) {
            continue;
        }
        // count how many operand moves will need to be scheduled
        unscheduledMoves++;
        if (moves.node(i).move().destination().isTriggering()) {
            int limit = moves.node(i).latestTriggerWriteCycle();
            // update starting cycle based on scheduled results
            // if necessary.
            cycle = (limit < cycle) ? limit : cycle;
        }
    }
    int counter = 0;

    // Trigger is scheduled, try to schedule other operands
    while (unscheduledMoves != scheduledMoves && counter < 5 && cycle >=0) {
        // try to schedule all input moveNodes, also find trigger
        // Try to schedule trigger first, otherwise the operand
        // may get scheduled in cycle where trigger does not fit and
        // later cycle will not be possible for trigger.
        trigger = findTrigger(po);
        if (trigger != NULL && !trigger->isScheduled()) {
            if (scheduleOperand(*trigger, cycle) == false) {
                break;
            }
            cycle = trigger->cycle();

            if (trigger->move().destination().isTriggering()) {
                // We schedulled trigger, this will give us latest possible cycle
                // in order not to have operands later then trigger.
                if (ddg_->hasNode(*trigger)) {
                    // handle moving fu deps.
                    // they may move trigger to later time.
                    ddg_->moveFUDependenciesToTrigger(*trigger);
                    int triggerLatest =
                        std::min(ddg_->latestCycle(*trigger), cycle);
                    triggerLatest = rm_->latestCycle(triggerLatest, *trigger);
                    if (triggerLatest != INT_MAX &&
                        triggerLatest > trigger->cycle()) {
                        unschedule(*trigger);
                        unscheduleInputOperandTempMoves(*trigger);
                        if (scheduleOperand(*trigger, triggerLatest) == false) {
                            // reschedule with new dependencies failed,
                            // bringing back originaly scheduled trigger
                            scheduleOperand(*trigger, cycle);
                        }
                        cycle = trigger->cycle();
                    }
                }
            }
        }

        for (int moveIndex = 0; moveIndex < moves.nodeCount(); ++moveIndex) {
            MoveNode& moveNode = moves.node(moveIndex);
            // skip the result reads
            if (!moveNode.isDestinationOperation()) {
                continue;
            }

            if (moveNode.isScheduled()) {
                continue;
            }
            if (scheduleOperand(moveNode, cycle) == false) {
                unscheduleInputOperandTempMoves(*trigger);
                break;
            }

        }
        // Tests if all input moveNodes were scheduled
        scheduledMoves = 0;
        for (int moveIndex = 0; moveIndex < moves.nodeCount(); ++moveIndex) {
            MoveNode& moveNode = moves.node(moveIndex);
            // skip the result reads
            if (!moveNode.isDestinationOperation()) {
                continue;
            }
            if (moveNode.isScheduled()) {
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
            return true;
        }
        cycle--;
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
BUBasicBlockScheduler::scheduleResultReads(
    MoveNodeGroup& moves, int cycle, bool bypass, bool bypassLate, bool dre)
    throw (Exception) {

    int maxResultCycle = cycle;
    int tempRegLimitCycle = cycle;

    for (int moveIndex = 0; moveIndex < moves.nodeCount(); ++moveIndex) {
        if (!moves.node(moveIndex).isSourceOperation()) {
            continue;
        }
        MoveNode& moveNode = moves.node(moveIndex);
        unsigned bypassedCount = 0;
        bool bypassSuccess = false;
        if (!moveNode.isScheduled()) {
            if (!moveNode.isSourceOperation()) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    (boost::format("Move to schedule '%s' is not "
                    "result move!") % moveNode.toString()).str());
            }
            if (bypass) {
                bool edgesCopied = false;
                // First try to bypass all uses of the result
                OrderedSet destinations = findBypassDestinations(moveNode, 1).first;
                if (destinations.size() > 0) {
                    for (OrderedSet::iterator it = destinations.begin(); 
                        it != destinations.end(); it++) {
                        if (!ddg_->guardsAllowBypass(moveNode, **it)) {
                            std::cerr << "\t\tguardsnotallowbypass" << std::endl;
                            bypassSuccess = false;
                            continue;
                        }
                        MoveNode* temp = succeedingTempMove(moveNode);                         
                        if (!(*it)->isScheduled() && temp == (*it)) {
                            // skip temp moves if unscheduled.
                            // The temp moves of reading operand could be 
                            // scheduled and bypass will try to skip those
                            // but temp moves of result are there for reason
                            // so bypass would only revert to original
                            // status which is unschedulable.
                            continue;
                        }
                        assert((*it)->isScheduled());
                        if ((*it)->isDestinationVariable()) {
                            MoveNode* firstWrite =
                            ddg_->firstScheduledRegisterWrite(
                                (*it)->move().destination().registerFile(),
                                (*it)->move().destination().index());                        
                            if (firstWrite != (*it)) {
                            // If bypassing to temporary register
                            // missing edges in DDG could cause 
                            // overwrite of temporary value before it is 
                            // consumed. Avoid this error for now.
                            // TODO: figure out some better logic, this leads
                            // to inefficiency.
                                continue;
                            }
                        }                        
                        int originalCycle = (*it)->cycle();
                        bypassDestinationsCycle_[&moveNode].push_back(
                            originalCycle);  
#ifdef DEBUG_BYPASS                            
                        ddg_->writeToDotFile("beforeUnschedule.dot");
#endif                        
                        unschedule(**it);
                        if (!ddg_->mergeAndKeep(moveNode, **it)) {
                            std::cerr << "Merge fail." << std::endl;
                            scheduleMove(**it, originalCycle);
                            bypassSuccess = false;
                            continue;
                        }
                        
                        bypassDestinations_[&moveNode].push_back(*it);       
                        assert((*it)->isScheduled() == false);
                        scheduleMove(**it, originalCycle + bypassDistance_);
#ifdef DEBUG_BYPASS                        
                        std::cerr << "Created " << (*it)->toString()
                        << " with original cycle " << originalCycle << std::endl;
#endif                        
                        if (!(*it)->isScheduled()) {
                            // Scheduling bypass failed, undo and try to 
                            // schedule other possible bypasses.
                            undoBypass(moveNode, *it, originalCycle);
                            bypassDestinations_[&moveNode].pop_back();
                            bypassDestinationsCycle_[&moveNode].pop_back();                        
                            bypassSuccess = false;
                        } else {
#ifdef DEBUG_BYPASS
                            if ((*it)->cycle() < originalCycle) {
                                std::cerr << "Bypassed node " << 
                                (*it)->toString() << "rescheduled "
                                "earlier then it's original location - " <<
                                originalCycle << std::endl;
                            }
#endif                        
                            maxResultCycle =
                                ((*it)->cycle() > maxResultCycle) ?
                                (*it)->cycle() : maxResultCycle;                    
                            bypassedCount++;
                            bypassSuccess = true;
                            if (!edgesCopied) {
                            // In case operands reads same register that
                            // result writes, removing result move after
                            // successfull bypass could lead to operand
                            // scheduled after the register it reads is 
                            // overwriten. This should prevent such situation.
                                ddg_->copyDepsOver(moveNode, true, true);
                                edgesCopied = true;
                            }
                        }
                    }
                }
                if (dre && bypassSuccess && 
                    bypassedCount == destinations.size() &&
                    !ddg_->resultUsed(moveNode)) {
                    // All uses of result were bypassed, result will be removed
                    // after whole operation is scheduled.
                    continue;
                }                
            }
            // Find out if RegCopyAdded create temporary copy for output move
            MoveNode* test = succeedingTempMove(moveNode);
            if (test != NULL) {
                // Output temp move exists. Check where the original move
                // will be writing, that is the temporary register.
                MoveNode* firstWrite =
                    ddg_->firstScheduledRegisterWrite(
                        moveNode.move().destination().registerFile(),
                        moveNode.move().destination().index());
                if (firstWrite != NULL) {
                    assert(firstWrite->isScheduled());
                    tempRegLimitCycle = firstWrite->cycle() - 1;
                }
            }

            // Schedule temporary move first.
            scheduleResultReadTempMoves(
                moveNode, moveNode, tempRegLimitCycle);
            // If there was temporary result read scheduled, write needs
            // to be at least one cycle earlier.    
            tempRegLimitCycle = std::min(tempRegLimitCycle - 1, cycle);
            scheduleMove(moveNode, tempRegLimitCycle);
            if (!moveNode.isScheduled()) {
                // Scheduling result failed due to some of the bypassed moves
                // will try again without bypassing anything.
                undoBypass(moveNode);
                return -1;
            }            
            if (bypassLate) {                
                bool edgesCopied = false;
                // First try to bypass all uses of the result
                OrderedSet destinations = findBypassDestinations(moveNode, 1).first;
                if (destinations.size() > 0) {
                    for (OrderedSet::iterator it = destinations.begin(); 
                         it != destinations.end(); it++) {
                        if (!ddg_->guardsAllowBypass(moveNode, **it)) {
                            std::cerr << "\t\tguardsnotallowbypass" << std::endl;
                            bypassSuccess = false;
                            continue;
                        }
                        assert((*it)->isScheduled());
                        if ((*it)->isDestinationVariable()) {
                            MoveNode* firstWrite =
                            ddg_->firstScheduledRegisterWrite(
                                (*it)->move().destination().registerFile(),
                                (*it)->move().destination().index());                        
                            if (firstWrite != (*it)) {
                                // If bypassing to temporary register
                                // missing edges in DDG could cause 
                                // overwrite of temporary value before it is 
                                // consumed. Avoid this error for now.
                                // TODO: figure out some better logic, this leads
                                // to inefficiency.
                                continue;
                            }
                        }                        
                        int originalCycle = (*it)->cycle();
                        bypassDestinationsCycle_[&moveNode].push_back(
                            originalCycle);  
#ifdef DEBUG_BYPASS                            
                        ddg_->writeToDotFile("beforeUnscheduleLate.dot");
#endif                        
                        unschedule(**it);
                        if (!ddg_->mergeAndKeep(moveNode, **it)) {
                            std::cerr << "Merge fail." << std::endl;
                            scheduleMove(**it, originalCycle);
                            bypassSuccess = false;
                            continue;
                        }
                        
                        bypassDestinations_[&moveNode].push_back(*it);       
                        assert((*it)->isScheduled() == false);
                        scheduleMove(**it, originalCycle + bypassDistance_);
#ifdef DEBUG_BYPASS                        
                        std::cerr << "Created late " << (*it)->toString()
                        << " with original cycle " << originalCycle << std::endl;
#endif                        
                        if (!(*it)->isScheduled()) {
                            // Scheduling bypass failed, undo and try to 
                            // schedule other possible bypasses.
                            undoBypass(moveNode, *it, originalCycle);
                            bypassDestinations_[&moveNode].pop_back();
                            bypassDestinationsCycle_[&moveNode].pop_back();                        
                            bypassSuccess = false;
                        } else {
#ifdef DEBUG_BYPASS
                            if ((*it)->cycle() < originalCycle) {
                                std::cerr << "Bypassed node late " << 
                                (*it)->toString() << "rescheduled "
                                "earlier then it's original location - " <<
                                originalCycle << std::endl;
                            }
#endif                        
                            maxResultCycle =
                                ((*it)->cycle() > maxResultCycle) ?
                                (*it)->cycle() : maxResultCycle;                    
                            bypassedCount++;
                            bypassSuccess = true;
                            if (!edgesCopied) {
                                // In case operands reads same register that
                                // result writes, removing result move after
                                // successfull bypass could lead to operand
                                // scheduled after the register it reads is 
                                // overwriten. This should prevent such situation.
                                ddg_->copyDepsOver(moveNode, true, true);
                                edgesCopied = true;
                            }
                        }
                    }
                }
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

    RegisterCopyAdder regCopyAdder(BasicBlockPass::interPassData(), *rm_, true);

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
//    ddg_->sanityCheck();
#endif

    scheduleRRTempMoves(moveNode, moveNode, endCycle_);
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
    int ddgCycle = endCycle_;
    if (moveNode.move().isControlFlowMove()) {
        ddgCycle = endCycle_ - targetMachine_->controlUnit()->delaySlots();

        if (ddg_->latestCycle(moveNode) < ddgCycle) {
            // Trying to schedule CFG move but data dependence does not
            // allow it to schedule in correct place
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                (boost::format("Move '%s' needs to be scheduled in %d,"
                " but data dependence does not allow it!")
                % moveNode.toString() % ddgCycle).str());
        }
    } else { // not a control flow move:
        ddgCycle = ddg_->latestCycle(moveNode);
        if (renamer_ != NULL) {
            int latestFromTrigger =
                (moveNode.isDestinationOperation()) ?
                    moveNode.latestTriggerWriteCycle() : endCycle_;
            int minRenamedEC = std::min(
                latestFromTrigger, ddg_->latestCycle(
                    moveNode, INT_MAX, true)); // TODO: 0 or INT_MAX

            // rename if can and may alow scheuduling later.
            if (minRenamedEC > ddgCycle) {
                minRenamedEC =  rm_->latestCycle(minRenamedEC, moveNode);
                if (minRenamedEC > ddgCycle) {
                    if (renamer_->renameSourceRegister(
                            moveNode, false, true, true, minRenamedEC)) {
                        ddgCycle = ddg_->latestCycle(moveNode);
                    }
#ifdef THIS_IS_BUGGY_WITH_REGCOPY_ADDER
                    else {
                        MoveNode *limitingAdep =
                            ddg_->findLimitingAntidependenceDestination(
                                moveNode);
                        if (limitingAdep != NULL) {
                            // don't try to rename is same operation.
                            // as it would not help at all.
                            if (!moveNode.isDestinationOperation() ||
                                !limitingAdep->isSourceOperation() ||
                                &moveNode.destinationOperation() !=
                                &limitingAdep->sourceOperation()) {
                                if (renamer_->renameDestinationRegister(
                                        *limitingAdep, false, true, true)) {
                                    ddgCycle =
                                        ddg_->latestCycle(moveNode);
                                }
                            }
                        }
                    }
#endif
                }
            }
        }
    }

    ddgCycle = (ddgCycle == INT_MAX) ? endCycle_ : ddgCycle;
    assert(ddgCycle != -1);
    // Earliest cycle from which to start, could depend on result ready
    // for result moves.
    int minCycle = std::min(latestCycle, ddgCycle);

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
    int earliestDDG = ddg_->earliestCycle(moveNode);
    minCycle = rm_->latestCycle(minCycle, moveNode);
    if (minCycle < earliestDDG) {
        // Bypassed move could get scheduld too early, this should prevent it
        return ;
    }
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
 * Schedules the (possible) temporary register copy moves (due to missing
 * connectivity) preceeding the given result read move.
 *
 * The function recursively goes through all the temporary moves added to
 * the given result move.
 *
 * @param resultMove A temp move whose predecessor has to be scheduled.
 * @param resultRead The original move of which temp moves to schedule.
 * @param firstWriteCycle Recursive function parameter.
 */
void
BUBasicBlockScheduler::scheduleResultReadTempMoves(
    MoveNode& resultMove,
    MoveNode& resultRead,
    int firstWriteCycle)
    throw (Exception) {

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
        // Found second temp move. First temp move will write register
        // which second will read.
        MoveNode* firstWrite =
            ddg_->firstScheduledRegisterWrite(
                tempMove1->move().destination().registerFile(),
                tempMove1->move().destination().index());
        if (firstWrite != NULL && firstWrite->isScheduled())
            firstWriteCycle = firstWrite->cycle();
    }
    scheduleResultReadTempMoves(*tempMove1, resultRead, firstWriteCycle);
    scheduleMove(*tempMove1, firstWriteCycle -1);
    assert(tempMove1->isScheduled());
    scheduledTempMoves_[&resultRead].insert(tempMove1);

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
BUBasicBlockScheduler::scheduleInputOperandTempMoves(
    MoveNode& operandMove, MoveNode& operandWrite)
    throw (Exception) {

    /* Because temporary register moves do not have WaR dependency edges
       between the other possible uses of the same temp register in
       the same operation, we have to limit the scheduling of the new
       temp reg move to the last use of the same temp register so
       we don't overwrite the temp registers of other operands. */
    int lastUse = endCycle_;

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
        // First cycle where temporary register will be read, this should
        // be actuall operand move cycle!
        MoveNode* firstRead =
            ddg_->firstScheduledRegisterRead(
                tempMove->move().destination().registerFile(),
                tempMove->move().destination().index());
        if (firstRead != NULL) {
            assert(firstRead->isScheduled());
            lastUse = firstRead->cycle();
            if (operandMove.isScheduled() && lastUse != operandMove.cycle()) {
                Application::logStream() << "\tFirst register read problem "
                << operandMove.toString() << " temp " << firstRead->toString()
                << std::endl;
            }
        }
    }

    if (tempMove == NULL)
        return; // no temp moves found

    scheduleMove(*tempMove, lastUse - 1);
    if (!tempMove->isScheduled()) {
        std::cerr << "temp move: " << tempMove->toString()
                  << "not scheduled."
                  << std::endl;
        ddg_->writeToDotFile("failTempNotSched.dot");
    }
    assert(tempMove->isScheduled());
    scheduledTempMoves_[&operandWrite].insert(tempMove);
    scheduleInputOperandTempMoves(*tempMove, operandWrite);
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
BUBasicBlockScheduler::scheduleRRTempMoves(
    MoveNode& regToRegMove, MoveNode& firstMove, int lastUse)
    throw (Exception) {

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
        MoveNode* firstWrite =
            ddg_->firstScheduledRegisterWrite(
                tempMove1->move().destination().registerFile(),
                tempMove1->move().destination().index());
        if (firstWrite != NULL) {
            assert(firstWrite->isScheduled());
            lastUse = firstWrite->cycle();
        }            
    }
    scheduleResultReadTempMoves(*tempMove1, firstMove, lastUse);
    scheduleMove(*tempMove1, lastUse -1);
    assert(tempMove1->isScheduled());
    scheduledTempMoves_[&firstMove].insert(tempMove1);
}

/**
 * Finds the temp move preceding the given movenode.
 *
 * If it does not have one, returns null.
 *
 * @param current MoveNode whose tempmoves we are searching
 * @return tempMove preceding given node, or null if does not exist.
 */
MoveNode*
BUBasicBlockScheduler::precedingTempMove(MoveNode& current) {

    DataDependenceGraph::NodeSet pred = ddg_->predecessors(current);
    MoveNode* result = NULL;
    for (DataDependenceGraph::NodeSet::iterator i = pred.begin();
         i != pred.end(); ++i) {
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
 * Checks existence of temporary moves and schedules operand according
 * to discovered dependencies.
 */
bool
BUBasicBlockScheduler::scheduleOperand(MoveNode& node, int cycle) {
    int tempRegLimitCycle = cycle;
    // Find out if RegCopyAdded create temporary move for input.
    // If so, the operand has to be scheduled before the other use
    // of same temporary register.
    MoveNode* test = precedingTempMove(node);
    if (test != NULL) {
        // Input temp move exists. Check where the move is reading data
        // from, so operand move can be scheduled earlier then
        // the already scheduled overwriting cycle.
        MoveNode* firstWrite =
        ddg_->firstScheduledRegisterWrite(
            node.move().source().registerFile(),
            node.move().source().index());
        if (firstWrite != NULL) {
            assert(firstWrite->isScheduled());
            tempRegLimitCycle = firstWrite->cycle() -1;
        }
    }
    cycle = std::min(cycle, tempRegLimitCycle);
    // This must pass, since we got latest cycle from RM.
    scheduleMove(node, cycle);
    if (node.isScheduled() == false) {
        // the latest cycle may change because scheduleMove may do things like
        // register renaming, so this may fail.
        return false;
    }
    scheduleInputOperandTempMoves(node, node);
    return true;
}

/**
 * Finds a destination where to bypass.
 *
 * Goes through ddg and checks for connectivity.
 *
 * @return pair of destination of bypass and amount of regs skipped by the bypass.
 */
std::pair<BUBasicBlockScheduler::OrderedSet, int>
BUBasicBlockScheduler::findBypassDestinations(
    MoveNode& node, int maxHopCount) {

    MoveNode* n = &node;
    OrderedSet okDestination;
    int hops = 0;
    // DDG can only handle single hops so far.
    // TODO: Fix when DDG could handle multiple destinations
    maxHopCount = 1;
    for (int i = 0; i < maxHopCount; i++) {
        MoveNodeSet rrDestinations = ddg_->onlyRegisterRawDestinations(*n);
        if (rrDestinations.count() == 0) {
            break;
        }
        for (int j = 0; j < rrDestinations.count(); j++) {
            n = &rrDestinations.at(j);
            if (ddg_->onlyRegisterEdgeIn(*n) == NULL) {
                // No bypassing of moves with multiple register definition
                // sources.
                // TODO: bypass destination with multiple definition sources
                // using inverse guard of guarded source.
                continue;
            }
            std::set<const TTAMachine::Port*> destinationPorts;            
            destinationPorts.insert(&n->move().destination().port());
            
            if (MachineConnectivityCheck::canSourceWriteToAnyDestinationPort(
                 node, destinationPorts)) {
                okDestination.insert(n);                
            }    
            destinationPorts.clear();
        }
        hops = i+1;
    }
    return std::pair<OrderedSet,int>(okDestination, hops);
}

/**
 * Remove all the bypasses of result write in moveNode and restore and schedule
 * original moves.
 */
void 
BUBasicBlockScheduler::undoBypass(
    MoveNode& moveNode, MoveNode* single, int oCycle) {
    if (single == NULL) {
        if (MapTools::containsKey(bypassDestinations_, &moveNode)) {
            std::vector<MoveNode*> destinationsVector = 
                bypassDestinations_[&moveNode];                    
            std::vector<std::pair<MoveNode*, int> > rescheduleVector;
                
            for (unsigned int j = 0; j < destinationsVector.size(); j++) {            
            // unschedule and unmerge all bypassed nodes
                MoveNode* dest = destinationsVector[j];
                if (!dest->isScheduled())
                    continue;
                unschedule(*dest);
                ddg_->unMerge(moveNode, *dest);
                int originalCycle =                 
                    bypassDestinationsCycle_[&moveNode][j];                
                rescheduleVector.push_back(
                    std::pair<MoveNode*, int>(dest, originalCycle));
            }
            for (unsigned int i = 0; i < rescheduleVector.size(); i++) {
            // reassign nodes to their original places
                std::pair<MoveNode*, int> dest = rescheduleVector[i];
                scheduleMove(*dest.first, dest.second);
                if (!dest.first->isScheduled()) {
                    ddg_->writeToDotFile("unbypassFailed.dot");
                    std::cerr << " Source: " << moveNode.toString()
                    << ", Original: " << dest.first->toString() << 
                    ", original cycle: " << dest.second << std::endl;                    
                }
                assert(dest.first->isScheduled());                
            }
            
            bypassDestinations_.erase(&moveNode);          
            bypassDestinationsCycle_.erase(&moveNode);                                      
        }
    } else {
        ddg_->unMerge(moveNode, *single);
        scheduleMove(*single, oCycle);
        if (!single->isScheduled()) {
            ddg_->writeToDotFile("unbypassFailed.dot");
            std::cerr << " Source: " << moveNode.toString()
            << ", Original: " << single->toString() << ", original cycle: " 
            << oCycle << std::endl;
        }
        assert(single->isScheduled());
    }
}
