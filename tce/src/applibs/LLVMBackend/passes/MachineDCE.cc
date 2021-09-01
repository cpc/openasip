/*
    Copyright (c) 2012-2020 Tampere University.

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
 * @file MachineDCE.cc
 *
 * Implementation of MachineDCE class.
 *
 * @author Mikael Lepistö 
 */

//===-- MachineDCE.cc - Find unreachable functions for codegen --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Simple symbol based bookkeeping of which functions are really referred in code.
// Analyse is used by code generator finalization. (Removes unneeded functions)
//
//===----------------------------------------------------------------------===//

#include <iostream>

#include "MachineDCE.hh"

using namespace llvm;

// register as analysis pass...
static RegisterPass<MachineDCE> 
R("machinedce","Symbol string based machine DCE for removing not used emulation functions", false, true);
char MachineDCE::ID = 0;

//#define DEBUG_MACHINE_DCE

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
        return true;
    }

    UserList &usesList = usersOfValue_[user];
    
    // check if this function is used by start point to be sure...
    for (UserList::iterator i= usesList.begin(); i != usesList.end(); i++) {
        if (canFindStart((*i), avoid_recursion)) {
            return true;
        }
    }    
    return false;
}

void MachineDCE::addInitializer(const Constant* init, std::string& name) {

    if (const GlobalValue* gv = dyn_cast<GlobalValue>(init)) {
#ifdef DEBUG_MACHINE_DCE
        errs() << "Added data " << name 
               << " to uses of value: " << gv->getName()
               << " and " << name << " to baseUsers." 
               << "\n";
#endif
        baseUsers_.insert(name);
        usersOfValue_[gv->getName().str()].insert(name);
    }
    
    if ((dyn_cast<ConstantArray>(init) != NULL) ||
        (dyn_cast<ConstantStruct>(init) != NULL) ||
        (dyn_cast<ConstantVector>(init) != NULL) ||
        (dyn_cast<ConstantExpr>(init) != NULL)) {
        
        for (unsigned i = 0, e = init->getNumOperands(); i != e; ++i) {
            //init->getOperand(i)->dump();
            //addInitializer(dyn_cast<const Constant*>(init->getOperand(i)), name);
            addInitializer(cast<Constant>(init->getOperand(i)), name);
        }
    }
}

bool MachineDCE::doInitialization(Module &M) {

#ifdef DEBUG_MACHINE_DCE
    std::cerr << "Initializing MachineDCE\n";
#endif

    // Add the first function to baseUsers, assume it's the startup
    // function.
    baseUsers_.insert(M.begin()->getName().str());
    for (Module::const_iterator f = M.begin(), e = M.end(); f != e; ++f) {
        if (!f->hasInternalLinkage())
            baseUsers_.insert(f->getName().data());
#ifdef DEBUG_MACHINE_DCE
        std::cerr << "Added " << f->getName().str()
                  << " to base functions due to it not having internal linkage"
                  << std::endl;
        f->dump();
#endif
        continue;
    }

    // Go through global variables to find out.
    for (Module::const_global_iterator i = M.global_begin();
         i != M.global_end(); i++) {
        std::string name = i->getName().str();

        if (!i->hasInitializer()) {
            continue;
            assert(false && "No initializer. External linkage?");
        }

        const Constant* initializer = i->getInitializer();
#if 0
        const Type* type = initializer->getType();
        errs() << "Data name: " << name
               << "\ttype: " << type->getDescription() << "\n";
#endif
        addInitializer(initializer, name);        
    }
    
    return true;
}

bool MachineDCE::runOnMachineFunction(MachineFunction &F) {
    std::string funcName = F.getFunction().getName().str();

    // add function to function map...
    functionMappings_[funcName] = &F;
    
    for (MachineFunction::const_iterator i = F.begin();
         i != F.end(); i++) {        
        for (MachineBasicBlock::const_iterator j = i->begin();
             j != i->end(); j++) {
            
            for (unsigned k = 0; k < j->getNumOperands(); k++) {
                const MachineOperand& mo = j->getOperand(k);
                
                std::string moName;

                if (mo.isGlobal()) {
                    moName = mo.getGlobal()->getName().str();
                } else if (mo.isSymbol()) {
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


bool MachineDCE::doFinalization(Module&) {    

    // errs() << "Finalizing MachineDCE\n";
    
    // For all functions check that they are reached from entrypoint of module.
    for (FunctionMap::iterator func = functionMappings_.begin(); 
         func != functionMappings_.end(); ++func) {

        AvoidRecursionSet avoid_recursion;

        if (!canFindStart(func->first, avoid_recursion)) {
#ifdef DEBUG_MACHINE_DCE
            std::cerr << "Function was not referred add it to dce data: " 
                      << func->first << "\n";
#endif
            removeableFunctions.insert(func->first);
        }
    }
   
    return true;
}
