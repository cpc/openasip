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
 * @file MachineDCE.hh
 *
 * Declaration of MachineDCE class.
 *
 * @author Mikael Lepistö 
 */

#include "CompilerWarnings.hh"

IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "llvm/CodeGen/Passes.h"
#include "tce_config.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Pass.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Analysis/CFG.h"

#include "llvm/Support/Compiler.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/ADT/DepthFirstIterator.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/ADT/StringMap.h"

POP_COMPILER_DIAGS

#include <map>

namespace llvm {    
  struct MachineDCE : public MachineFunctionPass {
    static char ID;
    MachineDCE() : MachineFunctionPass(ID) {}
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

    virtual bool doInitialization(Module &M) override;
    virtual bool runOnMachineFunction(MachineFunction &F) override;
    virtual bool doFinalization(Module &M) override;

#if LLVM_OLDER_THAN_4_0
    virtual const char *getPassName() const override {
#else
    virtual StringRef getPassName() const override {
#endif
        return "TCE deadcode elimination of unused emulation functions";
    }

    bool canFindStart(const std::string& user, AvoidRecursionSet& avoid_recursion);
    void addInitializer(const Constant* init, std::string& name);

  public:
    typedef std::set<std::string> UnusedFunctionsList;
    UnusedFunctionsList removeableFunctions;
  };
}
