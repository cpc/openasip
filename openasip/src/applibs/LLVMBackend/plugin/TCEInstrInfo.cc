/*
    Copyright (c) 2002-2009 Tampere University.

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
 * NOTE: Normal TCE coding guidelines do not apply here to makeo it
 *       easier to track and copy paste changes from LLVM.
 *       So please follow LLVM style when adding or fixing things.
 *
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 * @author Heikki Kultala 2011-2012 (heikki.kultala-no.spam-tut.fi)
 */

#include "TCEInstrInfo.hh"

#include <llvm/ADT/STLExtras.h>
#include <llvm/CodeGen/DFAPacketizer.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/CodeGen/MachineRegisterInfo.h>
#include <llvm/Support/ErrorHandling.h>

#include "TCETargetMachine.hh"
#include "TCETargetMachinePlugin.hh"
/*
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/ADT/SmallVector.h>

#include "TCEInstrInfo.hh"
*/
#include "TCEPlugin.hh"
//#include "tce_config.h"

// Include code generated with tceplugingen:

#define GET_INSTRINFO_CTOR_DTOR
#define GET_INSTRINFO_MC_DESC
#include "TCEGenInstrInfo.inc"
#undef GET_INSTRINFO_CTOR_DTOR
#undef GET_INSTRINFO_MC_DESC

using namespace llvm;

#define GET_INSTRMAP_INFO
#include "TCEGenDFAPacketizer.inc"

/**
 * Constructor.
 */
TCEInstrInfo::TCEInstrInfo(
    const TCETargetMachinePlugin* plugin) :
    TCEGenInstrInfo(TCE::ADJCALLSTACKDOWN, TCE::ADJCALLSTACKUP),
    ri_(*this), plugin_(plugin) {
}

/**
 * Destructor.
 */
TCEInstrInfo:: ~TCEInstrInfo() {   
}

/**
 * Inserts a branch instruction or brach instructions into llvm MBB.
 *
 * If the MBB already has an unconditional branch at end, does nothing.
 * 
 * @param mbb where to insert the branch instructions.
 * @param tbb jump target basic block
 * @param fbb false condition jump target, if insertin 2 branches
 *
 * @return number of branch instructions inserted
 */
unsigned
TCEInstrInfo::insertBranch(
    MachineBasicBlock& mbb,
    MachineBasicBlock* tbb,
    MachineBasicBlock* fbb,
    ArrayRef<MachineOperand> cond,
    const DebugLoc& dl
    , int *BytesAdded) const {
    assert(cond.size() == 0 || cond.size() == 2 || cond.size() == 3);

    if (mbb.size() != 0) {
        // already has a uncond branch, no need for another.
        // asserts to make sure it's to same BB in order to not create
        // broken code.
        if (mbb.back().getOpcode() == TCE::TCEBR ||
            mbb.back().getOpcode() == TCE::TCEBRIND) { 
            assert(cond.size() == 0);
            return 0;
        }
        if (cond.size() != 0) {
            assert (mbb.back().getOpcode() != TCE::TCEBRCOND && "c branch!");
            assert (mbb.back().getOpcode() != TCE::TCEBRICOND && "ic branch!");
            assert (mbb.back().getOpcode() != TCE::TCEBR && "has branch!(1)");
        } else {
            assert (mbb.back().getOpcode() != TCE::TCEBR && "has branch(2)!");
        }

    }

    if (fbb == 0) {
        if (cond.empty()) {
            // Can only insert uncond branches so far.
            BuildMI(&mbb, dl, get(TCE::TCEBR)).addMBB(tbb);
            return 1;
        } else {
            if (cond.size() == 2 && cond[1].getImm() == false) {
                // false jump
                BuildMI(&mbb, dl, get(TCE::TCEBRICOND)).
                    addReg(cond[0].getReg()).addMBB(tbb);
                return 1;
            } else if (cond.size() == 2 && cond[1].getImm() == true) {
                BuildMI(&mbb, dl, get(TCE::TCEBRCOND)).addReg(cond[0].getReg())
                    .addMBB(tbb);
                return 1;
            } else {
                insertCCBranch(mbb, *tbb, cond, dl);
                return 1;
            }
        }
    }

    assert(
        !cond.empty() &&
        "Two jumps need a condition");  // not allowed to have conditional
                                        // jump because we have an fbb

    if (cond.size() == 2 && cond[1].getImm() == false) {
        BuildMI(&mbb, dl, get(TCE::TCEBRICOND)).
            addReg(cond[0].getReg()).addMBB(tbb);
    } else if (cond.size() == 1 ||
               (cond.size() == 2 && cond[1].getImm() == true)) {
        BuildMI(&mbb, dl, get(TCE::TCEBRCOND)).
            addReg(cond[0].getReg()).addMBB(tbb);
    } else {
        insertCCBranch(mbb, *tbb, cond, dl);
    }
    BuildMI(&mbb, dl, get(TCE::TCEBR)).addMBB(fbb);

    return 2;
}

