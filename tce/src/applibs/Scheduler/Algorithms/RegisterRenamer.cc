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
 * @file RegisterRenamer.cc
 *
 * Definition of RegisterRenamer class.
 *
 * @todo rename the file to match the class name
 *
 * @author Heikki Kultala 2009-2011 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "MapTools.hh"

#include "RegisterRenamer.hh"

#include "RegisterFile.hh"
#include "Machine.hh"
#include "MachineConnectivityCheck.hh"
#include "BasicBlock.hh"
#include "DataDependenceGraph.hh"
#include "MoveNode.hh"
#include "Terminal.hh"
#include "DisassemblyRegister.hh"
#include "LiveRangeData.hh"
#include "TerminalRegister.hh"
#include "MoveNodeSelector.hh"
#include "Move.hh"
#include "LiveRange.hh"

#include "tce_config.h"

/**
 * Constructor.
 *
 * @param machine machine for which we are scheudling
 
RegisterRenamer::RegisterRenamer(const TTAMachine::Machine& machine) :
    machine_(machine) {
    initialize();
}
*/

/**
 * Constructor.
*
 * @param machine machine for which we are scheudling
 */
RegisterRenamer::RegisterRenamer(
    const TTAMachine::Machine& machine, TTAProgram::BasicBlock& bb) :
    machine_(machine), bb_(bb), ddg_(NULL){
    initialize();
}

void 
RegisterRenamer::initialize(DataDependenceGraph& ddg) {
    ddg_ = &ddg;
    initializeFreeRegisters();
}

void
RegisterRenamer::initialize() {
    TTAMachine::Machine::RegisterFileNavigator regNav =
        machine_.registerFileNavigator();

    std::map<const TTAMachine::Machine*, 
        std::vector <TTAMachine::RegisterFile*> >::iterator trCacheIter =
        tempRegFileCache_.find(&machine_);

    if (trCacheIter == tempRegFileCache_.end()) {
        tempRegFiles_ = MachineConnectivityCheck::tempRegisterFiles(machine_);
        tempRegFileCache_[&machine_] = 
            tempRegFiles_;
    } else {
        tempRegFiles_ = trCacheIter->second;
    }

    for (int i = 0; i < regNav.count(); i++) {
        bool isTempRf = false;
        TTAMachine::RegisterFile* rf = regNav.item(i);
        for (unsigned int j = 0; j < tempRegFiles_.size(); j++) {
            if (tempRegFiles_[j] == rf) {
                isTempRf = true;
            }
        }
        unsigned int regCount = isTempRf ? rf->size()-1 : rf->size();
        for (unsigned int j = 0; j < regCount; j++ ) {
            allNormalGPRs_.insert(DisassemblyRegister::registerName(*rf, j));
        }
    }
}

