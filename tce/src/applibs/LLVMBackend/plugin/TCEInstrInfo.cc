/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 * @author Heikki Kultala 2011 (heikki.kultala-no.spam-tut.fi)
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

#define GET_INSTRINFO_CTOR
#define GET_INSTRINFO_MC_DESC
#include "TCEGenInstrInfo.inc"


using namespace llvm;

/**
 * Constructor.
 */
TCEInstrInfo::TCEInstrInfo(const TCETargetMachinePlugin* plugin) :
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
 * @param mbb where to inser the branch instructions.
 * @param tbb jump target basic block
 * @param fbb false condition jump target, if insertin 2 branches
 *
 * @return number of branch instructions inserted
 */
unsigned 
TCEInstrInfo::InsertBranch(
    MachineBasicBlock& mbb,
    MachineBasicBlock* tbb,
    MachineBasicBlock* fbb,
    const llvm::SmallVectorImpl<llvm::MachineOperand>& cond,
    DebugLoc dl) const {

    if (mbb.size() != 0) {
        // already has a uncond branch, no need for another.
        // asserts to make sure it's to same BB in order to not create
        // broken code.
        if (mbb.back().getOpcode() == TCE::TCEBR) {
            assert(cond.size() == 0);
            assert(mbb.back().getOperand(0).getMBB() == tbb);
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
TCEInstrInfo::RemoveBranch(MachineBasicBlock &mbb) const {
    MachineBasicBlock::iterator i = mbb.end();
    if (i == mbb.begin()) return 0;
    i--;
    int opc = i->getOpcode();
    if (opc == TCE::TCEBRCOND || opc == TCE::TCEBRICOND ||
        opc == TCE::TCEBR) {
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

  // On the order of operands here: think "[FrameIdx + 0] = SrcReg".
  if (RC == TCE::I32RegsRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::STWrr)).addFrameIndex(FI).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
  else if (RC == TCE::F32RegsRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::STWfr)).addFrameIndex(FI).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
  else if (RC == TCE::RARegRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::STWRArr)).addFrameIndex(FI).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
  else if (RC == TCE::I1RegsRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::STQBrb)).addFrameIndex(FI).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
  else
    BuildMI(MBB, I, DL, get(plugin_->getStore(RC))).addFrameIndex(FI).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
#if 0    
  else if (RC == TCE::V2I32RegsRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::STW2vr)).addFrameIndex(FI).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
  else if (RC == TCE::V2F32RegsRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::STW2mr)).addFrameIndex(FI).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
  else
    assert(0 && "Can't store this register to stack slot");
#endif
}

void TCEInstrInfo::
loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                     unsigned DestReg, int FI,
                     const TargetRegisterClass *RC) const {
  DebugLoc DL;

  if (I != MBB.end()) DL = I->getDebugLoc();

  if (RC == TCE::I32RegsRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::LDWrr), DestReg).addFrameIndex(FI).addImm(0);
  else if (RC == TCE::F32RegsRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::LDWfr), DestReg).addFrameIndex(FI).addImm(0);
  else if (RC == TCE::RARegRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::LDWRAr), DestReg).addFrameIndex(FI).addImm(0);
  else if (RC == TCE::I1RegsRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::LDQBr), DestReg).addFrameIndex(FI).addImm(0);
  else
    BuildMI(MBB, I, DL, get(plugin_->getLoad(RC)), DestReg).addFrameIndex(FI).addImm(0);
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
    MachineBasicBlock::iterator mbbi, DebugLoc DL,
    unsigned destReg, unsigned srcReg,
    bool killSrc) const
{
    DebugLoc dl;
    if (mbbi != mbb.end()) dl = mbbi->getDebugLoc();

    if (TCE::I1RegsRegisterClass->contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI1rr), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::I8RegsRegisterClass->contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI8rr), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::I16RegsRegisterClass->contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI16rr), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::I32RegsRegisterClass->contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI32rr), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::I64RegsRegisterClass->contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI64rr), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::F32RegsRegisterClass->contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVF32rr), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::F64RegsRegisterClass->contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVF64rr), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::V2I32RegsRegisterClass->contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOV2vv), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::V2F32RegsRegisterClass->contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOV2mm), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::V4I32RegsRegisterClass->contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOV4ww), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::V4F32RegsRegisterClass->contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOV4nn), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::V8I32RegsRegisterClass->contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOV8xx), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::V8F32RegsRegisterClass->contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOV8oo), destReg)
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
TCEInstrInfo::ReverseBranchCondition(
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
TCEInstrInfo::AnalyzeBranch(
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
    case TCE::TCEBR: {
        // indirect jump cannot be analyzed
        if (!lastIns.getOperand(0).isMBB()) {
            return true;
        }

        if ( i == mbb.begin()) {
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
