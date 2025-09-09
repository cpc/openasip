/*
    Copyright (c) 2002-2025 Tampere University.

    This file is part of OpenASIP.

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
#include <llvm/CodeGen/TargetInstrInfo.h>
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

        const InstrItineraryData *
        getInstrItineraryData() const {
            return &InstrItins;
        }

        virtual const TargetRegisterInfo& getRegisterInfo() const { 
            return ri_; 
        }

        virtual unsigned insertBranch(
            MachineBasicBlock &MBB, MachineBasicBlock *TBB,
            MachineBasicBlock *FBB,
            ArrayRef<MachineOperand> Cond,
            const DebugLoc& DL
        , int *BytesAdded = nullptr) const override;
        unsigned removeBranch(
            MachineBasicBlock &mbb,
            int *BytesRemoved = nullptr) const override;

        virtual bool BlockHasNoFallThrough(
            const MachineBasicBlock &MBB) const;

        virtual void storeRegToStackSlot(
            MachineBasicBlock& mbb,
            MachineBasicBlock::iterator mbbi,
            unsigned srcReg, bool isKill, int frameIndex,
            const TargetRegisterClass* rc, Register vReg) const;

        virtual void storeRegToStackSlot(
            MachineBasicBlock& mbb,
            MachineBasicBlock::iterator mbbi,
            Register srcReg, bool isKill, int frameIndex,
            const TargetRegisterClass* rc, const TargetRegisterInfo*,
            Register vReg
#if LLVM_MAJOR_VERSION >= 21
            , MachineInstr::MIFlag Flags = MachineInstr::NoFlags
#endif
          ) const override {
            storeRegToStackSlot(mbb, mbbi, srcReg, isKill, frameIndex, rc, 0);
        }

        // TODO: this is in the form of the llvm 2.7 version of this method.
        // this is however called by the newer version of the function.
        virtual void loadRegFromStackSlot(
            MachineBasicBlock& mbb,
            MachineBasicBlock::iterator mbbi,
            unsigned destReg, int frameIndex,
            const TargetRegisterClass* rc, Register vReg) const;

        virtual void loadRegFromStackSlot(
            MachineBasicBlock& mbb,
            MachineBasicBlock::iterator mbbi,
            Register destReg, int frameIndex,
            const TargetRegisterClass* rc, const TargetRegisterInfo*,
            Register vReg
#if LLVM_MAJOR_VERSION >= 21
            , MachineInstr::MIFlag Flags = MachineInstr::NoFlags
#endif
          ) const override {
            loadRegFromStackSlot(mbb, mbbi, destReg, frameIndex, rc, 0);
        }

#if LLVM_MAJOR_VERSION >= 21
        virtual void copyPhysReg(
          MachineBasicBlock& mbb,
          MachineBasicBlock::iterator mbbi,
          const DebugLoc& DL,
          Register destReg, Register srcReg,
          bool killSrc,
          bool renamableDest = false,
          bool renamableSrc = false) const override;
#else
        virtual void copyPhysReg(
          MachineBasicBlock& mbb,
          MachineBasicBlock::iterator mbbi,
          const DebugLoc& DL,
          MCRegister destReg, MCRegister srcReg,
          bool killSrc) const override;
#endif
        virtual bool reverseBranchCondition(
            llvm::SmallVectorImpl<llvm::MachineOperand>& cond) const override;

        virtual bool analyzeBranch(
            MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
            MachineBasicBlock *&FBB,
            llvm::SmallVectorImpl<llvm::MachineOperand>& cond,
            bool allowModify = false)
            const override;

        /// Analyze loop L, which must be a single-basic-block loop, and if
        /// the conditions can be understood enough produce a
        /// PipelinerLoopInfo object.
        std::unique_ptr<PipelinerLoopInfo> analyzeLoopForPipelining(
            MachineBasicBlock *LoopBB) const override;

    virtual bool isPredicated(const MachineInstr& MI) const override;
    virtual bool isPredicable(const MachineInstr& MI) const override;

    virtual bool PredicateInstruction(
        MachineInstr &mi,
        ArrayRef<MachineOperand> cond) const override;

    virtual bool ClobbersPredicate(
        MachineInstr& MI, std::vector<MachineOperand>& Pred,
        bool SkipDead) const override;
        
    virtual bool
    SubsumesPredicate(
        ArrayRef<MachineOperand> Pred1,
        ArrayRef<MachineOperand> Pred2) const override {
        return false;
    }

    virtual void insertCCBranch(
        MachineBasicBlock& mbb,
        MachineBasicBlock& tbb,
        ArrayRef<MachineOperand> cond,
        const DebugLoc& dl) const;

    virtual bool isProfitableToIfCvt(MachineBasicBlock &MBB, unsigned NumCycles,
                     unsigned ExtraPredCycles,
                     BranchProbability Probability) const override;

    virtual bool isProfitableToIfCvt(MachineBasicBlock &TMBB,
                     unsigned NumTCycles, unsigned ExtraTCycles,
                     MachineBasicBlock &FMBB,
                     unsigned NumFCycles, unsigned ExtraFCycles,
                     BranchProbability Probability) const override;

    /**
     * Return opcode for pointer adjustment and new offset.
     *
     * Returns opcode available or suitable for pointer adjustment with
     * constant offset value.
     *
     * @param offset The offset for the pointer.
     * @return (opcode, new offset) tuple.
     */
    std::tuple<int, int> getPointerAdjustment(int offset) const;

    virtual DFAPacketizer *CreateTargetScheduleState(
        const TargetSubtargetInfo &) const override;

private:
    InstrItineraryData InstrItins;

    int getMatchingCondBranchOpcode(int Opc, bool inverted) const;

        const TCERegisterInfo ri_;
        const TCETargetMachinePlugin* plugin_;

        // implementation generated to Backend.inc from TDGen.cc
        bool copyPhysVectorReg(
            MachineBasicBlock& mbb,
            MachineBasicBlock::iterator mbbi,
            const DebugLoc& DL,
            MCRegister destReg, MCRegister srcReg,
            bool killSrc) const;
    };
}

#endif
