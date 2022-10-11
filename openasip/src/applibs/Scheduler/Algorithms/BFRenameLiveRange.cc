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
 * @file BFRenameLiveRange.cc
 *
 * Definition of BFRenameLiveRange class
 *
 * Renames a liverange, consisting of one or more writes,
 * and one or more either register reads or guadrd uses of the register.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFRenameLiveRange.hh"

#include "BF2Scheduler.hh"
#include "RegisterRenamer.hh"
#include "RegisterFile.hh"
#include "LiveRange.hh"
#include "BasicBlock.hh"
#include "TerminalRegister.hh"
#include "Machine.hh"
#include "BUMoveNodeSelector.hh"
#include "BF2ScheduleFront.hh"
#include "MoveGuard.hh"
#include "Guard.hh"


BFRenameLiveRange::BFRenameLiveRange(
    BF2Scheduler& sched,
    std::shared_ptr<LiveRange> liveRange,
    int targetCycle) :
    BFOptimization(sched), oldReg_(nullptr), liveRange_(liveRange),
    targetCycle_(targetCycle) {}


bool BFRenameLiveRange::operator()() {

    bb_ = &sched_.renamer()->bb();

    assert(!liveRange_->reads.empty());

    auto rfs = sched_.renamer()->findConnectedRFs(*liveRange_, false);

    bool usedAfter = true;
    std::set<TCEString> regs;

    if (!ii()) {

        regs = sched_.renamer()->findPartiallyUsedRegistersInRFAfterCycle(
            rfs, targetCycle_);
    }

    if (regs.empty()) {
        usedAfter = false;
        regs = sched_.renamer()->findFreeRegistersInRF(rfs);
    }

    if (regs.empty()) {
        return false;
    }

    return renameLiveRange(*liveRange_, *regs.begin(), usedAfter);
}

void BFRenameLiveRange::setTerminals() {

    TCEString rfName = newReg_.substr(0, newReg_.find('.'));
    TTAMachine::RegisterFile* rf =
        targetMachine().registerFileNavigator().item(rfName);
    int newRegIndex = atoi(newReg_.substr(newReg_.find('.')+1).c_str());

    // for writes.
    for (auto i: liveRange_->writes) {
        TTAProgram::Move& move = i->move();
        move.setDestination(new TTAProgram::TerminalRegister(
                                *rf->firstWritePort(), newRegIndex));
    }

    // for reads.
    for (auto i: liveRange_->reads) {
        TTAProgram::Move& move = i->move();
        move.setSource(new TTAProgram::TerminalRegister(
                           *rf->firstReadPort(), newRegIndex));
    }

    // for guards.
    for (auto i: liveRange_->guards) {
        TTAProgram::Move& move = i->move();
        setGuard(move, *rf, newRegIndex);
    }
}

void BFRenameLiveRange::setGuard(
    TTAProgram::Move& move, const TTAMachine::RegisterFile& rf, int regIndex){

    const TTAMachine::Guard& guard = move.guard().guard();
    TTAMachine::Bus* guardBus =  guard.parentBus();
    for (int j = 0 ; j < guardBus->guardCount(); j++) {
        TTAMachine::Guard *g = guardBus->guard(j);
        TTAMachine::RegisterGuard* rg =
            dynamic_cast<TTAMachine::RegisterGuard*>(g);
        if (rg) {
            if (rg->registerFile() == &rf &&
                rg->registerIndex() == regIndex &&
                rg->isInverted() == guard.isInverted()) {
                move.setGuard(new TTAProgram::MoveGuard(*g));
            }
        }
    }
}

void BFRenameLiveRange::undoSetTerminal() {

    // for writes.
    for (auto i: liveRange_->writes) {
        TTAProgram::Move& move = i->move();
        move.setDestination(oldReg_->copy());
    }

    // for reads.
    for (auto i: liveRange_->reads) {
        TTAProgram::Move& move = i->move();
        move.setSource(oldReg_->copy());
    }

    // for guards
    for (auto i: liveRange_->guards) {
        TTAProgram::Move& move = i->move();
        setGuard(move, oldReg_->registerFile(), oldReg_->index());
    }

}

void BFRenameLiveRange::undoOnlyMe() {
    // restore the DDG stage before the rename
    for (auto i: undoReadUpdateData_) {
        ddg().undo(i.second);
    }
    ddg().undo(undoWriteUpdateData_);

    undoSetTerminal();

    undoSetFirstDefsAndUses();
    undoSetRegDefsAndLastUses();

    unsetIncomingDeps();
    unsetOutgoingDeps();

    sched_.renamer()->revertedRenameToRegister(newReg_);

    undoNewAntiDeps();

}

void BFRenameLiveRange::undoNewAntiDeps() {
    for (auto e: createdAntidepEdges_) {
        ddg().removeEdge(*e);
    }
    createdAntidepEdges_.clear();
}

