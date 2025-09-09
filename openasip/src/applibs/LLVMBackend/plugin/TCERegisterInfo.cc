/*
    Copyright (c) 2002-2025 Tampere University.

    This file is part of OpenASIP.

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
 * @file TCERegisterInfo.cpp
 *
 * Implementation of TCERegisterInfo class.
 *
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 * @author Heikki Kultala 2011-2016 (heikki.kultala-no.spam-tut.fi)
 */

#include <assert.h>
#include "tce_config.h"
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/CodeGen/MachineInstrBuilder.h>
#include <llvm/CodeGen/MachineFrameInfo.h>
#include <llvm/CodeGen/TargetInstrInfo.h>
#include <llvm/Target/TargetOptions.h>

#include <llvm/ADT/STLExtras.h>
#include <llvm/CodeGen/RegisterScavenging.h>

#include "TCEPlugin.hh"
#include "TCERegisterInfo.hh"
#include "TCETargetMachine.hh"
#include "TCEInstrInfo.hh"
#include "TCEString.hh"
#include "Application.hh"
#include "tce_config.h"
#include "LLVMTCECmdLineOptions.hh"
#include "Exception.hh"

using namespace llvm;

#define GET_REGINFO_MC_DESC
#define GET_REGINFO_TARGET_DESC

#include "TCEFrameInfo.hh"
#include "TCEGenRegisterInfo.inc"
#include "ArgRegs.hh"

#ifdef TARGET64BIT
#define ADDIMM TCE::ADD64ssa
#define SUBIMM TCE::SUB64ssa
#define INTEGER_REG_CLASS TCE::R64IRegsRegClass
#else
#define ADDIMM TCE::ADDrri
#define SUBIMM TCE::SUBrri
#define INTEGER_REG_CLASS TCE::R32IRegsRegClass
#endif


/**
 * The Constructor.
 *
 * @param st Subtarget architecture.
 * @param tii Target architecture instruction info.
 */
TCERegisterInfo::TCERegisterInfo(
    const TargetInstrInfo& tii) :
    TCEGenRegisterInfo(TCE::RA),
    tii_(tii) {
}

/**
 * Returns list of callee saved registers.
 */
const MCPhysReg*
TCERegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
    static const uint16_t calleeSavedRegs[] = { TCE::FP, 0 };
    if (hasFP(*MF)) {
        return calleeSavedRegs+1; // skip first, it's reserved
    } else {
        return calleeSavedRegs;
    }
}

/**
 * Returns list of reserved registers.
 */
BitVector
TCERegisterInfo::getReservedRegs(const MachineFunction& mf) const {

    LLVMTCECmdLineOptions* options =
        dynamic_cast<LLVMTCECmdLineOptions*>(Application::cmdLineOptions());

    assert(&mf != NULL);
    BitVector reserved(getNumRegs());
    reserved.set(TCE::SP);
    reserved.set(TCE::KLUDGE_REGISTER);
    reserved.set(TCE::RA);
    const MachineFrameInfo* mfi = &mf.getFrameInfo();
    if (mfi->hasCalls() && tfi_->containsCall(mf)) {
        for (int i = 0; i < argRegCount; i++) {
            reserved.set(ArgRegs[i]);
        }
        setReservedVectorRegs(reserved);
    }
    if (hasFP(mf)) {
        reserved.set(TCE::FP);
    }

    return reserved;
}

