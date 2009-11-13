/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 */

#include "llvm/PassManager.h"
#include "llvm/Target/TargetRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/Target/TargetRegistry.h"

#include "TCETargetMachine.hh"
#include "TCETargetAsmInfo.hh"
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

extern "C" void LLVMInitializeTCETargetInfo() { 
    RegisterTarget<Triple::tce> X(TheTCETarget, "tce", "TTA Codesign Environment");
    RegisterTargetMachine<TCETargetMachine> Y(TheTCETarget);
    RegisterAsmInfo<TCETargetAsmInfo> Z(TheTCETarget);
}

//
// Data layout:
//--------------
// E-p:32:32:32-i1:8:8-i8:8:8-i32:32:32-i64:32:64-f32:32:32-f64:32:64"
// E = Big endian data
// -p:32:32:32 = Pointer size 32 bits, api & preferred alignment 32 bits.
// -i8:8:8 = 8bit integer api & preferred alignment 8 bits.
// etc.
// 
//  Frame info:
// -------------
// Grows down, alignment 4 bytes.
//
TCETargetMachine::TCETargetMachine(const Target &T, const std::string &TT,
                                   const std::string &FS)
    : LLVMTargetMachine(T,TT),
      Subtarget(TT,FS),
      DataLayout(
        "E-p:32:32:32"
        "-a0:32:32"
        "-i1:8:8"
        "-i8:8:8"
        "-i16:16:16"
        "-i32:32:32"
        "-i64:32:64"
        "-f32:32:32"
        "-f64:32:64"),
      FrameInfo(TargetFrameInfo::StackGrowsDown, 4, -4),
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

/**
 * Branch folding pass does worse than normal, so currently disabled.
 */
bool 
TCETargetMachine::getEnableTailMergeDefault() const { 
    return false; 
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

    if (!plugin_->hasSXHW()) missingOps_.insert(
        std::make_pair(llvm::ISD::SIGN_EXTEND_INREG, MVT::i16));

    if (!plugin_->hasSXQW()) missingOps_.insert(
	std::make_pair(llvm::ISD::SIGN_EXTEND_INREG, MVT::i8));

    // register machine to plugin
    plugin_->registerTargetMachine(*this);
}

/**
 * Creates an instruction selector instance.
 *
 * @param pm Function pass manager to add isel pass.
 * @param fast Not used.
 */                                     
bool
TCETargetMachine::addInstSelector(
    PassManagerBase& pm, CodeGenOpt::Level /* OptLevel */) {
    pm.add(plugin_->createISelPass(this));
    return false;
}

/**
 * Some extra passes needed by TCE
 *
 * @param pm Function pass manager to add isel pass.
 * @param fast Not used.
 */                                     
