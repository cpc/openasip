/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file CallPathTracker.cc
 *
 * @author Pekka Jääskeläinen 2010
 */

#include "CallPathTracker.hh"

void
CallPathTracker::addProcedureTransfer(
    ClockCycleCount cycle,
    InstructionAddress address,
    InstructionAddress sourceAddress,
    bool isEntry) {

    if (transfers_.size() > historyLength_)
        transfers_.pop_front();
    ProcedureTransfer transfer;
    transfer.cycle = cycle;
    transfer.address = address;
    transfer.sourceAddress = sourceAddress;
    transfer.isEntry = isEntry;
    transfers_.push_back(transfer);
}

/**
 * Returns the history in a "back trace form" with calls and returns not
 * contributing to the current call path stripped out.
 */
const CallPathTracker::ProcedureTransferQueue
CallPathTracker::backTrace() const {
    ProcedureTransferQueue allTransfers = transfers();
    ProcedureTransferQueue bt;
    int callsToSkip = 0;
    CallPathTracker::ProcedureTransferQueue::const_reverse_iterator i = 
        allTransfers.rbegin();
    // Define end iterator as an explilcit variable to avoid compiling
    // bug under Mac OS X.
    CallPathTracker::ProcedureTransferQueue::const_reverse_iterator e = 
        allTransfers.rend();
    for (; i != e; ++i) {
        CallPathTracker::ProcedureTransfer tr = *i;
        if (!tr.isEntry) {
            ++callsToSkip;
            continue;
        }
        if (callsToSkip == 0) {
            bt.push_back(tr);
        } else {
            callsToSkip--;
        }
    }
    return bt;
}
