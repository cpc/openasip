/*
    Copyright (c) 2002-2020 Tampere University.

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
 * @file BFClearLiveRangeUse.hh
 *
 * Declaration of scheduler operation which clears live range use.
 * bookkeeping for a reg.
 *
 * @author Heikki Kultala 2020 (hkultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFOptimization.hh"
#include "DataDependenceGraph.hh"
#include "LiveRangeData.hh"


class BFClearLiveRangeUse: public BFOptimization {

public:
    BFClearLiveRangeUse(BF2Scheduler& sched,
                        LiveRangeData::MoveNodeUseSet& set) :
        BFOptimization(sched), set_(set) {}

    virtual bool operator()();
    void undoOnlyMe();
    virtual ~BFClearLiveRangeUse() {}
private:
    LiveRangeData::MoveNodeUseSet& set_;
    LiveRangeData::MoveNodeUseSet backup;
};
