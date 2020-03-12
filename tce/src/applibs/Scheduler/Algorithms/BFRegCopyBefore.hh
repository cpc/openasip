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
 * @file BFRegCopyBefore.hh
 *
 * Declaration of BFRegCopyBefore class
 *
 * Creates a register-to-register copy before a move, modifying the source
 * of the original move into the temporaty register and making the source
 * of the temp move the original souce register.
 *
 * Used for temp-reg-copies of operand moves.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef BF_RECGOPY_BEFORE_HH
#define BF_RECGOPY_BEFORE_HH

#include "BFRegCopy.hh"
class BFRegCopyBefore: public BFRegCopy {
public:
    BFRegCopyBefore(BF2Scheduler& sched, MoveNode& mn, int lc,
                    const TTAMachine::RegisterFile* forbiddenRF = nullptr)
        : BFRegCopy(sched,mn,lc), forbiddenRF_(forbiddenRF) {}
    virtual ~BFRegCopyBefore() {}
protected:
    bool splitMove(BasicBlockNode& bbn);
    void undoSplit();
    const TTAMachine::RegisterFile* forbiddenRF_;
};

#endif