void
RegisterRenamer::initializeFreeRegisters() {
    
    assert(ddg_ != NULL);
    freeGPRs_ = allNormalGPRs_;
    onlyBeginPartiallyUsedRegs_.clear();
    onlyEndPartiallyUsedRegs_.clear();
    onlyMidPartiallyUsedRegs_.clear();

    std::map<TCEString,int> lastUses;

    // find regs inside this BB.
    for (int i = 0; i < ddg_->nodeCount(); i++) {
        MoveNode& node = ddg_->node(i);

        // any write to a reg means it's not alive.
        TTAProgram::Terminal& dest = node.move().destination();
        if (dest.isGPR()) {
            TCEString regName = DisassemblyRegister::registerName(
                dest.registerFile(), dest.index());
            onlyMidPartiallyUsedRegs_.insert(regName);
        }
        TTAProgram::Terminal& src = node.move().source();
        if (src.isGPR()) {
            TCEString regName = DisassemblyRegister::registerName(
                src.registerFile(), src.index());
            onlyMidPartiallyUsedRegs_.insert(regName);
        }
    }

    // then loop for deps outside or inside this bb.
    for (std::set<TCEString>::iterator allIter = freeGPRs_.begin(); 
         allIter != freeGPRs_.end();) {
        bool aliveOver = false;
        bool aliveAtBeginning = false;
        bool aliveAtEnd = false;
        bool aliveAtMid = false;
        // defined before and used here or after?
        if (bb_.liveRangeData_->regDefReaches_.find(*allIter) != 
            bb_.liveRangeData_->regDefReaches_.end()) {
            if (bb_.liveRangeData_->registersUsedAfter_.find(*allIter) 
                != bb_.liveRangeData_->registersUsedAfter_.end()) {
                aliveOver = true;
            }
            if (bb_.liveRangeData_->regFirstUses_.find(*allIter) != 
                bb_.liveRangeData_->regFirstUses_.end()) {
                aliveAtBeginning = true;
                LiveRangeData::MoveNodeUseMapSet::iterator i =
                    bb_.liveRangeData_->regLastUses_.find(*allIter);
                if (i != bb_.liveRangeData_->regLastUses_.end()) {
                    LiveRangeData::MoveNodeUseSet& lastUses = i->second;
                    for (LiveRangeData::MoveNodeUseSet::iterator j = 
                             lastUses.begin(); j != lastUses.end(); j++) {
                        if (j->pseudo()) {
                            aliveOver = true;
                        }
                    }
                }
            }
            aliveAtBeginning = true;
        }
        // used after this?
        if (bb_.liveRangeData_->registersUsedAfter_.find(*allIter) != 
            bb_.liveRangeData_->registersUsedAfter_.end()) {
            // defined here?
            if (bb_.liveRangeData_->regDefines_.find(*allIter) != 
                bb_.liveRangeData_->regDefines_.end()) {
                aliveAtEnd = true;
            }            
        }
        
        if (aliveAtEnd && aliveAtBeginning) {
            aliveOver = true;
        }

        // TODO: why was this here?
        if (onlyMidPartiallyUsedRegs_.find(*allIter) != 
            onlyMidPartiallyUsedRegs_.end()) {
            aliveAtMid = true;
        }

        if (aliveOver) {
            // can not be used for renaming.
            onlyMidPartiallyUsedRegs_.erase(*allIter);
            freeGPRs_.erase(allIter++);
        } else {
            if (aliveAtBeginning) {
                onlyBeginPartiallyUsedRegs_.insert(*allIter);

                onlyMidPartiallyUsedRegs_.erase(*allIter);
                freeGPRs_.erase(allIter++);
            } else {
                if (aliveAtEnd) {
                    onlyEndPartiallyUsedRegs_.insert(*allIter);

                    onlyMidPartiallyUsedRegs_.erase(*allIter);
                    freeGPRs_.erase(allIter++);
                } else { // only mid if has reads of writes?
                    if (aliveAtMid) {
                        freeGPRs_.erase(allIter++);
                    } else {
                        allIter++;
                    }
                }
            }
        }
    }
}

std::set<TCEString> 
RegisterRenamer::findFreeRegistersInRF(
    const RegisterRenamer::RegisterFileSet& rfs) const {

    std::set<TCEString> allowedGprs = registersOfRFs(rfs);
    std::set<TCEString> regs;
    SetTools::intersection(allowedGprs, freeGPRs_, regs);
    return regs;
}

std::set<TCEString> 
RegisterRenamer::registersOfRFs(
    const RegisterFileSet & rfs) const {

    std::set<TCEString> gprs;
    for (std::set<const TTAMachine::RegisterFile*,
             TTAMachine::MachinePart::Comparator>::iterator i = rfs.begin();
         i != rfs.end(); i++) {
        bool isTempRF = false;
        const TTAMachine::RegisterFile& rf = **i;
        for (unsigned int j = 0; j < tempRegFiles_.size(); j++) {
            if (tempRegFiles_[j] == &rf) {
                isTempRF = true;
            }
        }

        for (int j = 0; j < (isTempRF ? rf.size()-1 : rf.size()); j++ ) {
            gprs.insert(DisassemblyRegister::registerName(rf, j));
        }
    }
    return gprs;
}
/** 
 * Finds registers which are used but only before given earliestCycle.
 */ 
std::set<TCEString> 
RegisterRenamer::findPartiallyUsedRegistersInRFBeforeCycle(
    std::set<const TTAMachine::RegisterFile*,
    TTAMachine::MachinePart::Comparator>& rfs, 
    int earliestCycle) const {

    std::set<TCEString> availableRegs;
    // nothing can be scheduled earlier than cycle 0.
    // in that case we have empty set, no need to check.
    if (earliestCycle < 1) {
        return availableRegs;
    }

    std::set<TCEString> allowedGprs = registersOfRFs(rfs);
    std::set<TCEString> regs = usedGPRs_;
    AssocTools::append(onlyBeginPartiallyUsedRegs_, regs);
    AssocTools::append(onlyMidPartiallyUsedRegs_, regs);
    std::set<TCEString> regs2;
    SetTools::intersection(allowedGprs, regs, regs2);

    
    // find from used gprs.
    // todo: this is too conservative? leaves one cycle netween war?
    for (std::set<TCEString>::iterator i = regs2.begin(); 
         i != regs2.end(); i++) {
        TCEString rfName = i->substr(0, i->find('.'));
        TTAMachine::RegisterFile* rf = 
            machine_.registerFileNavigator().item(rfName);

        unsigned int regIndex = atoi(i->substr(i->find('.')+1).c_str());
        if (ddg_->lastRegisterCycle(*rf, regIndex) < earliestCycle) {
            availableRegs.insert(*i);
        }
    }
    return availableRegs;
}

