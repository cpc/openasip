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
 * @file BasicBlockPass.hh
 *
 * Declaration of BasicBlockPass interface.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BB_PASS_HH
#define TTA_BB_PASS_HH

#include "NullMachine.hh"
#include "Exception.hh"
#include "SchedulerPass.hh"
#include "DataDependenceGraph.hh"

class DDGPass;
class SimpleResourceManager;

class BasicBlock;
namespace TTAMachine {
    class Machine;
}

/**
 * Interface for scheduler passes that handle basic blocks.
 */
class BasicBlockPass : public SchedulerPass {
public:
    BasicBlockPass(InterPassData& data);
    virtual ~BasicBlockPass();

    virtual void handleBasicBlock(
        BasicBlock& basicBlock,
        const TTAMachine::Machine& targetMachine)
        throw (Exception);

    void executeDDGPass(
        BasicBlock& bb,
        const TTAMachine::Machine& targetMachine, 
        DDGPass& ddgPass)
        throw (Exception);

    virtual DataDependenceGraphBuilder& ddgBuilder() { return ddgBuilder_; }

protected:

    virtual DataDependenceGraph* createDDGFromBB(BasicBlock& bb);
    virtual void deleteRM(SimpleResourceManager* rm, BasicBlock& bb);
    virtual void copyRMToBB(
        SimpleResourceManager& rm, BasicBlock& bb, 
        const TTAMachine::Machine& targetMachine);

private:
    DataDependenceGraphBuilder ddgBuilder_;
};
#endif
