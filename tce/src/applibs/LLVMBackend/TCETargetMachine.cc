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
#include "llvm/Support/StandardPasses.h"
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

extern "C" void LLVMInitializeTCETargetInfo() { 
    RegisterTarget<Triple::tce> X(TheTCETarget, "tce", "TTA Codesign Environment");
    RegisterTargetMachine<TCETargetMachine> Y(TheTCETarget);
    RegisterAsmInfo<TCEMCAsmInfo> Z(TheTCETarget);
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
        "-a0:0:64"
        "-i1:8:8"
        "-i8:8:32"
        "-i16:16:32"
        "-i32:32:32"
        "-i64:32:64"
        "-f32:32:32"
        "-f64:64:64"),
      FrameInfo(TargetFrameInfo::StackGrowsDown, 4, -4),
#ifndef LLVM_2_7
      tsInfo(*this),
#endif
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

    if (!plugin_->hasSQRTF()) {
	missingOps_.insert(std::make_pair(llvm::ISD::FSQRT, MVT::f32));
	missingOps_.insert(std::make_pair(llvm::ISD::FSQRT, MVT::f64));
    }

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
    PassManagerBase& PM, CodeGenOpt::Level OptLevel) {
    
    // lower floating point stuff.. maybe could use plugin as param instead machine...    
    PM.add(createLowerMissingInstructionsPass(*ttaMach_));

    if (emulationModule_ != NULL) {
        PM.add(createLinkBitcodePass(*emulationModule_));
    }
  
    // if llvm-tce opt level is -O2 or -O3
    if (OptLevel != CodeGenOpt::None) {
        // get some pass lists from llvm/Support/StandardPasses.h from 
        // createStandardLTOPasses function. (do not add memcpyopt or dce!)
        PM.add(createInternalizePass(true));
        
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
 * The returned operations have to be expanded to emulation function calls
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

#include "llvm/Target/TargetMachine.h"
#include "llvm/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/GCStrategy.h"
#include "llvm/CodeGen/MachineFunctionAnalysis.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FormattedStream.h"
using namespace llvm;

namespace llvm {
  bool EnableFastISel;
}

static cl::opt<bool> DisablePostRA("disable-post-ra", cl::Hidden,
    cl::desc("Disable Post Regalloc"));
static cl::opt<bool> DisableBranchFold("disable-branch-fold", cl::Hidden,
    cl::desc("Disable branch folding"));
static cl::opt<bool> DisableCodePlace("disable-code-place", cl::Hidden,
    cl::desc("Disable code placement"));
static cl::opt<bool> DisableSSC("disable-ssc", cl::Hidden,
    cl::desc("Disable Stack Slot Coloring"));
static cl::opt<bool> DisableMachineLICM("disable-machine-licm", cl::Hidden,
    cl::desc("Disable Machine LICM"));
static cl::opt<bool> DisableMachineSink("disable-machine-sink", cl::Hidden,
    cl::desc("Disable Machine Sinking"));
static cl::opt<bool> DisableLSR("disable-lsr", cl::Hidden,
    cl::desc("Disable Loop Strength Reduction Pass"));
static cl::opt<bool> DisableCGP("disable-cgp", cl::Hidden,
    cl::desc("Disable Codegen Prepare"));
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

// Enable or disable FastISel. Both options are needed, because
// FastISel is enabled by default with -fast, and we wish to be
// able to enable or disable fast-isel independently from -O0.
static cl::opt<cl::boolOrDefault>
EnableFastISelOption("fast-isel", cl::Hidden,
  cl::desc("Enable the \"fast\" instruction selector"));

static void printAndVerify(PassManagerBase &PM,
                           const char *Banner,
                           bool allowDoubleDefs = false) {
  if (PrintMachineCode)
    PM.add(createMachineFunctionPrinterPass(errs(), Banner));

  if (VerifyMachineCode)
    PM.add(createMachineVerifierPass(allowDoubleDefs));
}



//#ifdef LLVM_2_7
//FileModel::Model
bool
TCETargetMachine::addPassesToEmitFile(PassManagerBase &PM,
                                      formatted_raw_ostream &Out,
                                      CodeGenFileType FileType,
                                      CodeGenOpt::Level OptLevel) {

#ifdef LLVM_2_7
    OwningPtr<MCContext> Context(new MCContext());
    // Add common CodeGen passes.
    if (addCommonCodeGenPasses(PM, OptLevel))
        return true;
#else
    MCContext* Context = 0;
    if (addCommonCodeGenPasses(
            PM, OptLevel, /*DisableVerify*/false, Context))
        return true;
    assert(Context != 0 && "Failed to get MCContext");
#endif

  switch (FileType) {
  default:
    break;
  case TargetMachine::CGFT_AssemblyFile: {
      const MCAsmInfo &MAI = *getMCAsmInfo();
      OwningPtr<MCStreamer> AsmStreamer;
#ifdef LLVM_2_7
      MCInstPrinter *InstPrinter =
          getTarget().createMCInstPrinter(MAI.getAssemblerDialect(), MAI, Out);
      AsmStreamer.reset(createAsmStreamer(
			    *Context, Out, MAI,
			    getTargetData()->isLittleEndian(),
			    TargetMachine::getAsmVerbosityDefault(),
			    InstPrinter,
			    /*codeemitter*/0));

      FunctionPass *Printer =
          getTarget().createAsmPrinter(
              Out, *this, *Context, *AsmStreamer, &MAI);

      if (Printer == 0) break;
      PM.add(Printer);
#else
    MCInstPrinter *InstPrinter =
      getTarget().createMCInstPrinter(MAI.getAssemblerDialect(), MAI);
    AsmStreamer.reset(createAsmStreamer(*Context, Out,
                                        getTargetData()->isLittleEndian(),
					TargetMachine::getAsmVerbosityDefault(),
					InstPrinter,
                                        /*codeemitter*/0));
      // TODO: make the asm printer work with LLVM 2.8
      break;
#endif   
      return false;
  }
  case TargetMachine::CGFT_ObjectFile:
//    if (getMachOWriterInfo())
//	return false;
      return true;
//    else if (getELFWriterInfo())
//	return false;
      return false;
  }

  return true;
}
//#endif

#ifdef LLVM_2_7
bool TCETargetMachine::addCommonCodeGenPasses(PassManagerBase &PM,
                                               CodeGenOpt::Level OptLevel) {
  // Standard LLVM-Level Passes.

  // Run loop strength reduction before anything else.
  if (OptLevel != CodeGenOpt::None && !DisableLSR) {
    PM.add(createLoopStrengthReducePass(getTargetLowering()));
    if (PrintLSR)
      PM.add(createPrintFunctionPass("\n\n*** Code after LSR ***\n", &errs()));
  }

  // Turn exception handling constructs into something the code generators can
  // handle.
  switch (getMCAsmInfo()->getExceptionHandlingType())
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
  
  if (addPreISel(PM, OptLevel))
      return true;

  if (OptLevel != CodeGenOpt::None && !DisableCGP)
    PM.add(createCodeGenPreparePass(getTargetLowering()));

  PM.add(createStackProtectorPass(getTargetLowering()));

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
  printAndVerify(PM, "After Instruction Selection",
                 /* allowDoubleDefs= */ true);

  if (OptLevel != CodeGenOpt::None) {
    if (!DisableMachineLICM)
      PM.add(createMachineLICMPass());
    if (!DisableMachineSink)
      PM.add(createMachineSinkingPass());
    printAndVerify(PM, "After MachineLICM and MachineSinking",
                   /* allowDoubleDefs= */ true);
  }

  // Run pre-ra passes.
  if (addPreRegAlloc(PM, OptLevel))
    printAndVerify(PM, "After PreRegAlloc passes",
                   /* allowDoubleDefs= */ true);

  // Perform register allocation.
  PM.add(createRegisterAllocator());
  printAndVerify(PM, "After Register Allocation");

  // Perform stack slot coloring.
  if (OptLevel != CodeGenOpt::None && !DisableSSC) {
    // FIXME: Re-enable coloring with register when it's capable of adding
    // kill markers.
    PM.add(createStackSlotColoringPass(false));
    printAndVerify(PM, "After StackSlotColoring");
  }

  // Run post-ra passes.
  if (addPostRegAlloc(PM, OptLevel))
    printAndVerify(PM, "After PostRegAlloc passes");

  PM.add(createLowerSubregsPass());
  printAndVerify(PM, "After LowerSubregs");

  // Insert prolog/epilog code.  Eliminate abstract frame index references...
  PM.add(createPrologEpilogCodeInserter());
  printAndVerify(PM, "After PrologEpilogCodeInserter");

  // Run pre-sched2 passes.
  if (addPreSched2(PM, OptLevel))
    printAndVerify(PM, "After PreSched2 passes");

  // Second pass scheduler.
  if (OptLevel != CodeGenOpt::None && !DisablePostRA) {
    PM.add(createPostRAScheduler(OptLevel));
    printAndVerify(PM, "After PostRAScheduler");
  }

  // Branch folding must be run after regalloc and prolog/epilog insertion.
  if (OptLevel != CodeGenOpt::None && !DisableBranchFold) {
    PM.add(createBranchFoldingPass(getEnableTailMergeDefault()));
    printAndVerify(PM, "After BranchFolding");
  }

  PM.add(createGCMachineCodeAnalysisPass());

  if (PrintGCInfo)
    PM.add(createGCInfoPrinter(errs()));

  // Fold redundant debug labels.

//  PM.add(createDebugLabelFoldingPass());
//  printAndVerify(PM, "After DebugLabelFolding");

  if (OptLevel != CodeGenOpt::None && !DisableCodePlace) {
    PM.add(createCodePlacementOptPass());
    printAndVerify(PM, "After CodePlacementOpt");
  }

  if (addPreEmitPass(PM, OptLevel))
    printAndVerify(PM, "After PreEmit passes");

  return false;
}

#else // LLVM 2.8svn


/// addCommonCodeGenPasses - Add standard LLVM codegen passes used for both
/// emitting to assembly files or machine code output.
///
bool TCETargetMachine::addCommonCodeGenPasses(PassManagerBase &PM,
					      CodeGenOpt::Level OptLevel,
					      bool DisableVerify,
					      MCContext *&OutContext) {
  // Standard LLVM-Level Passes.

  // Before running any passes, run the verifier to determine if the input
  // coming from the front-end and/or optimizer is valid.
  if (!DisableVerify)
    PM.add(createVerifierPass());

  // stupid LLVM creates some pseudo-global static variables into cpp file.
  // and these are not visible here.
  // Optionally, tun split-GEPs and no-load GVN.
  if (false /*LLVMTargetMachine::EnableSplitGEPGVN*/) {
    PM.add(createGEPSplitterPass());
    PM.add(createGVNPass(/*NoLoads=*/true));
  }

  // Run loop strength reduction before anything else.
  if (OptLevel != CodeGenOpt::None && !DisableLSR) {
    PM.add(createLoopStrengthReducePass(getTargetLowering()));
    if (PrintLSR)
      PM.add(createPrintFunctionPass("\n\n*** Code after LSR ***\n", &dbgs()));
  }

  // Turn exception handling constructs into something the code generators can
  // handle.
  switch (getMCAsmInfo()->getExceptionHandlingType()) {
  case ExceptionHandling::SjLj:
    // SjLj piggy-backs on dwarf for this bit. The cleanups done apply to both
    // Dwarf EH prepare needs to be run after SjLj prepare. Otherwise,
    // catch info can get misplaced when a selector ends up more than one block
    // removed from the parent invoke(s). This could happen when a landing
    // pad is shared by multiple invokes and is also a target of a normal
    // edge from elsewhere.
    PM.add(createSjLjEHPass(getTargetLowering()));
    PM.add(createDwarfEHPass(this, OptLevel==CodeGenOpt::None));
    break;
  case ExceptionHandling::Dwarf:
    PM.add(createDwarfEHPass(this, OptLevel==CodeGenOpt::None));
    break;
  case ExceptionHandling::None:
    PM.add(createLowerInvokePass(getTargetLowering()));
    break;
  }

  PM.add(createGCLoweringPass());

  // Make sure that no unreachable blocks are instruction selected.
  PM.add(createUnreachableBlockEliminationPass());

  // TCE 
  if (addPreISel(PM, OptLevel))
      return true;
  /// /TCE

  if (OptLevel != CodeGenOpt::None && !DisableCGP)
    PM.add(createCodeGenPreparePass(getTargetLowering()));

  PM.add(createStackProtectorPass(getTargetLowering()));

  if (PrintISelInput)
    PM.add(createPrintFunctionPass("\n\n"
                                   "*** Final LLVM Code input to ISel ***\n",
                                   &dbgs()));

  // All passes which modify the LLVM IR are now complete; run the verifier
  // to ensure that the IR is valid.
  if (!DisableVerify)
    PM.add(createVerifierPass());

  // Standard Lower-Level Passes.
  
  // Install a MachineModuleInfo class, which is an immutable pass that holds
  // all the per-module stuff we're generating, including MCContext.
  MachineModuleInfo *MMI = new MachineModuleInfo(*getMCAsmInfo());
  PM.add(MMI);
  OutContext = &MMI->getContext(); // Return the MCContext specifically by-ref.
  

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
  printAndVerify(PM, "After Instruction Selection",
                 /* allowDoubleDefs= */ true);

  // Optimize PHIs before DCE: removing dead PHI cycles may make more
  // instructions dead.
  if (OptLevel != CodeGenOpt::None)
    PM.add(createOptimizePHIsPass());

  // Delete dead machine instructions regardless of optimization level.
  PM.add(createDeadMachineInstructionElimPass());
  printAndVerify(PM, "After codegen DCE pass",
                 /* allowDoubleDefs= */ true);

  if (OptLevel != CodeGenOpt::None) {
    PM.add(createOptimizeExtsPass());
    if (!DisableMachineLICM)
      PM.add(createMachineLICMPass());
    PM.add(createMachineCSEPass());
    if (!DisableMachineSink)
      PM.add(createMachineSinkingPass());
    printAndVerify(PM, "After Machine LICM, CSE and Sinking passes",
                   /* allowDoubleDefs= */ true);
  }

  // stupid llvm static cpp variables again.
  // Pre-ra tail duplication.
  if (OptLevel != CodeGenOpt::None && true /*!DisableEarlyTailDup*/) {
    PM.add(createTailDuplicatePass(true));
    printAndVerify(PM, "After Pre-RegAlloc TailDuplicate",
                   /* allowDoubleDefs= */ true);
  }

  // Run pre-ra passes.
  if (addPreRegAlloc(PM, OptLevel))
    printAndVerify(PM, "After PreRegAlloc passes",
                   /* allowDoubleDefs= */ true);

  // Perform register allocation.
  PM.add(createRegisterAllocator(OptLevel));
  printAndVerify(PM, "After Register Allocation");

  // Perform stack slot coloring and post-ra machine LICM.
  if (OptLevel != CodeGenOpt::None) {
    // FIXME: Re-enable coloring with register when it's capable of adding
    // kill markers.
    if (!DisableSSC)
      PM.add(createStackSlotColoringPass(false));

    // stupid llvm static cpp variables.
    // Run post-ra machine LICM to hoist reloads / remats.
    if (true /*!DisablePostRAMachineLICM*/)
      PM.add(createMachineLICMPass(false));

    printAndVerify(PM, "After StackSlotColoring and postra Machine LICM");
  }

  // Run post-ra passes.
  if (addPostRegAlloc(PM, OptLevel))
    printAndVerify(PM, "After PostRegAlloc passes");

  PM.add(createLowerSubregsPass());
  printAndVerify(PM, "After LowerSubregs");

  // Insert prolog/epilog code.  Eliminate abstract frame index references...
  PM.add(createPrologEpilogCodeInserter());
  printAndVerify(PM, "After PrologEpilogCodeInserter");

  // Run pre-sched2 passes.
  if (addPreSched2(PM, OptLevel))
    printAndVerify(PM, "After PreSched2 passes");

  // Second pass scheduler.
  if (OptLevel != CodeGenOpt::None && !DisablePostRA) {
    PM.add(createPostRAScheduler(OptLevel));
    printAndVerify(PM, "After PostRAScheduler");
  }

  // Branch folding must be run after regalloc and prolog/epilog insertion.
  if (OptLevel != CodeGenOpt::None && !DisableBranchFold) {
    PM.add(createBranchFoldingPass(getEnableTailMergeDefault()));
//    printNoVerify(PM, "After BranchFolding");
  }

  // Tail duplication.
  if (OptLevel != CodeGenOpt::None && true /*!DisableTailDuplicate*/) {
    PM.add(createTailDuplicatePass(false));
//    printNoVerify(PM, "After TailDuplicate");
  }

  PM.add(createGCMachineCodeAnalysisPass());

  if (PrintGCInfo)
    PM.add(createGCInfoPrinter(dbgs()));

  if (OptLevel != CodeGenOpt::None && !DisableCodePlace) {
    PM.add(createCodePlacementOptPass());
//    printNoVerify(PM, "After CodePlacementOpt");
  }

//  if (
  addPreEmitPass(PM, OptLevel); //)
//    printNoVerify(PM, "After PreEmit passes");

  return false;
}
#endif
