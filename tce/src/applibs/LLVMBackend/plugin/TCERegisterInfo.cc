/**
 * @file TCERegisterInfo.cpp
 *
 * Implementation of TCERegisterInfo class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
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
 * Creates instructions for storing value from a register to stack slot.
 *
 * @param mbb Basic block where the store is done.
 * @param mbbi Iterator to the place where the store instruction is added.
 * @param srcReg Register to store.
 * @param fi Frame index of the stack slot.
 * @param rc Class of the register to store.
 */
void
TCERegisterInfo::storeRegToStackSlot(
    MachineBasicBlock& mbb,
    MachineBasicBlock::iterator mbbi,
    unsigned srcReg, int fi,
    const TargetRegisterClass* rc) const {

    if (rc == TCE::I32RegsRegisterClass) {
        BuildMI(mbb, mbbi, tii_.get(TCE::STWir))
            .addFrameIndex(fi).addImm(0).addReg(srcReg, false, false, true);
        //} else if (rc == TCE::I1RegsRegisterClass) {
        // TODO: STQBri assumes src reg is bool.
        //assert(srcReg == TCE::BOOL);
        //BuildMI(mbb, mbbi, tii_.get(TCE::STQbri))
        //.addFrameIndex(fi).addImm(0).addReg(srcReg, false, false, true);
    } else if (rc == TCE::F32RegsRegisterClass) {
        BuildMI(mbb, mbbi, tii_.get(TCE::STWir))
            .addFrameIndex(fi).addImm(0).addReg(srcReg, false, false, true);
/*
    } else if (rc == TCE::I64RegsRegisterClass) {
        BuildMI(mbb, mbbi, tii_.get(TCE::STDri))
            .addFrameIndex(fi).addImm(0).addReg(srcReg, false, false, true);
    } else if (rc == TCE::F64RegsRegisterClass) {
        BuildMI(mbb, mbbi, tii_.get(TCE::STDfri))
            .addFrameIndex(fi).addImm(0).addReg(srcReg, false, false, true);
*/
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
TCERegisterInfo::loadRegFromStackSlot(
    MachineBasicBlock& mbb,
    MachineBasicBlock::iterator mbbi,
    unsigned destReg, int fi,
    const TargetRegisterClass* rc) const {

    if (rc == TCE::I32RegsRegisterClass) {
        BuildMI(
            mbb, mbbi, tii_.get(TCE::LDWi),
            destReg).addFrameIndex(fi).addImm(0);

        //} else if (rc == TCE::I1RegsRegisterClass) {
        //BuildMI(
        //mbb, mbbi, tii_.get(TCE::LDQbri),
        ///destReg).addFrameIndex(fi).addImm(0);

    } else if (rc == TCE::F32RegsRegisterClass) {
        BuildMI(
            mbb, mbbi, tii_.get(TCE::LDWi),
            destReg).addFrameIndex(fi).addImm(0);
/*
    } else if (rc == TCE::I64RegsRegisterClass) {
        BuildMI(
            mbb, mbbi, tii_.get(TCE::LDDri),
            destReg).addFrameIndex(fi).addImm(0);
    } else if (rc == TCE::F64RegsRegisterClass) {
        BuildMI(
            mbb, mbbi, tii_.get(TCE::LDDfri),
            destReg).addFrameIndex(fi).addImm(0);
*/
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
TCERegisterInfo::copyRegToReg(
    MachineBasicBlock& mbb,
    MachineBasicBlock::iterator mbbi,
    unsigned destReg, unsigned srcReg,
    const TargetRegisterClass* rc) const {

    if (rc == TCE::I1RegsRegisterClass) {
        BuildMI(mbb, mbbi, tii_.get(TCE::MOVI1rr), destReg).addReg(srcReg);
    } else if (rc == TCE::I8RegsRegisterClass) {
        BuildMI(mbb, mbbi, tii_.get(TCE::MOVI8rr), destReg).addReg(srcReg);
    } else if (rc == TCE::I16RegsRegisterClass) {
        BuildMI(mbb, mbbi, tii_.get(TCE::MOVI16rr), destReg).addReg(srcReg);
    } else if (rc == TCE::I32RegsRegisterClass) {
        BuildMI(mbb, mbbi, tii_.get(TCE::MOVI32rr), destReg).addReg(srcReg);
    } else if (rc == TCE::I64RegsRegisterClass) {
        BuildMI(mbb, mbbi, tii_.get(TCE::MOVI64rr), destReg).addReg(srcReg);
    } else if (rc == TCE::F32RegsRegisterClass) {
        BuildMI(mbb, mbbi, tii_.get(TCE::MOVF32rr), destReg).addReg(srcReg);
    } else if (rc == TCE::F64RegsRegisterClass) {
        BuildMI(mbb, mbbi, tii_.get(TCE::MOVF64rr), destReg).addReg(srcReg);
    } else {
        assert(
            false && "TCERegisterInfo::copyRegToReg(): Can't copy register");
    }
}

/**
 * hasFP - Return true if the specified function
 * should have a dedicated frame pointer register.
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
    while (!mi.getOperand(i).isFrameIndex()) {
        i++;
        assert(i < mi.getNumOperands() && ("No FrameIndex operand found!"));
    }

#if defined(LLVM_2_1)
    int frameIndex = mi.getOperand(i).getFrameIndex();
#else
    int frameIndex = mi.getOperand(i).getIndex();
#endif

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
 * reMaterialize - Re-issue the specified 'original'
 * instruction at the specific location targeting a new destination register.
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

    MachineInstr* mi = orig->clone();
    mi->getOperand(0).setReg(destReg);
    mbb.insert(i, mi);
}
