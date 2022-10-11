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
 * @file BFPushDepsUp.hh
 *
 * Declaration of BFPushDepsUp class
 *
 * Tries to reschedule the predecessors of a move into earlier cycle.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef BF_PUSH_LOOPDEPSUP_HH
#define BF_PUSH_LOOPDEPSUP_HH

#include "BFOptimization.hh"

class MoveNode;

class BFPushDepsUp : public BFOptimization {
public:
    static int recurseCounter_;
    BFPushDepsUp(
	BF2Scheduler& sched, MoveNode &mn, int prefCycle) :
	BFOptimization(sched),
    mn_(mn), prefCycle_(prefCycle) {}
    bool operator()();
private:
    MoveNode& mn_;
    int prefCycle_;
};

#endif
