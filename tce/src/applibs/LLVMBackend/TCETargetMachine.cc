/*
    Copyright (c) 2002-2015 Tampere University.

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
 * @author Pekka Jääskeläinen 2007-2015
 */

#include "CompilerWarnings.hh"

IGNORE_COMPILER_WARNING("-Wunused-parameter")

#include "tce_config.h"

#include "llvm/IR/PassManager.h"
#ifdef LLVM_OLDER_THAN_6_0
#include "llvm/Target/TargetRegisterInfo.h"
#else
#include "llvm/CodeGen/TargetRegisterInfo.h"
#endif
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCInstPrinter.h"
#include "llvm/CodeGen/MachineModuleInfo.h"

#include "TCEStubTargetMachine.hh"
#include "TCETargetMachine.hh"
#include "TCEMCAsmInfo.hh"
#include "LLVMPOMBuilder.hh"
#include "PluginTools.hh"
#include "FileSystem.hh"
#include "ADFSerializer.hh"
#include "Conversion.hh"

#include <iostream>

#include "LLVMTCECmdLineOptions.hh"

POP_COMPILER_DIAGS

using namespace llvm;


Pass* createLowerMissingInstructionsPass(const TTAMachine::Machine& mach);
Pass* createLinkBitcodePass(Module& inputCode);
Pass* createProgramPartitionerPass();
Pass* createInstructionPatternAnalyzer();

class DummyInstPrinter : public MCInstPrinter {
public:
    DummyInstPrinter(
        const llvm::MCAsmInfo& mai, const llvm::MCInstrInfo& mii, 
        const llvm::MCRegisterInfo& mri) : llvm::MCInstPrinter(mai, mii, mri) {}
#ifdef LLVM_OLDER_THAN_10
    void printInst(
        const MCInst*, raw_ostream&, StringRef,
        const MCSubtargetInfo&) override {}    
#else
    void printInst(
        const MCInst*, uint64_t, StringRef,
        const MCSubtargetInfo&, raw_ostream&) override {}
#endif
};

// In TCE target we don't print the MCInsts from LLVM, but
// just want to run the codegen passes so we can get the
// MachineInstrs in to our own finalization phases. LLVM 3.7
// started to require MCInstPrinter to be created, a dummy
// must be created therefore.
MCInstPrinter *dummyInstrPrinterCtor(
    const Triple &,
    unsigned,
    const MCAsmInfo &MAI,
    const MCInstrInfo &MII,
    const MCRegisterInfo &MRI) {
    return new DummyInstPrinter(MAI, MII, MRI);
}

