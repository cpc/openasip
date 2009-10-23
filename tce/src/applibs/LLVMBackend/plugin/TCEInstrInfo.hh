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
            const MachineInstr* mi, int& frameIndex) const;

        virtual unsigned isStoreToStackSlot(
            const MachineInstr* mi, int& frameIndex) const;

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

        virtual bool copyRegToReg(
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
