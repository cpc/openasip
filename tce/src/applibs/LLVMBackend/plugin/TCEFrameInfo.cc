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
 * @file TCEFrameInfo.cpp
 *
 * Implementation of TCEFrameLowering class.
 *
 * @author Heikki Kultala 2010-2016 (hkultala-no.spam-cs.tut.fi)
 */

#include "TCEFrameInfo.hh"
#include <llvm/IR/Function.h>
#include <llvm/CodeGen/MachineFrameInfo.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>

#include <iostream> // DEBUG

#include "TCEPlugin.hh" // this includes the .inc files

using namespace llvm;

#include "TCEString.hh"

/**
 * Emits machine function prologue to machine functions.
 */

#ifdef LLVM_OLDER_THAN_3_9
#define ERASE_INSTR_AND_RETURN(I)                \
    MBB.erase(I);                                \
    return
#else
#define ERASE_INSTR_AND_RETURN(I) return MBB.erase(I)
#endif

/**
 * Eliminates call frame pseudo instructions.
 *
 * Stack space is already reserved in caller stack.
 */
#ifdef LLVM_OLDER_THAN_3_9
void
#else
MachineBasicBlock::iterator
#endif
TCEFrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator I) const {
    if (hasFP(MF)) {
        int opc = I->getOpcode();
        // convert stack down to sub
        if (opc == TCE::ADJCALLSTACKDOWN) {
#ifdef LLVM_OLDER_THAN_5_0
            MachineOperand mo1 = I->getOperand(1);
            MachineOperand mo2 = I->getOperand(2);
#else
            MachineOperand mo1 = I->getOperand(2);
            MachineOperand mo2 = I->getOperand(3);
#endif
            long val = I->getOperand(0).getImm();

            if (val == 0) {
                ERASE_INSTR_AND_RETURN(I);
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
                ERASE_INSTR_AND_RETURN(I);
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
        ERASE_INSTR_AND_RETURN(I);
    }
#ifndef LLVM_OLDER_THAN_3_9
    // LLVM 3.9 wants an iterator pointing to the instruction after 
    // the replaced one.
    return I++;
#endif
}
#undef ERASE_INSTR_AND_RETURN

bool TCEFrameLowering::hasFP(const MachineFunction &MF) const {
#if LLVM_OLDER_THAN_4_0
    if (MF.getFrameInfo()->hasVarSizedObjects()) {
#else
    if (MF.getFrameInfo().hasVarSizedObjects()) {
#endif
        return true;
    }
    return false;
}

bool
TCEFrameLowering::containsCall(MachineFunction& mf) const {
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
TCEFrameLowering::emitPrologue(MachineFunction& mf, MachineBasicBlock &MBB)
 const {
    MachineBasicBlock& mbb = mf.front();
#if LLVM_OLDER_THAN_4_0
    MachineFrameInfo& mfi = *mf.getFrameInfo();
#else
    MachineFrameInfo& mfi = mf.getFrameInfo();
#endif
    int numBytes = (int)mfi.getStackSize();

    // this unfortunately return true for inline asm.
    bool hasCalls = mfi.hasCalls();
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
#ifdef LITTLE_ENDIAN_TARGET
        BuildMI(mbb, ii, dl, tii_.get(TCE::ST32RArr))
            .addReg(TCE::SP)
            .addImm(0)
            .addReg(TCE::RA)
            .setMIFlag(MachineInstr::FrameSetup);
#else
        BuildMI(mbb, ii, dl, tii_.get(TCE::STWRArr))
            .addReg(TCE::SP)
            .addImm(0)
            .addReg(TCE::RA)
            .setMIFlag(MachineInstr::FrameSetup);
#endif
        numBytes += stackAlignment_;

        // Create metadata which says that this is an RA save
        MachineBasicBlock::iterator raStore = ii; raStore--;
#ifdef LLVM_OLDER_THAN_6_0
        LLVMContext& context = mbb.getParent()->getFunction()->getContext();
#else
        LLVMContext& context = mbb.getParent()->getFunction().getContext();
#endif
        llvm::Metadata* md =
            llvm::MDString::get(context, "AA_CATEGORY_RA_SAVE_SLOT");
        MDNode* mdNode =
            MDNode::get(context, llvm::ArrayRef<llvm::Metadata*>(&md, 1));

        MachineOperand metaDataOperand = MachineOperand::CreateMetadata(mdNode);
        raStore->addOperand(metaDataOperand);
    }

    if (hasFP(mf)) {
        // only need to save old FP if this function may return
#ifdef LLVM_OLDER_THAN_6_0
        if (!mf.getFunction()->doesNotReturn()) {
#else
        if (!mf.getFunction().doesNotReturn()) {
#endif
            BuildMI(mbb, ii, dl, tii_.get(TCE::SUBrri), TCE::SP)
                .addReg(TCE::SP)
                .addImm(stackAlignment_);

#ifdef LITTLE_ENDIAN_TARGET
            BuildMI(mbb, ii, dl, tii_.get(TCE::ST32rr))
                .addReg(TCE::SP)
                .addImm(0)
                .addReg(TCE::FP)
                .setMIFlag(MachineInstr::FrameSetup);
#else
            BuildMI(mbb, ii, dl, tii_.get(TCE::STWrr))
                .addReg(TCE::SP)
                .addImm(0)
                .addReg(TCE::FP)
                .setMIFlag(MachineInstr::FrameSetup);
#endif
            numBytes += stackAlignment_;
            // Create metadata which says that this is an FP save
            MachineBasicBlock::iterator fpStore = ii; fpStore--;
#ifdef LLVM_OLDER_THAN_6_0
            LLVMContext& context = mbb.getParent()->getFunction()->getContext();
#else
            LLVMContext& context = mbb.getParent()->getFunction().getContext();
#endif
            llvm::Metadata* md =
                llvm::MDString::get(context, "AA_CATEGORY_FP_SAVE_SLOT");
            MDNode* mdNode =
                MDNode::get(context, llvm::ArrayRef<llvm::Metadata*>(&md, 1));

            MachineOperand metaDataOperand = MachineOperand::CreateMetadata(mdNode);
            fpStore->addOperand(metaDataOperand);
        }
        // if FP used by this function, move SP to FP
        BuildMI(mbb, ii, dl, tii_.get(TCE::MOVI32rr), TCE::FP).addReg(TCE::SP)
            .setMIFlag(MachineInstr::FrameSetup);
    }

    mfi.setStackSize(numBytes);

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
TCEFrameLowering::emitEpilogue(
    MachineFunction& mf, MachineBasicBlock& mbb) const {

#if LLVM_OLDER_THAN_4_0
    MachineFrameInfo& mfi = *mf.getFrameInfo();
#else
    MachineFrameInfo& mfi = mf.getFrameInfo();
#endif

    MachineBasicBlock::iterator mbbi = std::prev(mbb.end());

    DebugLoc dl = mbbi->getDebugLoc();

    if (mbbi->getOpcode() != TCE::RETL) {
        assert(false && "ERROR: Inserting epilogue w/o return?");
    }

    unsigned numBytes = mfi.getStackSize();
    unsigned varBytes = numBytes;

    if (hasFP(mf)) {
        varBytes -= stackAlignment_;
    }

    // this unfortunately return true for inline asm.
    bool hasCalls = mfi.hasCalls();
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
#ifdef LITTLE_ENDIAN_TARGET
        BuildMI(mbb, mbbi, dl, tii_.get(TCE::LD32rr), TCE::FP)
            .addReg(TCE::FP)
            .addImm(0)
            .setMIFlag(MachineInstr::FrameSetup);
#else
        BuildMI(mbb, mbbi, dl, tii_.get(TCE::LDWrr), TCE::FP)
            .addReg(TCE::FP)
            .addImm(0)
            .setMIFlag(MachineInstr::FrameSetup);
#endif

        // Create metadata which says that this is an FP load
        MachineBasicBlock::iterator fpLoad = mbbi; fpLoad--;

#ifdef LLVM_OLDER_THAN_6_0
        LLVMContext& context =
            mbb.getParent()->getFunction()->getContext();
#else
        LLVMContext& context =
            mbb.getParent()->getFunction().getContext();
#endif

        llvm::Metadata* md = llvm::MDString::get(context, "AA_CATEGORY_FP_SAVE_SLOT");
        MDNode* mdNode = MDNode::get(context, llvm::ArrayRef<llvm::Metadata*>(&md, 1));

        MachineOperand metaDataOperand = MachineOperand::CreateMetadata(mdNode);
        fpLoad->addOperand(metaDataOperand);

        // then the SP adjust
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
#ifdef LITTLE_ENDIAN_TARGET
        BuildMI(mbb, mbbi, dl, tii_.get(TCE::LD32RAr), TCE::RA)
            .addReg(TCE::SP)
            .addImm(0)
            .setMIFlag(MachineInstr::FrameSetup);
#else
        BuildMI(mbb, mbbi, dl, tii_.get(TCE::LDWRAr), TCE::RA)
            .addReg(TCE::SP)
            .addImm(0)
            .setMIFlag(MachineInstr::FrameSetup);
#endif

        // Create metadata which says that this is an RA load
        MachineBasicBlock::iterator raLoad = mbbi; raLoad--;


#ifdef LLVM_OLDER_THAN_6_0
        LLVMContext& context =
            mbb.getParent()->getFunction()->getContext();
#else
        LLVMContext& context =
            mbb.getParent()->getFunction().getContext();
#endif

        llvm::Metadata* md = llvm::MDString::get(context, "AA_CATEGORY_RA_SAVE_SLOT");
        MDNode* mdNode = MDNode::get(context, llvm::ArrayRef<llvm::Metadata*>(&md, 1));

        MachineOperand metaDataOperand = MachineOperand::CreateMetadata(mdNode);
        raLoad->addOperand(metaDataOperand);

        // then the SP adjust
        BuildMI(mbb, mbbi, dl, tii_.get(TCE::ADDrri), TCE::SP)
            .addReg(TCE::SP)
            .addImm(stackAlignment_);
    }
}


