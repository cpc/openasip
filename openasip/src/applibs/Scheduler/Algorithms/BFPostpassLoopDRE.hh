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
 * @file BFPostpassLoopDRE.hh
 *
 * Declaration of BFPostpassLoopDRE class
 *
 * Performs a dead-result elimination of a loop-bypassed move.
 * The move which is eliminated from the loop body is still left
 * in the epilog, to have correct value for the register after
 * the loop.
 *
 * This class is called in the postpass-bypass phase.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef TTA_BF_POSTPASS_LOOP_DRE_HH
#define TTA_BF_POSTPASS_LOOP_DRE_HH

#include "BFOptimization.hh"

class BFPostpassLoopDRE : public BFOptimization {
public:
    BFPostpassLoopDRE(BF2Scheduler& sched, MoveNode& mn, int srcCycle) :
        BFOptimization(sched), mn_(mn), srcCycle_(srcCycle) {}
    bool operator()() override;
    void undoOnlyMe() override;
private:

    bool dreAllowed();
    MoveNode& mn_;
    MoveNode* prologMN_;
    int srcCycle_;
};

#endif
