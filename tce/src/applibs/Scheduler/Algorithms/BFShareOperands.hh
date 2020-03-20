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
 * @file BFShareOperands.hh
 *
 * Declaration of BFShareOperands class
 *
 * Searches for potential operations to share operand with, and
 * then calls BFShareOperandWithScheduled to fo the actual operand sharing.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFOptimization.hh"
#include "SchedulerCmdLineOptions.hh"

class BFShareOperands : public BFOptimization {
public:
    BFShareOperands(BF2Scheduler& sched, MoveNode& mn, int lc) :
        BFOptimization(sched), mn_(mn), maxCycle_(lc+6) {
	SchedulerCmdLineOptions* opts =
	    dynamic_cast<SchedulerCmdLineOptions*>(Application::cmdLineOptions());
	if (opts != NULL) {
	    if (opts->operandShareDistance() > -1) {
		maxCycle_ = lc + opts->operandShareDistance();
	    }
	}
    }

    virtual bool operator()();
    virtual bool removedNode();
protected:
    virtual void undoOnlyMe();
private:
    MoveNode& mn_;
    int maxCycle_;
};