/** 
 * Finds registers which are used but only after given earliestCycle.
 */ 
std::set<TCEString> 
RegisterRenamer::findPartiallyUsedRegistersInRFAfterCycle(
    const RegisterRenamer::RegisterFileSet& rfs, int latestCycle) const {

    std::set<TCEString> availableRegs;
    // nothing can be scheduled earlier than cycle 0.
    // in that case we have empty set, no need to check.

    std::set<TCEString> allowedGprs = registersOfRFs(rfs);
    std::set<TCEString> regs = usedGPRs_;
    AssocTools::append(onlyEndPartiallyUsedRegs_, regs);
    AssocTools::append(onlyMidPartiallyUsedRegs_, regs);
    std::set<TCEString> regs2;
    SetTools::intersection(allowedGprs, regs, regs2);
    
    // find from used gprs.
    // todo: this is too conservative? leaves one cycle netween war?
    for (std::set<TCEString>::iterator i = regs2.begin(); 
         i != regs2.end(); i++) {
        TCEString rfName = i->substr(0, i->find('.'));
        TTAMachine::RegisterFile* rf = 
            machine_.registerFileNavigator().item(rfName);

        unsigned int regIndex = atoi(i->substr(i->find('.')+1).c_str());
        if (ddg_->firstRegisterCycle(*rf, regIndex) > latestCycle) {
            availableRegs.insert(*i);
        }
    }
    return availableRegs;
}

std::set<TCEString> 
RegisterRenamer::findPartiallyUsedRegistersBeforeCycle(
    int bitWidth, int earliestCycle) const {
    std::set<TCEString> availableRegs;
    // nothing can be scheduled earlier than cycle 0.
    // in that case we have empty set, no need to check.
    if (earliestCycle < 1) {
        return availableRegs;
    }

    std::set<TCEString> regs = onlyMidPartiallyUsedRegs_;
    AssocTools::append(onlyBeginPartiallyUsedRegs_, regs);
    AssocTools::append(usedGPRs_, regs);

    for (std::set<TCEString>::iterator i = regs.begin(); 
         i != regs.end(); i++) {

        TCEString rfName = i->substr(0, i->find('.'));
        TTAMachine::RegisterFile* rf = 
            machine_.registerFileNavigator().item(rfName);
        unsigned int regIndex = atoi(i->substr(i->find('.')+1).c_str());
        if (ddg_->lastRegisterCycle(*rf, regIndex) < earliestCycle && 
            rf->width() == bitWidth) {
            availableRegs.insert(*i);
        }
    }
    return availableRegs;
}

std::set<TCEString> 
RegisterRenamer::findPartiallyUsedRegistersAfterCycle(
    int bitWidth, int latestCycle) const {
    std::set<TCEString> availableRegs;

    std::set<TCEString> regs = onlyMidPartiallyUsedRegs_;
    AssocTools::append(onlyEndPartiallyUsedRegs_, regs);
    AssocTools::append(usedGPRs_, regs);

    for (std::set<TCEString>::iterator i = regs.begin(); 
         i != regs.end(); i++) {

        TCEString rfName = i->substr(0, i->find('.'));
        TTAMachine::RegisterFile* rf = 
            machine_.registerFileNavigator().item(rfName);
        unsigned int regIndex = atoi(i->substr(i->find('.')+1).c_str());
        if (ddg_->firstRegisterCycle(*rf, regIndex) > latestCycle && 
            rf->width() == bitWidth) {
            availableRegs.insert(*i);
        }
    }
    return availableRegs;
}

