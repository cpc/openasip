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
 * @file BF2ScheduleFront.cc
 *
 * Definition of BF2ScheduleFront class
 *
 * Tries to schedule a group of moves.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BF2ScheduleFront.hh"
#include "BF2Scheduler.hh"
#include "Move.hh"
#include "Machine.hh"
#include "ControlUnit.hh"
#include "SimpleResourceManager.hh"
#include "BasicBlockScheduler.hh"
#include "Operation.hh"
#include "Unit.hh"
#include "HWOperation.hh"
#include "BUMoveNodeSelector.hh"

#include "BFScheduleBU.hh"
#include "BFScheduleTD.hh"
#include "BFScheduleExact.hh"
#include "BFDREEarly.hh"

#include "BFSwapOperands.hh"
#include "BFDropPreShared.hh"
#include "FUPort.hh"
#include "Terminal.hh"

//#define DEBUG_BUBBLEFISH_SCHEDULER

bool
BF2ScheduleFront::operator()() {

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << std::endl << "Got: " << mn_.toString()
              << " to schedule(1)" << std::endl;
#endif
    requeueOtherMovesOfSameOp(mn_);
    MoveNode* mn2 = getMoveNodeFromFrontBU();
    assert(mn2 != NULL);

    bool ok = scheduleFrontFromMove(*mn2);
    // Do not waste memory by keeping it in the stack of performed
    // operations.
    // Not needed anymore. Can calculate again for next front.
    // Consider sharing between different fronts if still too slow.
    pathLengthCache_.clear();
    return ok;
}


bool BF2ScheduleFront::scheduleFrontFromMove(MoveNode& mn) {
    MoveNode* mn2 = &mn;
    int latest = lc_;
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "ScheduleFrontFromMove called: " <<mn.toString()<< std::endl;
#endif
    while (mn2 != NULL) {
        if (mn2->move().isControlFlowMove()) {
            latest = std::min(
                latest,
                lc_- targetMachine().controlUnit()->delaySlots());
        }

        if (!tryToScheduleMoveOuter(*mn2, latest)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	    std::cerr << "TryToScheduleMoveOuter " << mn2->toString() <<
		" failed!, latest now" << latest << std::endl;
#endif
            undo();
            int smallestRMCycle = rm().smallestCycle();
            if (smallestRMCycle == INT_MAX) {
                smallestRMCycle = lc_;
            }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "Latest after outer fail: " << latest << std::endl;
            std::cerr << "smallest rm cycle: " << smallestRMCycle <<
                " max latency+1: " << targetMachine().maximumLatency()+1
                      << std::endl;
#endif
            if (latest < 0 || latest <
                (smallestRMCycle - (targetMachine().maximumLatency()+1))) {
                if (Application::verboseLevel() > 1 ||
                    rm().initiationInterval() == 0) {
                    std::cerr << "Retry to too early cycle. cannot schedule: "
                              << mn2->toString()
                              << std::endl;
                    ddg().writeToDotFile("fail.dot");
                }
                return false;
            } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
		std::cerr << "OK or retry at earlier cycle.." << std::endl;
#endif
	    }
        }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	std::cerr << "TryToScheduleMoveOuter ok: " << mn2->toString() <<std::endl;
#endif
        clearSchedulingFront();
        requeueOtherMovesOfSameOp(mn);
        mn2 = getMoveNodeFromFrontBU();
    }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "Schedulingfront scheduled ok!: " << this << std::endl;
    printFront("\t");
#endif
    for (auto i : schedulingFront_) {
        if (!i->isScheduled()) {
            if (!sched_.isDeadResult(*i) &&
                !sched_.isPreLoopSharedOperand(*i)) {
                std::cerr << "Front Has unscheduled move: "
                          << (*i).toString() << std::endl;
                ddg().writeToDotFile("front_unscheduled_move.dot");
                assert(0);
            }
        } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tNotifying scheduled: " << (*i).toString()
                      << std::endl;
#endif
            selector().notifyScheduled(*i);

        }
    }

    for (auto n: nodesToNotify_) {
        if (!sched_.isDeadResult(*n) && !n->isScheduled()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tMight be ready: " << n->toString() << std::endl;
#endif
            selector().mightBeReady(*n);
        }
    }
    clearSchedulingFront();
    return true;
}



