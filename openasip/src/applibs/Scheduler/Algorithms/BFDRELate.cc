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
 * @file BFDRELate.cc
 *
 * Definition of BFDRELate class
 *
 * Performs a dead-result elimination after a late bypass.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFDRELate.hh"
#include "DataDependenceGraph.hh"
#include "BF2Scheduler.hh"
#include "Machine.hh"

bool
BFDRELate::operator()() {
    if (!sched_.killDeadResults() && !targetMachine().alwaysWriteResults()) {
        return false;
    }

    if (!ddg().resultUsed(mn_)) {

        // Take the predecssors of the dre'd src because their
        // antideps may change
         DataDependenceGraph::NodeSet pred = ddg().predecessors(mn_);

         createdEdges_ =  ddg().copyDepsOver(mn_, true, false);
        assert(!mn_.isScheduled());
        ddg().dropNode(mn_);
        sched_.nodeAndCopyKilled(mn_);

        // now mark antideps sources to be notified
        for (auto n: pred) {
            mightBeReady(*n);
        }

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	std::cerr << "\t\t\tLate DRE ok: " << mn_.toString() << std::endl;
#endif
        return true;
    } else {
        return false;
    }
}
