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
 * @file BFPostpassBypasser.cc
 *
 * Definition of BFPostpassBypasser class
 *
 * After scheduling all moves, searches for a bypasses which can be bypassed
 * after scheduling everything. Calls BFPostpassBypass to bypass these.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFPostpassBypasser.hh"
#include "DataDependenceGraph.hh"
#include "BF2Scheduler.hh"
#include "BFPostpassBypass.hh"

bool BFPostpassBypasser::operator()() {

    for (int i = 0; i < ddg().nodeCount(); i++) {
        MoveNode& n = ddg().node(i);
        bool dred = tryBypassNode(n);
        // make sure do not skip over one if dre removes something
        if (dred) {
            i--;
        }
    }
    return !postChildren_.empty();
}

bool BFPostpassBypasser::tryBypassNode(MoveNode& n) {

    DataDependenceEdge* bypassEdge = sched_.findBypassEdge(n);
    if (bypassEdge == NULL) {
        return false;
    }
    MoveNode& src = ddg().tailNode(*bypassEdge);

    if (!ddg().guardsAllowBypass(src, n)) {
        return false;
    }

    auto bypass = new BFPostpassBypass(sched_, src, n, bypassEdge->isBackEdge());
    if (!(runPostChild(bypass))) {
        return false;
    }
    return bypass->dred();
}
