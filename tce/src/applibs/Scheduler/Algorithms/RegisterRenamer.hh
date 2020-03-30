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
 * @file RegisterRenamer.hh
 *
 * Declaration of RegisterRenamer class.
 *
 * @author Heikki Kultala 2009-2011 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_REGISTER_RENAMER_HH
#define TTA_REGISTER_RENAMER_HH

#include <climits>

#include "TCEString.hh"
#include <set>
#include "MachinePart.hh"

namespace TTAMachine {
    class Machine;
    class RegisterFile;
}

namespace TTAProgram {
    class TerminalRegister;
    class BasicBlock;
}

struct LiveRange;
class DataDependenceGraph;
class MoveNode;
class MoveNodeSelector;

class RegisterRenamer {
public:
    typedef std::set<const TTAMachine::RegisterFile*,
                     TTAMachine::MachinePart::Comparator> RegisterFileSet;

    RegisterRenamer(
        const TTAMachine::Machine& machine, TTAProgram::BasicBlock& bb);
    unsigned int freeGPRCount() const { return freeGPRs_.size(); }

    void initialize(DataDependenceGraph& ddg);

    bool renameDestinationRegister(
        MoveNode& node, bool loopScheduling, 
        bool allowSameRf, bool differentRfOnlyDirectlyReachable,
        int earliestCycle=-1);

    bool renameSourceRegister(
        MoveNode& node, bool loopScheduling, 
        bool allowSameRf, bool differentRfOnlyDirectlyReachable,
        int latestCycle = INT_MAX);
    
    void setSelector(MoveNodeSelector* selector);

    //    typedef std::pair<DataDependenceGraph::NodeSet, 
    //DataDependenceGraph::NodeSet> LiveRange;

    bool renameLiveRange(
        LiveRange& liveRange, const TCEString& newReg, bool usedBefore,
        bool usedAfter, bool loopScheduling);

    std::set<TCEString> findPartiallyUsedRegistersInRFAfterCycle(
        const RegisterFileSet& rfs, int latestCycle) const;

    std::set<TCEString> findFreeRegistersInRF(
        const RegisterFileSet& rfs) const;

    RegisterFileSet findConnectedRFs(LiveRange& lr, bool allowLimm);

    TTAProgram::BasicBlock& bb() { return bb_; }

    void renamedToRegister(const TCEString& newReg);
    void revertedRenameToRegister(const TCEString& reg);
private:

    std::set<TCEString> registersOfRFs(const RegisterFileSet& rfs) const;

    void initializeFreeRegisters();

    std::set<TCEString> findPartiallyUsedRegistersInRFBeforeCycle(
        std::set<const TTAMachine::RegisterFile*,
        TTAMachine::MachinePart::Comparator>& rfs,
        int earliestCycle) const;
    
    std::set<TCEString> findPartiallyUsedRegistersBeforeCycle(
        int bitWidth, int earliestCycle) const;

    std::set<TCEString> findPartiallyUsedRegistersAfterCycle(
        int bitWidth, int latestCycle) const;
    
    std::set<TCEString> findFreeRegisters(int bitWidth) const;

    void updateAntiEdgesFromLRTo(
        LiveRange& liveRange, 
        const TCEString& newReg, 
        TTAProgram::BasicBlock& bb, 
        int loopDepth) const;

    void initialize();
    std::set<TCEString> allNormalGPRs_;
    std::set<TCEString> freeGPRs_;

    // already usd by the reg renamer, but can be reused of liveranges
    // cannot overlap?
    std::set<TCEString> usedGPRs_;

    // used partially by original code; 
    // used at beginning of bb, free at end.
    std::set<TCEString> onlyBeginPartiallyUsedRegs_;
    // used at end of bb, free at begin.
    std::set<TCEString> onlyEndPartiallyUsedRegs_;
    std::set<TCEString> onlyMidPartiallyUsedRegs_;

    static std::map<const TTAMachine::Machine*, 
                    std::vector <TTAMachine::RegisterFile*> >tempRegFileCache_;
    std::vector <TTAMachine::RegisterFile*> tempRegFiles_;

    const TTAMachine::Machine& machine_;
    TTAProgram::BasicBlock& bb_;
    DataDependenceGraph* ddg_;

    MoveNodeSelector* selector_;
};

#endif
