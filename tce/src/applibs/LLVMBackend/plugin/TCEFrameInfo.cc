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
 * @file TCEFrameInfo.cpp
 *
 * Implementation of TCEFrameInfo class.
 *
 * @author Heikki Kultala 2010-2016 (hkultala-no.spam-cs.tut.fi)
 */

#include "TCEFrameInfo.hh"
#include <llvm/IR/Function.h>
#include <llvm/CodeGen/MachineFrameInfo.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>

#include <iostream> // DEBUG

#define GET_INSTRINFO_ENUM
#include "TCEGenInstrInfo.inc"

#define GET_REGINFO_ENUM
#include "TCEGenRegisterInfo.inc"

using namespace llvm;

#include "TCEString.hh"

/**
 * Emits machine function prologue to machine functions.
 */

/**
 * Eliminates call frame pseudo instructions.
 *
 * Stack space is already reserved in caller stack.
 */
void
TCEFrameInfo::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator I) const {
    if (hasFP(MF)) {
        int opc = I->getOpcode();
        // convert stack down to sub
        if (opc == TCE::ADJCALLSTACKDOWN) {
            MachineOperand mo1 = I->getOperand(1);
            MachineOperand mo2 = I->getOperand(2);
            long val = I->getOperand(0).getImm();
            if (val == 0) {
                MBB.erase(I);
                return ;
            }
            I->setDesc(tii_.get(TCE::SUBrri));
            I->getOperand(0).ChangeToRegister(mo1.getReg(), mo1.isDef(),
                                              false/*mo.isImplicit()*/, mo1.isKill(),
                                              false/*dead*/, false/*undef*/,
                                              mo1.isDebug());
            I->getOperand(1).ChangeToRegister(mo2.getReg(), false, false, mo2.isKill(),
                                              false, false, mo2.isDebug());
            I->getOperand(2).ChangeToImmediate(val);

        // convert stack up to add
        } else if (opc == TCE::ADJCALLSTACKUP) {
            MachineOperand mo1 = I->getOperand(2);
            MachineOperand mo2 = I->getOperand(3);
            long val = I->getOperand(0).getImm();
            if (val == 0) {
                MBB.erase(I);
                return ;
            }
            I->setDesc(tii_.get(TCE::ADDrri));
            I->getOperand(0).ChangeToRegister(mo1.getReg(), mo1.isDef(),
                                              false/*mo.isImplicit()*/, mo1.isKill(),
                                              false/*dead*/, false/*undef*/,
                                              mo1.isDebug());
            I->getOperand(1).ChangeToRegister(mo2.getReg(), false, false, mo2.isKill(),
                                              false, false, mo2.isDebug());
            I->getOperand(2).ChangeToImmediate(val);
            I->RemoveOperand(3);
        }
    } else {
        MBB.erase(I);
    }
}


bool TCEFrameInfo::hasFP(const MachineFunction &MF) const {
    if (MF.getFrameInfo()->hasVarSizedObjects()) {
        assert(false && "var objects not yet supported");
        return true;
    }
    return false;
}

bool
TCEFrameInfo::containsCall(MachineFunction& mf) const {
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

/**
 * Emits machine function prologue to machine functions.
 */
void
#ifdef LLVM_OLDER_THAN_3_7
TCEFrameInfo::emitPrologue(MachineFunction& mf)
#else
TCEFrameInfo::emitPrologue(MachineFunction& mf, MachineBasicBlock &MBB)
#endif
 const {
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

    // stack size alignment
    numBytes = (numBytes + (stackAlignment_-1)) & ~(stackAlignment_-1);

    // stack size without RA/FP storage
    int varBytes = numBytes;

    MachineBasicBlock::iterator ii = mbb.begin();

    DebugLoc dl = (ii != mbb.end() ?
                   ii->getDebugLoc() : DebugLoc());

    // No need to save RA if does not call anything or does no return
    // if (hasCalls && !mf.getFunction()->doesNotReturn()) {
    // However, there is a bug elsewhere and this triggers it.
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

        numBytes += stackAlignment_;
    }

    if (hasFP(mf)) {
        // only need to save old FP if this function may return
        if (!mf.getFunction()->doesNotReturn()) {
            BuildMI(mbb, ii, dl, tii_.get(TCE::SUBrri), TCE::SP)
                .addReg(TCE::SP)
                .addImm(stackAlignment_);

            BuildMI(mbb, ii, dl, tii_.get(TCE::STWrr))
                .addReg(TCE::SP)
                .addImm(0)
                .addReg(TCE::FP)
                .setMIFlag(MachineInstr::FrameSetup);

            numBytes += stackAlignment_;
        }
        // if FP used by this function, move SP to FP
        BuildMI(mbb, ii, dl, tii_.get(TCE::MOVI32rr), TCE::FP).addReg(TCE::SP)
            .setMIFlag(MachineInstr::FrameSetup);
    }

    mfi->setStackSize(numBytes);

    // Adjust stack pointer
    if (varBytes != 0) {
        BuildMI(mbb, ii, dl, tii_.get(TCE::SUBrri), TCE::SP)
            .addReg(TCE::SP)
            .addImm(varBytes);
    }
}

/**
 * Emits machine function epilogue to machine functions.
 */
void
TCEFrameInfo::emitEpilogue(
    MachineFunction& mf, MachineBasicBlock& mbb) const {
    MachineFrameInfo* mfi = mf.getFrameInfo();

    MachineBasicBlock::iterator mbbi = std::prev(mbb.end());

    DebugLoc dl = mbbi->getDebugLoc();

    if (mbbi->getOpcode() != TCE::RETL && mbbi->getOpcode() != TCE::RETL_old) {
        assert(false && "ERROR: Inserting epilogue w/o return?");
    }

    unsigned numBytes = mfi->getStackSize();
    unsigned varBytes = numBytes;

    if (hasFP(mf)) {
        varBytes -= stackAlignment_;
    }

    // this unfortunately return true for inline asm.
    bool hasCalls = mfi->hasCalls();
    if (hasCalls) {
        // so then check again. Return false if only inline asm, no calls.
        hasCalls = containsCall(mf);
        if (hasCalls) {
            varBytes -= stackAlignment_;
        }
    }

    if (hasFP(mf)) {
        // move FP to SP
        BuildMI(mbb, mbbi, dl, tii_.get(TCE::MOVI32rr), TCE::SP)
            .addReg(TCE::FP)
            .setMIFlag(MachineInstr::FrameSetup);

        // restore old FP from stack
        BuildMI(mbb, mbbi, dl, tii_.get(TCE::LDWrr), TCE::FP)
            .addReg(TCE::FP)
            .addImm(0)
            .setMIFlag(MachineInstr::FrameSetup);

        BuildMI(mbb, mbbi, dl, tii_.get(TCE::ADDrri), TCE::SP)
            .addReg(TCE::SP)
            .addImm(stackAlignment_);
    } else {
        // no FP
        if (varBytes) {
            BuildMI(mbb, mbbi, dl, tii_.get(TCE::ADDrri), TCE::SP)
                .addReg(TCE::SP)
                .addImm(varBytes);
        }
    }

    if (hasCalls) {
        // Restore RA from stack.
        BuildMI(mbb, mbbi, dl, tii_.get(TCE::LDWRAr), TCE::RA)
            .addReg(TCE::SP)
            .addImm(0)
            .setMIFlag(MachineInstr::FrameSetup);

        BuildMI(mbb, mbbi, dl, tii_.get(TCE::ADDrri), TCE::SP)
            .addReg(TCE::SP)
            .addImm(stackAlignment_);
    }
}
