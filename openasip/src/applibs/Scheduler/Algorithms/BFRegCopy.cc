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
 * @file BFRegCopy.cc
 *
 * Definition of BFRegCopy class
 *
 *  Base class for creating a register-to-register copy.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFRegCopy.hh"
#include "ProgramAnnotation.hh"
#include "MoveNode.hh"
#include "DataDependenceGraph.hh"
#include "Move.hh"
#include "MachinePart.hh"
#include "BF2Scheduler.hh"
#include "BasicBlockNode.hh"
#include "BasicBlock.hh"
#include "RegisterFile.hh"

#include "BFConnectNodes.hh"
#include "BFInsertLiveRangeUse.hh"
#include "BFClearLiveRangeUse.hh"

bool
BFRegCopy::operator()() {

    if (mn_.move().isReturn()) {
        TTAProgram::ProgramAnnotation annotation(
            TTAProgram::ProgramAnnotation::ANN_STACKFRAME_PROCEDURE_RETURN);
        mn_.move().setAnnotation(annotation);
    }

    TTAProgram::ProgramAnnotation connMoveAnnotation(
        TTAProgram::ProgramAnnotation::ANN_CONNECTIVITY_MOVE);

    auto copyMove = mn_.move().copy();
    regCopy_ = new MoveNode(copyMove);
    BasicBlockNode& bbn = ddg().getBasicBlockNode(mn_);
    ddg().addNode(*regCopy_,bbn);
    copyMove->addAnnotation(connMoveAnnotation);

    if (splitMove(bbn)) {
        return true;
    }
    else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "Regcopy creation failing.." << std::endl;
#endif
        undoDDG();
        return false;
    }
}

void BFRegCopy::undoOnlyMe() {
    undoSplit();
    undoDDG();
}

void BFRegCopy::undoDDG() {

    assert(!regCopy_->isScheduled());

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tUndid merge for copy for: " << mn_.toString()
              << " reg copy: " << regCopy_->toString() << std::endl;
#endif
    ddg().removeNode(*regCopy_);
    sched_.deletingNode(regCopy_);
    delete regCopy_;
}

void BFRegCopy::createAntidepsForReg(
    MoveNode& defMove,
    MoveNode& useMove,
    const TTAMachine::RegisterFile& rf,
    int index,
    TCEString regName,
    BasicBlockNode& bbn,
    bool loopScheduling) {

    DataDependenceGraph::NodeSet firstScheduledDefs0 =
        ddg().firstScheduledRegisterWrites(rf, index);

    MoveNode* lastScheduledKill0 =
        ddg().lastScheduledRegisterKill(rf, index);

    for (auto i: firstScheduledDefs0) {
        if (!ddg().exclusingGuards(*i, useMove)) {
            DataDependenceEdge* war =
                new DataDependenceEdge(
                    DataDependenceEdge::EDGE_REGISTER,
                    DataDependenceEdge::DEP_WAR, regName);
            runPostChild(new BFConnectNodes(sched_, useMove, *i, war));
        }
        if (!ddg().exclusingGuards(*i, defMove)) {
            DataDependenceEdge* waw =
                new DataDependenceEdge(
                    DataDependenceEdge::EDGE_REGISTER,
                    DataDependenceEdge::DEP_WAW, regName);
            runPostChild(new BFConnectNodes(sched_, defMove, *i, waw));
        }
    }

    if (loopScheduling) {
        DataDependenceGraph::NodeSet lastScheduledReads0 =
            ddg().lastScheduledRegisterReads(rf, index);

        for (auto i: lastScheduledReads0) {
            if (!ddg().exclusingGuards(*i, defMove)) {
                DataDependenceEdge* war =
                    new DataDependenceEdge(
                        DataDependenceEdge::EDGE_REGISTER,
                        DataDependenceEdge::DEP_WAR, regName,
                        false, false, false, false, 1);
                runPostChild(new BFConnectNodes(sched_, *i, defMove, war));
            }
        }

        DataDependenceGraph::NodeSet lastScheduledDefs0 =
            ddg().lastScheduledRegisterWrites(rf, index);
        for (auto i: lastScheduledDefs0) {
            if (!ddg().exclusingGuards(*i, defMove)) {
                DataDependenceEdge* waw =
                    new DataDependenceEdge(
                        DataDependenceEdge::EDGE_REGISTER,
                        DataDependenceEdge::DEP_WAW, regName,
                        false, false, false, false, 1);
                runPostChild(new BFConnectNodes(sched_, *i, defMove, waw));
            }
        }
    }
    if (bbn.basicBlock().liveRangeData_ != NULL) {
        if (lastScheduledKill0 == NULL) {
            LiveRangeData::MoveNodeUseSet& lastWrites0 =
                bbn.basicBlock().liveRangeData_->regDefines_[regName];
            runPostChild(new BFInsertLiveRangeUse(
                             sched_, lastWrites0, MoveNodeUse(defMove)));

            LiveRangeData::MoveNodeUseSet& lastReads0 =
                    bbn.basicBlock().liveRangeData_->regLastUses_[regName];
                runPostChild(new BFInsertLiveRangeUse(
                                 sched_, lastReads0, MoveNodeUse(useMove)));
        }

        // last write, for WaW defs
        LiveRangeData::MoveNodeUseSet& firstDefs =
            bbn.basicBlock().liveRangeData_->regFirstDefines_[regName];

        if (useMove.move().isUnconditional()) {
            LiveRangeData::MoveNodeUseSet& firstReads =
                bbn.basicBlock().liveRangeData_->regFirstUses_[regName];

            runPostChild(new BFClearLiveRangeUse(sched_, firstReads));
            runPostChild(new BFClearLiveRangeUse(sched_, firstDefs));
            // TODO: what about updating the kill bookkeeping?
        }

        runPostChild(
            new BFInsertLiveRangeUse(
                sched_, firstDefs, MoveNodeUse(defMove)));
    }
}