bool BF2ScheduleFront::tryToScheduleMoveOuter(MoveNode& mn, int& latestCycle){
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << std::endl << "\tGot: " << mn.toString() << " to schedule(2)"
              << std::endl;
#endif
    while(true) {
        if (sched_.isPreLoopSharedOperand(mn)) {
            assert(prologRM() != NULL);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tShould scheudule pre-opshare to prefix: "
                      << mn.toString() << std::endl;
#endif
            BFOptimization* sbu = new BFDropPreShared(sched_, mn);
            bool ok = runPreChild(sbu);
            if (!ok) {
                int lcFront = latestScheduledOfFrontCycle();
                if (lcFront != -1 && lcFront <= latestCycle) {
                    latestCycle = lcFront -1;
                } else{
                    latestCycle--;
                }
                return false;
            } else {
                return true;
            }
        }

        // Kill (result) moves that write to values that are
        // never used(even when not bypassing).
        if (mn.isDestinationVariable()) {
            BFOptimization* dre = new BFDREEarly(sched_, mn);
            if (runPreChild(dre)) {
                return true;
            }
        }

        BF2Scheduler::SchedulingLimits limits = getPreferredLimits(mn);
        if (limits.direction == BF2Scheduler::EXACTCYCLE &&
            latestCycle < limits.latestCycle) {
            latestCycle = INT_MIN;
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tlatestCycle later than exact limit. failing."
                      << std::endl;
#endif
            return false;
        }

        limits.latestCycle = std::min(limits.latestCycle, latestCycle);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\tFirst all optimizations on" << std::endl;
#endif
        int schedRes = scheduleMove(mn, limits, true, true, true);
        if (schedRes >= 0) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tScheduling of: " << mn.toString() << " ok "
                      << std::endl;
#endif
            return true;
        }

        if (limits.direction == BF2Scheduler::TOPDOWN) {
            limits.direction = BF2Scheduler::BOTTOMUP;
            limits.earliestCycle = 0;
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tTOPDOWN failed, trying bottomup" << std::endl;
#endif
            if (scheduleMove(mn, limits) >= 0) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\ttopdown back to bottomup ok" << std::endl;
#endif
                return true;
            }
        }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	    std::cerr << "\tTrying without early sharing" << std::endl;
#endif
	    // disable early sharing;
	    if (scheduleMove(mn, limits, true, true, false) >=0) {
            return true;
	    }


#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	    std::cerr << "\tTrying without early BP" << std::endl;
#endif
	    // disable early bypass;
	    if (scheduleMove(mn, limits, false, true) >=0) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tok without early BP" << std::endl;
#endif
            return true;
	    }

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	    std::cerr << "\tTrying without early BP and without early sharing"
                  << std::endl;
#endif
        if (scheduleMove(mn, limits, false, true, false) >= 0) {
            return true;
        }

	    if (tryRevertEarlierBypass(mn)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tTrying to Revert earlier bypass.."
                      << std::endl;
#endif
		// do not make cycle go earlier, but forbid some bypass.
            return false;
	    }

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	    std::cerr << "\tTrying without late BP" << std::endl;
#endif
	    // disable late bypass
	    if (scheduleMove(mn, limits, true, false) >=0) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tok without late BP" << std::endl;
#endif
            return true;
	    }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	    std::cerr << "\tTrying without any BP" << std::endl;
#endif
	    // disable both bypasses
	    if (scheduleMove(mn, limits, false, false) >=0) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tok without any BP" << std::endl;
#endif
            return true;
	    }

	    if (mn.destinationOperationCount() > 1) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "forbidding operand share: " << mn.toString()
                      << std::endl;
#endif
            illegalOperandShares_.insert(&mn);
            return true;
	    }

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	    std::cerr << "\tScheduleMove failing, need to tr earlier cycle"
                  << std::endl;
#endif

	    int lcFront = latestScheduledOfFrontCycle();
	    if (lcFront != -1 && lcFront <= latestCycle) {
            latestCycle = lcFront -1;
	    } else{
            latestCycle--;
	    }
	    return false;
    }
    std::cerr << "end of schduleMoveOuter, should not be here!" << std::endl;
    return true;
}

