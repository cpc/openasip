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

#include "TCEPlugin.hh" // this includes the .inc files

using namespace llvm;

#include "TCEString.hh"
#include "Application.hh"
#include "LLVMTCECmdLineOptions.hh"

/**
 * Emits machine function prologue to machine functions.
 */


// Some LLVM 3.9 function wants iterator after the erased instruction.
#define ERASE_INSTR_AND_RETURN(I) return MBB.erase(I)

#ifdef TARGET64BIT
#define ADDIMM TCE::ADD64ssa
#define SUBIMM TCE::SUB64ssa
#define STREG  TCE::ST64ss
#define LDREG  TCE::LD64ss
#define LDRA   TCE::LD64RAs
#define STRA   TCE::ST64RAss
#define MOVREG TCE::MOV64ss
#else
#define ADDIMM TCE::ADDrri
#define SUBIMM TCE::SUBrri
#define MOVREG TCE::MOVI32rr
#ifdef LITTLE_ENDIAN_TARGET
#define STREG  TCE::ST32rr
#define LDREG  TCE::LD32rr
#define LDRA   TCE::LD32RAr
#define STRA   TCE::ST32RArr
#else // big-endian
#define STREG  TCE::STWrr
#define LDREG  TCE::LDWrr
#define LDRA   TCE::LDWRAr
#define STRA   TCE::STWRArr
#endif
#endif


/**
 * Eliminates call frame pseudo instructions.
 *
 * Stack space is already reserved in caller stack.
 */
MachineBasicBlock::iterator
TCEFrameLowering::eliminateCallFramePseudoInstr(
    MachineFunction &MF, MachineBasicBlock &MBB,
    MachineBasicBlock::iterator I) const {
    if (hasFP(MF)) {
        int opc = I->getOpcode();
        // convert stack down to sub
        if (opc == TCE::ADJCALLSTACKDOWN) {
            MachineOperand mo1 = I->getOperand(2);
            MachineOperand mo2 = I->getOperand(3);
            long val = I->getOperand(0).getImm();

            if (val == 0) {
                ERASE_INSTR_AND_RETURN(I);
            }
            auto spOpcAndOffset = tii_.getPointerAdjustment(-val);
            I->setDesc(tii_.get(std::get<0>(spOpcAndOffset)));
            I->getOperand(0).ChangeToRegister(mo1.getReg(), mo1.isDef(),
                                              false/*mo.isImplicit()*/, mo1.isKill(),
                                              false/*dead*/, false/*undef*/,
                                              mo1.isDebug());
            I->getOperand(1).ChangeToRegister(mo2.getReg(), false, false, mo2.isKill(),
                                              false, false, mo2.isDebug());
            I->getOperand(2).ChangeToImmediate(std::get<1>(spOpcAndOffset));

        // convert stack up to add
        } else if (opc == TCE::ADJCALLSTACKUP) {
            MachineOperand mo1 = I->getOperand(2);
            MachineOperand mo2 = I->getOperand(3);
            long val = I->getOperand(0).getImm();
            if (val == 0) {
                ERASE_INSTR_AND_RETURN(I);
            }
            I->setDesc(tii_.get(ADDIMM));
            I->getOperand(0).ChangeToRegister(mo1.getReg(), mo1.isDef(),
                                              false/*mo.isImplicit()*/, mo1.isKill(),
                                              false/*dead*/, false/*undef*/,
                                              mo1.isDebug());
            I->getOperand(1).ChangeToRegister(mo2.getReg(), false, false, mo2.isKill(),
                                              false, false, mo2.isDebug());
            I->getOperand(2).ChangeToImmediate(val);
            #ifdef LLVM_OLDER_THAN_15
            I->RemoveOperand(3);
            #else
            I->removeOperand(3);
            #endif
        }
    } else {
        ERASE_INSTR_AND_RETURN(I);
    }
    return I;
}
#undef ERASE_INSTR_AND_RETURN

bool TCEFrameLowering::hasFP(const MachineFunction &MF) const {
    if (MF.getFrameInfo().hasVarSizedObjects()) {
        return true;
    }
    return false;
}