std::set<TCEString> 
RegisterRenamer::findFreeRegisters(
    int bitWidth) const {

    std::set<TCEString> availableRegs;

    for (std::set<TCEString>::iterator i = freeGPRs_.begin(); 
         i != freeGPRs_.end(); i++) {

        TCEString rfName = i->substr(0, i->find('.'));
        TTAMachine::RegisterFile* rf = 
            machine_.registerFileNavigator().item(rfName);
        if (rf->width() == bitWidth) {
            availableRegs.insert(*i);
        }
    }
    return availableRegs;
}


/** 
 * Renames destination register of a move (from the move itself and 
 * from all other moves in same liverange)
 */
bool 
RegisterRenamer::renameDestinationRegister(
    MoveNode& node, bool loopScheduling, 
    bool allowSameRf, bool differentRfOnlyDirectlyReachable, 
    int earliestCycle) {

    if (!node.isMove() || !node.move().destination().isGPR()) {
        return false;
    }
    const TTAMachine::RegisterFile& rf = 
        node.move().destination().registerFile();

    // don't allow using same reg multiple times if loop scheduling.
    // unscheudling would cause problems, missing war edges.
    if (loopScheduling) {
        earliestCycle = -1;
    }
    // first find used fully scheduled ones!
    bool reused = true;
    
    std::unique_ptr<LiveRange> liveRange(
        ddg_->findLiveRange(node, true, false));

    if (liveRange->writes.empty()) {
        return false;
    }
    std::set<TCEString> availableRegisters;

    if (!liveRange->noneScheduled()) {
        if (!allowSameRf) {
            return false;
        }
        std::set<const TTAMachine::RegisterFile*,
            TTAMachine::MachinePart::Comparator> rfs;
        rfs.insert(&rf);
        earliestCycle = std::min(earliestCycle, liveRange->firstCycle());
        availableRegisters = 
	    findPartiallyUsedRegistersInRFBeforeCycle(rfs, earliestCycle);
    } else { // none scheduled.
        if (tempRegFiles_.empty()) {
            availableRegisters = 
                findPartiallyUsedRegistersBeforeCycle(rf.width(), earliestCycle);
        } else {
	    if (!differentRfOnlyDirectlyReachable) 
	    {
		// only connected RFs
		std::set < const TTAMachine::RegisterFile*, 
		    TTAMachine::MachinePart::Comparator >
		rfs = findConnectedRFs(*liveRange, false);
		availableRegisters = 
		    findPartiallyUsedRegistersInRFBeforeCycle(
			rfs, earliestCycle);
		if (availableRegisters.empty()) {
		    // allow usasge of limm.
		    rfs = findConnectedRFs(*liveRange, true);
		    availableRegisters = 
			findPartiallyUsedRegistersInRFBeforeCycle(
			    rfs, earliestCycle);
		}
	   }
       }
    }

    if (availableRegisters.empty()) {
        reused = false;

        if (!liveRange->noneScheduled()) {
            std::set<const TTAMachine::RegisterFile*,
                TTAMachine::MachinePart::Comparator> rfs;
            rfs.insert(&rf);
            availableRegisters = 
                findFreeRegistersInRF(rfs);
        } else {
            if (tempRegFiles_.empty()) {
                availableRegisters = 
                    findFreeRegisters(rf.width());
            } else {
		if (!differentRfOnlyDirectlyReachable) {
		    // only connected RFs
		    std::set<const TTAMachine::RegisterFile*,
			TTAMachine::MachinePart::Comparator> 
		    rfs = findConnectedRFs(*liveRange, false);
		    availableRegisters = 
			findFreeRegistersInRF(rfs);
		    if (availableRegisters.empty()) {
			// allow usage of LIMM
			rfs = findConnectedRFs(*liveRange, true);
			availableRegisters = 
			    findFreeRegistersInRF(rfs);
		    }
	        }
	    }
        }
        if (availableRegisters.empty()) {
            return false;
        }
    }
    
    // then actually do it.
    return renameLiveRange(
        *liveRange, *availableRegisters.begin(), 
        reused, false, loopScheduling);
}

/** 
 * Renames source register of a move (from the move itself and 
 * from all other moves in same liverange)
 */
