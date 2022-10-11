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
 * @file BFPushAntidepDown.hh
 *
 * Declaration of BFPushAntidepDown class.
 *
 * Tries to reschedule a move into later cycle to allow move which has an
 * antidep to it to be scheduler later.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef BF_PUSH_ANTIDEP_DOWN_HH
#define BF_PUSH_ANTIDEP_DOWN_HH

#include "BFRescheduleMove.hh"

namespace TTAMachine {
    class Bus;
}

class MoveNode;

class BFPushAntidepDown : public BFRescheduleMove {
public:
    BFPushAntidepDown(
	BF2Scheduler& sched, MoveNode& adepDest, int latency, int newLC) :
        BFRescheduleMove(sched, adepDest),
        lat_(latency), newLC_(newLC) {}
    bool operator()();
private:
    int lat_;
    int newLC_;
};

#endif
