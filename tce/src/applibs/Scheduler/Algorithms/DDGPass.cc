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
 * @file DDGPass.cc
 * 
 * Definition of a DDGPass class.
 *
 * @author Heikki Kultala 2007 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "DDGPass.hh"
#include "DataDependenceGraph.hh"
#include "Machine.hh"
#include "SimpleResourceManager.hh"

/**
 * Constructor.
 */
DDGPass::DDGPass(InterPassData& data) :
    SchedulerPass(data) {
}

/**
 * Destructor.
 */
DDGPass::~DDGPass() {
}

/**
 * Handles a given DDG.
 *
 * @param ddg DDG to handle
 * @param rm Resource manager that is to be used.
 * @param machine The target machine if any. (NullMachine::instance() if
 * target machine is irrelevant).
 * @exception In case handling is unsuccesful for any reason (basicBlock
 * might still get modified).
 */
int
DDGPass::handleDDG(
    DataDependenceGraph& ddg, SimpleResourceManager& rm,
    const TTAMachine::Machine& targetMachine, bool /* testOnly*/) {
    // just to avoid warnings -- need to keep the argument names for
    // Doxygen comments ;)
    ddg.nodeCount();
    rm.largestCycle();
    targetMachine.machineTester();
    abortWithError("Should never call this.");
}

int
DDGPass::handleLoopDDG(
    DataDependenceGraph&, SimpleResourceManager&, const TTAMachine::Machine&,
    int, SimpleResourceManager*, bool) {
    return -1;
}