/**
 * Removes branch or branches form end of llvm MachineBasicBlock
 *
 * @param mbb where to remove the branches from
 * @return number of braches removed
 */
unsigned
TCEInstrInfo::removeBranch(
    MachineBasicBlock &mbb, int *BytesRemoved) const {
    int j = 0;
    MachineBasicBlock::iterator i = mbb.end();
    while (i != mbb.begin()) {
        i--;
        int opc = i->getOpcode();
        if (i->getDesc().isBranch()) {
            i->eraseFromParent();
            i = mbb.end();  // not optimal, but we will not miss any
                            // instruction
            j++;
        }
    }
    return j;
}

/**
 * Returns true if program control can't fall through the last instruction
 * in the basic block, false otherwise.
 */
bool
TCEInstrInfo::BlockHasNoFallThrough(const MachineBasicBlock& MBB) const {
    /* Mips inspired */
    if (MBB.empty()) return false;
    switch (MBB.back().getOpcode()) {
    case TCE::RETL:    // Return.
    case TCE::TCEBR:  // Uncond branch.
    case TCE::TCEBRIND:  // Uncond indirect branch.
        return true;
    default: return false;
    }
}

void TCEInstrInfo::
storeRegToStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                    unsigned SrcReg, bool isKill, int FI,
                    const TargetRegisterClass *RC, Register vReg) const {
  DebugLoc DL;

  if (I != MBB.end()) DL = I->getDebugLoc();

  BuildMI(MBB, I, DL, get(plugin_->getStore(RC))).addFrameIndex(FI).addImm(0)
      .addReg(SrcReg, getKillRegState(isKill));

  LLVMContext& context = MBB.getParent()->getFunction().getContext();
  llvm::Metadata* md = llvm::MDString::get(context, "AA_CATEGORY_STACK_SLOT");
  MDNode* mdNode =
      MDNode::get(context, llvm::ArrayRef<llvm::Metadata*>(&md, 1));
  MachineOperand metaDataOperand = MachineOperand::CreateMetadata(mdNode);
  I--; // buildmi moves the iterator to next ins, point to the created one.
  I->addOperand(metaDataOperand);
}

void TCEInstrInfo::
loadRegFromStackSlot(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                     unsigned DestReg, int FI,
                     const TargetRegisterClass *RC, Register vReg) const {
  DebugLoc DL;

  if (I != MBB.end()) DL = I->getDebugLoc();
  BuildMI(MBB, I, DL, get(plugin_->getLoad(RC)), DestReg).addFrameIndex(FI)
      .addImm(0);

  LLVMContext& context = MBB.getParent()->getFunction().getContext();
  llvm::Metadata* md = llvm::MDString::get(context, "AA_CATEGORY_STACK_SLOT");
  MDNode* mdNode =
      MDNode::get(context, llvm::ArrayRef<llvm::Metadata*>(&md, 1));
  MachineOperand metaDataOperand = MachineOperand::CreateMetadata(mdNode);
  I--; // buildmi moves the iterator to next ins, point to the created one.
  I->addOperand(metaDataOperand);
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
void TCEInstrInfo::copyPhysReg(
    MachineBasicBlock& mbb,
    MachineBasicBlock::iterator mbbi,
    const DebugLoc& DL,
        MCRegister destReg, MCRegister srcReg,
    bool killSrc) const
{
    DebugLoc dl;
    if (mbbi != mbb.end()) dl = mbbi->getDebugLoc();
/*
    BuildMI(mbb, mbbi, dl, 
            get(plugin_->getRegCopy(destReg, srcReg)), destReg).
        .addReg(SrcReg, getKillRegState(isKillSrc));
*/
    if (copyPhysVectorReg(mbb, mbbi, dl, destReg, srcReg, killSrc)) {
        return;
    }

    if (TCE::R1RegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI1rr), destReg)
            .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::R32IRegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI32rr), destReg)
            .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::R64RegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOV64ss), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::FPRegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVff), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::HFPRegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVhh), destReg)
	    .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::R1RegsRegClass.contains(destReg) &&
               TCE::R32IRegsRegClass.contains(srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI32I1rr), destReg)
            .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::R1RegsRegClass.contains(srcReg) &&
               TCE::R32IRegsRegClass.contains(destReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI1I32rr), destReg)
            .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::GuardRegsRegClass.contains(destReg, srcReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVGrr), destReg)
            .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::GuardRegsRegClass.contains(srcReg) &&
               TCE::R32IRegsRegClass.contains(destReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVGI32rr), destReg)
            .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::R32IRegsRegClass.contains(srcReg) &&
               TCE::GuardRegsRegClass.contains(destReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI32Grr), destReg)
            .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::GuardRegsRegClass.contains(srcReg) &&
               TCE::R1RegsRegClass.contains(destReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVGI1rr), destReg)
            .addReg(srcReg, getKillRegState(killSrc));
    } else if (TCE::R1RegsRegClass.contains(srcReg) &&
               TCE::GuardRegsRegClass.contains(destReg)) {
        BuildMI(mbb, mbbi, dl, get(TCE::MOVI1Grr), destReg)
            .addReg(srcReg, getKillRegState(killSrc));
    } else {
        assert(
            false && "TCERegisterInfo::copyPhysReg(): Can't copy register");

    }
}