int BF2ScheduleFront::latestScheduledOfFrontCycle() {
    int lc = -1;
    for (auto mn : schedulingFront_) {
        if (mn->isScheduled() && mn->cycle() > lc) {
            lc = mn->cycle();
        }
    }
    return lc;
}



MoveNode* BF2ScheduleFront::getMoveNodeFromFrontBU() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\tGetting moveNode from front" << std::endl;
#endif
    int sd = -2;
    MoveNode* selectedMN = NULL;
    for (auto mn: schedulingFront_) {

        if (mn->isScheduled() || sched_.isDeadResult(*mn)) {
            continue;
        }

        int cursd;
        auto j = pathLengthCache_.find(mn);
        if (j != pathLengthCache_.end()) {
            cursd = j->second;
        } else {
            cursd = ddg().maxSourceDistance(*mn);
            pathLengthCache_[mn] = cursd;
        }

        // weight more last moves of unready ops
        if (mn->isDestinationOperation()) {
            if (mn->isLastUnscheduledMoveOfDstOp()) {
                cursd += 10000;
            }
        }

        if (cursd > sd &&
            !sched_.hasUnscheduledSuccessors(*mn)) {
            selectedMN = mn;
            sd = cursd;
        }
    }

    if (selectedMN != NULL && !sched_.isPreLoopSharedOperand(*selectedMN)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	std::cerr << "\t\tSelected:" << selectedMN->toString() << std::endl;
#endif
        MoveNode* trigger = getSisterTrigger(*selectedMN, targetMachine());
        if (trigger != NULL && !trigger->isScheduled() &&
            !sched_.hasUnscheduledSuccessors(*trigger)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\tReturning trigger instead:"
                      << trigger->toString() << std::endl;
#endif

            BFSwapOperands* bfswo = new BFSwapOperands(sched_, *trigger);
            if (runPreChild(bfswo)) {
                return bfswo->switchedMNs().second;
            } else {
                return trigger;
            }
        }
    }
    if (selectedMN != NULL) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	std::cerr << "\tSelected MN: " << selectedMN->toString() << std::endl;
#endif
    } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	std::cerr << "Front empty, returning NULL" << std::endl;
#endif
    }
    return selectedMN;
}

int BF2ScheduleFront::prefResultCycle(const MoveNode& mn) {
    int prefCycle = INT_MAX;
    if (mn.isSourceOperation()) {
        if (!mn.isDestinationOperation()) {
            const ProgramOperation& sop = mn.sourceOperation();
            for (int i = 0; i < sop.outputMoveCount(); i++) {
                const MoveNode& outNode = sop.outputMove(i);
                if (!outNode.isScheduled()) {
                    continue;
                }

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\tOut node: " << outNode.toString()
                          << " is scheduled!" << std::endl;
#endif
                const TTAMachine::HWOperation& hwop =
                    *sop.hwopFromOutMove(outNode);
                // find the OSAL id of the operand of the output being tested
                const int outNodeOutputIndex =
                    outNode.move().source().operationIndex();
                int onLatency = hwop.latency(outNodeOutputIndex);
                int latestTrigger = outNode.cycle() - onLatency;
                const int myOutIndex = mn.move().source().operationIndex();
                int myLatency = hwop.latency(myOutIndex);
                int myPreferredCycle = latestTrigger + myLatency;
                if (myPreferredCycle < prefCycle) {
                    prefCycle = myPreferredCycle;
                }
            }
        }
    }
    return prefCycle;
}

