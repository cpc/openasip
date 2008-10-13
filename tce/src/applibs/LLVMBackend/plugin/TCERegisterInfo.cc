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
 * @file TCERegisterInfo.cpp
 *
 * Implementation of TCERegisterInfo class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <assert.h>
#include <llvm/Type.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/CodeGen/MachineFrameInfo.h>
#include <llvm/Target/TargetInstrInfo.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/CodeGen/RegisterScavenging.h>

#include "TCEPlugin.hh"
#include "TCERegisterInfo.hh"
#include "tce_config.h"

#include <iostream> // DEBUG

using namespace llvm;

#include "TCEGenRegisterInfo.inc"

/**
 * The Constructor.
 *
 * @param st Subtarget architecture.
 * @param tii Target architecture instruction info.
 */
TCERegisterInfo::TCERegisterInfo(const TargetInstrInfo& tii) :
    TCEGenRegisterInfo(TCE::ADJCALLSTACKDOWN, TCE::ADJCALLSTACKUP),
    tii_(tii) {
}

/**
 * Return true if the specified function should have a dedicated frame 
 * pointer register.
 */
bool
TCERegisterInfo::hasFP(const MachineFunction& mf) const {
    return NoFramePointerElim || mf.getFrameInfo()->hasVarSizedObjects();
}


/**
 * Not Implemented yet.
MachineInstr*
TCERegisterInfo::foldMemoryOperand(
    MachineInstr* mi,
    unsigned opNum,
    int frameIndex) const {

    // TODO: can we fold something?
    return NULL;
}
 */

/**
 * Eliminates abstract frame index operands.
 *
 */
void
TCERegisterInfo::eliminateFrameIndex(
    MachineBasicBlock::iterator ii, int adj, RegScavenger* rs) const {

    MachineInstr& mi = *ii;
    MachineFunction& mf = *mi.getParent()->getParent();
    bool fp = hasFP(mf);

    // Frame index operand number
    unsigned i = 0;
    while (!mi.getOperand(i).isFI()) {
        i++;
        assert(i < mi.getNumOperands() && ("No FrameIndex operand found!"));
    }

    int frameIndex = mi.getOperand(i).getIndex();

    //unsigned baseRegister = fp ? TCE::FP : TCE::SP;
    assert(!fp);
    unsigned baseRegister = TCE::SP;
    mi.getOperand(i).ChangeToRegister(baseRegister, false);

    int offset = mf.getFrameInfo()->getObjectOffset(frameIndex);
    offset +=  mf.getFrameInfo()->getStackSize();

    // TODO: Use scavenged register instead of a kludge register.
    //assert(rs != NULL);
    //unsigned tmp = rs->FindUnusedReg(TCE::I32RegsRegisterClass, true);
    //assert(tmp != 0);
    if (offset != 0) {
        mi.getOperand(i).ChangeToRegister(TCE::KLUDGE_REGISTER, false);
        BuildMI(
            *mi.getParent(), ii, tii_.get(TCE::ADDri),
            TCE::KLUDGE_REGISTER).addReg(baseRegister).addImm(offset);
    }
}

/**
 * Returns list of callee saved registers.
 */
const unsigned* 
TCERegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
    static const unsigned calleeSavedRegs[] = {
        //TCE::RA,
        0 };
    return calleeSavedRegs;
}

/**
 * Returns list of callee saved register classes.
 *
 * The returned list has equal length with getCalleeSavedRegs() list
 * and the list items with same position correspond to each other.
 */
const TargetRegisterClass* const*
TCERegisterInfo::getCalleeSavedRegClasses(const MachineFunction *MF) const {
    static const TargetRegisterClass* const calleeSavedRegClasses[] = {
        //(&TCE::RARegRegClass),
        0 };

    return calleeSavedRegClasses;
}

/**
 * Emits machine function prologue to machine functions.
 */
void
TCERegisterInfo::emitPrologue(MachineFunction& mf) const {

    MachineBasicBlock& mbb = mf.front();
    MachineFrameInfo* mfi = mf.getFrameInfo();
    int numBytes = (int)mfi->getStackSize();

    bool fp = hasFP(mf);
    if (fp) {
        numBytes += 4; // FP.
    }

    numBytes = (numBytes + 3) & ~3; // stack size alignment

    MachineBasicBlock::iterator ii = mbb.begin();

    BuildMI(mbb, ii, tii_.get(TCE::SUBri), TCE::SP).addReg(
        TCE::SP).addImm(4);

    // Save RA to stack.
    BuildMI(mbb, ii, tii_.get(TCE::STWrr)).addReg(
        TCE::SP).addImm(0).addReg(TCE::RA);

    if (fp) {
        assert(false);
        // TODO: stack pointer adjustment
        // Save old FP to stack.
        //BuildMI(mbb, ii, tii_.get(TCE::STWrr)).addReg(
        //TCE::SP).addImm(0).addReg(TCE::FP);
    }

    // Adjust stack pointer
   if (numBytes != 0) {
        BuildMI(mbb, ii, tii_.get(TCE::SUBri), TCE::SP).addReg(
            TCE::SP).addImm(numBytes);
   }

    mfi->setStackSize(numBytes + 4);
}