bool 
RegisterRenamer::renameSourceRegister(
    MoveNode& node, bool loopScheduling, 
    bool allowSameRf, bool differentRfOnlyDirectlyReachable, int latestCycle) {

    if (!node.isMove() || !node.move().source().isGPR()) {
        return false;
    }
    const TTAMachine::RegisterFile& rf = 
        node.move().source().registerFile();
    
    if (loopScheduling) {
        latestCycle = -1;
    }

    // first find used fully scheduled ones!
    bool reused = true;

    std::unique_ptr<LiveRange> liveRange(
        ddg_->findLiveRange(node, false, false));

    if (liveRange->writes.empty()) {
        return false;
    }
    std::set<TCEString> availableRegisters;

    if (!liveRange->noneScheduled()) {
        if (!allowSameRf) {
            return false;
        }
        std::set<const TTAMachine::RegisterFile*,
            TTAMachine::MachinePart::Comparator> rfs;
        rfs.insert(&rf);
        latestCycle = std::max(latestCycle, liveRange->lastCycle());

        availableRegisters = 
        findPartiallyUsedRegistersInRFAfterCycle(rfs, latestCycle);
    } else { 
        if (tempRegFiles_.empty()) {
            availableRegisters = 
                findPartiallyUsedRegistersAfterCycle(rf.width(), latestCycle);
        } else {
            if (!differentRfOnlyDirectlyReachable) {
                // only connected RFs
                std::set<const TTAMachine::RegisterFile*, 
                    TTAMachine::MachinePart::Comparator> 
                rfs = findConnectedRFs(*liveRange, false);
                availableRegisters = 
                    findPartiallyUsedRegistersInRFAfterCycle(rfs, latestCycle);
                if (availableRegisters.empty()) {
                    rfs = findConnectedRFs(*liveRange, true);
                    availableRegisters = 
                        findPartiallyUsedRegistersInRFAfterCycle(
                            rfs, latestCycle);
                }
            }
        }
    }        

    if (availableRegisters.empty()) {
        reused = false;

        if (!liveRange->noneScheduled()) {
            std::set<const TTAMachine::RegisterFile*,
                TTAMachine::MachinePart::Comparator> rfs;
            rfs.insert(&rf);
            availableRegisters = 
                findFreeRegistersInRF(rfs);
        } else {
            if (tempRegFiles_.empty()) {
                availableRegisters = 
                    findFreeRegisters(rf.width());
            } else { 
                if (!differentRfOnlyDirectlyReachable) {
                    // only connected RFs
                    std::set<const TTAMachine::RegisterFile*,
                        TTAMachine::MachinePart::Comparator> 
                    rfs = findConnectedRFs(*liveRange, false);
                    availableRegisters = findFreeRegistersInRF(rfs);
                    if (availableRegisters.empty()) {
                        rfs = findConnectedRFs(*liveRange, true);
                        availableRegisters = findFreeRegistersInRF(rfs);
                    }
                }
            }
        }
        if (availableRegisters.empty()) {
            return false;
        }
    }
    
    return
        renameLiveRange(
            *liveRange, *availableRegisters.begin(), false, reused, 
            loopScheduling);
}

