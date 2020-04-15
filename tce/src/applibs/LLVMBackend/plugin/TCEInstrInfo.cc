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
 * @file TCEInstrInfo.cpp
 *
 * Implementation of TCEInstrInfo class.
 * 
 * NOTE: Normal TCE coding guidelines do not apply here to makeo it
 *       easier to track and copy paste changes from LLVM.
 *       So please follow LLVM style when adding or fixing things.
 *
 * @author Veli-Pekka J��skel�inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepist� 2009 (mikael.lepisto-no.spam-tut.fi)
 * @author Heikki Kultala 2011-2012 (heikki.kultala-no.spam-tut.fi)
 */

#include <llvm/ADT/STLExtras.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/CodeGen/MachineRegisterInfo.h>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>

#include "TCEInstrInfo.hh"
#include "TCETargetMachine.hh"
#include "TCETargetMachinePlugin.hh"

#include <iostream>
#include "TCEPlugin.hh"
//#include "tce_config.h"

// Include code generated with tceplugingen:

#define GET_INSTRINFO_CTOR_DTOR
#define GET_INSTRINFO_MC_DESC
#include "TCEGenInstrInfo.inc"
#undef GET_INSTRINFO_CTOR_DTOR
#undef GET_INSTRINFO_MC_DESC


using namespace llvm;

/**
 * Constructor.
 */
TCEInstrInfo::TCEInstrInfo(
    const TCETargetMachinePlugin* plugin) :
    TCEGenInstrInfo(TCE::ADJCALLSTACKDOWN, TCE::ADJCALLSTACKUP),
    ri_(*this), plugin_(plugin) {
}

/**
 * Destructor.
 */
TCEInstrInfo:: ~TCEInstrInfo() {   
}

/**
 * Inserts a branch instruction or brach instructions into llvm MBB.
 *
 * If the MBB already has an unconditional branch at end, does nothing.
 * 
 * @param mbb where to insert the branch instructions.
 * @param tbb jump target basic block
 * @param fbb false condition jump target, if insertin 2 branches
 *
 * @return number of branch instructions inserted
 */