void BFRenameLiveRange::setIncomingDeps() {
    if (liveRange_->writes.size() != 1 ||
        !(*liveRange_->writes.begin())->move().isUnconditional()) {
        return;
    }

    oldKill_ = bb_->liveRangeData_->regKills_[newReg_].first;
    oldRegFirstDefines_ = bb_->liveRangeData_->regFirstDefines_[newReg_];
    oldRegFirstUses_ = bb_->liveRangeData_->regFirstUses_[newReg_];
}
void BFRenameLiveRange::unsetIncomingDeps() {
    bb_->liveRangeData_->regKills_[newReg_].first = oldKill_;
    bb_->liveRangeData_->regFirstDefines_[newReg_] = oldRegFirstDefines_;
    bb_->liveRangeData_->regFirstUses_[newReg_] = oldRegFirstUses_;
}

void BFRenameLiveRange::unsetOutgoingDeps() {
    bb_->liveRangeData_->regLastKills_[newReg_].first = oldLastKill_;
    bb_->liveRangeData_->regDefines_[newReg_] = oldRegDefines_;
}

void BFRenameLiveRange::setOutgoingDeps() {
    if (liveRange_->writes.size() != 1 ||
        !(*liveRange_->writes.begin())->move().isUnconditional()) {
        return;
    }

    oldLastKill_ = bb_->liveRangeData_->regLastKills_[newReg_].first;
    oldRegDefines_ = bb_->liveRangeData_->regDefines_[newReg_];

    bb_->liveRangeData_->regLastKills_[newReg_].first =
        MoveNodeUse(**liveRange_->writes.begin());
    bb_->liveRangeData_->regDefines_[newReg_].clear();
}


void BFRenameLiveRange::setRegDefsAndLastUses() {
    // for writing.
    for (auto i: liveRange_->writes) {
        MoveNodeUse mnd(*i);
        bb_->liveRangeData_->regDefines_[newReg_].insert(mnd);
    }

    // for reading.
    for (auto i: liveRange_->reads) {
        MoveNodeUse mnd(*i);
        bb_->liveRangeData_->regLastUses_[newReg_].insert(mnd);
    }

    // for reading.
    for (auto i: liveRange_->guards) {
        MoveNodeUse mnd(*i, true);
        bb_->liveRangeData_->regLastUses_[newReg_].insert(mnd);
    }

}

void BFRenameLiveRange::undoSetRegDefsAndLastUses() {
    // for writing.
    for (auto i: liveRange_->writes) {
        MoveNodeUse mnd(*i);
        bb_->liveRangeData_->regDefines_[newReg_].erase(mnd);
    }

    // for reading.
    for (auto i: liveRange_->reads) {
        MoveNodeUse mnd(*i);
        bb_->liveRangeData_->regLastUses_[newReg_].erase(mnd);
    }

    // for reading.
    for (auto i: liveRange_->guards) {
        MoveNodeUse mnd(*i, true);
        bb_->liveRangeData_->regLastUses_[newReg_].erase(mnd);
    }
}

void BFRenameLiveRange::setFirstDefsAndUses() {

    // for writing.
    for (auto i: liveRange_->writes) {
        MoveNodeUse mnd(*i);
        bb_->liveRangeData_->regFirstDefines_[newReg_].insert(mnd);
        // TODO: only if intra-bb-antideps enabled?
//        static_cast<DataDependenceGraph*>(ddg().rootGraph())->
//            updateRegWrite(mnd, newReg, *bb_);
    }

    // for reading.
    for (auto i: liveRange_->reads) {
        MoveNodeUse mnd(*i);
        bb_->liveRangeData_->regFirstUses_[newReg_].insert(mnd);
        // no need to create raw deps here
    }

    // for guards.
    for (auto i: liveRange_->guards) {
        MoveNodeUse mnd(*i, true);
        bb_->liveRangeData_->regFirstUses_[newReg_].insert(mnd);
        // no need to create raw deps here
    }
}

void BFRenameLiveRange::undoSetFirstDefsAndUses() {

    // for writing.
    for (auto i: liveRange_->writes) {
        MoveNodeUse mnd(*i);
        bb_->liveRangeData_->regFirstDefines_[newReg_].erase(mnd);


        // TODO: only if intra-bb-antideps enabled?
//        static_cast<DataDependenceGraph*>(ddg().rootGraph())->
//            updateRegWrite(mnd, newReg, *bb_);
    }

    // for reading.
    for (auto i: liveRange_->reads) {
        MoveNodeUse mnd(*i);
        bb_->liveRangeData_->regFirstUses_[newReg_].erase(mnd);
    }

    // for guards.
    for (auto i: liveRange_->guards) {
        MoveNodeUse mnd(*i, true);
        bb_->liveRangeData_->regFirstUses_[newReg_].erase(mnd);
    }
}





