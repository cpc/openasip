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
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
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

/**
 * Return true if the specified function should have a dedicated frame 
 * pointer register.
 */
bool
TCERegisterInfo::hasFP(const MachineFunction& mf) const {
    return false;
}

/**
 * Eliminates call frame pseudo instructions by replacing them with suitable
 * instruction sequence.
 */
void
TCERegisterInfo::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator I) const {

  const TargetInstrInfo &TII = tii_;
  MachineInstr &MI = *I;
  DebugLoc dl = MI.getDebugLoc();
  int Size = MI.getOperand(0).getImm();
  if (MI.getOpcode() == TCE::ADJCALLSTACKDOWN)
    Size = -Size;

  // if (Size)
  //  BuildMI(MBB, I, dl, TII.get(TCE::ADDri), TCE::SP).addReg(TCE::SP).addImm(Size);

  MBB.erase(I);
}

void TCERegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj, RegScavenger *RS) const {
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

    std::cerr << "Eliminate frame index offset: " << Offset << std::endl;

    if (Offset != 0) {
        MI.getOperand(i).ChangeToRegister(TCE::KLUDGE_REGISTER, false);
        BuildMI(
            *MI.getParent(), II, MI.getDebugLoc(), TII.get(TCE::ADDri),
            TCE::KLUDGE_REGISTER).addReg(TCE::SP).addImm(Offset);
    } else {
        MI.getOperand(i).ChangeToRegister(TCE::SP, false);
    }
}

/**
 * Emits machine function prologue to machine functions.
 */
void
TCERegisterInfo::emitPrologue(MachineFunction& mf) const {

    MachineBasicBlock& mbb = mf.front();
    MachineFrameInfo* mfi = mf.getFrameInfo();
    int numBytes = (int)mfi->getStackSize();

    std::cerr << "Stack size in prolog: " << numBytes << "\n";

    numBytes = (numBytes + 3) & ~3; // stack size alignment

    MachineBasicBlock::iterator ii = mbb.begin();
    DebugLoc dl = (ii != mbb.end() ?
                   ii->getDebugLoc() : DebugLoc::getUnknownLoc());

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

    std::cerr << "Stack size in epilog: " << numBytes << "\n";

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

/**
 * Re-issue the specified 'original' instruction at the specific location 
 * targeting a new destination register.
 *
 * @param mbb Machine basic block of the new instruction.
 * @param i Position of the new instruction in the basic block.
 * @param destReg New destination register.
 * @param orig Original instruction.
 */
void
TCERegisterInfo::reMaterialize(
    MachineBasicBlock& mbb,
    MachineBasicBlock::iterator i,
    unsigned destReg,
    const MachineInstr* orig) const {
    
    assert(false && "It really was used");
    MachineInstr* mi = mbb.getParent()->CloneMachineInstr(orig);
    mi->getOperand(0).setReg(destReg);
    mbb.insert(i, mi);
}


/**
 * Not implemented: When is this method even called?
 */
unsigned
TCERegisterInfo::getRARegister() const {
    assert(false);
    return 0;
}

/**
 * Not implemented: When is this method even called?
 */
unsigned
TCERegisterInfo::getFrameRegister(MachineFunction& mf) const {
    assert(false);
    return 0;
}

int
TCERegisterInfo::getDwarfRegNum(unsigned RegNum, bool isEH) const {
    assert(0 && "Not implemented!");
    return -1;
}