bool
RegisterRenamer::renameLiveRange(
    LiveRange& liveRange, const TCEString& newReg, bool usedBefore,
    bool usedAfter, bool loopScheduling) {

    // > 0 breaks at least denbench
    if (!(liveRange.writes.size() == 1  && liveRange.reads.size() > 0)) {
        return false;
    } 

    assert(newReg.length() > 2);
    TCEString rfName = newReg.substr(0, newReg.find('.'));
    TTAMachine::RegisterFile* rf = 
        machine_.registerFileNavigator().item(rfName);
    
    int newRegIndex = 
        atoi(newReg.substr(newReg.find('.')+1).c_str());

    if (usedBefore) {
        // create antidependencies from the previous use of this temp reg.

        //todo: if in a loop, create antidependencies to first ones in the BB.
        DataDependenceGraph::NodeSet lastReads = 
            ddg_->lastScheduledRegisterReads(
                *rf, newRegIndex);
        
        DataDependenceGraph::NodeSet lastWrites = 
            ddg_->lastScheduledRegisterWrites(
                *rf, newRegIndex);

        DataDependenceGraph::NodeSet lastGuards = 
            ddg_->lastScheduledRegisterGuardReads(
                *rf, newRegIndex);

        // create the deps.
        for (DataDependenceGraph::NodeSet::iterator i = 
                 liveRange.writes.begin(); i != liveRange.writes.end(); i++) {

            // create WAR's from previous reads
            for (DataDependenceGraph::NodeSet::iterator 
                     j = lastReads.begin(); j != lastReads.end(); j++) {

                DataDependenceEdge* edge = new DataDependenceEdge(
                    DataDependenceEdge::EDGE_REGISTER,
                    DataDependenceEdge::DEP_WAR, newReg);

                ddg_->connectNodes(**j, **i, *edge);
            }

            // create WAR's from previous guard uses
            for (DataDependenceGraph::NodeSet::iterator 
                     j = lastGuards.begin(); j != lastGuards.end(); j++) {

                DataDependenceEdge* edge = new DataDependenceEdge(
                    DataDependenceEdge::EDGE_REGISTER,
                    DataDependenceEdge::DEP_WAR, newReg, true);

                ddg_->connectNodes(**j, **i, *edge);
            }

            // create WAW's from previous writes.
            for (DataDependenceGraph::NodeSet::iterator 
                     j = lastWrites.begin(); j != lastWrites.end(); j++) {

                DataDependenceEdge* edge = new DataDependenceEdge(
                    DataDependenceEdge::EDGE_REGISTER,
                    DataDependenceEdge::DEP_WAW, newReg);

                ddg_->connectNodes(**j, **i, *edge);
            }
        }
    } else {
        // this is not used before.

        // update bookkeeping about first use of this reg
        if (!usedAfter)
            assert(bb_.liveRangeData_->regFirstUses_[newReg].empty());

        // killing write.
        if (liveRange.writes.size() == 1 && 
            (*liveRange.writes.begin())->move().isUnconditional()) {
            bb_.liveRangeData_->regKills_[newReg].first = 
                MoveNodeUse(**liveRange.writes.begin());
            bb_.liveRangeData_->regFirstDefines_[newReg].clear();
            bb_.liveRangeData_->regFirstUses_[newReg].clear();            
        }

        // for writing.
        for (DataDependenceGraph::NodeSet::iterator i = 
                 liveRange.writes.begin(); i != liveRange.writes.end(); i++) {

            MoveNodeUse mnd(**i);
            bb_.liveRangeData_->regFirstDefines_[newReg].insert(mnd);
            // TODO: only if intra-bb-antideps enabled?
            static_cast<DataDependenceGraph*>(ddg_->rootGraph())->
                updateRegWrite(mnd, newReg, bb_);
        }

        // for reading.
        for (DataDependenceGraph::NodeSet::iterator i = 
                 liveRange.reads.begin(); i != liveRange.reads.end(); i++) {

            MoveNodeUse mnd(**i);
            bb_.liveRangeData_->regFirstUses_[newReg].insert(mnd);
            // no need to create raw deps here
        }
    }

    if (usedAfter) {
        
        DataDependenceGraph::NodeSet firstWrites = 
            ddg_->firstScheduledRegisterWrites(
                *rf, newRegIndex);

        // create the antidep deps.

        for (DataDependenceGraph::NodeSet::iterator 
                 j = firstWrites.begin(); j != firstWrites.end(); j++) {
            
            // WaW's
            for (DataDependenceGraph::NodeSet::iterator i = 
                     liveRange.writes.begin(); i != liveRange.writes.end(); 
                 i++) {
                
                DataDependenceEdge* edge = new DataDependenceEdge(
                    DataDependenceEdge::EDGE_REGISTER,
                    DataDependenceEdge::DEP_WAW, newReg);
                
                ddg_->connectNodes(**i,**j, *edge);
            }
        
            // WaR's
            for (DataDependenceGraph::NodeSet::iterator i = 
                     liveRange.reads.begin(); i != liveRange.reads.end(); 
                 i++) {
                DataDependenceEdge* edge = new DataDependenceEdge(
                    DataDependenceEdge::EDGE_REGISTER,
                    DataDependenceEdge::DEP_WAR, newReg);
                
                ddg_->connectNodes(**i,**j, *edge);
            }
        }

    } else {

        // killing write.
        if (liveRange.writes.size() == 1 && 
            (*liveRange.writes.begin())->move().isUnconditional()) {
            bb_.liveRangeData_->regLastKills_[newReg].first = 
                MoveNodeUse(**liveRange.writes.begin());
            bb_.liveRangeData_->regDefines_[newReg].clear();
        }

        // for writing.
        for (DataDependenceGraph::NodeSet::iterator i = 
                 liveRange.writes.begin(); i != liveRange.writes.end(); i++) {

            MoveNodeUse mnd(**i);
            bb_.liveRangeData_->regDefines_[newReg].insert(mnd);
        }

        // for reading.
        for (DataDependenceGraph::NodeSet::iterator i = 
                 liveRange.reads.begin(); i != liveRange.reads.end(); i++) {

            MoveNodeUse mnd(**i);
            bb_.liveRangeData_->regLastUses_[newReg].insert(mnd);
        }

        // need to create backedges to first if we are loop scheduling.
        if (loopScheduling) {
            updateAntiEdgesFromLRTo(liveRange, newReg, bb_, 1);
        }
    }

    // first update the movenodes.

    // for writes.
    for (DataDependenceGraph::NodeSet::iterator i = liveRange.writes.begin();
         i != liveRange.writes.end(); i++) {
        TTAProgram::Move& move = (**i).move();
        const TTAMachine::Port& oldPort = move.destination().port();
        if (oldPort.parentUnit() == rf) {
            move.setDestination(new TTAProgram::TerminalRegister(
                                    oldPort, newRegIndex));
        } else {
            move.setDestination(new TTAProgram::TerminalRegister(
                                    *rf->firstWritePort(), newRegIndex));
        }

    }

    // for reads.
    for (DataDependenceGraph::NodeSet::iterator i = liveRange.reads.begin();
         i != liveRange.reads.end(); i++) {
        TTAProgram::Move& move = (**i).move();
        const TTAMachine::Port& oldPort = move.source().port();
        if (oldPort.parentUnit() == rf) {
            move.setSource(new TTAProgram::TerminalRegister(
                           oldPort, newRegIndex));
        } else {
            move.setSource(new TTAProgram::TerminalRegister(
                               *rf->firstReadPort(), newRegIndex));
        }
    }

    // then update ddg and notify selector.

    // for writes.
    for (DataDependenceGraph::NodeSet::iterator i = liveRange.writes.begin();
         i != liveRange.writes.end(); i++) {

        DataDependenceGraph::NodeSet writeSuccessors =
            ddg_->successors(**i);

        DataDependenceGraph::NodeSet writePredecessors =
            ddg_->predecessors(**i);

        ddg_->destRenamed(**i);

        // notify successors of write to prevent orphan nodes.
        for (DataDependenceGraph::NodeSet::iterator iter =
                 writeSuccessors.begin();
             iter != writeSuccessors.end(); iter++) {
            selector_->mightBeReady(**iter);
        }

        // notify successors of write to prevent orphan nodes.
        for (DataDependenceGraph::NodeSet::iterator iter =
                 writePredecessors.begin();
             iter != writePredecessors.end(); iter++) {
            selector_->mightBeReady(**iter);
        }

    }

    // for reads
    for (DataDependenceGraph::NodeSet::iterator i = liveRange.reads.begin();
         i != liveRange.reads.end(); i++) {
        DataDependenceGraph::NodeSet successors =
            ddg_->successors(**i);

        ddg_->sourceRenamed(**i);

        DataDependenceGraph::NodeSet predecessors =
            ddg_->predecessors(**i);
        
        // notify successors to prevent orphan nodes.
        for (DataDependenceGraph::NodeSet::iterator iter =
                 successors.begin();
             iter != successors.end(); iter++) {
            selector_->mightBeReady(**iter);
        }

        // notify successors to prevent orphan nodes.
        for (DataDependenceGraph::NodeSet::iterator iter =
                 predecessors.begin();
             iter != predecessors.end(); iter++) {
            selector_->mightBeReady(**iter);
        }

    }

    renamedToRegister(newReg);
    return true;
}

