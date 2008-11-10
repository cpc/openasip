/**
 * @file LinkBitcode.cc
 *
 * Links other bitcode module to module.
 *
 * @author Mikael Lepistö 2008 (mikael.lepisto@tut.fi)
 */

#define DEBUG_TYPE "linkbitcode"

#include "llvm/Support/Compiler.h"
#include "llvm/Support/Debug.h"
#include "llvm/Module.h"
#include "llvm/Pass.h"
#include "llvm/Linker.h"

using namespace llvm;

#include <iostream>
#include <map>

namespace {
    class VISIBILITY_HIDDEN LinkBitcode : public BasicBlockPass {
    public:
        static char ID; // Pass ID, replacement for typeid       
        LinkBitcode(Module& input);
        virtual ~LinkBitcode();        
        bool doInitialization(Module &M);
        bool doFinalization (Module &M);
        bool runOnBasicBlock(BasicBlock &BB);

    private:
        Module& inputModule_;
    };

    char LinkBitcode::ID = 0;
// cannot be used as plugin pass right now..
//    RegisterPass<LinkBitcode>
//    X("linkbitcode", "Links in other bitcode module to currently handled module.");    
}


/**
 * Constructor
 */
LinkBitcode::LinkBitcode(Module& input) :
    BasicBlockPass((intptr_t)&ID), inputModule_(input) {
}

/**
 * Destructor
 */
LinkBitcode::~LinkBitcode() {}


// Publically exposed interface to pass.
// const PassInfo* LinkBitcodeID = X.getPassInfo();

Pass* createLinkBitcodePass(Module& input) {
    return new LinkBitcode(input);
}

bool
LinkBitcode::doFinalization(Module& M) {
    std::string errors;
    if (Linker::LinkModules(&M, &inputModule_, &errors)) {
        DOUT << "Error during linking in LinkBitcodePass: " << errors << std::endl;
    }
    return true;
}

bool
LinkBitcode::doInitialization(Module& /*M*/) {
    return true;
}

bool
LinkBitcode::runOnBasicBlock(BasicBlock& /*BB*/) {
    return true;
}
