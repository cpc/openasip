/*
    Copyright (c) 2012 Tampere University.

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

#include "CompilerWarnings.hh"

IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "ProgramPartitioner.hh"
#include "TCETargetMachinePlugin.hh"
#include "TCETargetMachine.hh"
#include "hash_map.hh"
#include "Application.hh"
#include "tce_config.h"
#include <llvm/IR/Instruction.h>

POP_COMPILER_DIAGS

char ProgramPartitioner::ID = 0;    

using namespace llvm;

//#define DEBUG_PROGRAM_PARTITIONER
// define this if in case the DLP-partitioner does not find 
// a node id it should assign the registers to the EXTRAS node.
#define ASSIGN_UNKNOWN_TO_EXTRAS

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

#if (!(defined(LLVM_3_5)))
#ifdef DEBUG_PROGRAM_PARTITIONER
    std::cerr << "### ProgramPartitioner disabled for llvm 3.6+ " << std::endl;
#endif
    return true;
#endif

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
        bool changedT, changedD = false;
        // Loop through instruction in basic blocks looking for parent 
        // or metadata information about line assignment until there is 
        // no new information added.        
        do {
            changedT = false;
            changedD = false;
            for (MachineBasicBlock::const_iterator j = i->begin();
                j != i->end(); j++) {

                const llvm::MachineInstr& mi = *j; 
                
                if (partitions.find(&mi) != partitions.end())
                    continue; /* Already partitioned. */
                unsigned nodeIndex = tmPlugin.extractElementLane(mi);
            
                changedT |= findNodeIndex(mi, partitions, MF, nodeIndex);
                            
            }
     
#if 1
        // Same as before but starting from the bottom of basic block to the
        // top.

            for (MachineBasicBlock::const_iterator j = i->end();
                j != i->begin();) {
                j--;
                const llvm::MachineInstr& mi = *j; 
                
                if (partitions.find(&mi) != partitions.end())
                    continue; /* Already partitioned. */
                unsigned nodeIndex = tmPlugin.extractElementLane(mi);
            
                changedD |= findNodeIndex(mi, partitions, MF, nodeIndex);
                            
            }
#endif        
        } while (changedT || changedD);
    }
#ifdef ASSIGN_UNKNOWN_TO_EXTRAS
#ifdef DEBUG_PROGRAM_PARTITIONER
    std::cerr << "[setting the rest to EX]" << std::endl;
#endif
    /* Force the non-partitioned instructions to the
       extras node. This should include sequential C code and
       multi-WI address computations, branch condition code, 
       etc. for OpenCL C code. */
    for (MachineFunction::const_iterator i = MF.begin();
         i != MF.end(); i++) {
        
        for (MachineBasicBlock::const_iterator j = i->begin();
             j != i->end(); j++) {
            const llvm::MachineInstr& mi = *j; 

#ifdef LLVM_OLDER_THAN_10
            if (mi.getNumOperands() == 0 || !mi.getOperand(0).isReg() || 
                !mi.getOperand(0).isDef() || 
                llvm::TargetRegisterInfo::isPhysicalRegister(
                    mi.getOperand(0).getReg())) 
#else
            if (mi.getNumOperands() == 0 || !mi.getOperand(0).isReg() ||
                !mi.getOperand(0).isDef() ||
                Register::isPhysicalRegister(
                    mi.getOperand(0).getReg()))
#endif
                continue;
            
            if (partitions.find(&mi) != partitions.end())
                continue; /* Partitioned. */
            const llvm::MachineOperand& result = mi.getOperand(0);
            const llvm::TargetRegisterClass* newRegClass =
                tmPlugin.extrasRegClass(MRI.getRegClass(result.getReg()));
#ifdef DEBUG_PROGRAM_PARTITIONER
            std::cerr << "[ORIGINAL REG CLASS " 
                      << MRI.getRegClass(result.getReg())->getName() 
                      << "]" << std::endl;
#endif
            MRI.setRegClass(result.getReg(), newRegClass);
#ifdef DEBUG_PROGRAM_PARTITIONER
            std::cerr << "[ASSIGNED REG CLASS " 
                      << newRegClass->getName() << "]" << std::endl;
#endif
        }
    }
