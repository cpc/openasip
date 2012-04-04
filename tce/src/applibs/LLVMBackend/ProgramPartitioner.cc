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
    std::cerr << "### Running ProgramPartitioner for " << MF.getFunction()->getName().str() << std::endl;
#endif
    const TCETargetMachine& targetMach = 
        dynamic_cast<const TCETargetMachine&>(
            MF.getTarget());

    /* Partition only clustered machines with vector backend support for now. */
    if (targetMach.maxVectorSize() == 0) return false;

    const TCETargetMachinePlugin& tmPlugin = 
        dynamic_cast<const TCETargetMachinePlugin&>(targetMach.targetPlugin());

    const llvm::MachineRegisterInfo& MRI = MF.getRegInfo();

    hash_map<const llvm::MachineInstr*, unsigned> partitions;

    for (MachineFunction::const_iterator i = MF.begin();
         i != MF.end(); i++) {
        
        for (MachineBasicBlock::const_iterator j = i->begin();
             j != i->end(); j++) {

            const llvm::MachineInstr& mi = *j; 
            if (partitions.find(&mi) != partitions.end())
                continue;
            unsigned nodeIndex = UINT_MAX;
            if (tmPlugin.isExtractElement(mi.getDesc().getOpcode())) {
                assert(mi.getNumOperands() >= 3 && mi.getOperand(2).isImm());
                nodeIndex = (unsigned)mi.getOperand(2).getImm();
#ifdef DEBUG_PROGRAM_PARTITIONER
                std::cerr << "[EXTRACT lane " << nodeIndex << "] " << std::endl;
#endif
            } else {
                /* Check if one of the parents of this instruction is already
                   partitioned. Propagate its node index to the current instruction. 

                   TODO: do it recursively until a root instruction or a partitioned 
                   instruction is found. */
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
        }
    }
    return true;
}

bool 
ProgramPartitioner::doFinalization(llvm::Module& /*M*/) {
    return false;
}

