/*
    Copyright (c) 2002-2012 Tampere University of Technology.

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
 * @file TCETargeMachine.cpp
 *
 * Declaration of TCETargetMachine class.
 *
 * @author Veli-Pekka Jääskeläinen 2007 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam—tut.fi)
 * @author Pekka Jääskeläinen 2007-2012
 */

#include "llvm/PassManager.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/CodeGen/MachineModuleInfo.h"

#include "TCETargetMachine.hh"
#include "TCEMCAsmInfo.hh"
#include "LLVMPOMBuilder.hh"
#include "PluginTools.hh"
#include "FileSystem.hh"
#include "ADFSerializer.hh"

#include <iostream>

using namespace llvm;


namespace llvm {
    Target TheTCETarget;
}

Pass* createLowerMissingInstructionsPass(const TTAMachine::Machine& mach);
Pass* createLinkBitcodePass(Module& inputCode);
Pass* createProgramPartitionerPass();

extern "C" void LLVMInitializeTCETargetInfo() { 
    RegisterTarget<Triple::tce> X(TheTCETarget, "tce", "TTA Codesign Environment");
    RegisterTargetMachine<TCETargetMachine> Y(TheTCETarget);

// TODO:
// this to MVTargetDesc.cpp - where is it?

    RegisterMCAsmInfo<TCEMCAsmInfo> Z(TheTCETarget);


/*
extern "C" void LLVMInitializeSparcMCAsmInfo() {
    RegisterMCAsmInfo<TCEMCAsmInfo> Z(TheTCETarget);
}
*/

}

//
// Data layout:
//--------------
// E-p:32:32:32-a0:0:64-i1:8:8-i8:8:8-i32:32:32-i64:32:64-f32:32:32-f64:32:64" (old)
// E-p:32:32:32-a0:0:32-i1:8:8-i8:8:8-i32:32:32-i64:32:32-f32:32:32-f64:32:32" (new)
// E = Big endian data
// -p:32:32:32 = Pointer size 32 bits, api & preferred alignment 32 bits.
// -i8:8:8 = 8bit integer api & preferred alignment 8 bits.
// a0:0:32 = struct alignment (abi packed?), preferred 32 bits.
// etc.
// 

#ifdef LLVM_3_0
TCETargetMachine::TCETargetMachine(
    const Target &T, const std::string &TT, const std::string& CPU,
    const std::string &FS, Reloc::Model RM, CodeModel::Model CM)
    : LLVMTargetMachine(T,TT, CPU, FS, RM, CM),

#else
TCETargetMachine::TCETargetMachine(
    const Target &T, const std::string &TT, const std::string& CPU,
    const std::string &FS, const TargetOptions &Options,
    Reloc::Model RM, CodeModel::Model CM, CodeGenOpt::Level OL)
    : LLVMTargetMachine(T,TT, CPU, FS, Options, RM, CM, OL),
#endif

      Subtarget(TT,FS),
      DataLayout(
        "E-p:32:32:32"
        "-a0:0:32"
        "-i1:8:8"
        "-i8:8:32"
        "-i16:16:32"
        "-i32:32:32"
        "-i64:32:32"
        "-f16:32:32"
        "-f32:32:32"
        "-f64:32:32"
        "-v64:32:32"
        "-v128:32:32"
        "-v256:32:32"),
      tsInfo(*this),
      plugin_(NULL), pluginTool_(NULL) {
}

/**
 * The Destructor.
 */
TCETargetMachine::~TCETargetMachine() {
    if (pluginTool_ != NULL) {
        delete pluginTool_;
        pluginTool_ = NULL;
    }
}

#ifdef LLVM_3_0
/**
 * Branch folding pass does worse than normal, so currently disabled.
 */
bool 
TCETargetMachine::getEnableTailMergeDefault() const { 
    return false; 
}
#endif

void 
TCETargetMachine::setTargetMachinePlugin(TCETargetMachinePlugin& plugin) {

    plugin_ = &plugin;
    missingOps_.clear();
    if (!plugin_->hasSDIV()) missingOps_.insert(std::make_pair(llvm::ISD::SDIV, MVT::i32));
    if (!plugin_->hasUDIV()) missingOps_.insert(std::make_pair(llvm::ISD::UDIV, MVT::i32));
    if (!plugin_->hasSREM()) missingOps_.insert(std::make_pair(llvm::ISD::SREM, MVT::i32));
    if (!plugin_->hasUREM()) missingOps_.insert(std::make_pair(llvm::ISD::UREM, MVT::i32));
    if (!plugin_->hasMUL()) missingOps_.insert(std::make_pair(llvm::ISD::MUL, MVT::i32));
    if (!plugin_->hasROTL()) missingOps_.insert(std::make_pair(llvm::ISD::ROTL, MVT::i32));
    if (!plugin_->hasROTR()) missingOps_.insert(std::make_pair(llvm::ISD::ROTR, MVT::i32));

    if (!plugin_->hasSXHW()) missingOps_.insert(
        std::make_pair(llvm::ISD::SIGN_EXTEND_INREG, MVT::i16));

    if (!plugin_->hasSXQW()) missingOps_.insert(
        std::make_pair(llvm::ISD::SIGN_EXTEND_INREG, MVT::i8));

    if (!plugin_->hasSQRTF()) {
        missingOps_.insert(std::make_pair(llvm::ISD::FSQRT, MVT::f32));
        missingOps_.insert(std::make_pair(llvm::ISD::FSQRT, MVT::f64));
    }

    // register machine to plugin
    plugin_->registerTargetMachine(*this);
}

