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
 * @file BFRenameLiveRange.hh
 *
 * Declaration of BFRenameLiveRange class
 *
 * Renames a liverange, consisting of one or more writes,
 * and one or more either register reads or guadrd uses of the register.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef BF_RENAME_LIVERANGE_HH
#define BF_RENAME_LIVERANGE_HH

#include "BFOptimization.hh"
#include "LiveRangeData.hh"
#include "DataDependenceGraph.hh"
#include "Terminal.hh"

class BFRenameLiveRange : public BFOptimization {

public:
    BFRenameLiveRange(BF2Scheduler& sched,
                      std::shared_ptr<LiveRange> liveRange,
                      int targetCycle);

    virtual bool operator()();
    virtual ~BFRenameLiveRange();

protected:
    void undoSetTerminal();
    void setTerminals();

    void setGuard(TTAProgram::Move& move,
                  const TTAMachine::RegisterFile& rf, int regIndex);
    void setRegDefsAndLastUses();
    void undoSetRegDefsAndLastUses();
    void setFirstDefsAndUses();
    void undoSetFirstDefsAndUses();
    void setOutgoingDeps();
    void setIncomingDeps();
    void unsetIncomingDeps();
    void unsetOutgoingDeps();
    void undoNewAntiDeps();
    void notifySelector();

    bool renameLiveRange(
        class LiveRange& liveRange,
        const class TCEString& reg,
        bool usedAfter);

    virtual void undoOnlyMe() override;

    DataDependenceGraph::UndoData undoWriteUpdateData_;
    std::map<const MoveNode*,
             DataDependenceGraph::UndoData,
             MoveNode::Comparator>
    undoReadUpdateData_;

    class TTAProgram::Terminal* oldReg_;
    std::shared_ptr<LiveRange> liveRange_;
    TTAProgram::BasicBlock* bb_;
    TCEString newReg_;

    MoveNodeUse oldLastKill_;
    MoveNodeUse oldKill_;
    int targetCycle_;

    LiveRangeData::MoveNodeUseSet oldRegDefines_;
    LiveRangeData::MoveNodeUseSet oldRegFirstDefines_;
    LiveRangeData::MoveNodeUseSet oldRegFirstUses_;
    DataDependenceGraph::EdgeSet createdAntidepEdges_;


};

#endif