/**
 * Emits machine function epilogue to machine functions.
 */
void
TCERegisterInfo::emitEpilogue(
    MachineFunction& mf, MachineBasicBlock& mbb) const {

    MachineFrameInfo* mfi = mf.getFrameInfo();
    MachineBasicBlock::iterator mbbi = prior(mbb.end());

    if (mbbi->getOpcode() != TCE::RETL) {
        assert(false && "ERROR: Insertiing epilogue w/o return?");
    }

    unsigned numBytes = mfi->getStackSize();

    if (hasFP(mf)) {
        // TODO: Restore old FP & handle FP in stack.
        assert(false);
    }

    if (numBytes != 4) {
        BuildMI(mbb, mbbi, tii_.get(TCE::ADDri), TCE::SP).addReg(
            TCE::SP).addImm(numBytes - 4);
    }

    // Restore RA from stack.
    BuildMI(mbb, mbbi, tii_.get(TCE::LDWi), TCE::RA).addReg(
        TCE::SP).addImm(0);

    BuildMI(mbb, mbbi, tii_.get(TCE::ADDri), TCE::SP).addReg(
        TCE::SP).addImm(4);
}

/**
 * Not implemented: When is this method even called?
 */
unsigned
TCERegisterInfo::getRARegister() const {
    assert(false);
    return 0;
}

/**
 * Not implemented: When is this method even called?
 */
unsigned
TCERegisterInfo::getFrameRegister(MachineFunction& mf) const {
    assert(false);
    return 0;
}

/**
 * Returns list of reserved registers.
 */
BitVector
TCERegisterInfo::getReservedRegs(const MachineFunction& mf) const {
    BitVector reserved(getNumRegs());
    reserved.set(TCE::SP);
    reserved.set(TCE::KLUDGE_REGISTER);
    //reserved.set(TCE::FP);
    reserved.set(TCE::RA);
    return reserved;
}

/**
 * Eliminates call frame pseudo instructions by replacing them with suitable
 * instruction sequence.
 */
void
TCERegisterInfo::eliminateCallFramePseudoInstr(
    MachineFunction& mf,
    MachineBasicBlock& mbb,
    MachineBasicBlock::iterator i) const {

    if (hasFP(mf)) {
        assert (false && "TODO FP support.");

        MachineInstr& mi = *i;
#ifdef LLVM_2_1
        int sz =  mi.getOperand(0).getImmedValue();
#else
        int sz =  mi.getOperand(0).getImm();
#endif
        if (mi.getOpcode() == TCE::ADJCALLSTACKDOWN) {
            std::cerr << "TCE::ADJCALLSTACKDOWN" << std::endl;
            sz = -sz;
        } else if (mi.getOpcode() == TCE::ADJCALLSTACKUP) {
            std::cerr << "TCE::ADJCALLSTACKUP" << std::endl;
        } else {
            assert(false && "Unknown call frame pseudo instruction!");
        }
    
        std::cerr << "sz: " << sz << std::endl;
        if (sz) {
            std::cerr << "TCE::SP ADD " << sz << std::endl;
            BuildMI(mbb, i, tii_.get(TCE::ADDri), TCE::SP).addReg(
                TCE::SP).addImm(sz);
        }
    }
    mbb.erase(i);
}

/**
 * Re-issue the specified 'original' instruction at the specific location 
 * targeting a new destination register.
 *
 * @param mbb Machine basic block of the new instruction.
 * @param i Position of the new instruction in the basic block.
 * @param destReg New destination register.
 * @param orig Original instruction.
 */
void
TCERegisterInfo::reMaterialize(
    MachineBasicBlock& mbb,
    MachineBasicBlock::iterator i,
    unsigned destReg,
    const MachineInstr* orig) const {

#ifdef LLVM_2_3
    MachineInstr* mi = orig->clone();
#else
    MachineInstr* mi = mbb.getParent()->CloneMachineInstr(orig);
#endif
    mi->getOperand(0).setReg(destReg);
    mbb.insert(i, mi);
}

int
TCERegisterInfo::getDwarfRegNum(unsigned RegNum, bool isEH) const {
    assert(0 && "Not implemented!");
    return -1;
}


