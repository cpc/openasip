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
 */


#include "TCEInstrInfo.hh"
#include "TCETargetMachine.hh"

#include <llvm/ADT/STLExtras.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/CodeGen/MachineRegisterInfo.h>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>


#include <iostream>
#include "TCEPlugin.hh"
#include "tce_config.h"

// Include code generated with tceplugingen:
#include "TCEGenInstrInfo.inc"


using namespace llvm;

/**
 * Constructor.
 */
TCEInstrInfo::TCEInstrInfo() :
    TargetInstrInfoImpl(TCEInsts, sizeof(TCEInsts) / sizeof(TCEInsts[0])),
    ri_(*this) {

}

/**
 * Destructor.
 */
TCEInstrInfo:: ~TCEInstrInfo() {   
}

#ifdef LLVM_2_7		       
/**
 * Returns true if the instruction is a register to register move.
 */
bool
TCEInstrInfo::isMoveInstr(
    const MachineInstr &MI, unsigned &SrcReg, unsigned &DstReg, 
    unsigned &SrcSubIdx, unsigned &DstSubIdx) const {   

    // No sub registers
    SrcSubIdx = DstSubIdx = 0;
    
    if (MI.getOpcode() == TCE::MOVI1rr ||
        MI.getOpcode() == TCE::MOVI8rr ||
        MI.getOpcode() == TCE::MOVI16rr ||
        MI.getOpcode() == TCE::MOVI32rr ||
        MI.getOpcode() == TCE::MOVI64rr ||

        MI.getOpcode() == TCE::MOVI8I1rr ||
        MI.getOpcode() == TCE::MOVI16I1rr ||
        MI.getOpcode() == TCE::MOVI32I1rr ||
        MI.getOpcode() == TCE::MOVI64I1rr ||

        MI.getOpcode() == TCE::MOVI16I8rr ||
        MI.getOpcode() == TCE::MOVI32I8rr ||
        MI.getOpcode() == TCE::MOVI64I8rr ||

        MI.getOpcode() == TCE::MOVI32I16rr ||
        MI.getOpcode() == TCE::MOVI64I16rr ||

        MI.getOpcode() == TCE::MOVI64I32rr ||

        MI.getOpcode() == TCE::MOVF32rr ||
        MI.getOpcode() == TCE::MOVF64rr ||
        MI.getOpcode() == TCE::MOVFI32rr ||
        MI.getOpcode() == TCE::MOVIF32rr) {

        assert(MI.getOperand(0).isReg());
        assert(MI.getOperand(1).isReg());

        DstReg = MI.getOperand(0).getReg();
        SrcReg = MI.getOperand(1).getReg();
        return true;
    }        

    return false;
}
#endif

/**
 * Inserts a branch instruction.
 */
#ifdef LLVM_2_7
unsigned 
TCEInstrInfo::InsertBranch(
    MachineBasicBlock &MBB,
    MachineBasicBlock *TBB,
    MachineBasicBlock *FBB,
    const SmallVectorImpl<llvm::MachineOperand> &Cond) const {
#else
unsigned 
TCEInstrInfo::InsertBranch(
    MachineBasicBlock &MBB,
    MachineBasicBlock *TBB,
    MachineBasicBlock *FBB,
    const SmallVectorImpl<llvm::MachineOperand> &Cond,
    DebugLoc dl) const {
#endif

    // Can only insert uncond branches so far.
    assert(Cond.empty() && !FBB && TBB && 
           "Can only handle uncond branches!");

#ifdef LLVM_2_7    
    DebugLoc dl = DebugLoc::getUnknownLoc();
#endif

    BuildMI(&MBB, dl, get(TCE::TCEBR)).addMBB(TBB);
    return 1;
}

/**
 * Returns true if the instruction is a load from stack slot.
 */
unsigned
TCEInstrInfo::isLoadFromStackSlot(
    const MachineInstr* mi, int& frameIndex) const {

    if (mi->getOpcode() == TCE::LDWi) {
        
        if (mi->getOperand(1).isFI() &&
            mi->getOperand(2).isImm() &&
            mi->getOperand(2).getImm() == 0) {
            
            frameIndex = mi->getOperand(1).getIndex();

            return mi->getOperand(0).getReg();
        }
    }
    return 0;
}


/**
 * Returns true if the instruction is a store to stack slot.
 */
unsigned
TCEInstrInfo::isStoreToStackSlot(
    const MachineInstr* mi, int& frameIndex) const {
    
    if (mi->getOpcode() == TCE::STWir) {
        if (mi->getOperand(0).isFI() &&
            mi->getOperand(1).isImm() &&
            mi->getOperand(1).getImm() == 0) {

            frameIndex = mi->getOperand(0).getIndex();
           
            return mi->getOperand(2).getReg();
        }
    }
    return 0;
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
#ifdef LLVM_2_7
  DebugLoc DL = DebugLoc::getUnknownLoc();
#else
  DebugLoc DL;
#endif
  if (I != MBB.end()) DL = I->getDebugLoc();

  // On the order of operands here: think "[FrameIdx + 0] = SrcReg".
  if (RC == TCE::I32RegsRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::STWir)).addFrameIndex(FI).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
  else if (RC == TCE::F32RegsRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::STWFir)).addFrameIndex(FI).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
  else if (RC == TCE::RARegRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::STWRArr)).addFrameIndex(FI).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
  else if (RC == TCE::I1RegsRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::STQBib)).addFrameIndex(FI).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));
  else
    assert(0 && "Can't store this register to stack slot");
}

void TCEInstrInfo::
loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                     unsigned DestReg, int FI,
                     const TargetRegisterClass *RC) const {
#ifdef LLVM_2_7
  DebugLoc DL = DebugLoc::getUnknownLoc();
#else
  DebugLoc DL;
#endif
  if (I != MBB.end()) DL = I->getDebugLoc();

  if (RC == TCE::I32RegsRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::LDWi), DestReg).addFrameIndex(FI).addImm(0);
  else if (RC == TCE::F32RegsRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::LDWFi), DestReg).addFrameIndex(FI).addImm(0);
  else if (RC == TCE::RARegRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::LDWRAr), DestReg).addFrameIndex(FI).addImm(0);
  else if (RC == TCE::I1RegsRegisterClass)
    BuildMI(MBB, I, DL, get(TCE::LDQBr), DestReg).addFrameIndex(FI).addImm(0);
  else
    assert(0 && "Can't load this register from stack slot");
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
#ifdef LLVM_2_7
bool
TCEInstrInfo::copyRegToReg(
    MachineBasicBlock& mbb,
    MachineBasicBlock::iterator mbbi,
    unsigned destReg, unsigned srcReg,
    const TargetRegisterClass* dstRC,
    const TargetRegisterClass* srcRC) const 
{
    assert(srcRC == dstRC && "not yet implemented");

    DebugLoc dl = DebugLoc::getUnknownLoc();

    if (mbbi != mbb.end()) dl = mbbi->getDebugLoc();

    if (srcRC == TCE::I1RegsRegisterClass) {        
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI1rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::I8RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI8rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::I16RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI16rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::I32RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI32rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::I64RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI64rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::F32RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVF32rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::F64RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVF64rr), destReg).addReg(srcReg);
    } else {
        assert(
            false && "TCERegisterInfo::copyRegToReg(): Can't copy register");
    }
    return true;
}

#else

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
    } else {
        assert(
            false && "TCERegisterInfo::copyPhysReg(): Can't copy register");
    }
}

#endif
