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
 * @file ControlFlowGraphPass.cc
 *
 * Definition of ControlFlowGraphPass class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ControlFlowGraphPass.hh"
#include "Application.hh"
#include "ControlFlowGraph.hh"
#include "Machine.hh"
#include "BasicBlockPass.hh"

/**
 * Constructor.
 */
ControlFlowGraphPass::ControlFlowGraphPass(InterPassData& data) : 
    SchedulerPass(data) {
}

/**
 * Destructor.
 */
ControlFlowGraphPass::~ControlFlowGraphPass() {
}

/**
 * Handles a single control flow graph.
 *
 * The pass should work with any kind of control flow graph, it should not
 * assume the CFG represents a whole procedure, for example.
 *
 * @param cfg The control flow graph to handle.
 * @param machine The target machine if any. (NullMachine::instance() if
 * target machine is irrelevant).
 * @exception In case handling is unsuccesful for any reason (cfg might 
 * still get modified).
 */
void
ControlFlowGraphPass::handleControlFlowGraph(
    ControlFlowGraph& cfg, const TTAMachine::Machine& targetMachine) {
    BasicBlockPass* bbPass = dynamic_cast<BasicBlockPass*>(this);
    if (bbPass != NULL) {
        executeBasicBlockPass(cfg, targetMachine, *bbPass);
    } else {
        abortWithError("CFG Pass is not also a BB pass so you "
                       "must overload handleControlFlowGraph method!");
    }
}

/**
 * Executes the given basic block pass on each basic block of the given
 * control flow graph in the original program order.
 *
 * A helper function for implementing most simplest types of CFG passes.
 *
 * @param cfg The control flow graph to handle.
 * @param targetMachine The target machine, if any. (NullMachine::instance() if
 * target machine is irrelevant).
 * @param bbPass The basic block pass to execute.
 * @exception In case handling is unsuccesful for any reason (cfg might 
 * still get modified).
 */
void
ControlFlowGraphPass::executeBasicBlockPass(
    ControlFlowGraph& cfg, const TTAMachine::Machine& targetMachine,
    BasicBlockPass& bbPass) {
    int nodeCount = cfg.nodeCount();
    for (int bbIndex = 0; bbIndex < nodeCount; ++bbIndex) {
        BasicBlockNode& bb = dynamic_cast<BasicBlockNode&>(cfg.node(bbIndex));
        if (!bb.isNormalBB())
            continue;
        if (bb.isScheduled()) {
            continue;
        }

        bbPass.handleBasicBlock(
            bb.basicBlock(), targetMachine, 
            cfg.instructionReferenceManager(), &bb);
        bb.setScheduled();
        // if some node is removed, make sure does not skip some node and
        // then try to handle too many nodes.
        if (cfg.nodeCount() != nodeCount) {
            nodeCount = cfg.nodeCount();
            bbIndex = 0;
        }
    }
}
