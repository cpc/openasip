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

#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Module.h"
#include "llvm/Support/Debug.h"

#include <iostream>
#include <map>

using namespace llvm;

namespace {    
  class VISIBILITY_HIDDEN MachineDCE :
        public MachineFunctionPass {
      
    typedef std::map<const Function*, MachineFunction*> FunctionMap;
    typedef std::vector<const llvm::User*> UserList;
    typedef std::map<const Function*, UserList> UserRelations;
    typedef std::map<const llvm::User*, const Function*> ParentMap;

    FunctionMap functionMappings;
    UserRelations usersOfFunction;

    virtual bool runOnMachineFunction(MachineFunction &F);
    virtual bool doFinalization(Module &M);

    bool canFindStart(
      const Function* curr, const Function* start,
      UserRelations &uses, ParentMap &parents);


  public:
    static char ID; // Pass identification, replacement for typeid
    MachineDCE() : MachineFunctionPass((intptr_t)&ID) {}
  };
}
char MachineDCE::ID = 0;

Pass* createMachineDCE() {
    return new MachineDCE();
}

/**
 * New method in 2.4 
 */
void eraseFromParent(MachineBasicBlock* fake_this) {
    assert(fake_this->getParent() && "Not embedded in a function!");    
    MachineFunction* mf = fake_this->getParent();
    mf->getBasicBlockList().erase(fake_this);    
}

bool MachineDCE::runOnMachineFunction(MachineFunction &F) {
    const Function* currFunc = F.getFunction();    
    DOUT << currFunc->getNameStr() << " Address: " 
         << std::hex << (unsigned)currFunc << std::endl;    

    // Could be done in finalize as well.
    DOUT << "Users:";
    for (Value::use_const_iterator i = currFunc->use_begin(); 
         i != currFunc->use_end(); ++i) {
        DOUT << " " << *i << " Name: " << i->getNameStr();
        usersOfFunction[currFunc].push_back(*i);
    }
    DOUT << std::endl;
   
    functionMappings[F.getFunction()] = &F; 
    return true;
}

/**
 * Returns true if can find startpoint.
 */ 
bool MachineDCE::canFindStart(const Function* curr, const Function* start,
                              UserRelations &uses, ParentMap &parents) {

    UserList &usesVec = uses[curr];
    DOUT << "Checking " << curr->getNameStr() << std::endl;

    // current function is actually the start point.. nice job.
    if (curr == start) {
        DOUT << "Found it!" << std::endl;
        return true;
    }

    // check if this function is used by start point to be sure...
    for (unsigned int i=0; i < usesVec.size(); i++) {
        const Function* parent = NULL;
        if (parents.find(usesVec[i]) != parents.end()) {
            parent = parents[usesVec[i]];
        } else {
            DOUT << "Could not find parent! Lets assume it is needed then.\n";
            return true;
        }
        DOUT << "Parent name: " << parent->getNameStr() << std::endl;
        if (canFindStart(parent, start, uses, parents)) {
            return true;
        }
    }    

    DOUT << "Done without finding entry point " << curr->getNameStr() 
         << std::endl;
    return false;
}

bool MachineDCE::doFinalization(Module &M) {    
    // find entry function of prgogram...
    Function& startPoint = *(M.begin());
    ParentMap parentOfUser;

    // Get parent functions of llvm::User objects for tracing users.
    for (Module::iterator F = M.begin(), E = M.end(); F != E; ++F) 
        // For all basic blocks...
        for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
            // For all instructions...
            for (BasicBlock::iterator I = BB->begin(), E = BB->end(); 
                 I != E; ++I) {
                parentOfUser[&(*I)] = &(*F);
            }
    
    // For all functions check that they are reached from entrypoint of module.
    for (Module::iterator F = M.begin(), E = M.end(); F != E; ++F) {
        if (!canFindStart(&(*F), &startPoint, usersOfFunction, parentOfUser)) {
            // TODO: use DOUT
            std::cerr << "Function was not referred trying to delete all " 
                 << "MachineBasicBlocks of : " << F->getNameStr() 
                 << std::endl;

            MachineFunction* mf = functionMappings[&(*F)];            
            while (!mf->empty()) {
                MachineBasicBlock &BB = mf->front();
                eraseFromParent(&BB);
            }
        }
    }
   
    return true;
}
