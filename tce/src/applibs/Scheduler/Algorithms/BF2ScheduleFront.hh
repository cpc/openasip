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
 * @file BF2ScheduleFront.hh
 *
 * Declaration of BF2ScheduleFront class
 *
 * Tries to schedule a group of moves.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef BF2_SCHEDULE_FRONT_HH
#define BF2_SCHEDULE_FRONT_HH

#include "BFOptimization.hh"
#include "DataDependenceGraph.hh"
#include "BF2Scheduler.hh"

class MoveNode;
class TCEString;

namespace TTAMachine {
    class Port;
}

class BF2ScheduleFront : public BFOptimization {
public:

    BF2ScheduleFront(
        BF2Scheduler& sched, MoveNode& mn, int lc) :
        BFOptimization(sched), mn_(mn), lc_(lc) {}

    virtual bool operator()() override;

    void undoOnlyMe() override;

    MoveNode* findInducingBypassSource(MoveNode& mn);

    void bypassed(MoveNode& src, MoveNode& dst) {
        bypassSources_[&dst] = &src;
    }

    void undidBypass(MoveNode&, MoveNode& dst) {
        bypassSources_.erase(&dst);
    }

    DataDependenceGraph::NodeSet illegalBypassSources_;
    DataDependenceGraph::NodeSet illegalOperandShares_;

    static int prefResultCycle(const MoveNode& mn);

    void mightBeReady(MoveNode& n) override;

    void deletingNode(MoveNode* deletedNode) {
        nodesToNotify_.erase(deletedNode);
    }

    typedef BF2Scheduler::MoveNodeMap MoveNodeMap;
    void appendBypassSources(MoveNodeMap& map);

protected:

    DataDependenceGraph::NodeSet
    allNodesOfSameOperation(MoveNode& mn);

    int scheduleMove(
        MoveNode& move,
        BF2Scheduler::SchedulingLimits limits,
        bool allowEarlyBypass = true,
        bool allowLateBypass = true,
        bool allowEarlySharing = true);

    bool scheduleFrontFromMove(MoveNode& mn);

    void requeueOtherMovesOfSameOp(MoveNode& mn);

    bool tryToScheduleMoveOuter(MoveNode& mn, int& latestCycle);

    BF2Scheduler::SchedulingLimits getPreferredLimits(const MoveNode& mn);

    MoveNode* getMoveNodeFromFrontBU();

    void printFront(const TCEString& prefix);


    void clearSchedulingFront();

    int latestScheduledOfFrontCycle();

private:

    bool tryRevertEarlierBypass(MoveNode& mn);

    MoveNodeMap bypassSources_;

    MoveNode* findInducingBypassSourceFromOperation(
        ProgramOperation& po,
        const DataDependenceGraph::NodeSet& processedNodes,
        DataDependenceGraph::NodeSet& queue);

    DataDependenceGraph::NodeSet schedulingFront_;

    DataDependenceGraph::NodeSet nodesToNotify_;

    MoveNode& mn_;
    int lc_;

    typedef std::map<MoveNode*, int, MoveNode::Comparator> PathLengthCache;
    PathLengthCache pathLengthCache_;
};

#endif
