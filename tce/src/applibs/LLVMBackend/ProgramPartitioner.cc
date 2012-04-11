/*
    Copyright (c) 2012 Tampere University of Technology.

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
 * @file ProgramPartitioner.cc
 *
 * Declaration of ProgramPartitioner class.
 *
 * @author Pekka Jääskeläinen 2012
 */

#include <iostream>

#include "ProgramPartitioner.hh"
#include "TCETargetMachinePlugin.hh"
#include "TCETargetMachine.hh"
#include "hash_map.hh"
#include "Application.hh"

#include <llvm/Instruction.h>

char ProgramPartitioner::ID = 0;    

using namespace llvm;

//#define DEBUG_PROGRAM_PARTITIONER

llvm::Pass* 
createProgramPartitionerPass() {
    return new ProgramPartitioner();
}

bool 
ProgramPartitioner::doInitialization(llvm::Module& /*M*/) {
    return false;
}

bool 
ProgramPartitioner::runOnMachineFunction(llvm::MachineFunction& MF) {
#ifdef DEBUG_PROGRAM_PARTITIONER
    std::cerr << "### Running ProgramPartitioner for " 
              << MF.getFunction()->getName().str() << std::endl;
#endif
    const TCETargetMachine& targetMach = 
        dynamic_cast<const TCETargetMachine&>(
            MF.getTarget());

    /* Partition only clustered machines with vector backend support for now. */
    if (targetMach.maxVectorSize() == 0) return false;

#ifdef DEBUG_PROGRAM_PARTITIONER
    PRINT_VAR(targetMach.maxVectorSize());
#endif

    const TCETargetMachinePlugin& tmPlugin = 
        dynamic_cast<const TCETargetMachinePlugin&>(
            targetMach.targetPlugin());

    llvm::MachineRegisterInfo& MRI = MF.getRegInfo();

    hash_map<const llvm::MachineInstr*, unsigned> partitions;


    for (MachineFunction::const_iterator i = MF.begin();
         i != MF.end(); i++) {
        
        for (MachineBasicBlock::const_iterator j = i->begin();
             j != i->end(); j++) {

            const llvm::MachineInstr& mi = *j; 
            
            if (partitions.find(&mi) != partitions.end())
                continue; /* Already partitioned. */
            unsigned nodeIndex = UINT_MAX;
            if (tmPlugin.isExtractElement(mi.getDesc().getOpcode())) {
                assert(mi.getNumOperands() >= 3 && mi.getOperand(2).isImm());
                nodeIndex = (unsigned)mi.getOperand(2).getImm();
#ifdef DEBUG_PROGRAM_PARTITIONER
                std::cerr << "[EXTRACT lane " << nodeIndex << "] " << std::endl;
#endif
            } 

            if (nodeIndex == UINT_MAX) {
                /* Check if one of the parents of this instruction is already
                   partitioned. 

                   TODO: do it recursively until a root instruction or 
                   a partitioned instruction is found. */
                for (unsigned opr = 0; opr < mi.getNumOperands(); ++opr) {
                    const llvm::MachineOperand& operand = mi.getOperand(opr);
                    if (!operand.isReg()) continue;
                    const llvm::MachineInstr* parent = 
                        MRI.getVRegDef(operand.getReg());

                    if (parent == NULL) continue;
                    if (partitions.find(parent) == partitions.end()) continue;
                    nodeIndex = partitions[parent];
                    break;
                }
            }

            if (nodeIndex == UINT_MAX && 
                mi.memoperands_begin() != mi.memoperands_end()) {
                /* Check if one of the parents of this is a instruction for
                   which we can track the OpenCL work item id metadata. */
                for (llvm::MachineInstr::mmo_iterator mmoIter = 
                         mi.memoperands_begin();    
                     mmoIter != mi.memoperands_end(); ++mmoIter) {

                    llvm::MachineMemOperand* mo = *mmoIter;

                    if (mo->getValue() == NULL ||
                        !llvm::isa<const llvm::Instruction>(mo->getValue()))
                        continue;
                    // The matching instruction, if known.                    
                    const llvm::Instruction* instruction = 
                        llvm::cast<const llvm::Instruction>(mo->getValue());

                    SmallVector<std::pair<unsigned, MDNode *>, 4> MDs;
                    instruction->getAllMetadata(MDs);
                    for (SmallVectorImpl<std::pair<unsigned, MDNode *> >::iterator
                             MI = MDs.begin(), ME = MDs.end(); MI != ME; ++MI) {
                        MDNode *md = MI->second;
                        if (md->getNumOperands() < 5 || 
                            !isa<llvm::MDString>(md->getOperand(0)))                            
                            continue;
                        MDString* name = cast<llvm::MDString>(md->getOperand(0));
                        if (name->getString() != "WI_id") continue;
                        ConstantInt* id_x = cast<llvm::ConstantInt>(md->getOperand(1));
                        nodeIndex = 
                            (unsigned)id_x->getValue().getZExtValue() % 
                            targetMach.maxVectorSize();
#ifdef DEBUG_PROGRAM_PARTITIONER
                        std::cerr << "[FOUND OCL WI METADATA: " << nodeIndex << "]" << std::endl;
#endif                   

                    }
                }
            }


#ifdef DEBUG_PROGRAM_PARTITIONER
            if (nodeIndex != UINT_MAX) {
                std::cerr << "[NODE INDEX " << nodeIndex << "]: ";
                for (int pad = 0; pad < nodeIndex; ++pad) 
                    for (int ws = 0; ws < 30; ++ws) std::cerr << " ";
            } else {
                std::cerr << "[NODE INDEX UNKNOWN]: ";
            }
            mi.dump();
#endif
            if (nodeIndex == UINT_MAX) continue;
            partitions[&mi] = nodeIndex;

            /* TODO: Use the partition's register class instead of the super class to force
             the instruction's regs to be allocated from the partition. */
            if (mi.getNumOperands() == 0 || !mi.getOperand(0).isReg() || 
                !mi.getOperand(0).isDef()) continue;
            const llvm::MachineOperand& result = mi.getOperand(0);
            const llvm::TargetRegisterClass* nodeRegClass = 
                tmPlugin.nodeRegClass(nodeIndex, MRI.getRegClass(result.getReg()));
#ifdef DEBUG_PROGRAM_PARTITIONER
            std::cerr << "[ORIGINAL REG CLASS " 
                      << MRI.getRegClass(result.getReg())->getName() 
                      << "]" << std::endl;
#endif
            if (nodeRegClass == NULL) {
#ifdef DEBUG_PROGRAM_PARTITIONER
                std::cerr << "[NO REG CLASS FOUND FOR THE NODE]" << std::endl;
#endif                
            } else {
#ifdef DEBUG_PROGRAM_PARTITIONER
                std::cerr << "[ASSIGNED REG CLASS " 
                          << nodeRegClass->getName() << "]" << std::endl;
#endif
                MRI.setRegClass(result.getReg(), nodeRegClass);
            }
            
        }
    }
    return true;
}

bool 
ProgramPartitioner::doFinalization(llvm::Module& /*M*/) {
    return false;
}

