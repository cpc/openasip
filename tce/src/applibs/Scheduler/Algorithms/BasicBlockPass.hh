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

#include "Exception.hh"
#include "SchedulerPass.hh"
#include "DataDependenceGraphBuilder.hh"

class BasicBlockNode;
class DDGPass;
class DataDependenceGraph;
class SimpleResourceManager;

namespace TTAProgram {
    class BasicBlock;
}

namespace TTAMachine {
    class Machine;
}

namespace TTAProgram {
    class InstructionReferenceManager;
}

/**
 * Interface for scheduler passes that handle basic blocks.
 */
class BasicBlockPass : public SchedulerPass {
public:
    BasicBlockPass(InterPassData& data);
    virtual ~BasicBlockPass();

    virtual void handleBasicBlock(
        TTAProgram::BasicBlock& basicBlock,
        const TTAMachine::Machine& targetMachine,
        TTAProgram::InstructionReferenceManager& irm,
        BasicBlockNode* bbn = NULL);

    virtual void executeDDGPass(
        TTAProgram::BasicBlock& bb, const TTAMachine::Machine& targetMachine,
        TTAProgram::InstructionReferenceManager& irm,
        std::vector<DDGPass*> ddgPasses, BasicBlockNode* bbn = NULL);

    virtual bool executeLoopPass(
        TTAProgram::BasicBlock& bb, const TTAMachine::Machine& targetMachine,
        TTAProgram::InstructionReferenceManager& irm,
        std::vector<DDGPass*> ddgPasses, BasicBlockNode* bbn = NULL);

    static void copyRMToBB(
        SimpleResourceManager& rm, TTAProgram::BasicBlock& bb, 
        const TTAMachine::Machine& targetMachine,
        TTAProgram::InstructionReferenceManager& irm, int lastCycle = -1);

    virtual DataDependenceGraphBuilder& ddgBuilder() { return ddgBuilder_; }

protected:
    void ddgSnapshot(
        DataDependenceGraph* ddg,
        std::string& name,
        DataDependenceGraph::DumpFileFormat format,
        bool final);

    
    virtual DataDependenceGraph* createDDGFromBB(
        TTAProgram::BasicBlock& bb, const TTAMachine::Machine& mach);

private:
    DataDependenceGraphBuilder ddgBuilder_;
};
#endif