/*
 * Reverses a condition.
 * 
 * @param cond condition to reverse. This is modified.
 *
 * @return false if did reverse condition, true if could not.
 */
bool
TCEInstrInfo::reverseBranchCondition(
    llvm::SmallVectorImpl<llvm::MachineOperand>& cond) const {
    assert(cond.size() != 0);

    // from true to false
    if (cond.size() == 1) {
        cond.push_back(MachineOperand::CreateImm(false));
        return false;
    } else if (cond.size() == 2) {
        // from false to true
        if (cond[1].getImm() == false) {
            cond[1].setImm(true);
        } else {
            // from true to false
            assert(cond[1].getImm() == true);
            cond[1].setImm(false);
        }
        return false;
    } else if (cond.size() == 3) {
        switch (cond[2].getImm()) {
        case 2:
            // eq -> ne
            cond[2].setImm(3);
            return false;
        case 3:
            // ne -> eq
            cond[2].setImm(2);
            return false;
        case 4:
            // gt -> le
            cond[2].setImm(5);
            return false;
        case 5:
            // le -> gt
            cond[2].setImm(4);
            return false;
        case 6:
            // ltu -> geu
            cond[2].setImm(7);
            return false;
        case 7:
            // geu -> ltu
            cond[2].setImm(6);
            return false;
        case 14:
            // lt -> ge
            cond[2].setImm(16);
            return false;
        case 15:
            // ltu -> geu
            cond[2].setImm(17);
            return false;
        case 16:
            // ge -> lt
            cond[2].setImm(14);
            return false;
        case 17:
            // geu -> ltu
            cond[2].setImm(15);
            return false;
        // case 100+: register-immediate versions of branch ops
        case 102:
            // eg -> ne
            cond[2].setImm(103);
            return false;
        case 103:
            // ne -> eq
            cond[2].setImm(102);
            return false;
        case 104:
            // gt -> le
            cond[2].setImm(105);
            return false;
        case 105:
            // le -> gt
            cond[2].setImm(104);
            return false;
        case 106:
            // ltu -> geu
            cond[2].setImm(107);
            return false;
        case 107:
            // geu -> ltu
            cond[2].setImm(106);
            return false;
        case 114:
            // lt -> ge
            cond[2].setImm(116);
            return false;
        case 115:
            // ltu -> geu
            cond[2].setImm(117);
            return false;
        case 116:
            // ge -> lt
            cond[2].setImm(114);
            return false;
        case 117:
            // geu -> ltu
            cond[2].setImm(115);
            return false;
        default:
            return true;
        }
    }
    return true;
}

/**
 * Analyzes branches of MBB.
 * 
 * @param mbb MBB to analyze
 * @param tbb Puts the jump target or condition true target MBB here
 * @param fbb Puts the condition false target here, if not fall-thru
 * @param cond puts the condition data (predcate reg and T/F) here
 * @return false if could analyze, true if could not analyze
 */
