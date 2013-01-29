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
#include <llvm/Target/TargetInstrInfo.h>
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

        virtual unsigned InsertBranch(
            MachineBasicBlock &MBB, MachineBasicBlock *TBB,
            MachineBasicBlock *FBB,
            const SmallVectorImpl<MachineOperand> &Cond,
	    DebugLoc DL) const;

        unsigned RemoveBranch(MachineBasicBlock &mbb) const;

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
            const TargetRegisterClass* rc, const TargetRegisterInfo*) const {
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
            const TargetRegisterClass* rc, const TargetRegisterInfo*) const {
            loadRegFromStackSlot(mbb, mbbi, destReg, frameIndex, rc);
        }

	virtual void copyPhysReg(
	    MachineBasicBlock& mbb,
	    MachineBasicBlock::iterator mbbi, DebugLoc DL,
	    unsigned destReg, unsigned srcReg,
	    bool KillSrc) const;

        virtual bool ReverseBranchCondition(
            llvm::SmallVectorImpl<llvm::MachineOperand>& cond) const;

        virtual bool AnalyzeBranch(
            MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
            MachineBasicBlock *&FBB, 
            llvm::SmallVectorImpl<llvm::MachineOperand>& cond,
	    bool allowModify = false)
            const;

	virtual bool isPredicated(const MachineInstr *MI) const;
	virtual bool isPredicable(MachineInstr *MI) const;
	virtual bool PredicateInstruction(
	    MachineInstr *mi,
	    const SmallVectorImpl<MachineOperand> &cond) const;

	virtual bool DefinesPredicate(MachineInstr *MI,
				      std::vector<MachineOperand> &Pred) const;

	virtual bool
	SubsumesPredicate(const SmallVectorImpl<MachineOperand> &Pred1,
			  const SmallVectorImpl<MachineOperand> &Pred2) const {
	    return false;
	}

	virtual bool isProfitableToIfCvt(MachineBasicBlock &MBB, unsigned NumCycles,
					 unsigned ExtraPredCycles,
					 const BranchProbability &Probability) const;
	
	virtual bool isProfitableToIfCvt(MachineBasicBlock &TMBB,
					 unsigned NumTCycles, unsigned ExtraTCycles,
					 MachineBasicBlock &FMBB,
					 unsigned NumFCycles, unsigned ExtraFCycles,
					 const BranchProbability &Probability) const;



    private:
	int getMatchingCondBranchOpcode(int Opc, bool inverted) const;

        const TCERegisterInfo ri_;
        const TCETargetMachinePlugin* plugin_;
    };
}

#endif