bool TCERegisterInfo::eliminateFrameIndex(
    MachineBasicBlock::iterator II, int SPAdj,
    unsigned FIOperandNum,
    RegScavenger *RS) const {

    MachineInstr &MI = *II;
    const TCETargetMachine& tm =
        dynamic_cast<const TCETargetMachine&>(
            MI.getParent()->getParent()->getTarget());
    // Attempt to catch stack accesses using unsupported operation.
    auto osalOpName = tm.operationName(MI.getOpcode());
    if (!tm.validStackAccessOperation(osalOpName)
        && osalOpName.find("MOVE") == std::string::npos) {
        THROW_EXCEPTION(CompileError,
            "Error: Stack address space is not reachable with operation '"
            + tm.operationName(MI.getOpcode()) + "'.\n"
            + "Forgot to add the operation to the stack LSU?");
    }
    static int lastBypassReg = 0;
    const TCEInstrInfo &TII = dynamic_cast<const TCEInstrInfo&>(tii_);
    assert(SPAdj == 0 && "Unexpected");

    DebugLoc dl = MI.getDebugLoc();
    int FrameIndex = MI.getOperand(FIOperandNum).getIndex();
    // Addressable stack objects are accessed upwards from SP.
    MachineFunction &MF = *MI.getParent()->getParent();

    auto& frameinfo = MF.getFrameInfo();

    if (frameinfo.isSpillSlotObjectIndex(FrameIndex)) {
        for (MachineInstr::mmo_iterator i = MI.memoperands_begin();
             i != MI.memoperands_end(); i++) {
            const PseudoSourceValue* psv = (*i)->getPseudoValue();
            if (psv == NULL) {
                (*i)->setValue(new FixedStackPseudoSourceValue(FrameIndex, tm));
            }
        }
        if (MI.memoperands_begin() == MI.memoperands_end()) {
            // TODO: operation and size
            auto flags = static_cast<MachineMemOperand::Flags>(
                MI.mayLoad() * MachineMemOperand::MOLoad
                | MI.mayStore() * MachineMemOperand::MOStore);
#if LLVM_MAJOR_VERSION >= 21
            auto mmo = new MachineMemOperand(
              MachinePointerInfo(), flags, llvm::LocationSize::precise(0),
              Align(tfi_->stackAlignment()));
#else
            auto mmo = new MachineMemOperand(
              MachinePointerInfo(), flags, 0, Align(tfi_->stackAlignment()));
#endif
            mmo->setValue(new FixedStackPseudoSourceValue(FrameIndex, tm));
            MI.addMemOperand(MF, mmo);
        }
    }

    if (tfi_->hasFP(MF)) {
        int Offset = frameinfo.getObjectOffset(FrameIndex);
        int stackAlign = tfi_->stackAlignment();
        // FP storage space increases offset by stackAlign.
        Offset+= stackAlign;
        // RA storage space increases offset of incoming vars
        if (FrameIndex < 0  && frameinfo.hasCalls()
            && tfi_->containsCall(MF)) {
            Offset+= stackAlign;
        }

        if (Offset != 0) {
            // try to use a combined add+ld/st operation
            // (a base+offset load/store), if available
            // TODO: check that an offset port width is big enough for the
            // offset immediate.


            TCEString baseOffsetOp = "";
            TCEString originalOp = tm.operationName(MI.getOpcode());

            // TODO: are these the same for LE? ALD8 etc.?
            if (originalOp == "LDW" || originalOp == "LDHU" || 
                originalOp == "LDH" || originalOp == "LDQU" ||
                originalOp == "LDQ" || originalOp == "STW" ||
                originalOp == "STH" || originalOp == "STQ" ||
                originalOp == "LD32" || originalOp == "LDU16" || 
                originalOp == "LD16" || originalOp == "LDU8" ||
                originalOp == "LD8" || originalOp == "ST32" ||
                originalOp == "ST16" || originalOp == "ST8") {
                baseOffsetOp = TCEString("A") + originalOp;
            }
            if (baseOffsetOp != "" && tm.hasOperation(baseOffsetOp)) {
                const bool isStore = MI.getDesc().mayStore();
                unsigned storeDataReg = 0;
                uint64_t storeDataImm = 0;

                // the address should be always the 1st operand for the
                // base memory ops
                if (isStore) {
                    // operands: FI, 0, DATA
                    int storeDataOp = FIOperandNum + 2;
                    if (MI.getOperand(storeDataOp).isReg()) {                
                        storeDataReg = MI.getOperand(storeDataOp).getReg();
                    } else {
                        assert(MI.getOperand(storeDataOp).isImm());
                        storeDataImm = MI.getOperand(storeDataOp).getImm();
                    }
                }

                MI.setDesc(TII.get(tm.opcode(baseOffsetOp)));
                // now it should be safe to overwrite the 2nd operand
                // with the stack offset
                MI.getOperand(FIOperandNum).ChangeToRegister(TCE::FP, false);
                MI.getOperand(FIOperandNum + 1).setImm(Offset);

                if (isStore) {
                    // need to fix the 3rd input operand (the written data)
                    // for stores
                    if (storeDataReg) {
                        MI.getOperand(FIOperandNum + 2).ChangeToRegister(
                            storeDataReg, false);
                    } else {
                        MI.getOperand(FIOperandNum + 2).ChangeToImmediate(
                            storeDataImm);
                    }
                }
            } else { // FP, no base+offset ops
                MI.getOperand(FIOperandNum).ChangeToRegister(
                    TCE::KLUDGE_REGISTER, false, false, true/*iskill*/);
                //TODO clean up
                if (Offset > 0) {
                    BuildMI(
                        *MI.getParent(), II, MI.getDebugLoc(),
                        TII.get(ADDIMM), TCE::KLUDGE_REGISTER)
                            .addReg(TCE::FP).addImm(Offset);
                } else {
                    auto spOpcAndOffset = TII.getPointerAdjustment(Offset);
                    BuildMI(
                        *MI.getParent(), II, MI.getDebugLoc(),
                        TII.get(std::get<0>(spOpcAndOffset)),
                                TCE::KLUDGE_REGISTER)
                            .addReg(TCE::FP)
                            .addImm(std::get<1>(spOpcAndOffset));
                }
            }
        } else { // FP, offset 0
            MI.getOperand(FIOperandNum).ChangeToRegister(TCE::FP, false);
        }
    } else { // no FP
       int Offset = frameinfo.getObjectOffset(FrameIndex) + 
            frameinfo.getStackSize();

       if (Offset == 0) {
            MI.getOperand(FIOperandNum).ChangeToRegister(TCE::SP, false);
            #ifdef LLVM_OLDER_THAN_16
            return;
            #else
            return false;
            #endif
        }

       // try to use a combined add+ld/st operation (a base+offset load/store), 
       // if available
       // TODO: check that an offset port width is big enough for the offset 
       // immediate
       const TCETargetMachine& tm = 
           dynamic_cast<const TCETargetMachine&>(
               MI.getParent()->getParent()->getTarget());


       TCEString baseOffsetOp = "";
       TCEString originalOp = tm.operationName(MI.getOpcode());

       // TODO: are these the same for LE? ALD8 etc.?
       if (originalOp == "LDW" || originalOp == "LDHU" || 
           originalOp == "LDH" || originalOp == "LDQU" ||
           originalOp == "LDQ" || originalOp == "STW" ||
           originalOp == "STH" || originalOp == "STQ" ||
           originalOp == "LD32" || originalOp == "LDU16" || 
           originalOp == "LD16" || originalOp == "LDU8" ||
           originalOp == "LD8" || originalOp == "ST32" ||
           originalOp == "ST16" || originalOp == "ST8") {
           baseOffsetOp = TCEString("A") + originalOp;
       }
       if (baseOffsetOp != "" && tm.hasOperation(baseOffsetOp)) {
           const bool isStore = MI.getDesc().mayStore();
           unsigned storeDataReg = 0;
           uint64_t storeDataImm = 0;

           // the address should be always the 1st operand for the
           // base memory ops
           if (isStore) {
               // operands: FI, 0, DATA
               int storeDataOp = FIOperandNum + 2;
               if (MI.getOperand(storeDataOp).isReg()) {                
                   storeDataReg = MI.getOperand(storeDataOp).getReg();
               } else {
                   assert(MI.getOperand(storeDataOp).isImm());
                   storeDataImm = MI.getOperand(storeDataOp).getImm();
               }
           }

           MI.setDesc(TII.get(tm.opcode(baseOffsetOp)));
           // now it should be safe to overwrite the 2nd operand
           // with the stack offset
           MI.getOperand(FIOperandNum).ChangeToRegister(TCE::SP, false);
           MI.getOperand(FIOperandNum + 1).setImm(Offset);

           if (isStore) {
               // need to fix the 3rd input operand (the written data)
               // for stores
               if (storeDataReg) {
                   MI.getOperand(FIOperandNum + 2).ChangeToRegister(
                       storeDataReg, false);
               } else {
                   MI.getOperand(FIOperandNum + 2).ChangeToImmediate(
                       storeDataImm);
               }
           }
       } else {
           // generate the sp + offset addition before the use
           unsigned int tmp = 0;

           LLVMTCECmdLineOptions* options =
               dynamic_cast<LLVMTCECmdLineOptions*>(
                   Application::cmdLineOptions());
           if (RS != NULL) {
               tmp = RS->FindUnusedReg(&INTEGER_REG_CLASS);
           }

           if (tmp != 0) {
               MI.getOperand(FIOperandNum).ChangeToRegister(
                   tmp, false, false, true);
               BuildMI(
                   *MI.getParent(), II, MI.getDebugLoc(), TII.get(ADDIMM),
                   tmp).addReg(TCE::SP).addImm(Offset);
           } else {
               MI.getOperand(FIOperandNum).ChangeToRegister(
                   TCE::KLUDGE_REGISTER, false);
               BuildMI(
                   *MI.getParent(), II, MI.getDebugLoc(), TII.get(ADDIMM),
                   TCE::KLUDGE_REGISTER).addReg(TCE::SP).addImm(Offset);
           }
       }
    }
    #ifdef LLVM_OLDER_THAN_16
    return;
    #else
    return false;
    #endif
}


