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
 * @file TCERegisterInfo.cpp
 *
 * Implementation of TCERegisterInfo class.
 *
 * @author Veli-Pekka J��skel�inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepist� 2009 (mikael.lepisto-no.spam-tut.fi)
 * @author Heikki Kultala 2011-2016 (heikki.kultala-no.spam-tut.fi)
 */

#include <assert.h>
#include "tce_config.h"
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/CodeGen/MachineFrameInfo.h>
#ifdef LLVM_OLDER_THAN_6_0
#include <llvm/Target/TargetInstrInfo.h>
#else
#include <llvm/CodeGen/TargetInstrInfo.h>
#endif
#include <llvm/Target/TargetOptions.h>

#include <llvm/ADT/STLExtras.h>
#include <llvm/CodeGen/RegisterScavenging.h>

#include "TCEPlugin.hh"
#include "TCERegisterInfo.hh"
#include "Application.hh"
#include "tce_config.h"

using namespace llvm;

#define GET_REGINFO_MC_DESC
#define GET_REGINFO_TARGET_DESC

#include "TCEFrameInfo.hh"
#include "TCEGenRegisterInfo.inc"

/**
 * The Constructor.
 *
 * @param st Subtarget architecture.
 * @param tii Target architecture instruction info.
 */
TCERegisterInfo::TCERegisterInfo(
    const TargetInstrInfo& tii) :
    TCEGenRegisterInfo(TCE::RA),
    tii_(tii) {
}

/**
 * Returns list of callee saved registers.
 */
const MCPhysReg*
TCERegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
    static const uint16_t calleeSavedRegs[] = { TCE::FP, 0 };
    if (hasFP(*MF)) {
        return calleeSavedRegs+1; // skip first, it's reserved
    } else {
        return calleeSavedRegs;
    }
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

    if (hasFP(mf)) {
        reserved.set(TCE::FP);
    }

    return reserved;
}

void TCERegisterInfo::eliminateFrameIndex(
    MachineBasicBlock::iterator II, int SPAdj, 
    unsigned FIOperandNum,
    RegScavenger *RS) const {
    const TargetInstrInfo &TII = tii_;
    assert(SPAdj == 0 && "Unexpected");
    MachineInstr &MI = *II;
    DebugLoc dl = MI.getDebugLoc();
    int FrameIndex = MI.getOperand(FIOperandNum).getIndex();

    // Addressable stack objects are accessed using neg. offsets from %fp
    // NO THEY ARE NOT! apwards from SP!
    MachineFunction &MF = *MI.getParent()->getParent();

#if LLVM_OLDER_THAN_4_0
    auto& frameinfo = *MF.getFrameInfo();
#else
    auto& frameinfo = MF.getFrameInfo();
#endif

    if (tfi_->hasFP(MF)) {
        int Offset = frameinfo.getObjectOffset(FrameIndex);
        int stackAlign = tfi_->stackAlignment();
        // FP storage space increases offset by stackAlign.
        Offset+= stackAlign;
        // RA storage space increases offset of incoming vars
        if (FrameIndex < 0  && frameinfo.hasCalls() && tfi_->containsCall(MF)) {
            Offset+= stackAlign;
        }

        if (Offset != 0) {
            MI.getOperand(FIOperandNum).ChangeToRegister(
                TCE::KLUDGE_REGISTER, false, false, true/*iskill*/);
            if (Offset > 0) {
                BuildMI(
                    *MI.getParent(), II, MI.getDebugLoc(), TII.get(TCE::ADDrri),
                    TCE::KLUDGE_REGISTER).addReg(TCE::FP).addImm(Offset);
            } else { // for negative offsets used sub, not add
                BuildMI(
                    *MI.getParent(), II, MI.getDebugLoc(), TII.get(TCE::SUBrri),
                    TCE::KLUDGE_REGISTER).addReg(TCE::FP).addImm(-Offset);
            }
        } else {
            MI.getOperand(FIOperandNum).ChangeToRegister(TCE::FP, false);
        }
    } else {
        int Offset =
            frameinfo.getObjectOffset(FrameIndex) + frameinfo.getStackSize();

        if (Offset != 0) {
            MI.getOperand(FIOperandNum).ChangeToRegister(TCE::KLUDGE_REGISTER, false);
            BuildMI(
                *MI.getParent(), II, MI.getDebugLoc(), TII.get(TCE::ADDrri),
                TCE::KLUDGE_REGISTER).addReg(TCE::SP).addImm(Offset);
        } else {
            MI.getOperand(FIOperandNum).ChangeToRegister(TCE::SP, false);
        }
    }
}

unsigned
TCERegisterInfo::getRARegister() const {
    assert(false && "Remove this assert if this is really called.");
    return TCE::RA;
}

#ifdef LLVM_OLDER_THAN_9
unsigned
#else
Register
#endif
TCERegisterInfo::getFrameRegister(const MachineFunction& mf) const {
    if (hasFP(mf)) {
        return TCE::FP;
    } else {
        return 0;
    }
}

bool TCERegisterInfo::hasFP(const MachineFunction &MF) const {
    return tfi_->hasFP(MF);
}
