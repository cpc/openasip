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
 * @file BFEarlyBypass.cc
 *
 * Definition of BFEarlyBypass class
 *
 * Performs a bypass before scheduling an (operand) move
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFEarlyBypass.hh"
#include "BFDREEarly.hh"
#include "MachineConnectivityCheck.hh"
#include "DataDependenceGraph.hh"
#include "BF2Scheduler.hh"
#include "BF2ScheduleFront.hh"
#include "Machine.hh"
#include "BFMergeAndKeepUser.hh"

//#define DEBUG_BUBBLEFISH_SCHEDULER
//#define DEBUG_LOOP_SCHEDULER
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
#define DEBUG_LOOP_SCHEDULER
#endif

bool
BFEarlyBypass::operator()() {

#ifdef DEBUG_LOOP_SCHEDULER
    if (ii() != 0) {
        std::cerr << "\t\t\tEarly bypass for: " << src_.toString()
                  << " to " << dst_.toString() << std::endl;
    }
#endif
    // Take the predecssors so that they can be notified because their
    // antideps may change
    DataDependenceGraph::NodeSet pred = ddg().predecessors(dst_);

    assert(!dst_.isScheduled());
    if (!runPreChild(new BFMergeAndKeepUser(sched_, src_, dst_))) {
        return false;
    }

    // if alwayswriteresults, then must have exactly one result write.
    if (targetMachine().alwaysWriteResults()) {
        // if destination is variable, we have two variable writes. MUST DRE.
        if (dst_.isDestinationVariable()) {
            BFDREEarly* dre = new BFDREEarly(sched_,src_);
            if (!runPostChild(dre)) {
                undo();
                return false;
            }
        }
        // if destination is not variable, must NOT DRE.
    } else {
        BFDREEarly* dre = new BFDREEarly(sched_,src_);
        runPostChild(dre);
    }
    sched_.currentFront()->bypassed(src_, dst_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tEarly bypass ok for: " << dst_.toString() << std::endl;
#endif

    // now notify antideps sources
    for (auto i : pred) {
        mightBeReady(*i);
    }

    return true;
}

void
BFEarlyBypass::undoOnlyMe() {
    sched_.currentFront()->undidBypass(src_, dst_);
}