bool
    TCEInstrInfo::analyzeBranch(
    MachineBasicBlock &mbb, MachineBasicBlock *&tbb,
    MachineBasicBlock *&fbb, 
    llvm::SmallVectorImpl<llvm::MachineOperand>& cond, bool allowModify)
    const {
    if (mbb.empty()) {
        return false;
    }

    MachineBasicBlock::iterator i = mbb.end(); i--;

    MachineInstr& lastIns = *i;
    switch (lastIns.getOpcode()) {
    case TCE::TCEBRCOND:
        tbb = lastIns.getOperand(1).getMBB();
        cond.push_back(i->getOperand(0));
        cond.push_back(MachineOperand::CreateImm(true));
        return false;
    case TCE::TCEBRICOND:
        tbb = lastIns.getOperand(1).getMBB();
        cond.push_back(i->getOperand(0));
        cond.push_back(MachineOperand::CreateImm(false));
        return false;
    case TCE::TCEBRIND:
    case TCE::TCEBR: {
        // indirect jump cannot be analyzed
        if (!lastIns.getOperand(0).isMBB()) {
            return true;
        }

        if (i == mbb.begin()) {
            tbb = lastIns.getOperand(0).getMBB();
            return false; // uncond jump only ins in mbb.
        }
        i--;
        if (i->getOpcode() == TCE::TCEBRCOND) {
            tbb = i->getOperand(1).getMBB();
            fbb = lastIns.getOperand(0).getMBB();
            cond.push_back(i->getOperand(0));
            cond.push_back(MachineOperand::CreateImm(true));
            return false;
        }
        if (i->getOpcode() == TCE::TCEBRICOND) {
            tbb = i->getOperand(1).getMBB(); 
            fbb = lastIns.getOperand(0).getMBB();
            cond.push_back(i->getOperand(0));
            cond.push_back(MachineOperand::CreateImm(false));
            return false;
        }
        // two uncond branches not allowed
        assert(i->getOpcode() != TCE::TCEBR);

        if (i->getDesc().isBranch()) {
            tbb = i->getOperand(2).getMBB();
            fbb = lastIns.getOperand(0).getMBB();
            return plugin_->analyzeCCBranch(*i, cond);
        } else { // only conditional branch.
            tbb = lastIns.getOperand(0).getMBB();
            return false;
        }
    }
    default: 
        // if some another branch, it's unknown brach
        // if not brannch, it's fallthourgh
        if (lastIns.getDesc().isBranch()) {
            tbb = lastIns.getOperand(2).getMBB();
            return plugin_->analyzeCCBranch(lastIns, cond);
        } else {
            return false;
        }
    }
    // should never be here
    return true;
}

namespace {
class TCEPipelinerLoopInfo : public TargetInstrInfo::PipelinerLoopInfo {
public:
    TCEPipelinerLoopInfo() {}
    bool
    shouldIgnoreForPipelining(const MachineInstr *MI) const override {
        return false;
    }

    // If the trip count is statically known to be greater than TC, return
    // true. If the trip count is statically known to be not greater than TC,
    // return false. Otherwise return nullopt and fill out Cond with the test
    // condition.

    std::optional<bool>
    createTripCountGreaterCondition(
        int TC, MachineBasicBlock &MBB,
        SmallVectorImpl<MachineOperand> &Cond) override {
        return true;
    }

    void setPreheader(MachineBasicBlock *NewPreheader) override{};

    void adjustTripCount(int TripCountAdjust) override{};

    void disposed() override{};
};
}  // namespace

std::unique_ptr<TargetInstrInfo::PipelinerLoopInfo>
TCEInstrInfo::analyzeLoopForPipelining(MachineBasicBlock *LoopBB) const {
    return std::make_unique<TCEPipelinerLoopInfo>();
}

bool 
TCEInstrInfo::isPredicated(const MachineInstr& mi_ref) const {
    const MachineInstr* mi = &mi_ref;
    // TODO: should be conditional move here..
    if (mi->getOpcode() == TCE::RETL) {
        return false;
    }

    // KILL is not a predicated instruction.
    if (mi->getOpcode() == TCE::KILL) {
        return false;
    }

    TCEString opName = plugin_->operationName(mi->getOpcode());
    return opName[0] == '?' || opName[0] == '!';
}

bool
TCEInstrInfo::isPredicable(const MachineInstr& mi_ref) const {
    const MachineInstr* mi = &mi_ref;
    if (mi->getOpcode() == TCE::COPY) {
        return false;
    }

    // TODO: why is RETL not predicable?
    if (mi->getOpcode() == TCE::RETL) {
        return false;
    }

    if (isPredicated(*mi)) {
        return false;
    }

    if (getMatchingCondBranchOpcode(mi->getOpcode(),false) == -1) {
        return false;
    }

    for (int oper = mi->getNumOperands() - 1; oper >= 0; --oper) {
        MachineOperand mo = mi->getOperand(oper);

        if ((mo.isReg() && !mo.isUse() && !mo.isImplicit())) {
            continue;
        }

        // TODO: support operand changing for fp imms etc.!
        if (!mo.isReg() && !mo.isImm()) {
            return false;
        }
    }
    return true;
}

