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
 * @file BFRescheduleMove.cc
 *
 * Definition of BFReschduleMove class
 *
 * Base class for rescheduling moves.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFRescheduleMove.hh"
#include "Move.hh"
#include "DataDependenceGraph.hh"
#include "Bus.hh"
#include "UniversalMachine.hh"

void BFRescheduleMove::undoOnlyMe() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    for (int i = 0; i < recurseCounter_*2; i++)
        std::cerr << "\t";

    std::cerr << "\t\tUndoing rechedule: " << mn_.toString()
              << " setting bus to: " << oldBus_->name()
	      << std::endl;
#endif
    if (!mn_.isScheduled()) {
        for (int i = 0; i < recurseCounter_*2; i++)
            std::cerr << "\t";

        std::cerr << "\t\tCannot unschedule on undo of reschedule,"
                  << " mn not sched: " << mn_.toString() << std::endl;
        for (int i = 0; i < recurseCounter_*2; i++)
            std::cerr << "\t";

        std::cerr << "\t\tShould return it to cycle" << oldCycle_ << std::endl;
        assert(false);
    }
    unassign(mn_, false);

    undoAndRemoveChildren(midChildren_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    if (&mn_.move().bus() != &UniversalMachine::instance().universalBus()) {
        std::cerr << "\t\t\tWARNING: bus not universalbus: "
                  << mn_.move().bus().name() << std::endl;
        std::cerr << "\tOld bus: " << oldBus_->name() << std::endl;
    }
#endif
    returnOriginal();
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tundid reschedule, move now: " << mn_.move().toString()
              << std::endl;
#endif
}
