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
 * @file BFLoopBypass.cc
 *
 * Definition of BFLoopBypass class
 *
 * Performs an early bypass over a loop edge, reading the value
 * which was produced in the previous iteration.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFLoopBypass.hh"
#include "BFDREEarly.hh"
#include "MachineConnectivityCheck.hh"
#include "DataDependenceGraph.hh"
#include "BF2Scheduler.hh"
#include "BF2ScheduleFront.hh"
#include "BFUnscheduleMove.hh"
#include "BFScheduleMove.hh"
#include "MoveNodeDuplicator.hh"
#include "SimpleResourceManager.hh"
#include "BFDRELoop.hh"
#include "Move.hh"
#include "BFPushDepsUp.hh"
#include "BFMergeAndKeepUser.hh"

//#define DEBUG_BUBBLEFISH_SCHEDULER

bool
BFLoopBypass::operator()() {

    if (src_.cycle() < (int)ii()) {
        std::cerr << "src cycle too small on loopbypass, todo: WHY?:"
                  << src_.toString() << " ii: " << ii() << std::endl;
        ddg().writeToDotFile("srccycletoosmall.dot");
        return false;
    }

    // loop bypass source must be scheduled.
    if (!src_.isScheduled()) {
        return false;
    }
    // only bypass from operations, for now.
    if (!src_.isSourceOperation()) {
        return false;
    }

    int ddgc = ddg().latestCycle(dst_);

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tTrying to loop bypass, src: " << src_.toString()
              << " dst: " << dst_.toString() << std::endl;
    std::cerr << "\t\tii is: " << ii() << std::endl;
#endif

    // Create a copy to prolog.
    auto a = duplicator().duplicateMoveNode(dst_, true, true);
    prologMN_ = a.first;
    setPrologSrcFUAnno(*prologMN_, src_);
    createdPrologCopy_ = a.second;

    int srcCycle = src_.cycle();
    BFOptimization* unsched = new BFUnscheduleMove(sched_, src_);
    runMidChild(unsched); // cannot fail.
    assert(!dst_.isScheduled());

    if (!runMidChild(
            new BFMergeAndKeepUser(sched_, src_, dst_, true, false))) {
        unsched->undo();
        delete prologMN_;
        return false;
    }

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\tTrying to DRE source: " << src_.toString() << std::endl;
#endif
    bool dred = runMidChild(new BFDREEarly(sched_,src_));

    if (!dred && sched_.hasEpilog()) { // try loop-dre
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tTrying loop DRE instead" << std::endl;
#endif
        dred = runMidChild(new BFDRELoop(sched_, src_, srcCycle));
        if (dred) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\t\tLoop DRE successful!" << std::endl;
#endif
        }
    }

    sched_.currentFront()->bypassed(src_, dst_);

    if (!dred) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tCould not DRE source, returning it back."
                  << std::endl;
#endif
        BFOptimization* sched = new BFSchedule(sched_, src_, srcCycle);
        if (!runMidChild(sched)) { // screwed.
            undoAndRemoveChildren(midChildren_);

            if (createdPrologCopy_) {
                duplicator().disposeMoveNode(prologMN_);
            }
            undoAndRemovePreChildren();
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "loop bypass fail due cannot sched original to place"
                      << std::endl;
#endif
            return false;
        }
    }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\tLoop-bypass " << src_.toString()
              << " to " << dst_.toString() << std::endl;
    std::cerr << "\t\t\tDdgc in loop bypass: " << ddgc << std::endl;
#endif
    int ec = 0;
    if (ddgc < 0) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\tddg negative, ddgc: " << ddgc << " ec: "
                  << ec << "ii:" << ii() << std::endl;
        TCEString dotName("loopBypassNegative_ii");
        dotName << ii() << "dst_" << dst_.nodeID() << ".dot";
        ddg().writeToDotFile(dotName);
#endif
    }

    ec = ddg().earliestCycle(dst_, ii());

    // bypass saves one cycle in critical pass so allowing one slack here
    // does not usually cost anything.
    // cannot bypass to nodes in second half, put to first half.
    // allow more cycles if source DRE'd.
    if (ddgc == (signed)ii() && ec < (signed)ii()) {
        ddgc--;
    }

    if (ddgc >= 0 && ec > ddgc) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tEarliest(" << ec << ") is later than"
                  << " latest(" << ddgc
                  << "). Might be able to push something up.." << std::endl;