void RegisterRenamer::renamedToRegister(const TCEString& newReg) {

    freeGPRs_.erase(newReg);

    onlyBeginPartiallyUsedRegs_.erase(newReg);
    onlyEndPartiallyUsedRegs_.erase(newReg);
    onlyMidPartiallyUsedRegs_.erase(newReg);

    usedGPRs_.insert(newReg);

}

void RegisterRenamer::revertedRenameToRegister(const TCEString& reg) {
    if (bb_.liveRangeData_->regFirstUses_[reg].empty() &&
        bb_.liveRangeData_->regLastUses_[reg].empty() &&
        bb_.liveRangeData_->regDefines_[reg].empty() &&
        bb_.liveRangeData_->regFirstDefines_[reg].empty()) {
        freeGPRs_.insert(reg);
    }
}

/**
 * Registers the selector being used to the bypasser.
 *
 * If the bypasser has been registered to the selector,
 * bypasses can notify the selector about dependence changes.
 * Currently it notifies the successors of a node being removed due
 * dead result elimination.
 *
 * @param selector selector which bypasser notifies on some dependence changes.
 */
void
RegisterRenamer::setSelector(MoveNodeSelector* selector) {
    selector_ = selector;
}

/**
 * Updates antidep edges from this liverange to first def of some other bb.
 *
 * @param liveRange liverange which is the origin of the deps
 * @param newReg name of the new register
 * @param bb destination BB where to draw the edges to
 * @loopDepth loop depth of added edges.
 */
