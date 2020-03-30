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
 * @file BFLateBypass.cc
 *
 * Definition of BFLateBypass class
 *
 * Performs a bypass of noy-yet-scheduled result into already scheduled
 * use of the value.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFLateBypass.hh"
#include "BFScheduleBU.hh"
#include "BFScheduleExact.hh"

#include "SimpleResourceManager.hh"
#include "DataDependenceGraph.hh"
#include "MoveNode.hh"
#include "Move.hh"

#include "Bus.hh"
#include "MoveNodeDuplicator.hh"
#include "BFMergeAndKeepUser.hh"

//#define DEBUG_BUBBLEFISH_SCHEDULER
//#define DEBUG_LOOP_SCHEDULER

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
#define DEBUG_LOOP_SCHEDULER
#endif


bool
BFLateBypass::operator()() {

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
   std::cerr << "\t\tPerforming late bypass: " << dst_.toString() <<std::endl;
#else
#ifdef DEBUG_LOOP_SCHEDULER
    if (ii() != 0) {
        std::cerr << "\t\tPerforming late bypass, dst: "
                  << dst_.toString() << std::endl;
    }
#endif
#endif
    originalCycle_ = dst_.cycle();
    originalBus_ = &dst_.move().bus();
    MoveNode* prologMN = ii() ?  duplicator().getMoveNode(dst_) : NULL;
    if (prologMN != NULL) {
        prologBus_ = &prologMN->move().bus();
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\tProlog move: " << prologMN->toString()
                  << std::endl;
        std::cerr << "\t\t\t\tLate bypass saving prolog bus: "
                  << prologBus_->name() << std::endl;
#endif
    }

    bool dstTriggering = dst_.move().isTriggering();
    // this one did not create the prolog copy so do not remove it here.
    unassign(dst_, false);

    // Take the predecssors so that they can be notified because their
    // antideps may change
    // makes compiling too slow, disabled for now
    DataDependenceGraph::NodeSet pred = ddg().predecessors(dst_);

    if (!runPostChild(new BFMergeAndKeepUser(sched_, src_, dst_))) {
        assign(originalCycle_, dst_, originalBus_, NULL, NULL, prologBus_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tMerge and keep fail!" << std::endl;
#endif
        return false;
    }

    // TODO: this does too much optimizations?

    BFOptimization* sched = NULL;
    if ((dst_.move().isControlFlowMove() && dstTriggering) ||
        (ii() != 0 &&
         static_cast<DataDependenceGraph*>(ddg().rootGraph())->
         writesJumpGuard(dst_))) {
        if (lc_ < originalCycle_) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "Late bypass fail: must use originalcycle "
                "which is too late!!!" << std::endl;
#endif
            undo();
            return false;
        }
        sched = new BFScheduleExact(sched_, dst_, originalCycle_);
    } else {
        sched = new BFScheduleBU(sched_, dst_, lc_, false, false, false);
    }
    if ((*sched)()) {
        // TODO: get this from bypass distance setting?
        if (dst_.cycle() + 3 < originalCycle_) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\t\tToo long bypass dist, fail late bypass!"
                      << std::endl;
#endif
            sched->undo();
            delete sched;
            undo();
            return false;
        }

        postChildren_.push(sched);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\tLate bypass ok: " << dst_.toString() << std::endl;
#endif

        // now notify antideps sources
        for (auto i : pred) {
            mightBeReady(*i);
        }

        return true;
    } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\tLate bypass sched fail.: " << dst_.toString()
                  << std::endl;
#endif
        delete sched;
        undo();
        return false;
    }
}


void
BFLateBypass::undoOnlyMe() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\tUndoing late bypass: " << dst_.toString() << std::endl;
#endif

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\tReassigning move: " << dst_.toString() <<" to cycle: "
	      << originalCycle_ << " bus: " << originalBus_->name() << std::endl;
    if (!canAssign(
            originalCycle_, dst_, originalBus_, NULL, NULL, prologBus_)) {
        std::cerr << "assign to original cycle fail, cycle: "
                  << originalCycle_ << " movenode: "
                  << dst_.toString() << std::endl;
        for (int unsigned i = 0; i < dst_.destinationOperationCount(); i++) {
            std::cerr << "\tDestination op: "
                      << dst_.destinationOperation(i).toString() << std::endl;
        }
        ddg().writeToDotFile("late_bypass_undo_fail.dot");
        assert(false);
    }
#endif
    assign(originalCycle_, dst_, originalBus_, NULL, NULL, prologBus_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tReassigned move:" << dst_.toString() << std::endl;
#endif
}
