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
