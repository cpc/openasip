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
 * @file BFPushAntidepsDown.cc
 *
 * Definition of BFPushAntidepsDown class.
 *
 * Checks antidep successors of a move and tries to reschedule the limiting ones
 * to later cycle b calling BFPushAntidepDown for it.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFPushAntidepsDown.hh"
#include "BFPushAntidepDown.hh"

#include <list>

#include "DataDependenceGraph.hh"

//#include "DataDependenceGraph.hh"

bool BFPushAntidepsDown::operator()() {

    DataDependenceGraph::EdgeSet outEdges = ddg().outEdges(adepSource_);
    int newLC = maxLC_;

    std::list<BFPushAntidepDown*> rescheds;

    for (auto e: outEdges) {
        if ((e->edgeReason() == DataDependenceEdge::EDGE_REGISTER ||
             e->edgeReason() == DataDependenceEdge::EDGE_RA) &&
            e->isFalseDep()) {
            MoveNode& aDepHead = ddg().headNode(*e);
            if (!aDepHead.isScheduled()) {
                continue;
            }
            int cycle = aDepHead.cycle();
            int lat = 1;
            if (e->guardUse()) {
                lat = adepSource_.guardLatency();
            }
            if (cycle + lat > maxLC_) {
                continue;
            }
            int dstLC = ddg().latestCycle(aDepHead);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\tShould rescheduling move at: "
                      << aDepHead.toString() << std::endl;
            std::cerr << "\t\tdstLC: " << dstLC << std::endl;
#endif
            if (dstLC > aDepHead.cycle()) {
                rescheds.push_back(
                    new BFPushAntidepDown(sched_, aDepHead, lat, newLC));
            }


            int curLC = dstLC - lat + 1;
            if (curLC < newLC) {
                newLC = curLC;
            }
        }
    }
    // can we improve the situation at all?
    if (newLC <= oldLC_) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "newLC: " << newLC << " old LC: " << oldLC_ << std::endl;
#endif
        for (std::list<BFPushAntidepDown*>::iterator i = rescheds.begin();
             i != rescheds.end();) {
            BFPushAntidepDown* bfo = *i;
            delete bfo;
            rescheds.erase(i++);
        }
//	assert(rescheds.empty());
        return false;
    } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tCould improve? newLC: " << newLC << std::endl;
#endif
    }

    for (auto i = rescheds.begin(); i != rescheds.end();) {
        BFPushAntidepDown* bfo = *i;
        rescheds.erase(i++);
        runPostChild(bfo);
    }
    return !postChildren_.empty();
}