BF2Scheduler::SchedulingLimits
BF2ScheduleFront::getPreferredLimits(
    const MoveNode& mn) {
    BF2Scheduler::SchedulingLimits limits;
    int prefCycle = prefResultCycle(mn);

    if (prefCycle != INT_MAX) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "Schedulong TOP-DOWN(TD)" << mn.toString() << std::endl;
        std::cerr << "Setting earl. limit to pref:" << prefCycle << std::endl;
#endif
        limits.earliestCycle = prefCycle;
        limits.direction = BF2Scheduler::TOPDOWN;
    }
    if (mn.move().isControlFlowMove() &&
        getSisterTrigger(mn, targetMachine()) == &mn) {
        prefCycle = lc_- targetMachine().controlUnit()->delaySlots();
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "Control flow move requires exact cycle: "
                  << prefCycle << std::endl;
#endif
        limits.earliestCycle = limits.latestCycle = prefCycle;
        limits.direction = BF2Scheduler::EXACTCYCLE;
    }

    if (sched_.scheduleJumpGuardBetweenIters()) {
        if (ii() != 0 &&
            static_cast<DataDependenceGraph*>(
                ddg().rootGraph())->writesJumpGuard(mn)) {
            // if other writes to the guard reg, cannot schedule.
            if (ddg().hasOtherRegWawSources(mn)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "has other reg waw sources, cannot loopsched"
                          << std::endl;
#endif
                limits.earliestCycle = limits.latestCycle = -1;
                limits.direction = BF2Scheduler::EXACTCYCLE;
                return limits;
            }
            //TODO: check that no WAW deps to other moves?
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "Writes jump guard: " << mn.toString() << std::endl;
#endif
            assert(mn.isDestinationVariable());
            int glat = mn.move().destination().registerFile().guardLatency()+
                targetMachine().controlUnit()->globalGuardLatency();
            prefCycle = ii() - glat;
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "Correct place for guard wr would be: " << prefCycle
                      << std::endl;
#endif
            limits.earliestCycle = limits.latestCycle = prefCycle;
            limits.direction = BF2Scheduler::EXACTCYCLE;
        }
    }
    return limits;
}

int
BF2ScheduleFront::scheduleMove(
    MoveNode& mn,
    BF2Scheduler::SchedulingLimits limits,
    bool allowEarlyBypass, bool allowLateBypass, bool allowEarlySharing) {

    BFOptimization* sched;
    switch (limits.direction) {
    case BF2Scheduler::BOTTOMUP:
        if (!sched_.isPreLoopSharedOperand(mn)) {
            sched = new BFScheduleBU(
                sched_, mn, limits.latestCycle, allowEarlyBypass,
                allowLateBypass, allowEarlySharing);
            break;
        } else {
            std::cerr << "Is pre loop shared oper, sch to prolog instead: " <<
                mn.toString() << std::endl;
            assert(false);
            break;
        }
    case BF2Scheduler::TOPDOWN:
        sched = new BFScheduleTD(
	    sched_, mn, limits.earliestCycle, allowLateBypass);
        break;
    case BF2Scheduler::EXACTCYCLE:
        assert(limits.earliestCycle == limits.latestCycle);
        sched = new BFScheduleExact(sched_,mn,limits.earliestCycle);
        break;
    default:
        return -1;
    }
    return runPreChild(sched) ? 1 : -1;
}

void BF2ScheduleFront::requeueOtherMovesOfSameOp(MoveNode& mn) {
    DataDependenceGraph::NodeSet moves =
        allNodesOfSameOperation(mn);
    for (auto mn : moves) {
        if (!mn->isFinalized()) { // && !sched_.isPreLoopSharedOperand(*mn)) {
            mn->setIsInFrontier(true);
            schedulingFront_.insert(mn);
        }
    }
}

DataDependenceGraph::NodeSet
BF2ScheduleFront::allNodesOfSameOperation(MoveNode& mn) {

    DataDependenceGraph::NodeSet queue;
    DataDependenceGraph::NodeSet nodes;
    queue.insert(&mn);

    while (!queue.empty()) {
        MoveNode* mn = *queue.begin();
        nodes.insert(mn);
        queue.erase(mn);
        if (mn->isSourceOperation()) {
            BUMoveNodeSelector::queueOperation(
                mn->sourceOperation(), nodes, queue);
        }

        for (unsigned int i = 0; i < mn->destinationOperationCount(); i++) {
            BUMoveNodeSelector::queueOperation(
                mn->destinationOperation(i), nodes, queue);
        }

        if (BF2Scheduler::isSourceUniversalReg(*mn)) {
            if (ddg().hasNode(*mn)) {
                MoveNode* bypassSrc =
                    ddg().onlyRegisterRawSource(*mn, false, false);
                if (bypassSrc != NULL) {
                    if (nodes.find(bypassSrc) == nodes.end()) {
                        queue.insert(bypassSrc);
                    }
                } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                    std::cerr << "Warning:Cannot find src for forced bypass. "
                          << " Inst. scheduler may fail/deadlock" <<std::endl;
#endif
                }
            }
        }

        if (BF2Scheduler::isDestinationUniversalReg(*mn)) {
            if (ddg().hasNode(*mn)) {
                DataDependenceGraph::NodeSet rrDestinations =
                    ddg().onlyRegisterRawDestinations(*mn, false, false);
                for (auto n : rrDestinations) {
                    if (nodes.find(n) == nodes.end()) {
                        queue.insert(n);
                    }
                }
            }
        }
    }
    return nodes;
}


