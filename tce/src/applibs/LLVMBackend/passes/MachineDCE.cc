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
#include "llvm/Constants.h"
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

#include "llvm/ADT/StringMap.h"

#include <iostream>
#include <map>

using namespace llvm;

namespace {    
  class VISIBILITY_HIDDEN MachineDCE :
        public MachineFunctionPass {
      
    typedef std::map<std::string, MachineFunction*> FunctionMap;
    typedef std::set<std::string> UserList;
    typedef std::map<std::string, UserList> UserRelations;
    typedef std::set<const std::string*> AvoidRecursionSet;

    /// Function name to MachineFunction map.
    FunctionMap functionMappings_;

    /// List of users of a symbol.
    UserRelations usersOfValue_;
      
    /// If users are traced to this list it means that function 
    /// cannot be eliminated.
    UserList baseUsers_;

    virtual bool doInitialization(Module &M);
    virtual bool runOnMachineFunction(MachineFunction &F);
    virtual bool doFinalization(Module &M);

    bool canFindStart(const std::string& user, AvoidRecursionSet& avoid_recursion);
    void addInitializer(const Constant* init, std::string& name);


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


/**
 * Returns true if can find startpoint.
 */ 
bool MachineDCE::canFindStart(const std::string& user, AvoidRecursionSet& avoid_recursion) {

    if (avoid_recursion.find(&user) != avoid_recursion.end()) {
	return false;
    } else {
	avoid_recursion.insert(&user);
    }

    // current function is actually the start point.. nice job.
    if (baseUsers_.find(user) != baseUsers_.end()) {
        std::cerr << "Found it!" << std::endl;
        return true;
    }

    UserList &usesList = usersOfValue_[user];
    std::cerr << "Checking " << user << std::endl;

    // check if this function is used by start point to be sure...
    for (UserList::iterator i= usesList.begin(); i != usesList.end(); i++) {
        if (canFindStart((*i), avoid_recursion)) {
            return true;
        }
    }    

    std::cerr << "Done without finding entry point " << user 
              << std::endl;
    return false;
}

void MachineDCE::addInitializer(const Constant* init, std::string& name) {

    if (const GlobalValue* gv = dyn_cast<GlobalValue>(init)) {
        std::cerr << "Added data " << name 
                  << " to uses of value: " << gv->getNameStr()
                  << " and " << name << " to baseUsers." 
                  <<  std::endl;

        baseUsers_.insert(name);
        usersOfValue_[gv->getNameStr()].insert(name);
    }
    
    if ((dyn_cast<ConstantArray>(init) != NULL) ||
        (dyn_cast<ConstantStruct>(init) != NULL) ||
        (dyn_cast<ConstantVector>(init) != NULL) ||
        (dyn_cast<ConstantExpr>(init) != NULL)) {

        for (unsigned i = 0, e = init->getNumOperands(); i != e; ++i) {
            addInitializer(init->getOperand(i), name);
        }
    }
}

bool MachineDCE::doInitialization(Module &M) {    

    // add first function to baseUsers
    baseUsers_.insert(M.begin()->getNameStr());
    
    // Go through global variables to find out 
    for (Module::const_global_iterator i = M.global_begin();
         i != M.global_end(); i++) {
        
        std::string name = i->getNameStr();        

        if (!i->hasInitializer()) {
            continue;
            assert(false && "No initializer. External linkage?");
        }
        
        const Constant* initializer = i->getInitializer();
        const Type* type = initializer->getType();

        std::cerr << "Data name: " << name 
                  << "\ttype: " << type->getDescription() << std::endl;        
        
        addInitializer(initializer, name);        
    }

    return true;
}

bool MachineDCE::runOnMachineFunction(MachineFunction &F) {
    
    std::string funcName = F.getFunction()->getNameStr();

    // add function to function map...
    functionMappings_[funcName] = &F;
    
    for (MachineFunction::const_iterator i = F.begin();
         i != F.end(); i++) {
        
        for (MachineBasicBlock::const_iterator j = i->begin();
             j != i->end(); j++) {
            
            for (unsigned k = 0; k < j->getNumOperands(); k++) {
                const MachineOperand& mo = j->getOperand(k);
                
                std::string moName;

                if (mo.isGlobalAddress()) {
                    moName = mo.getGlobal()->getNameStr();
                } else if (mo.isExternalSymbol()) {
                    moName = mo.getSymbolName();
                }
                
                if (!moName.empty()) {
                    usersOfValue_[moName].insert(funcName);
                }
            }
        }        
    }    
    return true;
}

bool MachineDCE::doFinalization(Module &M) {    
    
    // For all functions check that they are reached from entrypoint of module.
    for (FunctionMap::iterator func = functionMappings_.begin(); 
         func != functionMappings_.end(); ++func) {

        AvoidRecursionSet avoid_recursion;

        if (!canFindStart(func->first, avoid_recursion)) {
            std::cerr << "Function was not referred trying to delete all " 
                      << "MachineBasicBlocks of : " << func->first 
                      << std::endl;

            MachineFunction* mf = func->second;
            while (!mf->empty()) {
                MachineBasicBlock &BB = mf->front();
                eraseFromParent(&BB);
            }
        }
    }
   
    return true;
}
