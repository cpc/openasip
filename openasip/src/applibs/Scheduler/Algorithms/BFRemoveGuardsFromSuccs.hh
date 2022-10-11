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
 * @file BFRemoveGuardsFromSuccs.hh
 *
 * Declaration of BFRemoveGuardsFromSuccs class
 *
 * When scheduling a move which writes a guard, check for the uses of the
 * guard and try to remove those guad uses to allows scheduling this guard write
 * later. Calls BFRemoveGuardFromSucc.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef BF_REMOVE_GUARDS_FROM_SUCCS_HH
#define BF_REMOVE_GUARDS_FROM_SUCCS_HH

#include "BFOptimization.hh"

class MoveNode;

class BFRemoveGuardsFromSuccs : public BFOptimization {
public:
    BFRemoveGuardsFromSuccs(
        BF2Scheduler& sched, MoveNode &guardWrite, int oldLC, int maxLC) :
        BFOptimization(sched),
        guardWrite_(guardWrite), oldLC_(oldLC), maxLC_(maxLC) {}
    bool operator()() override;
private:
    MoveNode& guardWrite_;
    int oldLC_;
    int maxLC_;
};

#endif