unsigned
#if LLVM_OLDER_THAN_4_0
TCEInstrInfo::InsertBranch(
#else
TCEInstrInfo::insertBranch(
#endif
    MachineBasicBlock& mbb,
    MachineBasicBlock* tbb,
    MachineBasicBlock* fbb,
    ArrayRef<MachineOperand> cond,
#ifdef LLVM_OLDER_THAN_3_9
    DebugLoc dl
#else
    const DebugLoc& dl
#endif
#ifdef LLVM_OLDER_THAN_4_0
    ) const {
#else
    , int *BytesAdded) const {
#endif

    if (mbb.size() != 0) {
        // already has a uncond branch, no need for another.
        // asserts to make sure it's to same BB in order to not create
        // broken code.
        if (mbb.back().getOpcode() == TCE::TCEBR ||
            mbb.back().getOpcode() == TCE::TCEBRIND) { 
            assert(cond.size() == 0);
            return 0;
        }
        if (cond.size() != 0) {
            assert (mbb.back().getOpcode() != TCE::TCEBRCOND && "c branch!");
            assert (mbb.back().getOpcode() != TCE::TCEBRICOND && "ic branch!");
            assert (mbb.back().getOpcode() != TCE::TCEBR && "has branch!(1)");
        } else {
            assert (mbb.back().getOpcode() != TCE::TCEBR && "has branch(2)!");
        }

    }

    if (fbb == 0) {
        if (cond.empty()) {
            // Can only insert uncond branches so far.
            BuildMI(&mbb, dl, get(TCE::TCEBR)).addMBB(tbb);
            return 1;
        } else {
            if (cond.size() == 2 && cond[1].getImm() == false) {
                // false jump
                BuildMI(&mbb, dl, get(TCE::TCEBRICOND)).
                    addReg(cond[0].getReg()).addMBB(tbb);
                return 1;
            }
            BuildMI(&mbb, dl, get(TCE::TCEBRCOND)).addReg(cond[0].getReg())
                .addMBB(tbb);
            return 1;
        }
    }
    assert(!cond.empty() && "Two jumps need a condition");

    if (cond.size() == 2 && cond[1].getImm() == false) {
        BuildMI(&mbb, dl, get(TCE::TCEBRICOND)).
            addReg(cond[0].getReg()).addMBB(tbb);
    } else {
        BuildMI(&mbb, dl, get(TCE::TCEBRCOND)).
            addReg(cond[0].getReg()).addMBB(tbb);
    }
    BuildMI(&mbb, dl, get(TCE::TCEBR)).addMBB(fbb);
    return 2;
}

/**
 * Removes branch or branches form end of llvm MachineBasicBlock
 *
 * @param mbb where to remove the branches from
 * @return number of braches removed
 */
unsigned
#if LLVM_OLDER_THAN_4_0
TCEInstrInfo::RemoveBranch(MachineBasicBlock &mbb) const {
#else
TCEInstrInfo::removeBranch(
    MachineBasicBlock &mbb, int *BytesRemoved) const {
#endif
    MachineBasicBlock::iterator i = mbb.end();
    if (i == mbb.begin()) return 0;
    i--;
    int opc = i->getOpcode();
    if (opc == TCE::TCEBRCOND || opc == TCE::TCEBRICOND ||
        opc == TCE::TCEBR || opc == TCE::TCEBRIND) {
        i->eraseFromParent();
    } else {
        return 0;
    }

    i = mbb.end(); 
    if (i == mbb.begin()) return 1;
    i--;
    if (i->getOpcode() == TCE::TCEBRCOND || 
        i->getOpcode() == TCE::TCEBRICOND) {
        i->eraseFromParent();
        return 2;
    } else {
        assert(i->getOpcode() != TCE::TCEBR);
        return 1;
    }
}

/**
 * Returns true if program control can't fall through the last instruction
 * in the basic block, false otherwise.
 */
bool
TCEInstrInfo::BlockHasNoFallThrough(const MachineBasicBlock& MBB) const {
    /* Mips inspired */
    if (MBB.empty()) return false;
    switch (MBB.back().getOpcode()) {
    case TCE::RETL:    // Return.
    case TCE::TCEBR:  // Uncond branch.
    case TCE::TCEBRIND:  // Uncond indirect branch.
        return true;
    default: return false;
    }
}

void TCEInstrInfo::
storeRegToStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                    unsigned SrcReg, bool isKill, int FI,
                    const TargetRegisterClass *RC) const {
  DebugLoc DL;

  if (I != MBB.end()) DL = I->getDebugLoc();

  BuildMI(MBB, I, DL, get(plugin_->getStore(RC))).addFrameIndex(FI).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));

#ifdef LLVM_OLDER_THAN_6_0
  LLVMContext& context = MBB.getParent()->getFunction()->getContext();
#else
  LLVMContext& context = MBB.getParent()->getFunction().getContext();
#endif
  llvm::Metadata* md = llvm::MDString::get(context, "AA_CATEGORY_STACK_SLOT");
  MDNode* mdNode =
      MDNode::get(context, llvm::ArrayRef<llvm::Metadata*>(&md, 1));
  MachineOperand metaDataOperand = MachineOperand::CreateMetadata(mdNode);
  I--; // buildmi moves the iterator to next ins, point to the created one.
  I->addOperand(metaDataOperand);
}

