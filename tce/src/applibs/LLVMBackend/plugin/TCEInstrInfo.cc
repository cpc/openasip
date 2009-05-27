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

    DebugLoc dl = DebugLoc::getUnknownLoc();
    if (mbb.begin() != mbb.end()) dl = mbb.front().getDebugLoc();

    BuildMI(&mbb, dl, get(TCE::TCEBR)).addMBB(tbb);
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

        if (mi->getOperand(1).isFI() &&
            mi->getOperand(2).isImm() &&
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

       if (mi->getOperand(0).isFI() &&
           mi->getOperand(1).isImm() &&
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
 
    DebugLoc dl = DebugLoc::getUnknownLoc();
    if (mbbi != mbb.end()) dl = mbbi->getDebugLoc();
    
    if (rc == TCE::I32RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::STWir))
            .addFrameIndex(fi).addImm(0).addReg(srcReg, getKillRegState(isKill));
    } else if (rc == TCE::F32RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::STWir))
            .addFrameIndex(fi).addImm(0).addReg(srcReg, getKillRegState(isKill));
    } else if (rc == TCE::I1RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::STQir))
            .addFrameIndex(fi).addImm(0).addReg(srcReg, getKillRegState(isKill));
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

    DebugLoc dl = DebugLoc::getUnknownLoc();
    if (mbbi != mbb.end()) dl = mbbi->getDebugLoc();

    if (rc == TCE::I32RegsRegisterClass) {
        BuildMI(
            mbb, mbbi, dl, get(TCE::LDWi),
            destReg).addFrameIndex(fi).addImm(0);
    } else if (rc == TCE::F32RegsRegisterClass) {
        BuildMI(
            mbb, mbbi, dl, get(TCE::LDWi),
            destReg).addFrameIndex(fi).addImm(0);
    } else if (rc == TCE::I1RegsRegisterClass) {
        BuildMI(
            mbb, mbbi, dl, get(TCE::LDQi),
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
bool
TCEInstrInfo::copyRegToReg(
    MachineBasicBlock& mbb,
    MachineBasicBlock::iterator mbbi,
    unsigned destReg, unsigned srcReg,
    const TargetRegisterClass* dstRC,
    const TargetRegisterClass* srcRC) const {

    assert(srcRC == dstRC && "not yet implemented");
    
    DebugLoc dl = DebugLoc::getUnknownLoc();
    if (mbbi != mbb.end()) dl = mbbi->getDebugLoc();

    if (srcRC == TCE::I1RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI1rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::I8RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI8rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::I16RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI16rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::I32RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI32rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::I64RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI64rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::F32RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVF32rr), destReg).addReg(srcReg);
    } else if (srcRC == TCE::F64RegsRegisterClass) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVF64rr), destReg).addReg(srcReg);
    } else {
        assert(
            false && "TCERegisterInfo::copyRegToReg(): Can't copy register");
    }
    return true;
}
