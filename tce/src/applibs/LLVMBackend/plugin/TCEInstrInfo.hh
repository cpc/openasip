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
#include "TCERegisterInfo.hh"

namespace llvm {

    class TCEInstrInfo : public TargetInstrInfo {

    public:
        TCEInstrInfo();
        virtual const MRegisterInfo& getRegisterInfo() const { return ri_; }

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

    private:
        const TCERegisterInfo ri_;
    };
}

#endif