void TCEInstrInfo::
loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                     unsigned DestReg, int FI,
                     const TargetRegisterClass *RC) const {
  DebugLoc DL;

  if (I != MBB.end()) DL = I->getDebugLoc();

  BuildMI(MBB, I, DL, get(plugin_->getLoad(RC)), DestReg).addFrameIndex(FI)
      .addImm(0);

#ifdef LLVM_OLDER_THAN_6_0
  LLVMContext& context = MBB.getParent()->getFunction()->getContext();
#else
  LLVMContext& context = MBB.getParent()->getFunction().getContext();
#endif
  llvm::Metadata* md = llvm::MDString::get(context, "AA_CATEGORY_STACK_SLOT");
  MDNode* mdNode =
      MDNode::get(context, llvm::ArrayRef<llvm::Metadata*>(&md, 1));
  MachineOperand metaDataOperand = MachineOperand::CreateMetadata(mdNode);
  I--; // buildmi moves the iterator to next ins, point to the created one.
  I->addOperand(metaDataOperand);
}

/**
 * Creates instruction for copying value from a register to another.
 *
 * @param mbb Basic block where the copy is done.
 * @param mbbi Iterator to the place where the copy instruction is added.
 * @param srcReg Register where the value is copied from.
 * @param destReg Register where the value is copied to.
 * @param rc Class of the register to copy.
 */
void TCEInstrInfo::copyPhysReg(
    MachineBasicBlock& mbb,
    MachineBasicBlock::iterator mbbi,
#ifdef LLVM_OLDER_THAN_3_9
    DebugLoc DL,
#else
    const DebugLoc& DL,
#endif
#ifdef LLVM_OLDER_THAN_10
        unsigned destReg, unsigned srcReg,
#else
        MCRegister destReg, MCRegister srcReg,
#endif
    bool killSrc) const
{
    DebugLoc dl;
    if (mbbi != mbb.end()) dl = mbbi->getDebugLoc();
/*
    BuildMI(mbb, mbbi, dl, 
            get(plugin_->getRegCopy(destReg, srcReg)), destReg).
        .addReg(SrcReg, getKillRegState(isKillSrc));
*/
    if (TCE::R1RegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI1rr), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::R32IRegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI32rr), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::R32FPRegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVF32ff), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::R32HFPRegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVF16hh), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::V2R32IRegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOV2vv), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::V2R32FPRegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOV2mm), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::V4R32IRegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOV4ww), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::V4R32FPRegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOV4nn), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::V8R32IRegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOV8xx), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::V8R32FPRegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOV8oo), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::R1RegsRegClass.contains(destReg) &&
               TCE::R32IRegsRegClass.contains(srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI32I1rr), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::R1RegsRegClass.contains(srcReg) &&
               TCE::R32IRegsRegClass.contains(destReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI1I32rr), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else {
        assert(
            false && "TCERegisterInfo::copyPhysReg(): Can't copy register");

    }
}

/*
 * Reverses a condition.
 * 
 * @param cond condition to reverse. This is modified.
 *
 * @return false if did reverse condition, true if could not.
 */
