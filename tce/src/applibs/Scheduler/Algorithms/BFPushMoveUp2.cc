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
 * @file BFPushMoveUp2.cc
 *
 * Definition of BFPushMoveUp2 class
 *
 * Tries to (recursively) reschedule move (and it's predecessors)
 * into earlier cycle.
 *
 * This version may unschedule whole operations and put them above some other
 * operations int he same FU.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFPushMoveUp2.hh"
#include "DataDependenceGraph.hh"
#include "BFPushDepsUp.hh"
#include "Move.hh"
#include "BF2Scheduler.hh"
#include "BFScheduleBU.hh"

bool
BFPushMoveUp2::operator()() {

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

    if (!mn_.isSourceOperation()) {
        return false;
    }

    // unschedule this (result) move
    ProgramOperation& po = mn_.sourceOperation();
    MoveNode* trigger = po.triggeringMove();
    unscheduleOriginal();

    int ddgLC = ddg().latestCycle(mn_,ii());
    int ddgEC = ddg().earliestCycle(mn_,ii());

    if (ddgLC == -1) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "ddg latest cycle -1 in pushmoveup2 of:"
                  << mn_.toString() << std::endl;
#endif
        returnOriginal();
        return false;
    }

    int lc = std::min(lc_, ddgLC);

    if (ddgEC > lc) {
        DataDependenceGraph::NodeSet unschedOperands;
        DataDependenceGraph::NodeSet unschedResults;
        // undo the inputs
        for (int i = 0; i < po.inputMoveCount(); i++) {
            MoveNode& operand = po.inputMove(i);
            if (operand.isScheduled()) {
                if (!runPostChild(new BFUnscheduleMove(sched_, operand))) {
                    undo();
                    return false;
                } else {
                    unschedOperands.insert(&operand);
                }
            }
        }

        for (int i = 0; i < po.outputMoveCount(); i++) {
            MoveNode& res = po.outputMove(i);
            if (res.isScheduled() &&
                (res.cycle() > lc ||
                 (isLoopBypass(res) && res.cycle()+((signed)ii()) > lc))) {
                if (!runPostChild(new BFUnscheduleMove(sched_, res))) {
                    undo();
                    return false;
                } else {
                    unschedResults.insert(&res);
                }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            } else {
                std::cerr << "res which is not scheduled:" << res.toString()
                          << std::endl;
#endif
            }
        }

        // now whole op is not scheduled?

        // reschedule other results
        for (int i = po.outputMoveCount()-1; i >= 0; i--) {
            MoveNode& res = po.outputMove(i);
            if (AssocTools::containsKey(unschedResults, &res)) {
                int myLC = lc - isLoopBypass(res) * ii();
                if (!runPostChild(
                        new BFScheduleBU(
                            sched_, res, myLC, false, false, false))) {
                    undo();
                    return false;
                }
            }
        }

        // reschedule me.
        if (!runPostChild(
                new BFScheduleBU(sched_, mn_, lc, false, false, false))) {
            undo();
            return false;
        }

        // reschdule trigger
        if (AssocTools::containsKey(unschedOperands, trigger)) {
            if (!runPostChild(
                    new BFScheduleBU(
                        sched_, *trigger, lc, false, false, false))) {
                undo();
                return false;
            }
        }

        // reschedule other operands
        for (int i = 0; i < po.inputMoveCount(); i++) {
            MoveNode& operand = po.inputMove(i);
            if (&operand != trigger) {
                if (AssocTools::containsKey(unschedOperands, &operand)) {
                    if (!runPostChild(
                            new BFScheduleBU(
                                sched_, operand, lc, false, false, false))) {
                        undo();
                        return false;
                    }
                }
            }
        }
    } else {
        undo();
        return false;
    }
    assert(mn_.isScheduled());
    return true;
}

bool BFPushMoveUp2::isLoopBypass(MoveNode&mn) {
    auto inEdges = ddg().operationInEdges(mn);
    for (auto e: inEdges) {
        if (e->isBackEdge()) {
            return true;
        }
    }

    return false;
}
