/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file TCERegisterInfo.h
 *
 * Declaration of TCERegisterInfo class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TCE_REGISTER_INFO_H
#define TCE_REGISTER_INFO_H

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

        void emitPrologue(MachineFunction& mf) const;
        void emitEpilogue(MachineFunction& mf, MachineBasicBlock& mbb) const;

        unsigned getRARegister() const;
        unsigned getFrameRegister(MachineFunction& mf) const;

        BitVector getReservedRegs(const MachineFunction &MF) const;

        void eliminateFrameIndex(MachineBasicBlock::iterator II,
                                 int adj, RegScavenger *RS = NULL) const;

        int getDwarfRegNum(unsigned regNum, bool isEH) const;

    private:
        const TargetInstrInfo& tii_;
    };
}


#endif
