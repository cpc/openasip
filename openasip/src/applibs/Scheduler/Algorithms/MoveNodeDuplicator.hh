/*
    Copyright (c) 2002-2015 Tampere University.

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
 * This class is used for copying moves from a basic block to another
 * corresponding basic block, copying also the programoperation structure.
 */

#ifndef MOVENODE_DUPLICATOR_HH
#define MOVENODE_DUPLICATOR_HH
#include "MoveNode.hh"
#include "ProgramOperation.hh"

class BasicBlockNode;
class DataDependenceGraph;

class MoveNodeDuplicator {
public:
    MoveNodeDuplicator(
        DataDependenceGraph& oldDDG, DataDependenceGraph& newDDG);

    std::pair<MoveNode*, bool> duplicateMoveNode(
        MoveNode& mn, bool addToDDG, bool ignoreSameBBBackEdges);
    std::shared_ptr<TTAProgram::Move> duplicateMove(TTAProgram::Move& old);

    ProgramOperationPtr duplicateProgramOperationPtr(
        ProgramOperationPtr old);

    void disposeMoveNode(MoveNode *newMN);

    void setBBN(BasicBlockNode& bbn) {
        bbn_ = &bbn;
    }

    MoveNode* getMoveNode(MoveNode& mn);
    ProgramOperationPtr getProgramOperation(ProgramOperationPtr old);
    void dumpDDG();

private:
    // old to copied mapping
    std::map<ProgramOperation*,
             ProgramOperationPtr,
             ProgramOperation::Comparator>
    programOperations_;
    // copied to old mapping
    std::map<ProgramOperation*,
             ProgramOperationPtr,
             ProgramOperation::Comparator>
    oldProgramOperations_;
    // old to copied mapping
    std::map<MoveNode*,MoveNode*,MoveNode::Comparator> moveNodes_;
    // copied to old mapping
    std::map<MoveNode*,MoveNode*,MoveNode::Comparator> oldMoveNodes_;
    std::map<TTAProgram::Move*,std::shared_ptr<TTAProgram::Move> > moves_;

    BasicBlockNode* bbn_;
    DataDependenceGraph* oldDDG_;
    DataDependenceGraph* newDDG_;
    DataDependenceGraph* bigDDG_;

};

#endif