#endif
    return true;
}

bool 
ProgramPartitioner::doFinalization(llvm::Module& /*M*/) {
    return false;
}

/**
 *  Attemts to find an index of lane to which the instruction belongs
 * based on metadata or parent instruction assignment.
 * 
 * @param mi Machine instruction to analyze
 * @param paritions map between machine instruction and the lane indexes
 * @param MF machine function to which the mi belongs
 * @param nodeIndex index found
 */
bool
ProgramPartitioner::findNodeIndex(
    const llvm::MachineInstr &mi, 
    hash_map<const llvm::MachineInstr*, unsigned>& partitions,
    llvm::MachineFunction& MF,
    unsigned int& nodeIndex) {
    
    const TCETargetMachine& targetMach = 
        dynamic_cast<const TCETargetMachine&>(
            MF.getTarget());    
        
    const TCETargetMachinePlugin& tmPlugin = 
        dynamic_cast<const TCETargetMachinePlugin&>(
            targetMach.targetPlugin());

    llvm::MachineRegisterInfo& MRI = MF.getRegInfo();
    
    if (nodeIndex != UINT_MAX) {
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

            llvm::MachineRegisterInfo::def_iterator di = 
                MRI.def_begin(operand.getReg());
            if (di.atEnd()) continue;

            const llvm::MachineInstr* parent = (*di).getParent();

            // TODO: this NULL check not needed anymore?
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

            if (!instruction->getMetadata("wi")) {
                continue;
            }
            MDNode *md = instruction->getMetadata("wi");
            ConstantInt* id_x = NULL;
            if (llvm::isa<MDNode>(md->getOperand(2))) {
                // new wi metadata format is:
                // operand 0 - WI_id
                // operand 1 - MDNode with region id
                // operand 2 - MDNode with XYZ coordinates
                MDNode *xyz = dyn_cast<MDNode>(md->getOperand(2));                                                
                // New XYZ metadata format from pocl
                // operand 0 - WI_xyz
                // operand 1 - x coordinate
                // operand 2 - y coordinate
                // operand 3 - z coordinate
                // pick X coordinate
                id_x = cast<llvm::ConstantInt>(
                    dyn_cast<llvm::ConstantAsMetadata>(
                        xyz->getOperand(1))->getValue());
            } else {
                // old metadata format was
                // operand 0 - WI_id
                // operand 1 - region id
                // operand 2 - x coordinate
                // operand 3 - y coordinate
                // operand 4 - z coordinate
                // operand 5 - instruction number
                id_x = cast<llvm::ConstantInt>(
                    dyn_cast<llvm::ConstantAsMetadata>(
                        md->getOperand(2))->getValue());
            }
            if (id_x == NULL)
                continue;
            nodeIndex = 
                (unsigned)id_x->getValue().getZExtValue() % 
                targetMach.maxVectorSize();                    
#ifdef DEBUG_PROGRAM_PARTITIONER
            std::cerr << "[FOUND OCL WI METADATA: " << 
                nodeIndex << "]" << std::endl;
#endif                   
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
#endif
    if (nodeIndex == UINT_MAX) return false;
    partitions[&mi] = nodeIndex;

    /* TODO: Use the partition's register class instead of the super class to force
        the instruction's regs to be allocated from the partition. */
    for (unsigned int i = 0; i < mi.getNumOperands(); i++) {
        if (mi.getOperand(i).isReg() && 
            mi.getOperand(i).isDef()) {
            
            const llvm::MachineOperand& result = mi.getOperand(i);
#ifdef LLVM_OLDER_THAN_10
            if (llvm::TargetRegisterInfo::isPhysicalRegister(
                    mi.getOperand(i).getReg())) continue;
#else
            if (Register::isPhysicalRegister(
                    mi.getOperand(i).getReg())) continue;
#endif

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