bool BFRenameLiveRange::renameLiveRange(
    LiveRange& liveRange, const TCEString& newReg, bool usedAfter) {

    bool usedBefore = false;

    newReg_ = newReg;
    oldReg_ = (*liveRange.writes.begin())->move().destination().copy();

    assert(newReg.length() > 2);

    if (usedAfter) {
        TCEString rfName = newReg_.substr(0, newReg_.find('.'));
        TTAMachine::RegisterFile* rf =
            targetMachine().registerFileNavigator().item(rfName);
        int newRegIndex = atoi(newReg_.substr(newReg_.find('.')+1).c_str());
        DataDependenceGraph::NodeSet firstWrites =
            ddg().firstScheduledRegisterWrites(*rf, newRegIndex);

        // make sure no circular antidep path
        for (auto j: firstWrites) {
            for (auto i: liveRange.reads) {
                if (ddg().hasPath(*j, *i)) {
                    return false;
                }
            }

            for (auto i: liveRange.writes) {
                if (ddg().hasPath(*j, *i)) {
                    return false;
                }
            }
        }

        // antideps
        for (auto j: firstWrites) {
            for (auto i: liveRange.writes) {
                DataDependenceEdge* newWAW =
                    new DataDependenceEdge(
                        DataDependenceEdge::EDGE_REGISTER,
                        DataDependenceEdge::DEP_WAW, newReg);
                ddg().connectNodes(*i,*j, *newWAW);
                createdAntidepEdges_.insert(newWAW);
            }

            for (auto i: liveRange.reads) {
                DataDependenceEdge* newWAR =
                    new DataDependenceEdge(
                        DataDependenceEdge::EDGE_REGISTER,
                        DataDependenceEdge::DEP_WAR, newReg);
                ddg().connectNodes(*i,*j, *newWAR);
                createdAntidepEdges_.insert(newWAR);
            }
        }
    } else {
        setOutgoingDeps();
        setRegDefsAndLastUses();


// TODO
#if 0
        // need to create backedges to first if we are loop scheduling.
        if (loopScheduling) {
            updateAntiEdgesFromLRTo(liveRange, newReg, bb_, 1);
        }
#endif
    }

    if (usedBefore) {
        return false;
    } else {
        setIncomingDeps();
        setFirstDefsAndUses();
    }

    setTerminals();
    notifySelector();

    // for writes.
    assert (liveRange_->writes.size() == 1);
    for (auto i: liveRange_->writes) {
        undoWriteUpdateData_ = ddg().destRenamed(*i);
    }

    // for reads
    for (auto i: liveRange_->reads) {
        undoReadUpdateData_[i] = ddg().sourceRenamed(*i);
    }

    assert(liveRange_->guards.size() == 0);
    // for guards
    for (auto i: liveRange_->guards) {
        undoReadUpdateData_[i] = ddg().guardRenamed(*i);
    }

    sched_.renamer()->renamedToRegister(newReg);
    notifySelector();
    return true;
}


void BFRenameLiveRange::notifySelector() {
    // then update ddg and notify selector.

    // for writes.
    for (auto i: liveRange_->writes) {
        DataDependenceGraph::NodeSet writeSuccessors =
            ddg().successors(*i);
        DataDependenceGraph::NodeSet writePredecessors =
            ddg().predecessors(*i);

        // notify successors of write to prevent orphan nodes.
        for (auto j: writeSuccessors) {
            sched_.currentFront()->mightBeReady(*j);
        }

        // notify successors of write to prevent orphan nodes.
        for (auto j: writePredecessors) {
            sched_.currentFront()->mightBeReady(*j);
        }

    }

    // for reads
    for (auto i: liveRange_->reads) {
        DataDependenceGraph::NodeSet successors =
            ddg().successors(*i);
        DataDependenceGraph::NodeSet predecessors =
            ddg().predecessors(*i);

        // notify successors to prevent orphan nodes.
        for (auto j: successors) {
            sched_.currentFront()->mightBeReady(*j);
        }

        // notify successors to prevent orphan nodes.
        for (auto j: predecessors) {
            sched_.currentFront()->mightBeReady(*j);
        }
    }

    // for guards
    for (auto i: liveRange_->guards) {
        DataDependenceGraph::NodeSet successors =
            ddg().successors(*i);
        DataDependenceGraph::NodeSet predecessors =
            ddg().predecessors(*i);

        // notify successors to prevent orphan nodes.
        for (auto j: successors) {
            sched_.currentFront()->mightBeReady(*j);
        }

        // notify successors to prevent orphan nodes.
        for (auto j: predecessors) {
            sched_.currentFront()->mightBeReady(*j);
        }
    }
}

BFRenameLiveRange::~BFRenameLiveRange() {
    delete oldReg_;
}