bool
#if LLVM_OLDER_THAN_4_0
TCEInstrInfo::ReverseBranchCondition(
#else
TCEInstrInfo::reverseBranchCondition(
#endif
    llvm::SmallVectorImpl<llvm::MachineOperand>& cond) const {

    assert(cond.size() != 0);

    // from true to false
    if (cond.size() == 1) {
        cond.push_back(MachineOperand::CreateImm(false));
    } else {
        // from false to true
        if (cond[1].getImm() == false) {
            cond[1].setImm(true);
        } else {
            // from true to false
            assert(cond[1].getImm() == true);
            cond[1].setImm(false);
        }
    }

    return false;
}

/**
 * Analyzes branches of MBB.
 * 
 * @param mbb MBB to analyze
 * @param tbb Puts the jump target or condition true target MBB here
 * @param fbb Puts the condition false target here, if not fall-thru
 * @param cond puts the condition data (predcate reg and T/F) here
 * @return false if could analyze, true if could not analyze
 */
bool
#ifdef LLVM_OLDER_THAN_3_9
    TCEInstrInfo::AnalyzeBranch(
#else
    TCEInstrInfo::analyzeBranch(
#endif
    MachineBasicBlock &mbb, MachineBasicBlock *&tbb,
    MachineBasicBlock *&fbb, 
    llvm::SmallVectorImpl<llvm::MachineOperand>& cond, bool allowModify)
    const {

    if (mbb.empty()) {
        return false;
    }

    MachineBasicBlock::iterator i = mbb.end(); i--;

    MachineInstr& lastIns = *i;
    switch (lastIns.getOpcode()) {
    case TCE::TCEBRCOND:
        tbb = lastIns.getOperand(1).getMBB();
        cond.push_back(i->getOperand(0));
        return false;
    case TCE::TCEBRICOND:
        tbb = lastIns.getOperand(1).getMBB();
        cond.push_back(i->getOperand(0));
        cond.push_back(MachineOperand::CreateImm(false));
        return false;
    case TCE::TCEBRIND:
    case TCE::TCEBR: {
        // indirect jump cannot be analyzed
        if (!lastIns.getOperand(0).isMBB()) {
            return true;
        }

        if (i == mbb.begin()) {
            tbb = lastIns.getOperand(0).getMBB();
            return false; // uncond jump only ins in mbb.
        }
        i--;
        if (i->getOpcode() == TCE::TCEBRCOND) {
            tbb = i->getOperand(1).getMBB();
            fbb = lastIns.getOperand(0).getMBB();
            cond.push_back(i->getOperand(0));
            return false;
        }
        if (i->getOpcode() == TCE::TCEBRICOND) {
            tbb = i->getOperand(1).getMBB(); 
            fbb = lastIns.getOperand(0).getMBB();
            cond.push_back(i->getOperand(0));
            cond.push_back(MachineOperand::CreateImm(false));
            return false;
        }
        // two uncond branches not allowed
        assert(i->getOpcode() != TCE::TCEBR);

        tbb = lastIns.getOperand(0).getMBB();
        return false; // uncond jump.
    }
    default: 
        return false; // only fall-thru
    }
    // should never be here
    return true;
}

bool 
#ifdef LLVM_OLDER_THAN_3_9
TCEInstrInfo::isPredicated(const MachineInstr *mi) const {
#else
TCEInstrInfo::isPredicated(const MachineInstr& mi_ref) const {
    const MachineInstr* mi = &mi_ref;
#endif
    // TODO: should be conditional move here..
    if (mi->getOpcode() == TCE::RETL) {
        return false;
    }

    // KILL is not a predicated instruction.
    if (mi->getOpcode() == TCE::KILL) {
        return false;
    }

    TCEString opName = plugin_->operationName(mi->getOpcode());
    return opName[0] == '?' || opName[0] == '!';
}

bool
#ifdef LLVM_OLDER_THAN_3_9
TCEInstrInfo::isPredicable(MachineInstr *mi) const {
#else
#ifdef LLVM_OLDER_THAN_5_0
TCEInstrInfo::isPredicable(MachineInstr& mi_ref) const {
    MachineInstr* mi = &mi_ref;
#else
TCEInstrInfo::isPredicable(const MachineInstr& mi_ref) const {
    const MachineInstr* mi = &mi_ref;
#endif

#endif
    if (mi->getOpcode() == TCE::COPY) {
        return false;
    }

    // TODO: why is RETL not predicable?
    if (mi->getOpcode() == TCE::RETL) {
        return false;
    }

#ifdef LLVM_OLDER_THAN_3_9
    if (isPredicated(mi)) {
#else
    if (isPredicated(*mi)) {
#endif
        return false;
    }

    if (getMatchingCondBranchOpcode(mi->getOpcode(),false) == -1) {
        return false;
    }

    return true;
}

// todo: mostlly ripped from hexagon.
// check the legal things
bool TCEInstrInfo::PredicateInstruction(
#ifdef LLVM_OLDER_THAN_3_9
    MachineInstr *mi,
#else
    MachineInstr& mi_ref,
#endif
    ArrayRef<MachineOperand> cond
) const {

#ifndef LLVM_OLDER_THAN_3_9
    MachineInstr *mi = &mi_ref;
#endif

    int opc = mi->getOpcode();

#ifdef LLVM_OLDER_THAN_3_9
    assert (isPredicable(mi) && "Expected predicable instruction");
#else
    assert (isPredicable(*mi) && "Expected predicable instruction");
#endif

    bool invertJump = (cond.size() >1 && cond[1].isImm() &&
                       (cond[1].getImm() == 0));
    
    mi->setDesc(get(getMatchingCondBranchOpcode(opc, invertJump)));
    //
    // This assumes that the predicate is always the first operand
    // in the set of inputs.
    //
    mi->addOperand(mi->getOperand(mi->getNumOperands()-1));
    int oper;
    // why -3 in hexagon?
    for (oper = mi->getNumOperands() - 2; oper >= 0; --oper) {
        MachineOperand mo = mi->getOperand(oper);
	// todo: why this break in hexagon?
        if ((mo.isReg() && !mo.isUse() && !mo.isImplicit())) {
            break;
        }

        if (mo.isReg()) {
            mi->getOperand(oper+1).ChangeToRegister(mo.getReg(), mo.isDef(),
						    mo.isImplicit(), mo.isKill(),
						    mo.isDead(), mo.isUndef(),
						    mo.isDebug());
        } else if (mo.isImm()) {
            mi->getOperand(oper+1).ChangeToImmediate(mo.getImm());
        } else if (mo.isFPImm()) {
            mi->getOperand(oper+1).ChangeToFPImmediate(mo.getFPImm());
        } else if (mo.isGlobal()) {
            // TODO: what to do here? 
            llvm_unreachable("Unexpected operand type");
            mi->getOperand(oper+1).ChangeToImmediate(mo.getImm());
        } else {
            llvm_unreachable("Unexpected operand type");
        }
    }

    MachineOperand PredMO = cond[0];
    mi->getOperand(oper+1).ChangeToRegister(PredMO.getReg(), PredMO.isDef(),
                                            PredMO.isImplicit(), PredMO.isKill(),
                                            PredMO.isDead(), PredMO.isUndef(),
                                            PredMO.isDebug());
    
    return true;
}


int TCEInstrInfo::getMatchingCondBranchOpcode(int opc, bool inv) const {
    
    if (!inv) {
        return plugin_->getTruePredicateOpcode(opc);
    } else {
        return plugin_->getFalsePredicateOpcode(opc);
    }
}


// mostly ripped from hexagon
bool
#ifdef LLVM_OLDER_THAN_3_9
TCEInstrInfo::DefinesPredicate(
    MachineInstr *MI, std::vector<MachineOperand> &Pred) const {
#else
TCEInstrInfo::DefinesPredicate(
    MachineInstr& MI_ref, std::vector<MachineOperand> &Pred) const {

    MachineInstr *MI = &MI_ref;
#endif
    for (unsigned oper = 0; oper < MI->getNumOperands(); ++oper) {
	MachineOperand MO = MI->getOperand(oper);
	if (MO.isReg() && MO.isDef()) {
	    const TargetRegisterClass* RC = ri_.getMinimalPhysRegClass(MO.getReg());
	    if (RC == &TCE::R1RegsRegClass) {
		Pred.push_back(MO);
		return true;
	    }
	}
    }
    return false;
}

bool
TCEInstrInfo::
isProfitableToIfCvt(MachineBasicBlock &MBB,
                    unsigned NumCycles,
                    unsigned ExtraPredCycles,
#ifdef LLVM_OLDER_THAN_3_8
                    const BranchProbability &Probability) const {
#else
                    BranchProbability Probability) const {
#endif
    return true;
}


bool
TCEInstrInfo::
isProfitableToIfCvt(MachineBasicBlock &TMBB,
                    unsigned NumTCycles,
                    unsigned ExtraTCycles,
                    MachineBasicBlock &FMBB,
                    unsigned NumFCycles,
                    unsigned ExtraFCycles,
#ifdef LLVM_OLDER_THAN_3_8
                    const BranchProbability &Probability) const {
#else
                    BranchProbability Probability) const {
#endif
    return true;
}
