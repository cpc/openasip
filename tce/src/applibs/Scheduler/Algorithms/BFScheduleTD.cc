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
 * @file BFScheduleTD.cc
 *
 * Definition of BFScheduleTD class
 *
 * Tries to schedule a move in top-down order.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFScheduleTD.hh"
#include "BFRegCopyAfter.hh"
#include "BFRegCopyBefore.hh"
#include "SimpleResourceManager.hh"
#include "MoveNode.hh"
#include "DataDependenceGraph.hh"

#include "BFLateBypasses.hh"
#include "BF2Scheduler.hh"

#include "UniversalMachine.hh"

#include "Move.hh"
#include "Bus.hh"

#include <climits>

//#define DEBUG_BUBBLEFISH_SCHEDULER

bool
BFScheduleTD::operator()() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\tPerforming BFSchduleTD for: " << mn_.toString() <<
      " ec limit: " << ec_ << std::endl;
#endif
    assert(!mn_.isScheduled());
    if (&mn_.move().bus() != &UniversalMachine::instance().universalBus()) {
        mn_.move().setBus(UniversalMachine::instance().universalBus());
    }

    if (mn_.isDestinationVariable() && allowLateBypass_) {
        BFLateBypasses* lbp = new BFLateBypasses(sched_, mn_, ec_);
        if (runPreChild(lbp)) {
            if (lbp->removedSource()) {
                return true;
            }
        }
    }


    BFRegCopy* regCopyBefore = NULL;
    BFRegCopy* regCopyAfter = NULL;
    if (!canBeScheduled(mn_)) {
        if (/*isRegCopyAfter_ || */ mn_.isSourceOperation()) {
            regCopyAfter = new BFRegCopyAfter(sched_,mn_, INT_MAX);
        } else {
            regCopyBefore = new BFRegCopyBefore(sched_,mn_, INT_MAX);
            if (!runPreChild(regCopyBefore)) {
                return false;
            }
        }
    }

    int ddglc = std::min(sched_.maximumAllowedCycle(),
                         ddg().latestCycle(mn_, ii(), false, false));
    int ddgec = std::max(ec_,
			 ddg().earliestCycle(mn_, ii(), false, false));
    int rmec = rmEC(ddgec, mn_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tDDG lc: " << ddglc << std::endl;
    std::cerr << "\t\tDDG ec: " << ddgec << std::endl;
    std::cerr << "\t\tRM ec: " << rmec << std::endl;
#endif
    if (rmec != -1 && ddglc >= rmec) {
        createdCopy_ = assign(rmec, mn_);
        if (regCopyAfter != NULL) {
            if (!runPostChild(regCopyAfter)) {
                undoAndRemovePreChildren();
                return false;
            }
        }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\tTD Assigned ok:" << mn_.toString()
                  << " bus: " << mn_.move().bus().name() << std::endl;
#endif
        return true;
    } else {
        undoAndRemovePreChildren();
        return false;
    }
}

void
BFScheduleTD::undoOnlyMe() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "unassign td: " << mn_.toString() << std::endl;
#endif
    if (mn_.isScheduled()) {
        unassign(mn_, createdCopy_);
    }
    if (&mn_.move().bus() != &UniversalMachine::instance().universalBus()) {
        mn_.move().setBus(UniversalMachine::instance().universalBus());
    }
}