bool
TCEFrameLowering::containsCall(const MachineFunction& mf) const {
    for (MachineFunction::const_iterator i = mf.begin(); i != mf.end(); i++) {
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
    MachineFrameInfo& mfi = mf.getFrameInfo();
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
        auto spOpcAndOffset = tii_.getPointerAdjustment(-stackAlignment_);
        BuildMI(mbb, ii, dl, tii_.get(std::get<0>(spOpcAndOffset)), TCE::SP)
            .addReg(TCE::SP)
            .addImm(std::get<1>(spOpcAndOffset));

        // Save RA to stack.
        BuildMI(mbb, ii, dl, tii_.get(STRA))
            .addReg(TCE::SP)
            .addImm(0)
            .addReg(TCE::RA)
            .setMIFlag(MachineInstr::FrameSetup);
        numBytes += stackAlignment_;

        // Create metadata which says that this is an RA save
        MachineBasicBlock::iterator raStore = ii; raStore--;
        LLVMContext& context = mbb.getParent()->getFunction().getContext();
        llvm::Metadata* md =
            llvm::MDString::get(context, "AA_CATEGORY_RA_SAVE_SLOT");
        MDNode* mdNode =
            MDNode::get(context, llvm::ArrayRef<llvm::Metadata*>(&md, 1));

        MachineOperand metaDataOperand = MachineOperand::CreateMetadata(mdNode);
        raStore->addOperand(metaDataOperand);
    }

    if (hasFP(mf)) {
        // only need to save old FP if this function may return
        if (!mf.getFunction().doesNotReturn()) {
            auto spOpcAndOffset = tii_.getPointerAdjustment(-stackAlignment_);
            BuildMI(mbb, ii, dl, tii_.get(std::get<0>(spOpcAndOffset)),
                    TCE::SP)
                .addReg(TCE::SP)
                .addImm(std::get<1>(spOpcAndOffset));

            BuildMI(mbb, ii, dl, tii_.get(STREG))
                .addReg(TCE::SP)
                .addImm(0)
                .addReg(TCE::FP)
                .setMIFlag(MachineInstr::FrameSetup);

            numBytes += stackAlignment_;
            // Create metadata which says that this is an FP save
            MachineBasicBlock::iterator fpStore = ii; fpStore--;
            LLVMContext& context = mbb.getParent()->getFunction().getContext();
            llvm::Metadata* md =
                llvm::MDString::get(context, "AA_CATEGORY_FP_SAVE_SLOT");
            MDNode* mdNode =
                MDNode::get(context, llvm::ArrayRef<llvm::Metadata*>(&md, 1));

            MachineOperand metaDataOperand = MachineOperand::CreateMetadata(mdNode);
            fpStore->addOperand(metaDataOperand);
        }
        // if FP used by this function, move SP to FP
        BuildMI(mbb, ii, dl, tii_.get(MOVREG), TCE::FP).addReg(TCE::SP)
            .setMIFlag(MachineInstr::FrameSetup);
    }

    mfi.setStackSize(numBytes);

    // Adjust stack pointer
    if (varBytes != 0) {
        auto spOpcAndOffset = tii_.getPointerAdjustment(-varBytes);
        BuildMI(mbb, ii, dl, tii_.get(std::get<0>(spOpcAndOffset)), TCE::SP)
            .addReg(TCE::SP)
            .addImm(std::get<1>(spOpcAndOffset));
    }
}

/**
 * Emits machine function epilogue to machine functions.
 */
void
TCEFrameLowering::emitEpilogue(
    MachineFunction& mf, MachineBasicBlock& mbb) const {

    MachineFrameInfo& mfi = mf.getFrameInfo();

    MachineBasicBlock::iterator mbbi = std::prev(mbb.end());

    DebugLoc dl = mbbi->getDebugLoc();

    if (mbbi->getOpcode() != TCE::RETL) {
        assert(false && "ERROR: Inserting epilogue w/o return?");
    }

    unsigned numBytes = mfi.getStackSize();
    numBytes = (numBytes + (stackAlignment_-1)) & ~(stackAlignment_-1);
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
        BuildMI(mbb, mbbi, dl, tii_.get(MOVREG), TCE::SP)
            .addReg(TCE::FP)
            .setMIFlag(MachineInstr::FrameSetup);

        // restore old FP from stack
        BuildMI(mbb, mbbi, dl, tii_.get(LDREG), TCE::FP)
            .addReg(TCE::FP)
            .addImm(0)
            .setMIFlag(MachineInstr::FrameSetup);

        // Create metadata which says that this is an FP load
        MachineBasicBlock::iterator fpLoad = mbbi; fpLoad--;

        LLVMContext& context =
            mbb.getParent()->getFunction().getContext();

        llvm::Metadata* md = llvm::MDString::get(context, "AA_CATEGORY_FP_SAVE_SLOT");
        MDNode* mdNode = MDNode::get(context, llvm::ArrayRef<llvm::Metadata*>(&md, 1));

        MachineOperand metaDataOperand = MachineOperand::CreateMetadata(mdNode);
        fpLoad->addOperand(metaDataOperand);

        // then the SP adjust
        BuildMI(mbb, mbbi, dl, tii_.get(ADDIMM), TCE::SP)
            .addReg(TCE::SP)
            .addImm(stackAlignment_);
    } else {
        // no FP
        if (varBytes) {
            BuildMI(mbb, mbbi, dl, tii_.get(ADDIMM), TCE::SP)
                .addReg(TCE::SP)
                .addImm(varBytes);
        }
    }

    if (hasCalls) {
        // Restore RA from stack.
        BuildMI(mbb, mbbi, dl, tii_.get(LDRA), TCE::RA)
            .addReg(TCE::SP)
            .addImm(0)
            .setMIFlag(MachineInstr::FrameSetup);
        // Create metadata which says that this is an RA load
        MachineBasicBlock::iterator raLoad = mbbi; raLoad--;

        LLVMContext& context =
            mbb.getParent()->getFunction().getContext();

        llvm::Metadata* md = llvm::MDString::get(context, "AA_CATEGORY_RA_SAVE_SLOT");
        MDNode* mdNode = MDNode::get(context, llvm::ArrayRef<llvm::Metadata*>(&md, 1));

        MachineOperand metaDataOperand = MachineOperand::CreateMetadata(mdNode);
        raLoad->addOperand(metaDataOperand);

        // then the SP adjust
        BuildMI(mbb, mbbi, dl, tii_.get(ADDIMM), TCE::SP)
            .addReg(TCE::SP)
            .addImm(stackAlignment_);
    }
}



