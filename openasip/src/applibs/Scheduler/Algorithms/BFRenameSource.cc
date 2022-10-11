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
 * @file BFRenameSource.cc
 *
 * Definition of BFRenameSource class
 *
 * Renames a liverange, starting from a register read.
 * Class BFRenameLiverange to perform the actual work.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFRenameSource.hh"
#include "BFRenameLiveRange.hh"
#include "LiveRange.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "BF2Scheduler.hh"

BFRenameSource::BFRenameSource(BF2Scheduler& sched, MoveNode& mn,
                               int originalDDGLC, int targetCycle) :
    BFOptimization(sched), mn_(mn),
    originalDDGLC_(originalDDGLC),
    targetCycle_(targetCycle) {}


bool BFRenameSource::operator()() {

    if (sched_.renamer() == nullptr) {
        return false;
    }

    // not yet working with loop scheduling
    if (ii()) {
        return false;
    }

    TTAProgram::Move& m = mn_.move();
    TTAProgram::Terminal& t = m.source();
    if (!t.isGPR()) {
        return false;
    }

    std::shared_ptr<LiveRange> liveRange(ddg().findLiveRange(mn_, false, false));
    if (liveRange->writes.size() != 1 ||
        liveRange->reads.empty() ||
        !liveRange->guards.empty()) {
        return false;
    }

    for (auto i: liveRange->writes) {
        if (i->isScheduled()) {
            return false;
        }
    }

    // for reads.
    for (auto i: liveRange->reads) {
        if (i->isScheduled()) {
            return false;
        }
    }

    // for reads.
    for (auto i: liveRange->guards) {
        if (i->isScheduled()) {
            return false;
        }
    }

    return runPreChild(new BFRenameLiveRange(sched_, liveRange, targetCycle_));
}
