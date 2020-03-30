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
 * @file BFPushAntidepDown.cc
 *
 * Definition of BFPushAntidepDown class.
 *
 * Tries to reschedule a move into later cycle to allow move which has an
 * antidep to it to be scheduler later.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFPushAntidepDown.hh"

#include "DataDependenceGraph.hh"
#include "SimpleResourceManager.hh"
#include "MoveNode.hh"
#include "Move.hh"
#include "Bus.hh"

#include "BF2Scheduler.hh"
#include "MoveNodeDuplicator.hh"
#include "UniversalMachine.hh"

//#define DEBUG_BUBBLEFISH_SCHEDULER

bool
BFPushAntidepDown::operator()() {
    if (mn_.move().isControlFlowMove()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tCannot reschedule control flow move!" << std::endl;
#endif
        return false;
    }

    if (&mn_ == sched_.guardWriteNode()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t|TCannot reschedule guard write move!" << std::endl;
#endif
        return false;
    }

    unscheduleOriginal();

    int ddgLC = ddg().latestCycle(mn_);
    int ddgEC = ddg().earliestCycle(mn_);

    // first try to schedule top-down close to the optimal as possible
    int prefEc = newLC_ - lat_ +1;
    if (ddgEC > prefEc) {
        prefEc = ddgEC;
    }

    int ec = rmEC(prefEc, mn_);
    if (ec <= std::min(ddgLC,sched_.maximumAllowedCycle()) && ec != -1) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	std::cerr << "\t\t\t\t\ttopdown resched: " << ec << std::endl;
#endif
	assign(ec, mn_);
	return true;
    } else {
	// topdown failed, bottom-up then be it
	int lc = rmLC(
        (std::min(ddgLC, sched_.maximumAllowedCycle())),mn_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	std::cerr << "\t\t\t\tbottomup resched ddglc: " << ddgLC << " latest_: "
              << sched_.maximumAllowedCycle() << " lc: " << lc << std::endl;
#endif
	if (lc < ddgEC || lc == oldCycle_) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	    std::cerr << "\t\tPushing down antidep dest: " << mn_.toString()
                  << " failed lc: "<< lc << " ddgEC: " << ddgEC << std::endl;
        std::cerr << "\t\tPushAntidepsDown seting to old bus: "
                  << mn_.toString() << " to bus "
                  << oldBus_->name() << std::endl;
#endif
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	    assert(canAssign(oldCycle_, mn_, oldBus_, NULL, NULL, prologBus_));
#endif
	    assign(oldCycle_, mn_, oldBus_, NULL, NULL, prologBus_,
               oldImmWriteCycle_, prologImmWriteCycle_);
	    return false;
	}
	assert(lc != -1);
	assign(lc, mn_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	std::cerr << "\t\tPushed antidep down: " << mn_.toString() << " bus: "
              << mn_.move().bus().name() << std::endl;
#endif
	return true;
    }
    return false;
}