bool
TCETargetMachine::addPreISel(
    PassManagerBase& PM, CodeGenOpt::Level /* OptLevel */) {
    
    std::cerr << "EXTRA PASSES WERE ADDED\n";

    // lower floating point stuff.. maybe could use plugin as param instead machine...    
    PM.add(createLowerMissingInstructionsPass(*ttaMach_));

    if (emulationModule_ != NULL) {
        PM.add(createLinkBitcodePass(*emulationModule_));
    }
  
    // to allow machine dead basic block elimination for emulation code
    PM.add(createInternalizePass(true));

    // NOTE: This must be added before Machine function analysis pass..
    // needed by POMBuilder to prevent writing debug data to data section
    // might be good to disable when printing out machine function code...
    PM.add(createStripSymbolsPass(/*bool OnlyDebugInfo=*/true));

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
 * The returned operations have to be expanded to emulation fucntion calls
 * or emulation patterns in TCETargetLowering.
 */
const std::set<std::pair<unsigned, llvm::MVT::SimpleValueType> >*
TCETargetMachine::missingOperations() {
    return &missingOps_;
}

/**
 * This part is copied from CodeGen/LLVMTargetMachine.cpp
 *
 * LLVM 2.6 does not have addPreISel hook so we override the whole function
 * for adding stuff before lower level llvm passes. 
 * hopefully llvm-2.7 will have necessary hook.
 */

#include <llvm/CodeGen/Passes.h>
#include <llvm/PassManager.h>
#include <llvm/Pass.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/CodeGen/AsmPrinter.h>
#include <llvm/CodeGen/Passes.h>
#include <llvm/CodeGen/GCStrategy.h>
#include <llvm/CodeGen/MachineFunctionAnalysis.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetAsmInfo.h>
#include <llvm/Target/TargetRegistry.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/FormattedStream.h>

namespace llvm {
  bool EnableFastISel;
}

static cl::opt<bool> PrintLSR("print-lsr-output", cl::Hidden,
    cl::desc("Print LLVM IR produced by the loop-reduce pass"));
static cl::opt<bool> PrintISelInput("print-isel-input", cl::Hidden,
    cl::desc("Print LLVM IR input to isel pass"));
static cl::opt<bool> PrintEmittedAsm("print-emitted-asm", cl::Hidden,
    cl::desc("Dump emitter generated instructions as assembly"));
static cl::opt<bool> PrintGCInfo("print-gc", cl::Hidden,
    cl::desc("Dump garbage collector data"));
static cl::opt<bool> VerifyMachineCode("verify-machineinstrs", cl::Hidden,
    cl::desc("Verify generated machine code"),
    cl::init(getenv("LLVM_VERIFY_MACHINEINSTRS")!=NULL));

// When this works it will be on by default.
static cl::opt<bool>
DisablePostRAScheduler("disable-post-RA-scheduler",
                       cl::desc("Disable scheduling after register allocation"),
                       cl::init(true));

// Enable or disable FastISel. Both options are needed, because
// FastISel is enabled by default with -fast, and we wish to be
// able to enable or disable fast-isel independently from -fast.
static cl::opt<cl::boolOrDefault>
EnableFastISelOption("fast-isel", cl::Hidden,
  cl::desc("Enable the experimental \"fast\" instruction selector"));

static void printAndVerify(PassManagerBase &PM,
                           bool allowDoubleDefs = false) {
  if (PrintMachineCode)
    PM.add(createMachineFunctionPrinterPass(cerr));

  if (VerifyMachineCode)
    PM.add(createMachineVerifierPass(allowDoubleDefs));
}

FileModel::Model
TCETargetMachine::addPassesToEmitFile(PassManagerBase &PM,
                                      formatted_raw_ostream &Out,
                                      CodeGenFileType FileType,
                                      CodeGenOpt::Level OptLevel) {
  // Add common CodeGen passes.
  if (addCommonCodeGenPasses(PM, OptLevel))
    return FileModel::Error;

  // Fold redundant debug labels.
  PM.add(createDebugLabelFoldingPass());

  if (PrintMachineCode)
    PM.add(createMachineFunctionPrinterPass(cerr));

  if (addPreEmitPass(PM, OptLevel) && PrintMachineCode)
    PM.add(createMachineFunctionPrinterPass(cerr));

  if (OptLevel != CodeGenOpt::None)
    PM.add(createCodePlacementOptPass());

  switch (FileType) {
  default:
    break;
  case TargetMachine::AssemblyFile:
    if (addAssemblyEmitter(PM, OptLevel, getAsmVerbosityDefault(), Out))
      return FileModel::Error;
    return FileModel::AsmFile;
  case TargetMachine::ObjectFile:
    if (getMachOWriterInfo())
      return FileModel::MachOFile;
    else if (getELFWriterInfo())
      return FileModel::ElfFile;
  }

  return FileModel::Error;
}

bool TCETargetMachine::addCommonCodeGenPasses(PassManagerBase &PM,
                                               CodeGenOpt::Level OptLevel) {
  // Standard LLVM-Level Passes.

  // Run loop strength reduction before anything else.
  if (OptLevel != CodeGenOpt::None) {
    PM.add(createLoopStrengthReducePass(getTargetLowering()));
    if (PrintLSR)
      PM.add(createPrintFunctionPass("\n\n*** Code after LSR ***\n", &errs()));
  }

  // Turn exception handling constructs into something the code generators can
  // handle.
  switch (getTargetAsmInfo()->getExceptionHandlingType())
  {
  case ExceptionHandling::SjLj:
    // SjLj piggy-backs on dwarf for this bit. The cleanups done apply to both
    PM.add(createDwarfEHPass(getTargetLowering(), OptLevel==CodeGenOpt::None));
    PM.add(createSjLjEHPass(getTargetLowering()));
    break;
  case ExceptionHandling::Dwarf:
    PM.add(createDwarfEHPass(getTargetLowering(), OptLevel==CodeGenOpt::None));
    break;
  case ExceptionHandling::None:
    PM.add(createLowerInvokePass(getTargetLowering()));
    break;
  }

  PM.add(createGCLoweringPass());

  // Make sure that no unreachable blocks are instruction selected.
  PM.add(createUnreachableBlockEliminationPass());

  if (OptLevel != CodeGenOpt::None)
    PM.add(createCodeGenPreparePass(getTargetLowering()));

  PM.add(createStackProtectorPass(getTargetLowering()));

  if (addPreISel(PM, OptLevel))
      return true;

  if (PrintISelInput)
    PM.add(createPrintFunctionPass("\n\n"
                                   "*** Final LLVM Code input to ISel ***\n",
                                   &errs()));

  // Standard Lower-Level Passes.

  // Set up a MachineFunction for the rest of CodeGen to work on.
  PM.add(new MachineFunctionAnalysis(*this, OptLevel));

  // Enable FastISel with -fast, but allow that to be overridden.
  if (EnableFastISelOption == cl::BOU_TRUE ||
      (OptLevel == CodeGenOpt::None && EnableFastISelOption != cl::BOU_FALSE))
    EnableFastISel = true;

  // Ask the target for an isel.
  if (addInstSelector(PM, OptLevel))
    return true;

  // Print the instruction selected machine code...
  printAndVerify(PM, /* allowDoubleDefs= */ true);

  if (OptLevel != CodeGenOpt::None) {
    PM.add(createMachineLICMPass());
    PM.add(createMachineSinkingPass());
    printAndVerify(PM, /* allowDoubleDefs= */ true);
  }

  // Run pre-ra passes.
  if (addPreRegAlloc(PM, OptLevel))
    printAndVerify(PM, /* allowDoubleDefs= */ true);

  // Perform register allocation.
  PM.add(createRegisterAllocator());

  // Perform stack slot coloring.
  if (OptLevel != CodeGenOpt::None)
    // FIXME: Re-enable coloring with register when it's capable of adding
    // kill markers.
    PM.add(createStackSlotColoringPass(false));

  printAndVerify(PM);           // Print the register-allocated code

  // Run post-ra passes.
  if (addPostRegAlloc(PM, OptLevel))
    printAndVerify(PM);

  PM.add(createLowerSubregsPass());
  printAndVerify(PM);

  // Insert prolog/epilog code.  Eliminate abstract frame index references...
  PM.add(createPrologEpilogCodeInserter());
  printAndVerify(PM);

  // Second pass scheduler.
  if (OptLevel != CodeGenOpt::None && !DisablePostRAScheduler) {
    PM.add(createPostRAScheduler());
    printAndVerify(PM);
  }

  // Branch folding must be run after regalloc and prolog/epilog insertion.
  if (OptLevel != CodeGenOpt::None) {
    PM.add(createBranchFoldingPass(getEnableTailMergeDefault()));
    printAndVerify(PM);
  }

  PM.add(createGCMachineCodeAnalysisPass());
  printAndVerify(PM);

  if (PrintGCInfo)
    PM.add(createGCInfoPrinter(*cerr));

  return false;
}
