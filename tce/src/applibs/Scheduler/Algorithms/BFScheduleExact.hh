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
 * @file BFScheduleExact.hh
 *
 * Declaration of BFScheduleExact class
 *
 * Tries to schedule moev to an exact cycle (no order).
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef BF_SCHEDULE_EXACT_HH
#define BF_SCHEDULE_EXACT_HH

#include <cstddef>
#include "BFScheduleMove.hh"

class MoveNode;

namespace TTAProgram {
    class Move;
}

class BFScheduleExact : public BFScheduleMove {
public:

    BFScheduleExact(BF2Scheduler& sched, MoveNode& mn, int c,
                    bool allowRegCopy = true,
                    const TTAMachine::FunctionUnit* srcFU = nullptr) :
        BFScheduleMove(sched, mn),
	allowRegCopy_(allowRegCopy),
	c_(c), jumpLimitMove_(0),
        createdCopy_(false), srcFU_(srcFU) {}

    virtual bool operator()();
    virtual void undoOnlyMe();
private:
    bool allowRegCopy_;
    int c_;
    TTAProgram::Move* jumpLimitMove_;
    int oldJumpLimit_;
    int oldJumpLimitWidth_;
    bool createdCopy_;
    const TTAMachine::FunctionUnit* srcFU_;
};

#endif