void 
RegisterRenamer::updateAntiEdgesFromLRTo(
    LiveRange& liveRange, const TCEString& newReg, TTAProgram::BasicBlock& bb,
    int loopDepth) const {
    std::set<MoveNodeUse>& firstDefs = 
        bb.liveRangeData_->regFirstDefines_[newReg];
    
    for (std::set<MoveNodeUse>::iterator i = firstDefs.begin();
         i != firstDefs.end(); i++) {
        
        const MoveNodeUse& destination = *i;
        if (ddg_->hasNode(*destination.mn())) {
            
            //WaW's of writes.
            for (DataDependenceGraph::NodeSet::iterator j = 
                     liveRange.writes.begin(); 
                 j != liveRange.writes.end(); j++) {
                
                // create dependency edge
                DataDependenceEdge* dde =
                    new DataDependenceEdge(
                        DataDependenceEdge::EDGE_REGISTER,
                        DataDependenceEdge::DEP_WAW, newReg, 
                        false, false, false, destination.pseudo(), loopDepth);
                
                // and connect.
                ddg_->connectOrDeleteEdge(
                    **j, *destination.mn(), dde);
            }
            
            //War's of reads.
            for (DataDependenceGraph::NodeSet::iterator j = 
                     liveRange.reads.begin(); 
                 j != liveRange.reads.end(); j++) {
                
                // create dependency edge
                DataDependenceEdge* dde =
                    new DataDependenceEdge(
                        DataDependenceEdge::EDGE_REGISTER,
                        DataDependenceEdge::DEP_WAR, newReg, 
                        false, false, false, destination.pseudo(), 1);
                
                // and connect.
                ddg_->connectOrDeleteEdge(
                    **j, *destination.mn(), dde);
            }
        }
    }
}

RegisterRenamer::RegisterFileSet
RegisterRenamer::findConnectedRFs(LiveRange& lr, bool allowLimm) {

    assert(!lr.writes.empty());
    TTAMachine::RegisterFile* originalRF = 
        dynamic_cast<TTAMachine::RegisterFile*>(
            (*lr.writes.begin())->move().destination().port().parentUnit());
    assert(originalRF != 0);
    int bitwidth = originalRF->width();
    std::set<const TTAMachine::RegisterFile*, 
        TTAMachine::MachinePart::Comparator> rv;

    // TODO: loop over movenoeds or RF's? this routine could be made faster.
    TTAMachine::Machine::RegisterFileNavigator rfNav = 
        machine_.registerFileNavigator();
    for (int i = 0; i < rfNav.count(); i++) {
        TTAMachine::RegisterFile* rf = rfNav.item(i);
        if (rf->width() != bitwidth) {
            continue;
        }
        auto writePorts = MachineConnectivityCheck::findWritePorts(*rf);
        bool connected = true;
        
        for (DataDependenceGraph::NodeSet::iterator j = lr.writes.begin();
             j != lr.writes.end() && connected; j++) {
            const MoveNode& write = **j;
            switch (MachineConnectivityCheck::canSourceWriteToAnyDestinationPort(
                        write, writePorts)) {
            case -1:
                connected = allowLimm;
                break;
            case 0:
                connected = false;
            default:
                break;
            }
        }
        if (!connected) {
            continue;
        }
        
        auto readPorts = MachineConnectivityCheck::findReadPorts(*rf);
        
        for (DataDependenceGraph::NodeSet::iterator j = lr.reads.begin();
             j != lr.reads.end() && connected; j++) {
            const MoveNode& read = **j;
            if (!MachineConnectivityCheck::canAnyPortWriteToDestination(
                    readPorts, read)) {
                connected = false;
            }
        }
        if (connected) {
            rv.insert(rf);
        }
    }
    return rv;
}


/// To avoid reanalysing machine every time hen new rr created.
std::map<const TTAMachine::Machine*, std::vector <TTAMachine::RegisterFile*> >
RegisterRenamer::tempRegFileCache_;