#endif
        if (runMidChild(new BFPushDepsUp(sched_, dst_, ddgc))) {
            ec = ddg().earliestCycle(dst_, ii());
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\t\tPushed antidep something up, ec now: " << ec
                      << std::endl;
            std::cerr << "\t\t\tMove is now: " << dst_.toString() <<std::endl;
#endif
        } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\t\tPush antidep up fail, aborting loop bypas"
                      << std::endl;
#endif
            undoAndRemovePostChildren();
            undoAndRemoveChildren(midChildren_);
            if (createdPrologCopy_) {
                duplicator().disposeMoveNode(prologMN_);
            }
            undoAndRemovePreChildren();
            return false;
        }
    }


    if (ddgc >= 0 && ddgc < (signed)ii()) {
        // need the guard set?
        // TODO: currently may get earlier than guard write.
        if ((needJumpGuard(dst_, ddgc))) {
            if (!dst_.move().isUnconditional() ||
                jumpGuardAvailableCycle(dst_) > ddgc) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "Cannot guard conditional move!" << std::endl;
#endif
                undoAndRemovePostChildren();
                undoAndRemoveChildren(midChildren_);
                if (createdPrologCopy_) {
                    duplicator().disposeMoveNode(prologMN_);
                }
                undoAndRemovePreChildren();
                return false;
            } else {
                useJumpGuard_ = true;
                setJumpGuard(dst_);
            }
        }
        int rmlc = rm().latestCycle(ddgc, dst_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\tRMLC for loop bypass: " << rmlc << std::endl;
#endif
        if (rmlc >= ec &&
            (!useJumpGuard_ || rmlc >= jumpGuardAvailableCycle(dst_))) {
            rm().assign(rmlc, dst_);
            setPrologDstFUAnno(*prologMN_, dst_);
            int prologCycle = BF2Scheduler::PROLOG_CYCLE_BIAS + rmlc;
            if (prologRM()->canAssign(prologCycle, *prologMN_)) {
                prologRM()->assign(prologCycle, *prologMN_);
                prologMoves_[&dst_] = prologMN_;
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\t\t\tProlog mn assigned: "
                          << prologMN_->toString() << std::endl;
                std::cerr << "\t\t\t\tLoop bypass all done and ok!"
                          << std::endl;
#endif
                return true;
            } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\t\t\tLoop bypass prolog assign fail: "
                          << prologMN_->toString() << std::endl;
#endif
                rm().unassign(dst_);
            }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        } else {
            std::cerr << "loop bypass assign fail: " << dst_.toString()
                      << std::endl;
#endif
        }

        if (useJumpGuard_) {
            unsetJumpGuard(dst_);
        }
    } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "ddg problem, ddgc: " << ddgc << " ec: " << ec
                  << "ii:" << ii() << std::endl;
        TCEString dotName("loopBypassFail_ii");
        dotName << ii() << "dst_" << dst_.nodeID() << ".dot";
        ddg().writeToDotFile(dotName);
#endif
    }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "Could not assign loop-bypassed move" << std::endl;
#endif
    // !ok
    undoAndRemovePostChildren();
    undoAndRemoveChildren(midChildren_);
    if (createdPrologCopy_) {
        duplicator().disposeMoveNode(prologMN_);
    }

    undoAndRemovePreChildren();
    return false;
}

void BFLoopBypass::undoMerge() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tReverted loop bypass of: " << src_.toString()
              << " to " << dst_.toString() << std::endl;
#endif
    sched_.currentFront()->undidBypass(src_, dst_);
}
void
BFLoopBypass::undoOnlyMe() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "undo only me calld for loop bypassed move: "
              << dst_.toString() << std::endl;
#endif
    assert(prologMN_ != NULL);
    assert(prologRM()!=NULL);
    assert(&dst_ != NULL);
    assert(dst_.isScheduled());
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "prolog move: " << prologMN_->toString() << std::endl;
#endif
    prologRM()->unassign(*prologMN_);
    prologMoves_.erase(&dst_);

    rm().unassign(dst_);
    if (useJumpGuard_) {
        unsetJumpGuard(dst_);
    }

    undoAndRemoveChildren(midChildren_);
    if (createdPrologCopy_) {
        duplicator().disposeMoveNode(prologMN_);
    }

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "only only me done for loop bypassed move: "
              << dst_.toString() << std::endl;
#endif
}


/**
 * Tries to run another Reversible (before running this).
 *
 * If the running succeeds, makes it a pre-child.
 * If the running fails, deletes it.
 *
 * @return true if running child succeeded, false if failed.
 */
bool BFLoopBypass::runMidChild(Reversible* midChild) {
    return runChild(midChildren_, midChild);
}
