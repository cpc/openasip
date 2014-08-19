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
 * @author Heikki Kultala 2011 (heikki.kultala-no.spam-tut.fi)
 */

#include <assert.h>
#include "tce_config.h"
#if (defined(LLVM_3_1) || defined(LLVM_3_2))
#include <llvm/Type.h>
#include <llvm/Function.h>
#else
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#endif
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/CodeGen/MachineFrameInfo.h>
#include <llvm/Target/TargetInstrInfo.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/CodeGen/RegisterScavenging.h>

#include "TCEPlugin.hh"
#include "TCERegisterInfo.hh"
#include "TCEString.hh"
#include "Application.hh"
#include "tce_config.h"

#include <iostream> // DEBUG

using namespace llvm;

#define GET_REGINFO_MC_DESC
#define GET_REGINFO_TARGET_DESC

#include "TCEGenRegisterInfo.inc"

/**
 * The Constructor.
 *
 * @param st Subtarget architecture.
 * @param tii Target architecture instruction info.
 */
TCERegisterInfo::TCERegisterInfo(
    const TargetInstrInfo& tii, int stackAlignment) :
    TCEGenRegisterInfo(TCE::RA),
    tii_(tii),
    stackAlignment_(stackAlignment) {
}

/**
 * Returns list of callee saved registers.
 */
const uint16_t*
TCERegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
    static const uint16_t calleeSavedRegs[] = { 0 };
    return calleeSavedRegs;
}

/**
 * Returns list of reserved registers.
 */
