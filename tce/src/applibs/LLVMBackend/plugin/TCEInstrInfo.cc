/**
 * @file TCEInstrInfo.cpp
 *
 * Implementation of TCEInstrInfo class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2007 (vjaaskel@cs.tut.fi)
 */


#include <iostream>
#include <llvm/CodeGen/MachineInstrBuilder.h>

#include "TCEInstrInfo.hh"
#include "TCEPlugin.hh"

// Include code generated with tceplugingen:
#include "TCEGenInstrInfo.inc"


using namespace llvm;

/**
 * Constructor.
 */
TCEInstrInfo::TCEInstrInfo() :
    TargetInstrInfo(TCEInsts, sizeof(TCEInsts) / sizeof(TCEInsts[0])),
    ri_(*this) {

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

    if (//mi->getOpcode() == TCE::LDQri ||
        //mi->getOpcode() == TCE::LDHri ||
        mi->getOpcode() == TCE::LDWi) {

#if defined(LLVM_2_1)
        if (mi->getOperand(1).isFrameIndex() &&
            mi->getOperand(2).isImmediate() &&
            mi->getOperand(2).getImmedValue() == 0) {
            frameIndex = mi->getOperand(1).getFrameIndex();
#else
        if (mi->getOperand(1).isFrameIndex() &&
            mi->getOperand(2).isImmediate() &&
            mi->getOperand(2).getImm() == 0) {
            frameIndex = mi->getOperand(1).getIndex();
#endif
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
    
    if (//mi->getOpcode() == TCE::STQri ||
        //mi->getOpcode() == TCE::STHri ||
        mi->getOpcode() == TCE::STWir) {

#if defined(LLVM_2_1)
       if (mi->getOperand(0).isFrameIndex() &&
           mi->getOperand(1).isImmediate() &&
           mi->getOperand(1).getImmedValue() == 0) {
           frameIndex = mi->getOperand(1).getFrameIndex();
#else
       if (mi->getOperand(0).isFrameIndex() &&
           mi->getOperand(1).isImmediate() &&
           mi->getOperand(1).getImm() == 0) {
           frameIndex = mi->getOperand(1).getIndex();
#endif

            return mi->getOperand(2).getReg();
        }
    }
    return 0;
}

bool
TCEInstrInfo::BlockHasNoFallThrough(MachineBasicBlock& mbb) const {

    std::cerr << "BlochHasNoFallThrough(): ";

    if (mbb.empty()) return false;

    if (mbb.back().getOpcode() ==  TCE::TCEBR) {
        std::cerr << "TRUE" << std::endl;
        return true; // Unconditional branch.
    }
    std::cerr << "false" << std::endl;
    return false;
}