/**
 * Re-issue the specified 'original' instruction at the specific location 
 * targeting a new destination register.
 *
 * @param mbb Machine basic block of the new instruction.
 * @param i Position of the new instruction in the basic block.
 * @param destReg New destination register.
 * @param orig Original instruction.

void
TCERegisterInfo::reMaterialize(
    MachineBasicBlock& mbb,
    MachineBasicBlock::iterator i,
    unsigned destReg,
    const MachineInstr* orig) const {
    assert(false && "It really was used");
    MachineInstr* mi = mbb.getParent()->CloneMachineInstr(orig);
    mi->getOperand(0).setReg(destReg);
    mbb.insert(i, mi);
}
*/

/**
 * Not implemented: When is this method even called?
 */
unsigned
TCERegisterInfo::getRARegister() const {
    assert(false && "Remove this assert if this is really called.");
    return TCE::RA;
}

Register
TCERegisterInfo::getFrameRegister(const MachineFunction& mf) const {
    if (hasFP(mf)) {
        return TCE::FP;
    } else {
        return 0;
    }
}

bool
TCERegisterInfo::hasFP(const MachineFunction &MF) const {
    return tfi_->hasFP(MF);
}

bool
TCERegisterInfo::requiresRegisterScavenging(const MachineFunction&) const {
    return false;
}


