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
 * @file BFRemoveGuardsFromSuccs.cc
 *
 * Definition of BFRemoveGuardsFromSuccs class
 *
 * When scheduling a move wihch writes a guard, check for the uses of the
 * guard and try to remove those guad uses to allows scheduling this guard write
 * later. Calls BFRemoveGuardFromSucc.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFRemoveGuardsFromSuccs.hh"
#include "BFRemoveGuardFromSucc.hh"
#include "DataDependenceGraph.hh"
#include "Move.hh"
#include <list>
#include "SimpleResourceManager.hh"

bool BFRemoveGuardsFromSuccs::operator()() {

    DataDependenceGraph::EdgeSet outEdges = ddg().outEdges(guardWrite_);

//    std::list<BFRemoveGuardFromSucc*> guardRemoves;
    std::list<MoveNode*> guardRemoveNodes;

    for (auto e: outEdges) {
        if (e->guardUse() &&
            e->dependenceType() == DataDependenceEdge::DEP_RAW) {
            MoveNode& head = ddg().headNode(*e);
            if (head.move().isUnconditional()) {
                std::cerr << "guard edge to uncond move?" << head.toString()
                          << std::endl;
                continue;
            }
            if (head.isScheduled()) {
                // this is not in critical path.
                int cycle = head.cycle();
                if (cycle > maxLC_ + head.guardLatency()) {
                    continue;
                } else {
                    if (canBeSpeculated(head)) {
                        guardRemoveNodes.push_back(&head);
                    } else {
                        maxLC_ = std::min(
                            maxLC_, head.cycle() - head.guardLatency());
                        continue;
                    }
                }
            }
        }
    }

    for (auto i: guardRemoveNodes) {
        MoveNode& mn = *i;
        if (mn.cycle() - mn.guardLatency() <  maxLC_) {
            runPostChild(new BFRemoveGuardFromSucc(sched_, mn));
        }
    }
    if (postChildren_.empty()) {
        return false;
    }

    // if could not improve schedule, return everything back the way it was
    int ddglc = ddg().latestCycle(guardWrite_, ii(), false, false);
    int rmlc = rm().latestCycle(ddglc, guardWrite_);
    if (rmlc <= oldLC_) {
        undo();
        return false;
    }

    return true;
}
