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
 * @file BFScheduleResultClose.cc
 *
 * Definition of BFScheduleResultClose class.
 *
 * Tries to reschedule a result move closer to operands into more
 * optimal position.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFRescheduleResultClose.hh"
#include "BFUnscheduleMove.hh"
#include "BFScheduleTD.hh"
#include "BFRescheduleSuccessorsClose.hh"
#include "Move.hh"
bool BFRescheduleResultClose::operator()() {
    if (result_.move().isControlFlowMove()) {
        return false;
    }
    int originalCycle = result_.cycle();
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\tPushing result close: " <<  result_.toString() << std::endl;
#endif
    BFUnscheduleMove* unsched = new BFUnscheduleMove(sched_, result_);
    if (!(runPreChild(unsched))) {
        return false;
    }

    BFScheduleTD* td = new BFScheduleTD(sched_, result_, ec_, false);
    if ((!runPreChild(td)) || result_.cycle() == originalCycle) {
        undo();
        return false;
    }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\tPushed result close: " <<  result_.toString() << std::endl;
#endif
    auto succs = new BFRescheduleSuccessorsClose(sched_, result_);
    runPostChild(succs);
    return true;
}
