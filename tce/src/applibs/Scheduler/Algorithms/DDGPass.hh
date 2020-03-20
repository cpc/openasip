/*
    Copyright (c) 2002-2009 Tampere University.

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
 *
 * @file DDGPass.hh
 *
 * Definition of a DDGPass interface.
 * 
 * @author Heikki Kultala 2007 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DDG_PASS_HH
#define TTA_DDG_PASS_HH

#include "SchedulerPass.hh"
#include "Exception.hh"

class DataDependenceGraph;
class BasicBlock;
class SimpleResourceManager;

namespace TTAMachine {
    class Machine;
}

/*
 * Interface for a scheduler passes that handle data dependence graphs.
 */
class DDGPass : public SchedulerPass {
public:
    DDGPass(InterPassData& data);
    virtual ~DDGPass();
    virtual int handleDDG(
        DataDependenceGraph& ddg, SimpleResourceManager& rm,
        const TTAMachine::Machine& targetMachine, bool testOnly = false);

    /**
     * For BasicBlockPass to be able to call this method...
     */
    virtual int handleLoopDDG(
        DataDependenceGraph&, SimpleResourceManager&,
        const TTAMachine::Machine&, int tripCount,
        SimpleResourceManager* prologRM = NULL,
        bool testOnly = false);
};

#endif