extern "C" void LLVMInitializeTCETarget() { 
    RegisterTargetMachine<TCETargetMachine> Y(TheTCETarget);
    RegisterTargetMachine<TCETargetMachine> X(TheTCELETarget);

    RegisterMCAsmInfo<TCEMCAsmInfo> Z(TheTCETarget);
    RegisterMCAsmInfo<TCEMCAsmInfo> V(TheTCELETarget);
    TargetRegistry::RegisterMCInstPrinter(TheTCETarget, dummyInstrPrinterCtor);
    TargetRegistry::RegisterMCInstPrinter(
        TheTCELETarget, dummyInstrPrinterCtor);
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

#ifdef LLVM_OLDER_THAN_3_9
TCETargetMachine::TCETargetMachine(
    const Target &T, const Triple& TTriple,
    const std::string& CPU, const std::string &FS, 
    const TargetOptions &Options,
    Reloc::Model RM, CodeModel::Model CM, CodeGenOpt::Level OL) : 
    TCEBaseTargetMachine(T, TTriple, CPU, FS, Options, RM, CM, OL), 
    plugin_(NULL), pluginTool_(NULL) {
}
#elif LLVM_OLDER_THAN_6_0
TCETargetMachine::TCETargetMachine(
    const Target &T, const Triple& TTriple,
    const std::string& CPU, const std::string &FS,
    const TargetOptions &Options,
    Optional<Reloc::Model> RM, CodeModel::Model CM, CodeGenOpt::Level OL) :
    TCEBaseTargetMachine(T, TTriple, CPU, FS, Options,
                         RM?*RM:Reloc::Model::Static, CM, OL),
    // Note: Reloc::Model does not have "Default" named member. "Static" is ok?
    plugin_(NULL), pluginTool_(NULL) {
}
#else
TCETargetMachine::TCETargetMachine(
    const Target &T, const Triple& TTriple,
    const std::string& CPU, const std::string &FS,
    const TargetOptions &Options,
    Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM, CodeGenOpt::Level OL, bool) :
    TCEBaseTargetMachine(T, TTriple, CPU, FS, Options,
                         RM?*RM:Reloc::Model::Static, CM?*CM:CodeModel::Small, OL),
    // Note: Reloc::Model does not have "Default" named member. "Static" is ok?
    // Note: CodeModel does not have "Default" named member. "Small" is ok?
    plugin_(NULL), pluginTool_(NULL) {
}
#endif

/**
 * The Destructor.
 */
TCETargetMachine::~TCETargetMachine() {
    if (pluginTool_ != NULL) {
        delete pluginTool_;
        pluginTool_ = NULL;
    }
}

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

    if (!plugin_->hasSHL()) customLegalizedOps_.insert(std::make_pair(llvm::ISD::SHL, MVT::i32));
    if (!plugin_->hasSHR()) customLegalizedOps_.insert(std::make_pair(llvm::ISD::SRA, MVT::i32));
    if (!plugin_->hasSHRU()) customLegalizedOps_.insert(std::make_pair(llvm::ISD::SRL, MVT::i32));

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

    initAsmInfo();

    // Set data layout with correct stack alignment.
    unsigned alignBits = getMaxMemoryAlignment() * 8;
    TCEString dataLayoutStr("");
    if (plugin_->isLittleEndian()) {
        dataLayoutStr += "e-p:32:32:32";
    } else {
        dataLayoutStr += "E-p:32:32:32";
    }
    dataLayoutStr += "-a0:0:" + Conversion::toString(alignBits);
    dataLayoutStr += "-i1:8:8";
    dataLayoutStr += "-i8:8:32";
    dataLayoutStr += "-i16:16:32";
    dataLayoutStr += "-i32:32:32";
    dataLayoutStr += "-i64:32:32";
    dataLayoutStr += "-f16:16:16";
    dataLayoutStr += "-f32:32:32";
    dataLayoutStr += "-f64:32:64";
    dataLayoutStr += "-v64:64:64";
    dataLayoutStr += "-v128:128:128";
    dataLayoutStr += "-v256:256:256";
    dataLayoutStr += "-v512:512:512";
    dataLayoutStr += "-v1024:1024:1024";
#if LLVM_HAS_CUSTOM_VECTOR_EXTENSION == 2
    dataLayoutStr += "-v2048:2048:2048";
    dataLayoutStr += "-v4096:4096:4096";
#endif

    DataLayout* dl = plugin_->getDataLayout();
    dl->reset(dataLayoutStr.c_str());
}

/**
 * Creates an instruction selector instance.
 *
 */
bool
TCEPassConfig::addInstSelector() 
{
    addPass(plugin_->createISelPass(static_cast<TCETargetMachine*>(TM)));
    return false;
}

/**
 * Some extra passes needed by TCE
 *
 * @param pm Function pass manager to add isel pass.
 * @param fast Not used.
 */
void
TCEPassConfig::addPreRegAlloc() {
    LLVMTCECmdLineOptions *options =
        dynamic_cast<LLVMTCECmdLineOptions*>(Application::cmdLineOptions());
    addPass(createProgramPartitionerPass());
    if (options != NULL && options->analyzeInstructionPatterns())
        addPass(createInstructionPatternAnalyzer());
}


bool
TCEPassConfig::addPreISel() {
    // lower floating point stuff.. maybe could use plugin as param instead machine...    
    addPass(createLowerMissingInstructionsPass(
                *((static_cast<TCETargetMachine*>(TM))->ttaMach_)));
    
    if ((static_cast<TCETargetMachine*>(TM))->emulationModule_ != NULL) {
        addPass(createLinkBitcodePass(
                  *((static_cast<TCETargetMachine*>(TM))->emulationModule_)));
    }

    CodeGenOpt::Level OptLevel = getOptLevel();

    // if llvm-tce opt level is -O2 or -O3
    if (OptLevel != CodeGenOpt::None) {
        // get some pass lists from llvm/Support/StandardPasses.h from 
        // createStandardLTOPasses function. (do not add memcpyopt or dce!)
        addPass(createInternalizePass());
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

/**
 * Returns list of llvm::ISD SelectionDAG opcodes for operations that are not
 * supported in the target architecture but will be custom-selected.
 */
const std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> >*
TCETargetMachine::customLegalizedOperations() {
    return &customLegalizedOps_;
}

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

void
TCEPassConfig::addPreSched2() {
    addPass(&IfConverterID);
}
