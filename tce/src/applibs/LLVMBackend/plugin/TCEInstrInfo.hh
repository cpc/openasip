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
 * @file TCEInstrInfo.h
 *
 * Declaration of TCEInstrInfo class.
 *
 * @author Veli-Pekka Jaaskelainen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Heikki Kultala 2011 (heikki.kultala-no.spam-tut.fi)
 */

#ifndef TCE_INSTR_INFO_H
#define TCE_INSTR_INFO_H

#include <llvm/Support/ErrorHandling.h>
#include "tce_config.h"
#ifdef LLVM_OLDER_THAN_6_0
#include <llvm/Target/TargetInstrInfo.h>
#else
#include <llvm/CodeGen/TargetInstrInfo.h>
#endif
#include "TCERegisterInfo.hh"

#define GET_INSTRINFO_HEADER
#include "TCEGenInstrInfo.inc"

namespace llvm {

    class TCETargetMachine;
    class TCETargetMachinePlugin;
    /** !! Important !! *************
     * ON EVERY LLVM UPDATE CHECK THESE INTERFACES VERY CAREFULLY
     * FROM include/llvm/Target/TargetInstrInfo.h
     *
     * Compiler doesn warn or give error if parameter lists are changed.
     * Many times also base class implementation works, but does not do
     * very good job.
     */

    class TCEInstrInfo : public TCEGenInstrInfo {
    public:
        TCEInstrInfo(const TCETargetMachinePlugin* plugin);
        virtual ~TCEInstrInfo();

        virtual const TargetRegisterInfo& getRegisterInfo() const { 
            return ri_; 
        }

#ifdef LLVM_OLDER_THAN_4_0
        virtual unsigned InsertBranch(
#else
        virtual unsigned insertBranch(
#endif
            MachineBasicBlock &MBB, MachineBasicBlock *TBB,
            MachineBasicBlock *FBB,
            ArrayRef<MachineOperand> Cond,
#ifdef LLVM_OLDER_THAN_3_9
            DebugLoc DL
#else
            const DebugLoc& DL
#endif
#ifdef LLVM_OLDER_THAN_4_0
        ) const override;
#else
        , int *BytesAdded = nullptr) const override;
#endif


#ifdef LLVM_OLDER_THAN_4_0
        unsigned RemoveBranch(MachineBasicBlock &mbb) const override;
#else
        unsigned removeBranch(
            MachineBasicBlock &mbb,
            int *BytesRemoved = nullptr) const override;
#endif

        virtual bool BlockHasNoFallThrough(
            const MachineBasicBlock &MBB) const;

        virtual void storeRegToStackSlot(
            MachineBasicBlock& mbb,
            MachineBasicBlock::iterator mbbi,
            unsigned srcReg, bool isKill, int frameIndex,
            const TargetRegisterClass* rc) const;

        // changed in LLVM 2.8:
        virtual void storeRegToStackSlot(
            MachineBasicBlock& mbb,
            MachineBasicBlock::iterator mbbi,
            unsigned srcReg, bool isKill, int frameIndex,
            const TargetRegisterClass* rc, const TargetRegisterInfo*) const override {
            storeRegToStackSlot(mbb, mbbi, srcReg, isKill, frameIndex, rc);
        }

        // TODO: this is in the form of the llvm 2.7 version of this method.
        // this is however called by the newer version of the function.
        virtual void loadRegFromStackSlot(
            MachineBasicBlock& mbb,
            MachineBasicBlock::iterator mbbi,
            unsigned destReg, int frameIndex,
            const TargetRegisterClass* rc) const;

        // changed in LLVM 2.8:
        virtual void loadRegFromStackSlot(
            MachineBasicBlock& mbb,
            MachineBasicBlock::iterator mbbi,
            unsigned destReg, int frameIndex,
            const TargetRegisterClass* rc, const TargetRegisterInfo*) const override {
            loadRegFromStackSlot(mbb, mbbi, destReg, frameIndex, rc);
        }

    virtual void copyPhysReg(
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
        bool KillSrc) const override;

#if LLVM_OLDER_THAN_4_0
        virtual bool ReverseBranchCondition(
#else
        virtual bool reverseBranchCondition(
#endif
            llvm::SmallVectorImpl<llvm::MachineOperand>& cond) const override;

#ifdef LLVM_OLDER_THAN_3_9
        virtual bool AnalyzeBranch(
#else
        virtual bool analyzeBranch(
#endif
            MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
            MachineBasicBlock *&FBB, 
            llvm::SmallVectorImpl<llvm::MachineOperand>& cond,
            bool allowModify = false)
            const override;

#ifdef LLVM_OLDER_THAN_3_9
    virtual bool isPredicated(const MachineInstr *MI) const override;
    virtual bool isPredicable(MachineInstr *MI) const override;
#else
    virtual bool isPredicated(const MachineInstr& MI) const override;
#ifdef LLVM_OLDER_THAN_5_0
    virtual bool isPredicable(MachineInstr& MI) const override;
#else
    virtual bool isPredicable(const MachineInstr& MI) const override;
#endif
#endif

#if defined LLVM_OLDER_THAN_3_9
    virtual bool PredicateInstruction(
        MachineInstr *mi,
        ArrayRef<MachineOperand> cond) const override;
#else
    virtual bool PredicateInstruction(
        MachineInstr &mi,
        ArrayRef<MachineOperand> cond) const override;
#endif

    virtual bool DefinesPredicate(
#ifdef LLVM_OLDER_THAN_3_9
        MachineInstr *MI,
#else
        MachineInstr& MI,
#endif
        std::vector<MachineOperand> &Pred) const override;

	virtual bool
	SubsumesPredicate(ArrayRef<MachineOperand> Pred1,
                      ArrayRef<MachineOperand> Pred2) const override {
	    return false;
	}

#ifdef LLVM_OLDER_THAN_3_8
    virtual bool isProfitableToIfCvt(MachineBasicBlock &MBB, unsigned NumCycles,
                     unsigned ExtraPredCycles,
                     const BranchProbability &Probability) const override;

    virtual bool isProfitableToIfCvt(MachineBasicBlock &TMBB,
                     unsigned NumTCycles, unsigned ExtraTCycles,
                     MachineBasicBlock &FMBB,
                     unsigned NumFCycles, unsigned ExtraFCycles,
                     const BranchProbability &Probability) const override;
#else
    virtual bool isProfitableToIfCvt(MachineBasicBlock &MBB, unsigned NumCycles,
                     unsigned ExtraPredCycles,
                     BranchProbability Probability) const override;

    virtual bool isProfitableToIfCvt(MachineBasicBlock &TMBB,
                     unsigned NumTCycles, unsigned ExtraTCycles,
                     MachineBasicBlock &FMBB,
                     unsigned NumFCycles, unsigned ExtraFCycles,
                     BranchProbability Probability) const override;
#endif

    private:
	int getMatchingCondBranchOpcode(int Opc, bool inverted) const;

        const TCERegisterInfo ri_;
        const TCETargetMachinePlugin* plugin_;
    };
}

#endif
