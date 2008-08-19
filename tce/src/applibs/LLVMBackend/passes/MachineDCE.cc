//===-- UnreachableBlockElim.cpp - Remove unreachable blocks for codegen --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This pass is an extremely simple version of the SimplifyCFG pass.  Its sole
// job is to delete LLVM basic blocks that are not reachable from the entry
// node.  To do this, it performs a simple depth first traversal of the CFG,
// then deletes any unvisited nodes.
//
// Note that this pass is really a hack.  In particular, the instruction
// selectors for various targets should just not generate code for unreachable
// blocks.  Until LLVM has a more systematic way of defining instruction
// selectors, however, we cannot really expect them to handle additional
// complexity.
//
//===----------------------------------------------------------------------===//

#include "llvm/CodeGen/Passes.h"
#include "llvm/Constant.h"
#include "llvm/Instructions.h"
#include "llvm/Function.h"
#include "llvm/Pass.h"
#include "llvm/Type.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/CFG.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/ADT/DepthFirstIterator.h"

#include <iostream>

using namespace llvm;

namespace {
  class VISIBILITY_HIDDEN UnreachableMachineBlockElim :
        public MachineFunctionPass {
    virtual bool runOnMachineFunction(MachineFunction &F);
         
      std::vector<MachineBasicBlock*> AllBlocks;
      std::vector<MachineBasicBlock*> ReacheableBlocks;

  public:
    static char ID; // Pass identification, replacement for typeid
    UnreachableMachineBlockElim() : MachineFunctionPass((intptr_t)&ID) {}
  };
}
char UnreachableMachineBlockElim::ID = 0;

Pass* createMachineDCE() {
    return new UnreachableMachineBlockElim();
}

/**
 * New method in 2.4 
 */
void eraseFromParent(MachineBasicBlock* fake_this) {
    assert(fake_this->getParent() && "Not embedded in a function!");    
    MachineFunction* mf = fake_this->getParent();
    mf->getBasicBlockList().erase(fake_this);
    
}

bool UnreachableMachineBlockElim::runOnMachineFunction(MachineFunction &F) {
    
    /*
    
    // Mark all reachable blocks.
    for (df_ext_iterator<MachineFunction*> I = df_ext_begin(&F, Reachable),
             E = df_ext_end(&F, Reachable); I != E; ++I)
    
    // Loop over all dead blocks, remembering them and deleting all instructions
    // in them.
    std::vector<MachineBasicBlock*> DeadBlocks;
    for (MachineFunction::iterator I = F.begin(), E = F.end(); I != E; ++I) {
    MachineBasicBlock *BB = I;

    // Test for deadness.
    if (!Reachable.count(BB)) {
      DeadBlocks.push_back(BB);

      while (BB->succ_begin() != BB->succ_end()) {
        MachineBasicBlock* succ = *BB->succ_begin();

        MachineBasicBlock::iterator start = succ->begin();
        while (start != succ->end() &&
               start->getOpcode() == TargetInstrInfo::PHI) {
          for (unsigned i = start->getNumOperands() - 1; i >= 2; i-=2)
            if (start->getOperand(i).isMBB() &&
                start->getOperand(i).getMBB() == BB) {
              start->RemoveOperand(i);
              start->RemoveOperand(i-1);
            }

          start++;
        }

        BB->removeSuccessor(BB->succ_begin());
      }
    }


    }
  
  std::cerr << "Blocks to kills: " << DeadBlocks.size() << std::endl;

  // Actually remove the blocks now.
  for (unsigned i = 0, e = DeadBlocks.size(); i != e; ++i) {
      // DeadBlocks[i]->eraseFromParent();
      eraseFromParent(DeadBlocks[i]);
  }

  // Cleanup PHI nodes.
  for (MachineFunction::iterator I = F.begin(), E = F.end(); I != E; ++I) {
    MachineBasicBlock *BB = I;
    // Prune unneeded PHI entries.
    SmallPtrSet<MachineBasicBlock*, 8> preds(BB->pred_begin(),
                                             BB->pred_end());
    MachineBasicBlock::iterator phi = BB->begin();
    while (phi != BB->end() &&
           phi->getOpcode() == TargetInstrInfo::PHI) {
      for (unsigned i = phi->getNumOperands() - 1; i >= 2; i-=2)
        if (!preds.count(phi->getOperand(i).getMBB())) {
          phi->RemoveOperand(i);
          phi->RemoveOperand(i-1);
        }

      if (phi->getNumOperands() == 3) {
        unsigned Input = phi->getOperand(1).getReg();
        unsigned Output = phi->getOperand(0).getReg();

        MachineInstr* temp = phi;
        ++phi;

        temp->eraseFromParent();

        if (Input != Output)
          F.getRegInfo().replaceRegWith(Output, Input);

        continue;
      }

      ++phi;
    }
  }

  F.RenumberBlocks();

  return DeadBlocks.size();
*/
    return true;
}
