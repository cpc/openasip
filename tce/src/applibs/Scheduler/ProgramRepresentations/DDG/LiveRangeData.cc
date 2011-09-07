/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
        dst.insert(MoveNodeUse(mnu, setLoopProperty));
    }
}
