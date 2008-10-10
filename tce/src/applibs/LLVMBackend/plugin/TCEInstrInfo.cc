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
 * @file TCEInstrInfo.cpp
 *
 * Implementation of TCEInstrInfo class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 */


#include <iostream>
#include <llvm/CodeGen/MachineInstrBuilder.h>

#include "TCEInstrInfo.hh"
#include "TCEPlugin.hh"

#include "tce_config.h"

// Include code generated with tceplugingen:
#include "TCEGenInstrInfo.inc"


using namespace llvm;

/**
 * Constructor.
 */
TCEInstrInfo::TCEInstrInfo() :
    //TargetInstrInfo(TCEInsts, sizeof(TCEInsts) / sizeof(TCEInsts[0])),
    TargetInstrInfoImpl(TCEInsts, sizeof(TCEInsts) / sizeof(TCEInsts[0])),
    ri_(*this) {

}

/**
 * Destructor.
 */
TCEInstrInfo:: ~TCEInstrInfo() {   
}

			       
/**
 * Returns true if the instruction is a register to register move.
 */
bool
TCEInstrInfo::isMoveInstr(
    const MachineInstr& mi, 
    unsigned& srcReg,
    unsigned& dstReg) const {

    if (mi.getOpcode() == TCE::MOVI1rr ||
        mi.getOpcode() == TCE::MOVI8rr ||
        mi.getOpcode() == TCE::MOVI16rr ||
        mi.getOpcode() == TCE::MOVI32rr ||
        mi.getOpcode() == TCE::MOVI64rr ||
        mi.getOpcode() == TCE::MOVF32rr ||
        mi.getOpcode() == TCE::MOVF64rr) {

        assert(mi.getOperand(0).isReg());
        assert(mi.getOperand(1).isReg());

        dstReg = mi.getOperand(0).getReg();
        srcReg = mi.getOperand(1).getReg();
        return true;
    }

    return false;
}

/**
 * Inserts a branch instruction.
 */
unsigned
TCEInstrInfo::InsertBranch(
    MachineBasicBlock& mbb,
    MachineBasicBlock* tbb,
    MachineBasicBlock* fbb,
    const std::vector<MachineOperand>& cond) const {

    // Can only insert uncond branches so far.
    assert(cond.empty() && !fbb && tbb && "Can only handle uncond branches!");
    BuildMI(&mbb, get(TCE::TCEBR)).addMBB(tbb);
    return 1;
}

/**
 * Returns true if the instruction is a load from stack slot.
 */
unsigned
TCEInstrInfo::isLoadFromStackSlot(
    MachineInstr* mi, int& frameIndex) const {

    if (mi->getOpcode() == TCE::LDQi ||
        mi->getOpcode() == TCE::LDHi ||
        mi->getOpcode() == TCE::LDWi) {

        if (mi->getOperand(1).isFrameIndex() &&
            mi->getOperand(2).isImmediate() &&
            mi->getOperand(2).getImm() == 0) {
            
            frameIndex = mi->getOperand(1).getIndex();
            return mi->getOperand(0).getReg();
        }
    }
    return 0;
}


/**
 * Returns true if the instruction is a store to stack slot.
 */
unsigned
TCEInstrInfo::isStoreToStackSlot(MachineInstr* mi, int& frameIndex) const {
    
    if (mi->getOpcode() == TCE::STQri ||
        mi->getOpcode() == TCE::STHri ||
        mi->getOpcode() == TCE::STWir) {

       if (mi->getOperand(0).isFrameIndex() &&
           mi->getOperand(1).isImmediate() &&
           mi->getOperand(1).getImm() == 0) {
           
           frameIndex = mi->getOperand(0).getIndex();
           return mi->getOperand(2).getReg();
        }
    }
    return 0;
}

/**
 * Returns true if program control can't fall through the last instruction
 * in the basic block, false otherwise.
 */
bool
TCEInstrInfo::BlockHasNoFallThrough(MachineBasicBlock& mbb) const {

    if (mbb.empty()) return false;

    if (mbb.back().getOpcode() ==  TCE::TCEBR) {
        return true; // Unconditional branch.
    }

    return false;
}


