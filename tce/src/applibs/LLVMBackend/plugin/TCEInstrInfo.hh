/**
 * @file TCEInstrInfo.h
 *
 * Declaration of TCEInstrInfo class.
 *
 * @author Veli-Pekka Jaaskelainen 2007 (vjaaskel@cs.tut.fi)
 */

#ifndef TCE_INSTR_INFO_H
#define TCE_INSTR_INFO_H

#include <llvm/Target/TargetInstrInfo.h>
#include <llvm/Target/TargetRegisterInfo.h>
#include "TCERegisterInfo.hh"

namespace llvm {

    class TCEInstrInfo : public TargetInstrInfoImpl {

    public:
        TCEInstrInfo();
        virtual ~TCEInstrInfo();

        virtual const TargetRegisterInfo& getRegisterInfo() const { 
            return ri_; 
        }

        virtual bool isMoveInstr(
            const MachineInstr& mi, 
            unsigned& srcReg,
            unsigned& dstReg) const;

        virtual unsigned isLoadFromStackSlot(
            MachineInstr* mi, int& frameIndex) const;

        virtual unsigned isStoreToStackSlot(
            MachineInstr* mi, int& frameIndex) const;

        virtual unsigned InsertBranch(
            MachineBasicBlock& mbb,
            MachineBasicBlock* tbb,
            MachineBasicBlock* fbb,
            const std::vector<MachineOperand>& cond) const;

        virtual bool BlockHasNoFallThrough(
            MachineBasicBlock& mbb) const;

        virtual void storeRegToStackSlot(
            MachineBasicBlock& mbb,
            MachineBasicBlock::iterator mbbi,
            unsigned srcReg, bool isKill, int frameIndex,
            const TargetRegisterClass* rc) const;

        virtual void loadRegFromStackSlot(
            MachineBasicBlock& mbb,
            MachineBasicBlock::iterator mbbi,
            unsigned destReg, int frameIndex,
            const TargetRegisterClass* rc) const;

        virtual void copyRegToReg(
            MachineBasicBlock& mbb,
            MachineBasicBlock::iterator mbbi,
            unsigned destReg, unsigned srcReg,
            const TargetRegisterClass* dstRC,
            const TargetRegisterClass* srcRC) const;

    private:
        const TCERegisterInfo ri_;
    };
}

#endif
