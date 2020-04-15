/*
    Copyright (c) 2002-2015 Tampere University.

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
 * @file ConstantTransformer.cc
 *
 * Implementation of ConstantTransformer class.
 *
 * @author Pekka Jääskeläinen 2015 
 * @note reting: red
 */

#include "CompilerWarnings.hh"

IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "ConstantTransformer.hh"

#include "MachineInfo.hh"
#include "TCETargetMachine.hh"
#include "OperationPool.hh"
#include "Operation.hh"
#include "Operand.hh"

#include <llvm/CodeGen/MachineFunction.h>
#include <llvm/CodeGen/MachineBasicBlock.h>
#include <llvm/MC/MCInstrInfo.h>
#ifdef LLVM_OLDER_THAN_6_0
#include <llvm/Target/TargetInstrInfo.h>
#include <llvm/Target/TargetSubtargetInfo.h>
#include <llvm/Target/TargetOpcodes.h>
#else
#include <llvm/CodeGen/TargetInstrInfo.h>
#include <llvm/CodeGen/TargetSubtargetInfo.h>
#include <llvm/CodeGen/TargetOpcodes.h>
#endif
#include <llvm/CodeGen/MachineInstrBuilder.h>

POP_COMPILER_DIAGS

#include <sstream>

using llvm::MachineBasicBlock;
using llvm::MachineFunction;
using llvm::MachineOperand;
using llvm::TCETargetMachine;
using llvm::TCETargetMachinePlugin;

char ConstantTransformer::ID; 

bool
ConstantTransformer::doInitialization(llvm::Module&) {
    return false;
}

/**
 * In case the given operandId is an input operand in the
 * MachineInstr, returns the corresponding OSAL operand id
 * (starting from 1), 0 otherwise.
 */
unsigned
osalInputIndex(
    const Operation& operation, 
    const llvm::MachineInstr& instr, 
    unsigned operandId) {

    const TCETargetMachine& tm = 
        dynamic_cast<const TCETargetMachine&>(
            instr.getParent()->getParent()->getTarget());

    TCEString operationName = tm.operationName(instr.getDesc().getOpcode());
    bool hasGuard = operationName[0] == '?' || operationName[0] == '!';

    unsigned osalIndex = 0;
    for (unsigned operandI = 0; operandI < instr.getNumOperands(); ++operandI) {
        const MachineOperand& mo = instr.getOperand(operandI);
        if (hasGuard && operandI == 0) continue;
        // Output or metadata.
        if ((mo.isReg() && (mo.isDef() || mo.isImplicit())) || mo.isMetadata())
            continue;
        ++osalIndex;
        if (operandI == operandId) return osalIndex;
        // LLVM machineinstructions always present the addresses in the
        // base + offset form, thus consume two input operands per one
        // OSAL operand. Skip the offset operand in case the OSAL operation
        // only the takes a single absolute address.
        if (operation.operand(osalIndex).isAddress() && 
            !operation.isBaseOffsetMemOperation()) {
            ++operandI;
        }
    }

    return 0;
}