BitVector
TCERegisterInfo::getReservedRegs(const MachineFunction& mf) const {
    assert(&mf != NULL);
    BitVector reserved(getNumRegs());
    reserved.set(TCE::SP);
    reserved.set(TCE::KLUDGE_REGISTER);
    reserved.set(TCE::RA);
    reserved.set(TCE::IRES0);
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

#if (defined(LLVM_3_1) || defined(LLVM_3_2))
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
#endif

void TCERegisterInfo::eliminateFrameIndex(
    MachineBasicBlock::iterator II, int SPAdj, 
#if (!(defined(LLVM_3_1) || defined(LLVM_3_2)))
    unsigned FIOperandNum,
#endif
    RegScavenger *RS) const {
    const TargetInstrInfo &TII = tii_;
    assert(SPAdj == 0 && "Unexpected");
    MachineInstr &MI = *II;
    DebugLoc dl = MI.getDebugLoc();
#if (defined(LLVM_3_1) || defined(LLVM_3_2))
    unsigned FIOperandNum = 0;
    while (!MI.getOperand(FIOperandNum).isFI()) {
        ++FIOperandNum;
        assert(FIOperandNum < MI.getNumOperands() && "Instr doesn't have FrameIndex operand!");
    }
#endif
    int FrameIndex = MI.getOperand(FIOperandNum).getIndex();

    // Addressable stack objects are accessed using neg. offsets from %fp
    // NO THEY ARE NOT! apwards from SP!
    MachineFunction &MF = *MI.getParent()->getParent();

    int Offset = 
        MF.getFrameInfo()->getObjectOffset(FrameIndex) + MF.getFrameInfo()->getStackSize();

    if (Offset != 0) {
        MI.getOperand(FIOperandNum).ChangeToRegister(TCE::KLUDGE_REGISTER, false);
        BuildMI(
            *MI.getParent(), II, MI.getDebugLoc(), TII.get(TCE::ADDrri),
            TCE::KLUDGE_REGISTER).addReg(TCE::SP).addImm(Offset);
    } else {
        MI.getOperand(FIOperandNum).ChangeToRegister(TCE::SP, false);
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
    if (mfi->hasVarSizedObjects()) {
        TCEString errMsg;
        errMsg << "ERROR: function '" 
	       << (std::string)(mf.getFunction()->getName())
               << "' contains dynamic stack objects which are not supported by tcecc yet.\n"
               << "See the user manual section Unsupported C Language Constructs for more info.\n";
        std::cerr << errMsg;
        exit(1);
    }
    // this unfortunately return true for inline asm.
    bool hasCalls = mfi->hasCalls();
    if (hasCalls) {
        // so then check again. Return false if only inline asm, no calls.
        hasCalls = containsCall(mf);
    }

    numBytes = (numBytes + 3) & ~3; // stack size alignment

    MachineBasicBlock::iterator ii = mbb.begin();

    DebugLoc dl = (ii != mbb.end() ?
                   ii->getDebugLoc() : DebugLoc());

    if (hasCalls) {
        BuildMI(mbb, ii, dl, tii_.get(TCE::SUBrri), TCE::SP)
            .addReg(TCE::SP)
            .addImm(stackAlignment_);

        // Save RA to stack.
        BuildMI(mbb, ii, dl, tii_.get(TCE::STWRArr))
            .addReg(TCE::SP)
            .addImm(0)
            .addReg(TCE::RA)
            .setMIFlag(MachineInstr::FrameSetup);

        mfi->setStackSize(numBytes + stackAlignment_);

        // Adjust stack pointer
        if (numBytes != 0) {
            BuildMI(mbb, ii, dl, tii_.get(TCE::SUBrri), TCE::SP)
                .addReg(TCE::SP)
                .addImm(numBytes);
        }
    } else { // leaf function
        mfi->setStackSize(numBytes);

        // Adjust stack pointer
        if (numBytes != 0) {
            BuildMI(mbb, ii, dl, tii_.get(TCE::SUBrri), TCE::SP)
                .addReg(TCE::SP)
                .addImm(numBytes);
        }
    }
}

/**
 * Emits machine function epilogue to machine functions.
 */
void
TCERegisterInfo::emitEpilogue(
    MachineFunction& mf, MachineBasicBlock& mbb) const {

    MachineFrameInfo* mfi = mf.getFrameInfo();

#if (defined(LLVM_3_2) || defined(LLVM_3_3) || defined(LLVM_3_4))
    MachineBasicBlock::iterator mbbi = prior(mbb.end());
#else
    MachineBasicBlock::iterator mbbi = std::prev(mbb.end());
#endif

    DebugLoc dl = mbbi->getDebugLoc();

    if (mbbi->getOpcode() != TCE::RETL && mbbi->getOpcode() != TCE::RETL_old) {
        assert(false && "ERROR: Insertiing epilogue w/o return?");
    }
    
    unsigned numBytes = mfi->getStackSize();

    // this unfortunately return true for inline asm.
    bool hasCalls = mfi->hasCalls();
    if (hasCalls) {
        // so then check again. Return false if only inline asm, no calls.
        hasCalls = containsCall(mf);
    }

    if (hasCalls) {
        if (numBytes != stackAlignment_) {
            BuildMI(mbb, mbbi, dl, tii_.get(TCE::ADDrri), TCE::SP)
                .addReg(TCE::SP)
                .addImm(numBytes - stackAlignment_);
        }

        // Restore RA from stack.
        BuildMI(mbb, mbbi, dl, tii_.get(TCE::LDWRAr), TCE::RA)
            .addReg(TCE::SP)
            .addImm(0)
            .setMIFlag(MachineInstr::FrameSetup);
        
        BuildMI(mbb, mbbi, dl, tii_.get(TCE::ADDrri), TCE::SP)
            .addReg(TCE::SP)
            .addImm(stackAlignment_);
    } else { // leaf function
        
        // adjust by stack size
        if (numBytes != 0) {
            BuildMI(mbb, mbbi, dl, tii_.get(TCE::ADDrri), TCE::SP)
                .addReg(TCE::SP)
                .addImm(numBytes);
        }
    }
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

bool
TCERegisterInfo::containsCall(MachineFunction& mf) const {
    for (MachineFunction::iterator i = mf.begin(); i != mf.end(); i++) {
        const MachineBasicBlock& mbb = *i;
        for (MachineBasicBlock::const_iterator j = mbb.begin();
             j != mbb.end(); j++){
            const MachineInstr& ins = *j;
            if (ins.getOpcode() == TCE::CALL || 
                ins.getOpcode() == TCE::CALL_MEMrr || 
                ins.getOpcode() == TCE::CALL_MEMri) {
                return true;
            }
        }
    }
    return false;
}
