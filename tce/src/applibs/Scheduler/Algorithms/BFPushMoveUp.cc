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
 * @file BFPushMoveUp.cc
 *
 * Definition of BFPushMoveUp class
 *
 * Tries to (recursively) reschedule move (and it's predecessors)
 * into earlier cycle.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFPushMoveUp.hh"
#include "DataDependenceGraph.hh"
#include "BFPushDepsUp.hh"
#include "Move.hh"
#include "BF2Scheduler.hh"
#include "Terminal.hh"

//#define DEBUG_BUBBLEFISH_SCHEDULER

bool
BFPushMoveUp::operator()() {

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    static int dotCount = 0;
#endif
    bool isTrigger = mn_.isDestinationOperation() &&
        mn_.move().destination().isTriggering();

    if (lc_ == -1) {
        std::cerr << "incoming lc -1 on pushmoveup!"  << std::endl;
        assert(false);
    }
    if (mn_.move().isControlFlowMove()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t|TCannot reschedule control flow move!" << std::endl;
#endif
        return false;
    }

    if (&mn_ == sched_.guardWriteNode()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t|TCannot reschedule guard write move!" << std::endl;
#endif
        return false;
    }

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    recurseCounter_++;

    for (int i = 0; i < recurseCounter_*2; i++)
        std::cerr << "\t";

    std::cerr << "\t\tPushing move up: " << mn_.toString() << " to cycle: "
              << lc_ << std::endl;
#endif

    // first push operands if I'm trigger?
    if (isTrigger && mn_.destinationOperation().inputMoveCount() > 1) {
        ProgramOperation& dstPO = mn_.destinationOperation();
        for (int i = 0; i < dstPO.inputMoveCount(); i++) {
            MoveNode& input = dstPO.inputMove(i);
            if (input.isScheduled() &&
                !input.move().destination().isTriggering()) {
                if (input.cycle() > lc_) {
                    // TODO: what if this fails??
                    runPreChild(new BFPushMoveUp(sched_, input, lc_));
                }
            }
        }
    }
    unscheduleOriginal();

    int ddgLC = ddg().latestCycle(mn_,ii());
    int ddgEC = ddg().earliestCycle(mn_,ii());

    if (ddgLC == -1) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "ddg latest cycle -1 in pushmoveup of:"
                  << mn_.toString() << std::endl;
#endif
        returnOriginal();
        return false;
    }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    for (int i = 0; i < recurseCounter_*2; i++)
        std::cerr << "\t";

    std::cerr << "\t\tPushing move: " << mn_.toString()
              << " up, ddgLC now: " << ddgLC << std::endl;
#endif
    int lc = std::min(lc_, ddgLC);
    int rmlc = rmLC(lc, mn_);

    if (ddgEC > lc) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        for (int i = 0; i < recurseCounter_*2; i++)
            std::cerr << "\t";

        std::cerr << "\t\tMay need to push predecessors.." << std::endl;
#endif
        BFPushDepsUp* pusher = new BFPushDepsUp(sched_, mn_, lc);
        if ((*pusher)()) {
            midChildren_.push(pusher);
            ddgEC = ddg().earliestCycle(mn_, ii());
            if (ddgEC > lc) {
                std::cerr << "\t\tDDG earliest cycle still too early: "
                          << ddgEC << std::endl;
                ddg().writeToDotFile("ec_still_early.dot");
                assert(false);
            }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        for (int i = 0; i < recurseCounter_*2; i++)
            std::cerr << "\t";

            std::cerr << "\t\tPredecessors pushed ok!" << std::endl;
#endif
//            assert(ddg().earliestCycle(mn_) <= lc);
        } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER

            for (int i = 0; i < recurseCounter_*2; i++)
                std::cerr << "\t";

            std::cerr << "\t\tDDG ec too late, Need to recursively push more,"
                      << " which failed" << std::endl;
#endif
            delete pusher;
            returnOriginal();
            undoAndRemoveChildren(preChildren_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            recurseCounter_--;
#endif
            return false;
        }
        // TODO: this is buggy. should have back edge
        ddgLC = ddg().latestCycle(mn_,ii());
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        for (int i = 0; i < recurseCounter_*2; i++)
            std::cerr << "\t";

        std::cerr << "\t\tddgLC of " << mn_.toString()
                  << " after pushing preds: " << ddgLC << std::endl;
#endif
        rmlc = rmLC(std::min(lc_, ddgLC), mn_);
    }
    if (rmlc == -1 || rmlc < ddgEC) {
        if (lc < 1) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\tlc too small, cannot push again" << std::endl;
#endif
            undoAndRemoveChildren(midChildren_);
            returnOriginal();
            undoAndRemoveChildren(preChildren_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            recurseCounter_--;
#endif
            return false;
        }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        for (int i = 0; i < recurseCounter_*2; i++)
            std::cerr << "\t";

        std::cerr << "\t\trmlc problem: " <<rmlc
                  << "ddgec: " << ddgEC << std::endl;
        TCEString dotName("rm_-1_pushup");
        dotName << mn_.nodeID() << "_" << ii() << "_" << dotCount++ << ".dot";
        std::cerr << "Writing dot: " << dotName << std::endl;
        ddg().writeToDotFile(dotName);

        for (int i = 0; i < recurseCounter_*2; i++)
            std::cerr << "\t";

        std::cerr << "\t\tRM lc -1 when pushing move up, failing:"
                  << mn_.toString() << " target cycle: " << lc_ << std::endl;

        for (int i = 0; i < recurseCounter_*2; i++)
            std::cerr << "\t";

        std::cerr << "\t\tddgLC is: " << ddgLC << std::endl;
//        assert(false);

        std::cerr << "Trying to push preds one more up." << std::endl;

#endif
        BFPushDepsUp* pusher2 = new BFPushDepsUp(sched_, mn_, lc-1);
        bool fail = false;

        if ((*pusher2)()) {
            midChildren_.push(pusher2);
            ddgEC = ddg().earliestCycle(mn_, ii());
            if (ddgEC > lc) {
                std::cerr << "\t\tDDG earliest cycle STILL too early: "
                          << ddgEC << std::endl;
                ddg().writeToDotFile("ec_still_early.dot");
                assert(false);
            }
            ddgLC = ddg().latestCycle(mn_,ii());
            rmlc = rmLC(std::min(lc_, ddgLC), mn_);
            if (rmlc == -1 || rmlc < ddgEC) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "still failing, now giving up" << std::endl;
#endif
                fail = true;
            }
        } else {
            delete pusher2;
            fail = true;
        }

        if (fail) {
            for (int i = 0; i < recurseCounter_*2; i++)
                std::cerr << "\t";
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\tUndoing recursive pushs of predecessors"
                      << std::endl;
#endif
            undoAndRemoveChildren(midChildren_);

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            for (int i = 0; i < recurseCounter_*2; i++)
                std::cerr << "\t";

            std::cerr << "\t\tReturning the original" << std::endl;
#endif
            BFRescheduleMove::returnOriginal();
            undoAndRemoveChildren(preChildren_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER

            for (int i = 0; i < recurseCounter_*2; i++)
                std::cerr << "\t";

            std::cerr << "\t\treturned original: "<<mn_.toString()
                      << std::endl;
            recurseCounter_--;
#endif
            return false;
        }
    }

    assign(rmlc, mn_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    for (int i = 0; i < recurseCounter_*2; i++)
        std::cerr << "\t";

    std::cerr << "\tPushed move successfully: " << mn_.toString()
              << std::endl;
    recurseCounter_--;
#endif
    return true;
}
