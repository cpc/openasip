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
 * @file BFUnscheduleMove.hh
 *
 * Declaration of BFUnscheduleMove class
 *
 * Unschedules a move.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef BF_UNSCHEDULE_MOVE_HH
#define BF_UNSCHEDULE_MOVE_HH

#include "BFOptimization.hh"

namespace TTAMachine {
    class Bus;
    class FunctionUnit;
}

class MoveNode;

class BFUnscheduleMove: public BFOptimization {

public:
    BFUnscheduleMove(
        BF2Scheduler& sched, MoveNode& mn) :
        BFOptimization(sched), mn_(mn), prologBus_(NULL),
        oldImmWriteCycle_(-1), prologImmWriteCycle_(-1), dstFU_(NULL),
        srcFU_(NULL), immu_(NULL), immRegIndex_(-1) {}
    void unscheduleOriginal();
    void undoOnlyMe();
    bool operator()();
protected:
    void returnOriginal();
    MoveNode& mn_;
    int oldCycle_;
    const TTAMachine::Bus* oldBus_;
    const TTAMachine::Bus* prologBus_;
    int oldImmWriteCycle_;
    int prologImmWriteCycle_;
    const TTAMachine::FunctionUnit* dstFU_;
    const TTAMachine::FunctionUnit *srcFU_;
    const TTAMachine::ImmediateUnit* immu_;
    int immRegIndex_;
    static int recurseCounter_;
};

#endif
