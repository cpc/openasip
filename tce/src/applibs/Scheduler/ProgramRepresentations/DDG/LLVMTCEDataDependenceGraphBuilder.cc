/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file LLVMTCEDataDependenceGraphBuilder.cc
 *
 * @author Pekka Jääskeläinen 2010 (pjaaskel)
 */

#include "LLVMTCEDataDependenceGraphBuilder.hh"
#include "MoveNodeGroupBuilder.hh"
#include "MoveNodeGroup.hh"
#include "ProgramOperation.hh"
#include "Terminal.hh"
#include "Move.hh"

DataDependenceGraph*
LLVMTCEDataDependenceGraphBuilder::build(
    ControlFlowGraph& cGraph, const UniversalMachine*) {

    currentDDG_ = new DataDependenceGraph(allParamRegs_);
    for (int bbi = 0; bbi < cGraph.nodeCount(); ++bbi) {
        BasicBlockNode& bbnode = cGraph.node(bbi);
        buildLocalDDG(bbnode.basicBlock());
    }
    /* TODO: 
       buildGlobalEdges(cGraph);
    */
    currentDDG_->writeToDotFile("ddg.dot");
    return currentDDG_;
}

DataDependenceGraph*
LLVMTCEDataDependenceGraphBuilder::build(
    TTAProgram::BasicBlock& bb, const UniversalMachine*) 
    throw (IllegalProgram) {
    currentDDG_ = new DataDependenceGraph(allParamRegs_);
    buildLocalDDG(bb);
    return currentDDG_;
}

void
LLVMTCEDataDependenceGraphBuilder::buildLocalDDG(TTAProgram::BasicBlock& bb) {
    MoveNodeGroupBuilder mngb;
    MoveNodeGroupBuilder::MoveNodeGroupList* mngs =
        mngb.build(bb);

    /*
      For starters, ensure the original order by chaining the
      MNs with UNKNOWN edges and add operation edges also.

      This should trigger the problems with RM so those can be
      worked on simultaneously.
     */
    MoveNode* lastMN = NULL;
    for (MoveNodeGroupBuilder::MoveNodeGroupList::const_iterator i =
             mngs->begin(); i != mngs->end(); ++i) {
        MoveNodeGroup& mng = **i;
//        PRINT_VAR(mng.toString());
        if (mng.isOperation()) {
            currentDDG_->addProgramOperation(mng.programOperationPtr());
        }
        MoveNode* newLastMN = &mng.node(0);
        for (int mni = 0; mni < mng.nodeCount(); ++mni) {
            MoveNode& mn = mng.node(mni);
            currentDDG_->addNode(mn);
            if (lastMN != NULL) {
                DataDependenceEdge* edge =
                    new DataDependenceEdge(
                        DataDependenceEdge::EDGE_REGISTER, 
                        DataDependenceEdge::DEP_UNKNOWN);             
                currentDDG_->connectNodes(*lastMN, mn, *edge);
            }
            if (mn.move().source().isFUPort())
                newLastMN = &mn;
        }
        lastMN = newLastMN;
    }

    for (MoveNodeGroupBuilder::MoveNodeGroupList::const_iterator i =
             mngs->begin(); i != mngs->end(); ++i) {
        MoveNodeGroup& mng = **i;
        if (mng.isOperation()) {
            createOperationEdges(mng.programOperationPtr());
        }
    }   
}
