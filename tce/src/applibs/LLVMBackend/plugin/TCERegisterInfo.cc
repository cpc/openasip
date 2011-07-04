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
 * @file TCERegisterInfo.cpp
 *
 * Implementation of TCERegisterInfo class.
 *
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 */

#include <assert.h>
#include <llvm/Type.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/CodeGen/MachineFrameInfo.h>
#include <llvm/Target/TargetInstrInfo.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/CodeGen/RegisterScavenging.h>

#include "TCEPlugin.hh"
#include "TCERegisterInfo.hh"
#include "tce_config.h"

#include <iostream> // DEBUG

using namespace llvm;

#include "TCEGenRegisterInfo.inc"

/**
 * The Constructor.
 *
 * @param st Subtarget architecture.
 * @param tii Target architecture instruction info.
 */
TCERegisterInfo::TCERegisterInfo(const TargetInstrInfo& tii) :
    TCEGenRegisterInfo(TCE::ADJCALLSTACKDOWN, TCE::ADJCALLSTACKUP),
    tii_(tii) {
}

/**
 * Returns list of callee saved registers.
 */
const unsigned* 
TCERegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
    static const unsigned calleeSavedRegs[] = { 0 };
    return calleeSavedRegs;
}

/**
 * Returns list of reserved registers.
 */
BitVector
TCERegisterInfo::getReservedRegs(const MachineFunction& mf) const {
    BitVector reserved(getNumRegs());
    reserved.set(TCE::SP);
    reserved.set(TCE::KLUDGE_REGISTER);
    reserved.set(TCE::RA);

    reserved.set(TCE::FSP);
    reserved.set(TCE::FKLUDGE);
    return reserved;
}

/**
 * Returns list of callee saved register classes.
 *
 * The returned list has equal length with getCalleeSavedRegs() list
 * and the list items with same position correspond to each other.
 */
const TargetRegisterClass* const*
TCERegisterInfo::getCalleeSavedRegClasses(const MachineFunction *MF) const {
    static const TargetRegisterClass* const calleeSavedRegClasses[] = { 0 };
    return calleeSavedRegClasses;
}


#if (defined(LLVM_2_7) || defined(LLVM_2_8))
/**
 * Return true if the specified function should have a dedicated frame 
 * pointer register.
 */
bool
TCERegisterInfo::hasFP(const MachineFunction& mf) const {
    return false;
}
#endif

/**
 * Eliminates call frame pseudo instructions. 
 *
 * Stack space is already reserved in caller stack.
 */
void
TCERegisterInfo::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator I) const {
    MBB.erase(I);
}

#ifdef LLVM_2_7
unsigned TCERegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                              int SPAdj, int *Value,
                                              RegScavenger *RS) const {
#else
void TCERegisterInfo::eliminateFrameIndex(
    MachineBasicBlock::iterator II, int SPAdj, RegScavenger *RS) const {
#endif
    const TargetInstrInfo &TII = tii_;
    assert(SPAdj == 0 && "Unexpected");
    unsigned i = 0;
    MachineInstr &MI = *II;
    DebugLoc dl = MI.getDebugLoc();
    while (!MI.getOperand(i).isFI()) {
        ++i;
        assert(i < MI.getNumOperands() && "Instr doesn't have FrameIndex operand!");
    }

    int FrameIndex = MI.getOperand(i).getIndex();

    // Addressable stack objects are accessed using neg. offsets from %fp
    MachineFunction &MF = *MI.getParent()->getParent();

    int Offset = MF.getFrameInfo()->getObjectOffset(FrameIndex) + MF.getFrameInfo()->getStackSize();

    if (Offset != 0) {
        MI.getOperand(i).ChangeToRegister(TCE::KLUDGE_REGISTER, false);
        BuildMI(
            *MI.getParent(), II, MI.getDebugLoc(), TII.get(TCE::ADDri),
            TCE::KLUDGE_REGISTER).addReg(TCE::SP).addImm(Offset);
    } else {
        MI.getOperand(i).ChangeToRegister(TCE::SP, false);
    }
#ifdef LLVM_2_7
    return 0;
#endif
}

/**
 * Emits machine function prologue to machine functions.
 */
void
TCERegisterInfo::emitPrologue(MachineFunction& mf) const {

    MachineBasicBlock& mbb = mf.front();
    MachineFrameInfo* mfi = mf.getFrameInfo();
    int numBytes = (int)mfi->getStackSize();

    numBytes = (numBytes + 3) & ~3; // stack size alignment

    MachineBasicBlock::iterator ii = mbb.begin();
#ifdef LLVM_2_7
    DebugLoc dl = (ii != mbb.end() ?
                   ii->getDebugLoc() : DebugLoc::getUnknownLoc());
#else
    DebugLoc dl = (ii != mbb.end() ?
                   ii->getDebugLoc() : DebugLoc());
#endif

    BuildMI(mbb, ii, dl, tii_.get(TCE::SUBri), TCE::SP).addReg(
        TCE::SP).addImm(4);

    // Save RA to stack.
    BuildMI(mbb, ii, dl, tii_.get(TCE::STWRArr)).addReg(
        TCE::SP).addImm(0).addReg(TCE::RA);

    // Adjust stack pointer
   if (numBytes != 0) {
        BuildMI(mbb, ii, dl, tii_.get(TCE::SUBri), TCE::SP).addReg(
            TCE::SP).addImm(numBytes);
   }

    mfi->setStackSize(numBytes + 4);
}

/**
 * Emits machine function epilogue to machine functions.
 */
void
TCERegisterInfo::emitEpilogue(
    MachineFunction& mf, MachineBasicBlock& mbb) const {

    MachineFrameInfo* mfi = mf.getFrameInfo();
    MachineBasicBlock::iterator mbbi = prior(mbb.end());
    DebugLoc dl = mbbi->getDebugLoc();

    if (mbbi->getOpcode() != TCE::RETL) {
        assert(false && "ERROR: Insertiing epilogue w/o return?");
    }
    
    unsigned numBytes = mfi->getStackSize();

    if (numBytes != 4) {
        BuildMI(mbb, mbbi, dl, tii_.get(TCE::ADDri), TCE::SP).addReg(
            TCE::SP).addImm(numBytes - 4);
    }

    // Restore RA from stack.
    BuildMI(mbb, mbbi, dl, tii_.get(TCE::LDWRAr), TCE::RA).addReg(
        TCE::SP).addImm(0);

    BuildMI(mbb, mbbi, dl, tii_.get(TCE::ADDri), TCE::SP).addReg(
        TCE::SP).addImm(4);
}

unsigned
TCERegisterInfo::getRARegister() const {
    assert(false && "Remove this assert if this is really called.");
    return TCE::RA;
}

unsigned
TCERegisterInfo::getFrameRegister(const MachineFunction& mf) const {
    return 0;
}

int
TCERegisterInfo::getDwarfRegNum(unsigned RegNum, bool /*isEH*/) const {
    return TCEGenRegisterInfo::getDwarfRegNumFull(RegNum, 0); 
}

int
TCERegisterInfo::getLLVMRegNum(unsigned RegNum, bool /*isEH */) const {
#ifdef LLVM_2_9
    return -1;
#else
    return TCEGenRegisterInfo::getLLVMRegNumFull(RegNum, 0);
#endif
}