#ifdef LLVM_3_0
/**
 * Creates an instruction selector instance.
 *
 * @param pm Function pass manager to add isel pass.
 * @param fast Not used.
 */                                     
bool
TCETargetMachine::addInstSelector(
    PassManagerBase& pm, CodeGenOpt::Level /* OptLevel */) 
{
    pm.add(plugin_->createISelPass(this));
    return false;
}

#else
/**
 * Creates an instruction selector instance.
 *
 */
bool
TCEPassConfig::addInstSelector() 
{
#ifdef LLVM_3_1
    PM->add(plugin_->createISelPass(static_cast<TCETargetMachine*>(TM)));
#else
    PM.add(plugin_->createISelPass(static_cast<TCETargetMachine*>(TM)));
#endif
    return false;
}

#endif //LLVM_3_0


/**
 * Some extra passes needed by TCE
 *
 * @param pm Function pass manager to add isel pass.
 * @param fast Not used.
 */                                     
#ifdef LLVM_3_0 
bool
TCETargetMachine::addPreISel(
    PassManagerBase& PM, CodeGenOpt::Level OptLevel) {
    // lower floating point stuff.. maybe could use plugin as param instead machine...    
    PM.add(createLowerMissingInstructionsPass(*ttaMach_));
    
    if (emulationModule_ != NULL) {
        PM.add(createLinkBitcodePass(*emulationModule_));
    }

#else

bool
TCEPassConfig::addPreRegAlloc() {
#ifdef LLVM_3_1
    PM->add(createProgramPartitionerPass());
#else
    PM.add(createProgramPartitionerPass());
#endif
    return false;
}

bool
TCEPassConfig::addPreISel() {
    // lower floating point stuff.. maybe could use plugin as param instead machine...    
#ifdef LLVM_3_1
    PM->add(createLowerMissingInstructionsPass(
               *((static_cast<TCETargetMachine*>(TM))->ttaMach_)));
    
    if ((static_cast<TCETargetMachine*>(TM))->emulationModule_ != NULL) {
        PM->add(createLinkBitcodePass(
                   *((static_cast<TCETargetMachine*>(TM))->emulationModule_)));
    }
#else
    PM.add(createLowerMissingInstructionsPass(
               *((static_cast<TCETargetMachine*>(TM))->ttaMach_)));
    
    if ((static_cast<TCETargetMachine*>(TM))->emulationModule_ != NULL) {
        PM.add(createLinkBitcodePass(
                   *((static_cast<TCETargetMachine*>(TM))->emulationModule_)));
    }
#endif

    CodeGenOpt::Level OptLevel = getOptLevel();

#endif
    // if llvm-tce opt level is -O2 or -O3
    if (OptLevel != CodeGenOpt::None) {
        // get some pass lists from llvm/Support/StandardPasses.h from 
        // createStandardLTOPasses function. (do not add memcpyopt or dce!)
#ifndef LLVM_3_1
        PM.add(createInternalizePass(true));
#else
        PM->add(createInternalizePass(true));
#endif
        // TODO: find out which optimizations are beneficial here..
        //PM.add(createIPSCCPPass());
        //PM.add(createGlobalOptimizerPass());
        //PM.add(createConstantMergePass());
        //PM.add(createDeadArgEliminationPass());
        //PM.add(createInstructionCombiningPass());
        //PM.add(createFunctionInliningPass());
        //PM.add(createPruneEHPass());   // Remove dead EH info.
        //PM.add(createGlobalOptimizerPass());
        //PM.add(createArgumentPromotionPass());
        //PM.add(createInstructionCombiningPass());
        //PM.add(createJumpThreadingPass());
        //PM.add(createScalarReplAggregatesPass());
        //PM.add(createFunctionAttrsPass()); // Add nocapture.
        //PM.add(createGlobalsModRefPass()); // IP alias analysis.
        //PM.add(createLICMPass());      // Hoist loop invariants.
        //PM.add(createGVNPass());       // Remove redundancies.
        //PM.add(createDeadStoreEliminationPass());
        //PM.add(createInstructionCombiningPass());
        //PM.add(createJumpThreadingPass());
        // PM.add(createCFGSimplificationPass()); 
    }
    
    // NOTE: This must be added before Machine function analysis pass..
    // needed by POMBuilder to prevent writing debug data to data section
    // might be good to disable when printing out machine function code...
    // However, it need to comment out to support debug info
//    PM.add(createStripSymbolsPass(/*bool OnlyDebugInfo=*/true));

    return false;
}

/**
 * Creates a TTAMachine::Machine object of the target architecture.
 */
TTAMachine::Machine*
TCETargetMachine::createMachine() {
    ADFSerializer serializer;
    serializer.setSourceString(*plugin_->adfXML());
    return serializer.readMachine();
}

/**
 * Returns list of llvm::ISD SelectionDAG opcodes for operations that are not
 * supported in the target architecture.
 *
 * The returned operations have to be expanded to emulation function calls
 * or emulation patterns in TCETargetLowering.
 */
const std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> >*
TCETargetMachine::missingOperations() {
    return &missingOps_;
}

#ifndef LLVM_3_0

TargetPassConfig* 
TCETargetMachine::createPassConfig(
    PassManagerBase &PM) {
    TCEPassConfig *tpc;
    tpc =  new TCEPassConfig(this, PM, plugin_);
    // TODO: is this false by default if not set?
    // false seems to give slightly better performance than true.
    tpc->setEnableTailMerge(false);
    return tpc;
}
#endif
