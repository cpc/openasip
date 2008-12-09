/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file SequentialSchedulerModule.hh
 *
 * Declaration of SequentialSchedulerModule class.
 * 
 * Sequential scheduler which uses the new pass hierarchy,
 * resource manager and registercopyadder.
 * Does not use DDG.
 *
 * Runs quickly but creates very slow code, 
 * just for debugging/testing purposes.
 *
 * @author Heikki Kultala 2008 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SEQUENTIAL_SCHEDULER_MODULE_HH
#define TTA_SEQUENTIAL_SCHEDULER_MODULE_HH

#include "StartableSchedulerModule.hh"

#include "SequentialScheduler.hh"

/**
 * Implementation of a simple sequential scheduling algorithm.
 */
class SequentialSchedulerModule : public StartableSchedulerModule {
public:
    SequentialSchedulerModule();
    virtual ~SequentialSchedulerModule();

    virtual void start()        
        throw (Exception);
    bool needsProgram() const { return true; }
    bool needsTarget() const { return true; }

    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;
};

SCHEDULER_PASS(SequentialSchedulerModule)

#endif
