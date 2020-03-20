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
 * @file BFRenameSource.hh
 *
 * Declaration of BFRenameSource class
 *
 * Renames a liverange, starting from a register read.
 * Class BFRenameLiverange to perform the actual work.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef BF_RENAME_SOURCE_HH
#define BF_RENAME_SOURCE_HH

#include "BFOptimization.hh"

class BFRenameSource : public BFOptimization {

public:
    BFRenameSource(BF2Scheduler& sched, MoveNode& mn,
                   int originalDDGLC, int targetCycle);

    virtual bool operator()();
    virtual ~BFRenameSource() {}

protected:
    MoveNode& mn_;
    int originalDDGLC_;
    int targetCycle_;

};

#endif
