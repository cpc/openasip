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
 * @file LiveRangeData.hh
 *
 * Declaration of LiveRangeData class.
 *
 * Contains live range-related data for one basic block.
 * This is generated and used by ddg builder.
 * This may also be used by later parts of the scheduler.
 *
 * @author Heikki Kultala 2011 (hkultala@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_LIVERANGE_DATA_HH
#define TTA_LIVERANGE_DATA_HH

#include "MoveNodeUse.hh"
#include "TCEString.hh"
#include <set>
#include <map>

struct LiveRangeData {
    std::set<TCEString> registersAlive(
        int cycle, int delaySlots, class DataDependenceGraph& ddg);

    typedef std::set<MoveNodeUse > MoveNodeUseSet;
    typedef std::map<TCEString, MoveNodeUseSet > MoveNodeUseMapSet;
    typedef std::map<TCEString, MoveNodeUse > MoveNodeUseMap;
    typedef std::map<TCEString, std::pair<MoveNodeUse, MoveNodeUse> > 
    MoveNodeUseMapPair;

    typedef std::pair<TCEString, MoveNodeUseSet > MoveNodeUseSetPair;
    typedef std::pair<TCEString, MoveNodeUse> MoveNodeUsePair;

    const std::set<TCEString>& usedAfter() {
        return registersUsedAfter_;
    }

    // merges liverangedata of successor into this.
    void merge(LiveRangeData& succ);

    static bool appendUseMapSets(
        const MoveNodeUseMapSet& srcMap, 
        MoveNodeUseMapSet& dstMap,
        bool addLoopProperty);

    static void appendMoveNodeUse(
        const MoveNodeUseSet& src, 
        MoveNodeUseSet& dst,
        bool setLoopProperty);

    // dependencies out from this BB
    MoveNodeUseMapSet regDefines_;
    MoveNodeUseMapSet regLastUses_;
    MoveNodeUseMapPair regLastKills_;
    
    std::map<TCEString, std::pair<MoveNodeUse, bool> >potentialRegKills_;

    // dependencies in to this BB
    MoveNodeUseMapPair regKills_;
    MoveNodeUseMapSet regFirstUses_;
    MoveNodeUseMapSet regFirstDefines_;
    
    // dependencies from previous BBs.
    MoveNodeUseMapSet regDefReaches_;
    MoveNodeUseMapSet regUseReaches_;
    
    // all alive after this BB.
    MoveNodeUseMapSet regDefAfter_;
    MoveNodeUseMapSet regUseAfter_;
    
    // dependencies out from this BB
    MoveNodeUseMapSet memDefines_;
    MoveNodeUseMapSet memLastUses_;
    MoveNodeUseMap memLastKill_;
    
    // dependencies into this one
    MoveNodeUseMap memKills_;
    MoveNodeUseMapSet memFirstUses_;
    MoveNodeUseMapSet memFirstDefines_;
    
    // deps from previous BBs
    MoveNodeUseMapSet memDefReaches_;
    MoveNodeUseMapSet memUseReaches_;
    
    // all alive after this
    MoveNodeUseMapSet memDefAfter_;
    MoveNodeUseMapSet memUseAfter_;
    
    // fu state deps
    MoveNodeUseSet fuDepReaches_;
    MoveNodeUseSet fuDeps_;
    MoveNodeUseSet fuDepAfter_;
    
    // live range information
    std::set<TCEString> registersUsedAfter_;
    std::set<TCEString> registersUsedInOrAfter_;
};

#endif
