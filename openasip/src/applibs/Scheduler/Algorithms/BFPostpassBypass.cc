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
 * @file BFPostpassBypass.cc
 *
 * Definition of BFPostpassBypass class
 *
 * Performs a bypass after scheduling everything.
 * May also bypass over a loop edge.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "DataDependenceGraph.hh"
#include "BFPostpassBypass.hh"
#include "BFUnscheduleMove.hh"
#include "BFScheduleExact.hh"
#include "Move.hh"
#include "BFScheduleBU.hh"
#include "Terminal.hh"
#include "BFPostpassDRE.hh"
#include "BFUnscheduleFromBody.hh"
#include "BFAssignToBody.hh"
#include "BFPostpassLoopDRE.hh"
#include "MoveNodeDuplicator.hh"
#include "BFMergeAndKeepUser.hh"

bool BFPostpassBypass::operator()() {


    assert(dst_.isScheduled());
    assert(src_.isScheduled());
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    static int counter = 0;
    static int counter2 = 0;
#endif

    int originalCycle = dst_.cycle();
    int srcCycle = src_.cycle();

    if (loopBypass_ && originalCycle >= (int)ii()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "too late cycle for loop bypass" << std::endl;
#endif
        return false;
    }

    const TTAMachine::FunctionUnit* ssfu = src_.isSourceOperation() ?
        &src_.move().source().functionUnit() : nullptr;

    const TTAMachine::FunctionUnit* ddfu = dst_.isDestinationOperation() ?
        &dst_.move().destination().functionUnit() : nullptr;

    if (!loopBypass_) {
        runPreChild(new BFUnscheduleMove(sched_, dst_));
    } else {
        runPreChild(new BFUnscheduleFromBody(sched_, dst_));
    }
    runPreChild(new BFUnscheduleMove(sched_, src_));

    if (!runPreChild(
            new BFMergeAndKeepUser(sched_, src_, dst_, loopBypass_))) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "merge and keep fail." << src_.toString()
                  << " and " << dst_.toString() << std::endl;
#endif
        undoAndRemovePreChildren();
        return false;
    }

    dred_ = runPostChild(new BFPostpassDRE(sched_, src_));

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\tTried ordinary dre: " << dred_ << std::endl;
#endif
    if (loopBypass_ && !dred_) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tsrccycle: " << srcCycle << std::endl;
#endif
        if (srcCycle >= (int)ii()) {
            dred_ = runPostChild(
                new BFPostpassLoopDRE(sched_, src_, srcCycle));
        } else {
            if (ddg().latestCycle(src_) >= (signed)ii()) {
                dred_ = runPostChild(
                    new BFPostpassLoopDRE(sched_, src_, ii()));
            }
        }
    }

    // TODO: try to DRE here

    if (!dred_) {
        // if returning the old fails, abort everything
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tundoing unscheduling of src" << std::endl;
#endif
        // if not dre, return the result
        if (!runPostChild(
                new BFScheduleExact(sched_, src_, srcCycle, false, ssfu))) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "Scheduling back src:" << src_.toString()
                      << " to cycle: " << srcCycle
                      << " after post-pass bypass move failed" << std::endl;
            ddg().writeToDotFile("postpass_fail1.dot");
#endif
            undo();
            return false;
        }

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tundid unscheduling of src" << std::endl;
#endif
    }

    BFOptimization* schedDst = NULL;
    if (dst_.move().isControlFlowMove() ||
        (ii() != 0 &&
         static_cast<DataDependenceGraph*>(ddg().rootGraph())->
         writesJumpGuard(dst_))) {
        if (!loopBypass_) {
            schedDst = new BFScheduleExact(
                sched_, dst_, originalCycle, false);
        } else {
            schedDst = new BFAssignToBody(
                sched_, dst_, originalCycle, ssfu, ddfu);
        }
    } else {
        if (!loopBypass_) {
            schedDst = new BFScheduleExact(
                sched_, dst_, originalCycle, false);
        } else {
            schedDst = new BFAssignToBody(
                sched_, dst_, originalCycle, ssfu, ddfu);
        }
    }

    if (!runPostChild(schedDst)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\tScheduling post-pass bypassed move failed:"
                  <<  dst_.toString() << "cycle: " << originalCycle
                  << std::endl;
        ddg().writeToDotFile("postpass_fail2.dot");
#endif
        undo();
        return false;
    }
    return true;
}

void BFPostpassBypass::undoOnlyMe() {

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "isloopbypass?" << loopBypass_ << std::endl;
    std::cerr << "\t\t\tPerforming unmerge user of: "
              << src_.toString() << " and: " << dst_.toString()
              << std::endl;
#endif
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\tPerformed unmerge user of: "
              << src_.toString() << " and: " << dst_.toString()
              << std::endl;
#endif
}
