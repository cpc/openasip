/**
 * @file TCERegisterInfo.h
 *
 * Declaration of TCERegisterInfo class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 */

#ifndef TCE_REGISTER_INFO_H
#define TCE_REGISTER_INFO_H

#include <llvm/Target/MRegisterInfo.h>
#include <llvm/ADT/BitVector.h>

#include "TCESubtarget.hh"

#include "TCEGenRegisterInfo.h.inc"

namespace llvm {
    class TargetInstrInfo;
    class Type;

    /**
     * Class which handles registers in the TCE backend.
     */
    class TCERegisterInfo : public TCEGenRegisterInfo {
    public:
        TCERegisterInfo(const TargetInstrInfo& tii);
        virtual ~TCERegisterInfo() {};

        void storeRegToStackSlot(
            MachineBasicBlock& mbb,
            MachineBasicBlock::iterator mbbi,
            unsigned srcReg, int frameIndex,
            const TargetRegisterClass* rc) const;

        void loadRegFromStackSlot(
            MachineBasicBlock& mbb,
            MachineBasicBlock::iterator mbbi,
            unsigned destReg, int frameIndex,
            const TargetRegisterClass* rc) const;

        void copyRegToReg(
            MachineBasicBlock& mbb,
            MachineBasicBlock::iterator mbbi,
            unsigned destReg, unsigned srcReg,
            const TargetRegisterClass* rc) const;

  
        //virtual MachineInstr* foldMemoryOperand(
        //MachineInstr* mi,
        //unsigned opNum,
        //int frameIndex) const;

        virtual void reMaterialize(
            MachineBasicBlock &MBB, MachineBasicBlock::iterator MI,
            unsigned DestReg, const MachineInstr *Orig) const;

        bool hasFP(const MachineFunction& mf) const;

        void eliminateCallFramePseudoInstr(
            MachineFunction &MF,
            MachineBasicBlock &MBB,
            MachineBasicBlock::iterator I) const;

        void eliminateFrameIndex(MachineBasicBlock::iterator ii) const;        
        const unsigned *getCalleeSavedRegs(const MachineFunction *MF = 0) const;
        const TargetRegisterClass* const* getCalleeSavedRegClasses(
            const MachineFunction *MF = 0) const;
//        const unsigned* getCalleeSavedRegs() const;
//        const TargetRegisterClass* const* getCalleeSavedRegClasses() const;
        void emitPrologue(MachineFunction& mf) const;
        void emitEpilogue(MachineFunction& mf, MachineBasicBlock& mbb) const;

        unsigned getRARegister() const;
        unsigned getFrameRegister(MachineFunction& mf) const;

        BitVector getReservedRegs(const MachineFunction &MF) const;

        void eliminateFrameIndex(MachineBasicBlock::iterator II,
                                 int adj, RegScavenger *RS = NULL) const;

    private:
        const TargetInstrInfo& tii_;
    };
}


#endif
