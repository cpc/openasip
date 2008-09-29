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