/**
 * Creates instructions for storing value from a register to stack slot.
 *
 * @param mbb Basic block where the store is done.
 * @param mbbi Iterator to the place where the store instruction is added.
 * @param srcReg Register to store.
 * @param fi Frame index of the stack slot.
 * @param rc Class of the register to store.
 */
void
TCEInstrInfo::storeRegToStackSlot(
    MachineBasicBlock& mbb,
    MachineBasicBlock::iterator mbbi,
    unsigned srcReg, bool isKill, int fi,
    const TargetRegisterClass* rc) const {

    if (rc == TCE::I32RegsRegisterClass) {
        BuildMI(mbb, mbbi, get(TCE::STWir))
            .addFrameIndex(fi).addImm(0).addReg(srcReg, false, false, isKill);
    } else if (rc == TCE::F32RegsRegisterClass) {
        BuildMI(mbb, mbbi, get(TCE::STWir))
            .addFrameIndex(fi).addImm(0).addReg(srcReg, false, false, isKill);
    } else if (rc == TCE::I1RegsRegisterClass) {
        BuildMI(mbb, mbbi, get(TCE::STQir))
            .addFrameIndex(fi).addImm(0).addReg(srcReg, false, false, isKill);
    } else {
        assert(0 && "Can't store this register to stack slot");
    }
}

/**
 * Creates instructions for loading value from a stack slot to a register.
 *
 * @param mbb Basic block where the load is done.
 * @param mbbi Iterator to the place where the load instruction is added.
 * @param destReg Register where the value is loaded.
 * @param fi Frame index of the stack slot.
 * @param rc Class of the register to load.
 */
void
TCEInstrInfo::loadRegFromStackSlot(
    MachineBasicBlock& mbb,
    MachineBasicBlock::iterator mbbi,
    unsigned destReg, int fi,
    const TargetRegisterClass* rc) const {

    if (rc == TCE::I32RegsRegisterClass) {
        BuildMI(
            mbb, mbbi, get(TCE::LDWi),
            destReg).addFrameIndex(fi).addImm(0);
    } else if (rc == TCE::F32RegsRegisterClass) {
        BuildMI(
            mbb, mbbi, get(TCE::LDWi),
            destReg).addFrameIndex(fi).addImm(0);
    } else if (rc == TCE::I1RegsRegisterClass) {
        BuildMI(
            mbb, mbbi, get(TCE::LDQi),
        destReg).addFrameIndex(fi).addImm(0);
    } else {

        assert(0 && "Can't load this register from stack slot");
    }  
}

/**
 * Creates instruction for copying value from a register to another.
 *
 * @param mbb Basic block where the copy is done.
 * @param mbbi Iterator to the place where the copy instruction is added.
 * @param srcReg Register where the value is copied from.
 * @param destReg Register where the value is copied to.
 * @param rc Class of the register to copy.
 */
void
TCEInstrInfo::copyRegToReg(
    MachineBasicBlock& mbb,
    MachineBasicBlock::iterator mbbi,
    unsigned destReg, unsigned srcReg,
    const TargetRegisterClass* dstRC,
    const TargetRegisterClass* srcRC) const {

    assert(srcRC == dstRC && "not yet implemented");

    if (srcRC == TCE::I1RegsRegisterClass) {
        BuildMI(mbb, mbbi, get(TCE::MOVI1rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::I8RegsRegisterClass) {
        BuildMI(mbb, mbbi, get(TCE::MOVI8rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::I16RegsRegisterClass) {
        BuildMI(mbb, mbbi, get(TCE::MOVI16rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::I32RegsRegisterClass) {
        BuildMI(mbb, mbbi, get(TCE::MOVI32rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::I64RegsRegisterClass) {
        BuildMI(mbb, mbbi, get(TCE::MOVI64rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::F32RegsRegisterClass) {
        BuildMI(mbb, mbbi, get(TCE::MOVF32rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::F64RegsRegisterClass) {
        BuildMI(mbb, mbbi, get(TCE::MOVF64rr), destReg).addReg(srcReg);
    } else {
        assert(
            false && "TCERegisterInfo::copyRegToReg(): Can't copy register");
    }
}
