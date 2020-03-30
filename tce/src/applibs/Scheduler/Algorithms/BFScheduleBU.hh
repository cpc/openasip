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
 * @file BFScheduleBU.hh
 *
 * Declaration of BFScheduleBU class
 *
 * Tries to schedule a move in bottom-up order. Before the actual scheduling,
 * Tries to call many other optimization classes to do things like
 * bypassing, operand sharing etc.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef BF_SCHEDULE_BU_HH
#define BF_SCHEDULE_BU_HH

#include "BFScheduleMove.hh"

class MoveNode;

class BFScheduleBU : public BFScheduleMove {
public:

    BFScheduleBU(
	BF2Scheduler& sched, MoveNode& mn, int lc,
	bool allowEarlyBypass, bool allowLateBypass, bool allowEarlySharing) :
        BFScheduleMove(sched, mn), lc_(lc),
        allowEarlyBypass_(allowEarlyBypass),
        allowLateBypass_(allowLateBypass),
        allowEarlySharing_(allowEarlySharing),
        bypasserScheduled_(false),
        createdCopy_(false) {}

    virtual bool operator()();
    void undoOnlyMe();
private:
    int lc_;
    bool allowEarlyBypass_;
    bool allowLateBypass_;
    bool allowEarlySharing_;
    bool bypasserScheduled_;
    bool createdCopy_;
};

#endif