bool
ConstantTransformer::runOnMachineFunction(llvm::MachineFunction& mf) {

    const TCETargetMachine& tm = 
        dynamic_cast<const TCETargetMachine&>(mf.getTarget());
    TCETargetMachinePlugin& plugin = tm.targetPlugin();

    OperationPool osal;

    bool changed = false;
    for (MachineFunction::iterator i = mf.begin(); i != mf.end(); i++) {
        MachineBasicBlock& mbb = *i;
        for (MachineBasicBlock::iterator j = mbb.begin();
             j != mbb.end(); j++) {
#if LLVM_OLDER_THAN_4_0
            const llvm::MachineInstr* mi = j;
#else
            const llvm::MachineInstr* mi = &*j;
#endif
            unsigned opc = mi->getOpcode();

            const llvm::MCInstrDesc& opDesc = mi->getDesc();
            if (opDesc.isReturn()) {
                continue;
            }
            if (opc == llvm::TargetOpcode::DBG_VALUE || 
                opc == llvm::TargetOpcode::KILL) {
                continue;
            }

            TCEString opname = tm.operationName(opc);
            if (opname == "") continue;
            bool hasGuard = opname[0] == '?' || opname[0] == '!';
            if (hasGuard) opname = opname.substr(1);

            const Operation& op = osal.operation(opname.c_str());
            if (op.isNull() || op.isBranch() || op.isCall() || 
                op.readsMemory()) {
                // isNull = INLINE asm, a pseudo asm block or similar.
                // TODO: add support for at least INLINE and MOVE.

                // In case the operation reads memory, assume the 
                // possible immediate operand is a global address
                // we cannot fix at this point.
                // TODO: Fix global address constants. Needs to have new type
                // of data symbol/relocation info in case an address
                // constant is broken down.

                continue;
            }

            for (unsigned operandI = 0; operandI < mi->getNumOperands();
                 ++operandI) {
                
                const MachineOperand& mo = mi->getOperand(operandI);
                if (!mo.isImm()) continue;

                unsigned inputIndex = osalInputIndex(op, *mi, operandI);
                if (inputIndex == 0) continue; 
                const Operand& operand = op.operand(inputIndex);
                if (operand.isNull() || !operand.isInput()) {
                    Application::errorStream() 
                        << "Input " << inputIndex 
                        << " not found for operation "
                        << opname << std::endl;
                    assert(false);
                }
                assert(operand.isInput());
                size_t operandWidth = operand.width();

                if (MachineInfo::canEncodeImmediateInteger(
                        mach_, mo.getImm(), operandWidth)) continue;
                    
                // check if it's possible to convert C to SUB(0, -C) such
                // that it can be encoded for the target
                if (MachineInfo::canEncodeImmediateInteger(mach_, 0) &&
                    MachineInfo::canEncodeImmediateInteger(
                        mach_, -mo.getImm()) && /* SUB is 32b */
                    MachineInfo::supportsOperation(mach_, "SUB")) {  

#if LLVM_OLDER_THAN_6_0
                    const llvm::MCInstrInfo* iinfo =
                        mf.getTarget().getSubtargetImpl(
                            *mf.getFunction())->getInstrInfo();
#else
                    const llvm::MCInstrInfo* iinfo = 
                        mf.getTarget().getSubtargetImpl(
                            mf.getFunction())->getInstrInfo();
#endif
#if 0
                    Application::logStream() 
                        << "ConstantTransformer: converting constant in ";
#endif
                    // RV_HIGH = SUB 0 -X
                    BuildMI(
                        mbb, j, j->getDebugLoc(), iinfo->get(plugin.opcode("SUB")), 
                        plugin.rvHighDRegNum()).addImm(0).addImm(-mo.getImm());
                        
                    // replace the original instruction's immediate operand
                    // with the RV_HIGH
                    llvm::MachineInstrBuilder mib = 
                        BuildMI(mbb, j, j->getDebugLoc(), j->getDesc());
                    for (unsigned opr = 0; opr < j->getNumOperands(); ++opr) {
                        MachineOperand& orig = j->getOperand(opr);
                        if (opr == operandI) {
#ifdef LLVM_OLDER_THAN_5_0
                            mib.addOperand(
                                MachineOperand::CreateReg(plugin.rvHighDRegNum(), false));
#else
			    mib.add(MachineOperand::CreateReg(
					plugin.rvHighDRegNum(), false));
#endif
                            continue;
                        }
#ifdef LLVM_OLDER_THAN_5_0
                        mib.addOperand(orig);
#else
                        mib.add(orig);
#endif
                        orig.clearParent();
                    }
                        
                    // the original instruction can be now deleted
                    j->eraseFromParent();
                    // start scanning the MBB from the beginning due to
                    // possibly invalidated iterators from adding the 
                    // new instructions
                    j = mbb.begin();
                    changed = true;
                } else {
                    std::ostringstream errMsg;
                    errMsg << "Program uses constant '"
                           << mo.getImm() << "' that cannot be encoded "
                           << "for the machine by the current compiler.";
                    throw CompileError(
                        __FILE__, __LINE__, __func__, errMsg.str());
                }
            }
        }
           
    }
    return changed;
}

bool
ConstantTransformer::doFinalization(llvm::Module&) {
    return false;
}