void BF2ScheduleFront::printFront(const TCEString& prefix) {
    for (auto node : schedulingFront_) {
        if (sched_.isDeadResult(*node)) {
            std::cerr << "DEAD ";
        }
        std::cerr << prefix << node->toString() << std::endl;
    }
}


void BF2ScheduleFront::undoOnlyMe() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "should undo front. printing front:" << std::endl;
    printFront("\t");
#endif
    clearSchedulingFront();

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "should have cleared. printing front:" << std::endl;
    printFront("\t");
#endif
}

void BF2ScheduleFront::clearSchedulingFront() {
    for (auto node : schedulingFront_) {
        node->setIsInFrontier(false);
    }
    schedulingFront_.clear();
}


MoveNode* BF2ScheduleFront::findInducingBypassSource(MoveNode& mn) {
    DataDependenceGraph::NodeSet queue;
    DataDependenceGraph::NodeSet processedNodes;
    queue.insert(&mn);

    while (!queue.empty()) {
        MoveNode* mn = *queue.begin();
        processedNodes.insert(mn);
        queue.erase(mn);

        if (!sched_.isDestinationUniversalReg(*mn)) {
            for (auto i : bypassSources_) {
                if (i.second == mn) {
                    return mn;
                }
            }
        }

        if (mn->isSourceOperation()) {
            MoveNode *result =
                findInducingBypassSourceFromOperation(
                    mn->sourceOperation(), processedNodes, queue);
            if (result != NULL) {
                return result;
            }
        }

        for (unsigned int i = 0; i < mn->destinationOperationCount(); i++) {
            MoveNode *result =
                findInducingBypassSourceFromOperation(
                    mn->destinationOperation(i), processedNodes, queue);
            if (result != NULL) {
                return result;
            }
        }
    }
    return NULL;
}

MoveNode* BF2ScheduleFront::findInducingBypassSourceFromOperation(
    ProgramOperation& po,
    const DataDependenceGraph::NodeSet& processedNodes,
    DataDependenceGraph::NodeSet& queue) {
    for (int j = 0; j < po.inputMoveCount(); j++) {
        MoveNode& inputMove = po.inputMove(j);
        // only add if not already added
        if (processedNodes.find(&inputMove) == processedNodes.end()) {
            queue.insert(&inputMove);
        }
    }

    for (int j = 0; j < po.outputMoveCount(); j++) {
        MoveNode& outputMove = po.outputMove(j);
        // only add if not already added
        if (processedNodes.find(&outputMove) == processedNodes.end()) {
            if (!sched_.isDestinationUniversalReg(outputMove)) {
                for (auto i : bypassSources_) {
                    if (i.second == &outputMove) {
                        return &outputMove;
                    }
                }
            }
            queue.insert(&outputMove);
        }
    }
    return NULL;
}

bool BF2ScheduleFront::tryRevertEarlierBypass(MoveNode& mn) {

    MoveNode* inducingBypass = findInducingBypassSource(mn);
    while (inducingBypass != NULL &&
	   sched_.isDestinationUniversalReg(*inducingBypass)) {
        for (auto i : bypassSources_) {
            if (i.second == inducingBypass) {
                inducingBypass = findInducingBypassSource(*(i.first));
                break;
            }
        }
    }
    if (inducingBypass == NULL) {
        return false;
    } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\tMaking illegal bypas of src: "
                  << inducingBypass->toString() << std::endl;
#endif
        if (illegalBypassSources_.count(inducingBypass)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\tIs already illegal bypass! " << std::endl;
#endif
            return false;
        }
        illegalBypassSources_.insert(inducingBypass);
        return true;
    }
}

void BF2ScheduleFront::mightBeReady(MoveNode& n) {
    nodesToNotify_.insert(&n);
}

void BF2ScheduleFront::appendBypassSources(MoveNodeMap& map) {
    AssocTools::append(bypassSources_, map);
}