// todo: mostlly ripped from hexagon.
// check the legal things
bool TCEInstrInfo::PredicateInstruction(
    MachineInstr& mi_ref,
    ArrayRef<MachineOperand> cond
) const {

    MachineInstr *mi = &mi_ref;

    int opc = mi->getOpcode();

    assert (isPredicable(*mi) && "Expected predicable instruction");

    bool invertJump = (cond.size() >1 && cond[1].isImm() &&
                       (cond[1].getImm() == 0));
    
    mi->setDesc(get(getMatchingCondBranchOpcode(opc, invertJump)));
    //
    // This assumes that the predicate is always the first operand
    // in the set of inputs.
    //
    mi->addOperand(mi->getOperand(mi->getNumOperands()-1));
    int oper;
    // why -3 in hexagon?
    for (oper = mi->getNumOperands() - 2; oper >= 0; --oper) {
        MachineOperand mo = mi->getOperand(oper);
	// todo: why this break in hexagon?
        if ((mo.isReg() && !mo.isUse() && !mo.isImplicit())) {
            break;
        }

        if (mo.isReg()) {
            mi->getOperand(oper+1).ChangeToRegister(mo.getReg(), mo.isDef(),
						    mo.isImplicit(), mo.isKill(),
						    mo.isDead(), mo.isUndef(),
						    mo.isDebug());
        } else if (mo.isImm()) {
            mi->getOperand(oper+1).ChangeToImmediate(mo.getImm());
        } else if (mo.isFPImm()) {
            mi->getOperand(oper+1).ChangeToFPImmediate(mo.getFPImm());
        } else if (mo.isGlobal()) {
            // TODO: what to do here?
            llvm_unreachable("Unexpected operand type");
            mi->getOperand(oper+1).ChangeToImmediate(mo.getImm());
        } else {
            llvm_unreachable("Unexpected operand type");
        }
    }

    MachineOperand PredMO = cond[0];
    mi->getOperand(oper+1).ChangeToRegister(PredMO.getReg(), PredMO.isDef(),
                                            PredMO.isImplicit(), PredMO.isKill(),
                                            PredMO.isDead(), PredMO.isUndef(),
                                            PredMO.isDebug());
    
    return true;
}


int TCEInstrInfo::getMatchingCondBranchOpcode(int opc, bool inv) const {
    
    if (!inv) {
        return plugin_->getTruePredicateOpcode(opc);
    } else {
        return plugin_->getFalsePredicateOpcode(opc);
    }
}


bool
TCEInstrInfo::ClobbersPredicate(
    MachineInstr& MI, std::vector<MachineOperand>& Pred,
    bool SkipDead) const {
    for (unsigned oper = 0; oper < MI.getNumOperands(); ++oper) {
        MachineOperand MO = MI.getOperand(oper);
        if (MO.isReg() && MO.isDef()) {
            const TargetRegisterClass* RC =
                ri_.getMinimalPhysRegClass(MO.getReg());
            if (RC == &TCE::GuardRegsRegClass || RC == &TCE::R1RegsRegClass) {
                Pred.push_back(MO);
                return true;
            }
        }
    }
    return false;
}

bool
TCEInstrInfo::
isProfitableToIfCvt(MachineBasicBlock &MBB,
                    unsigned NumCycles,
                    unsigned ExtraPredCycles,
                    BranchProbability Probability) const {
    return true;
}


bool
TCEInstrInfo::
isProfitableToIfCvt(MachineBasicBlock &TMBB,
                    unsigned NumTCycles,
                    unsigned ExtraTCycles,
                    MachineBasicBlock &FMBB,
                    unsigned NumFCycles,
                    unsigned ExtraFCycles,
                    BranchProbability Probability) const {
    return true;
}


std::tuple<int, int>
TCEInstrInfo::getPointerAdjustment(int offset) const {
    return plugin_->getPointerAdjustment(offset);
}

DFAPacketizer *
TCEInstrInfo::CreateTargetScheduleState(
    const TargetSubtargetInfo &STI) const {
    const InstrItineraryData *II = STI.getInstrItineraryData();
    DFAPacketizer *dfa =
        static_cast<const TCESubtarget &>(STI).createDFAPacketizer(II);
    assert(dfa != nullptr);
    return dfa;
}

