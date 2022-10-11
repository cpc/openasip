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
 * @file BFPushDepsUp.cc
 *
 * Definition of BFPushDepsUp class
 *
 * Tries to reschedule the predecessors of a move into earlier cycle.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFPushDepsUp.hh"
#include "DataDependenceGraph.hh"
#include "BFPushMoveUp.hh"
#include "BFPushMoveUp2.hh"

bool BFPushDepsUp::operator()() {

    if (prefCycle_ < 0) {
        std::cerr << "push deps up prefcycle negative! "
                  << prefCycle_ << " move: " << mn_.toString() << std::endl;
        assert(false);
    }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    recurseCounter_++;
    for (int i = 0; i < recurseCounter_*2; i++)
        std::cerr << "\t";
    std::cerr << "\tPushing loop deps up: " << mn_.toString() << std::endl;
#endif

    bool pushed = false;
    DataDependenceGraph::EdgeSet inEdges = ddg().inEdges(mn_);
    for (auto e : inEdges) {
        MoveNode& loopDepTail = ddg().tailNode(*e);
        if (loopDepTail.isScheduled()) {
            int eLat = ddg().edgeLatency(*e, ii(), &loopDepTail, &mn_);
            int prefTailCycle = prefCycle_ - eLat;
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\t\tmy prefcycle is: " << prefCycle_
                      << " latency is: " << eLat
                      << " so preftailcycle is: " << prefTailCycle
                      << std::endl;
            std::cerr << "\t\t\t\tedge: " <<e->toString() << std::endl;
#endif
            if (loopDepTail.cycle() > prefTailCycle) {
                if (prefTailCycle < 0) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                    recurseCounter_--;
#endif
                    undoAndRemovePreChildren();
                    return false;
                }
                BFOptimization* pushMoveUp;
                if (loopDepTail.isSourceOperation()) {
                    pushMoveUp =
                        new BFPushMoveUp2(sched_, loopDepTail, prefTailCycle);
                } else {
                    pushMoveUp =
                        new BFPushMoveUp(sched_, loopDepTail, prefTailCycle);
                }
                if (runPreChild(pushMoveUp)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                    for (int i = 0; i < recurseCounter_*2; i++)
                        std::cerr << "\t";
                    std::cerr << "\tpushed up succesfully: "
                              << loopDepTail.toString()
                              << " preChildren size: "
                              << preChildren_.size() << std::endl;
#endif
                    pushed = true;
                } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                    for (int i = 0; i < recurseCounter_*2; i++)
                        std::cerr << "\t";
                    std::cerr << "\t\tpushing up fail: "
                              << loopDepTail.toString()
                              << " preChildren size: "
                              << preChildren_.size() << std::endl;
#endif
                    undoAndRemovePreChildren();
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                    recurseCounter_--;
#endif
                    return false;
                }
            }
        }
    }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    recurseCounter_--;
#endif

    return pushed;
}

int BFPushDepsUp::recurseCounter_ = 0;
