/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file OperationPoolPimpl.cc
 *
 * Definition of OperationPoolPimpl (private implementation) class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011
 * @note rating: red
 */

#include <string>
#include <vector>

#include "OperationPool.hh"
#include "OperationModule.hh"
#include "Operation.hh"
#include "Operand.hh"
#include "OperationBehaviorProxy.hh"
#include "OperationDAGBehavior.hh"
#include "OperationBehaviorLoader.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "Application.hh"
#include "AssocTools.hh"
#include "SequenceTools.hh"
#include "StringTools.hh"
#include "Application.hh"
#include "OperationIndex.hh"
#include "OperationPoolPimpl.hh"
#include "OperationSerializer.hh"
#include "TCEString.hh"

// disable warnings from LLVm headers
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include <llvm/Target/TargetInstrDesc.h>
#include <llvm/Target/TargetInstrInfo.h>

#pragma GCC diagnostic warning "-Wunused-parameter"

using std::vector;
using std::string;

OperationPoolPimpl::OperationTable OperationPoolPimpl::operationCache_;
std::vector<OperationBehaviorProxy*> OperationPoolPimpl::proxies_;
OperationIndex* OperationPoolPimpl::index_(NULL);
OperationBehaviorLoader* OperationPoolPimpl::loader_(NULL);
const llvm::TargetInstrInfo* OperationPoolPimpl::llvmTargetInstrInfo_(NULL);

/**
 * The constructor
 */
OperationPoolPimpl::OperationPoolPimpl() {
    // if this is a first created instance of OperationPool,
    // initialize the OperationIndex instance with the search paths
    if (index_ == NULL) {
        index_ = new OperationIndex();
        vector<string> paths = Environment::osalPaths();
        for (unsigned int i = 0; i < paths.size(); i++) {
            index_->addPath(paths[i]);
        }
        loader_ = new OperationBehaviorLoader(*index_);
    }
}

/**
 * The destructor
 */
OperationPoolPimpl::~OperationPoolPimpl() {
}

/**
 * Cleans up the static Operation cache.
 *
 * Deletes also the Operation instances, so be sure you are not using
 * them after calling this!
 */
void 
OperationPoolPimpl::cleanupCache() {
    AssocTools::deleteAllValues(operationCache_);
    SequenceTools::deleteAllItems(proxies_);
    delete index_;
    index_ = NULL;
    delete loader_;
    loader_ = NULL;
}

/**
 * Looks up an operation identified by its name and returns a reference to it.
 *
 * The first found operation is returned. If operation is not found, a null 
 * operation is returned.
 * 
 * @param name The name of the operation.
 * @return The wanted operation.
 */
Operation&
OperationPoolPimpl::operation(const char* name) {
  
    OperationTable::iterator it = 
        operationCache_.find(StringTools::stringToLower(name));
    if (it != operationCache_.end()) {
        return *((*it).second);
    }
    
    // If llvmTargetInstrInfo_ is set, the scheduler is called
    // directly from LLVM code gen. Use the TargetInstrDesc as
    // the source for operation info instead.
    if (llvmTargetInstrInfo_ != NULL) {
        for (unsigned opc = 0; opc < llvmTargetInstrInfo_->getNumOpcodes();
             ++opc) {
#ifdef LLVM_2_9
            const llvm::TargetInstrDesc& tid = llvmTargetInstrInfo_->get(opc);
#else
            const llvm::MCInstrDesc& tid = llvmTargetInstrInfo_->get(opc);
#endif
            TCEString operName = TCEString(tid.getName()).lower();
            if (operName == TCEString(name).lower()) {
                Operation* llvmOperation = loadFromLLVM(tid);
                operationCache_[operName] = llvmOperation;
                return *llvmOperation;
            } 
        }
        abortWithError(
            TCEString("Did not find info for LLVM operation ") + name);
    }

    OperationModule& module = index_->moduleOf(name);
    if (&module == &NullOperationModule::instance()) {
        return NullOperation::instance();
    }
    
    Operation* found = NULL;

    serializer_.setSourceFile(module.propertiesModule());
    ObjectState* root = serializer_.readState();
    ObjectState* child = NULL;
    
    // load operations
    for (int i = 0; i < root->childCount(); i++) {
        child = root->child(i);
        const std::string operName = 
            root->child(i)->stringAttribute(Operation::OPRN_NAME);
      
        Operation* oper = 
            new Operation(operName, NullOperationBehavior::instance());
      
        oper->loadState(child);
        operationCache_[StringTools::stringToLower(operName)] = oper;
      
        if (StringTools::ciEqual(operName, name)) {
            found = oper;
        }            
    }
    
    // add behaviours
    for (std::map<std::string, Operation*>::iterator 
             iter = operationCache_.begin();
         iter != operationCache_.end(); iter++) {
      
        Operation* oper = iter->second;
      
        OperationBehaviorProxy* proxy = 
            new OperationBehaviorProxy(*oper, *loader_);
        proxies_.push_back(proxy);
        oper->setBehavior(*proxy);
    }
    
    delete root;
    root = NULL;

    if (found != NULL) {
        return *found;
    } else {
        return NullOperation::instance();            
    }
}

/**
 * Loads an OSAL Operation from LLVM TargetInstrDesc.
 *
 * Used for avoiding the need for .xml OSAL databases when TCE scheduler
 * is called directly for non-TTA LLVM targets.
 */
Operation*
OperationPoolPimpl::loadFromLLVM(
#ifdef LLVM_2_9
const llvm::TargetInstrDesc& tid
#else
const llvm::MCInstrDesc& tid
#endif
) {
    TCEString opName = TCEString(tid.getName());
    Operation* op = new Operation(opName, NullOperationBehavior::instance());
   
    unsigned outputs = tid.getNumDefs();
    unsigned inputs = tid.getNumOperands() - outputs;

    // at least a minimal implicit trigger input
    // RET of SPU does not include the implicit link register
    // operand even though it will be generated in the final 
    // assembly to just an absolute jump to it 
    if (inputs == 0) 
        inputs = 1; 

    for (unsigned opr = 0; opr < outputs; ++opr) {
        Operand* operand = 
            new Operand(false, inputs + opr + 1, Operand::UINT_WORD);
        op->addOutput(operand);
    }

    for (unsigned opr = 0; opr < inputs; ++opr) {
        Operand* operand = new Operand(true, opr + 1, Operand::UINT_WORD);
        op->addInput(operand);
    }

    return op;
}

/**
 * Returns the operation index of operation pool.
 *
 * @return The operation index.
 */
OperationIndex&
OperationPoolPimpl::index() {
    assert(index_ != NULL);
    return *index_;
}

