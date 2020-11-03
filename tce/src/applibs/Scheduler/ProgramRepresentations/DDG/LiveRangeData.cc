/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file LiveRangeData.cc
 *
 * Implementation of LiveRangeData class.
 *
 * Contains live range-related data for one basic block.
 * This is geenrated and used by ddg builder.
 * This may also be used by later parts of the scheduler.
 *
 * @author Heikki Kultala 2011 (hkultala@cs.tut.fi)
 * @note rating: red
 */

#include "LiveRangeData.hh"
#include "DataDependenceGraph.hh"
#include "Move.hh"

/**
 * merges liverangedata of successor into this.
 *
 * @param succ later basic block which is merged into this one.
 */
void LiveRangeData::merge(LiveRangeData& succ) {

    // copy outgoing live information as it is from successor
    appendUseMapSets(succ.regDefines_, regDefines_, false);
    appendUseMapSets(succ.regLastUses_, regLastUses_, false);

    // after this BB alive regs are those that are alive after the latter bb.
    registersUsedAfter_ = succ.registersUsedAfter_;

    // then in incoming deps, merge from both.

    appendUseMapSets(succ.regFirstDefines_, regFirstDefines_, false);
    appendUseMapSets(succ.regFirstUses_, regFirstUses_, false);
    appendUseMapSets(succ.regDefReaches_ , regDefReaches_, false);

}

/**
 * This appends the data from one MoveNodeUseMapSet to another.
 *
 * it traverses the map, and for every string, set pair it
 * finds or creates the corresponging set in the destination and appends
 * the set to that set.
 * This is used for copying alive definitions.
 *
 * @param srcMap source where to copy from
 * @param dstMap destination where to copy to.
 * @param addLoopProperty whether to add loop property to the copied
 *        bookkeeping, ie create edges with loop property.
 * @return true if destination changed (needs updating)
 */
bool
LiveRangeData::appendUseMapSets(
    const MoveNodeUseMapSet& srcMap, MoveNodeUseMapSet& dstMap,
    bool addLoopProperty) {
    bool changed = false;
    for (MoveNodeUseMapSet::const_iterator srcIter = srcMap.begin();
         srcIter != srcMap.end(); srcIter++) {
        TCEString reg = srcIter->first;
        const MoveNodeUseSet& srcSet = srcIter->second;
        MoveNodeUseSet& dstSet = dstMap[reg];
        // dest set size before appending.
        size_t size = dstSet.size();
        appendMoveNodeUse(srcSet, dstSet, addLoopProperty);
        // if size has changed, dest is changed.
        if (dstSet.size() > size) {
            changed = true;
        }
    }
    return changed;
}

/**
 * Appends a MoveNodeUseSet to another. May set loop property of copied
 * moves to true.
 *
 * @param src source set
 * @param dst destination set
 * @param setLoopProperty whether to set loop property true in copied data
 */
void LiveRangeData::appendMoveNodeUse(
    const LiveRangeData::MoveNodeUseSet& src, 
    LiveRangeData::MoveNodeUseSet& dst,
    bool setLoopProperty) {

    for (LiveRangeData::MoveNodeUseSet::const_iterator i =
             src.begin(); i != src.end(); i++) {
        const MoveNodeUse& mnu = *i;
        if (setLoopProperty || mnu.loop()) {
            dst.insert(MoveNodeUse(mnu, MoveNodeUse::LOOP));
        } else {
            dst.insert(MoveNodeUse(mnu, MoveNodeUse::INTRA_BB));
        }
    }
}

/**
 * Returns the set of registers that are alive at the given cycle.
 */
std::set<TCEString> 
LiveRangeData::registersAlive(
    int cycle, int delaySlots, DataDependenceGraph& ddg) {

    std::set<TCEString> aliveRegs;

    // Part 1: Live ranges incoming to this BB.
    // handles both incoming and overgoing live ranges
    for (MoveNodeUseMapSet::iterator rdrIter = regDefReaches_.begin();
         rdrIter != regDefReaches_.end(); rdrIter++) {
        const TCEString& reg = rdrIter->first;

        // if use after and not killed here, alive for the whole BB.
        if (registersUsedAfter_.find(reg) != registersUsedAfter_.end()) {
            // nothing in this BB overwrites this.
            if (regKills_.find(reg) == regKills_.end()) {
                aliveRegs.insert(reg);
            }
        }

        // check for first uses in this BB. If before a read, is used.
        MoveNodeUseSet& firstUses = regFirstUses_[reg];
        for (MoveNodeUseSet::iterator iter = firstUses.begin();
             iter != firstUses.end(); iter++) {
            const MoveNode& mn = *(iter->mn());
            if (mn.isScheduled()) {
                int mnCycle = mn.cycle();
                if (iter->pseudo()) {
                    mnCycle += delaySlots;
                }
                if (cycle <= mnCycle) {
                    aliveRegs.insert(reg);
                }
            } else { // unscheduled.. later?
                aliveRegs.insert(reg);
            }
        }
    }

    // Part 2: check deps going out from this.
    for (std::set<TCEString>::iterator ruaIter = registersUsedAfter_.begin();
         ruaIter != registersUsedAfter_.end(); ruaIter++) {
        const TCEString& reg = *ruaIter;

        // check against last defines.
        MoveNodeUseSet& lastDefs = regDefines_[reg];
        for (MoveNodeUseSet::iterator iter = lastDefs.begin();
             iter != lastDefs.end(); iter++) {
            const MoveNode& mn = *iter->mn();
            if (mn.isScheduled()) {
                int mnCycle = mn.cycle();
                if (iter->pseudo()) {
                    mnCycle += delaySlots;
                }
                if (cycle >= mnCycle) {
                    aliveRegs.insert(reg);
                }
            }
            // if not scheduld, is at end?
        }
    }

    // part 3: Check edges inside this BB. done from DDG.

    // TODO: psedo-dep delay slots.
    for (int i = 0; i < ddg.nodeCount(); i++) {
        MoveNode& node = ddg.node(i);
        // only check writes that are scheduled.
        if (!node.isScheduled() || node.cycle() > cycle) {
            continue; // write after the given cycle
        }

        // Find all edges out from this
        DataDependenceGraph::EdgeSet edges = ddg.outEdges(node);
        for (DataDependenceGraph::EdgeSet::iterator eIter = 
                 edges.begin(); eIter != edges.end(); eIter++) {
            DataDependenceEdge& edge = **eIter;

            // we are only interested in register RAWs.
            if ((edge.edgeReason() == DataDependenceEdge::EDGE_REGISTER || 
                 edge.edgeReason() == DataDependenceEdge::EDGE_RA) &&
                (edge.dependenceType() == DataDependenceEdge::DEP_RAW)) {
                const TCEString& reg = edge.data();
                MoveNode &headNode = ddg.headNode(edge);
                // if tail pseudo, delay by delaySlots amount
                if (edge.tailPseudo()) {
                    if (node.cycle() + delaySlots > cycle) {
                        continue;
                    }
                }
                // if pseudo, delay mncycle by delayslot amount
                int delay = edge.headPseudo() ? delaySlots : 0;
                // is the read after this
                if (headNode.isScheduled() && 
                    headNode.cycle()+delay >= cycle) {
                    aliveRegs.insert(reg);
                }
            }
        }
    }
    return aliveRegs;
}
