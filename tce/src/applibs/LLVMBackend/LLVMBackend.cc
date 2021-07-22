/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file LLVMBackend.cc
 *
 * TCE runtime retargeting compiler backend.
 *
 * @author Veli-Pekka Jääskeläinen 2008 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2009-2021
 * @note rating: red
 */

#ifdef NDEBUG
#undef NDEBUG
#endif
#include "CompilerWarnings.hh"
IGNORE_COMPILER_WARNING("-Wunused-parameter")
IGNORE_COMPILER_WARNING("-Wcomment")

#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>

#include "tce_config.h"
#include <llvm/IR/Dominators.h>

#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Pass.h>
//#include <llvm/ModuleProvider.h>

#include <llvm/CodeGen/AsmPrinter.h>
#include <llvm/CodeGen/Passes.h>
#include <llvm/CodeGen/GCStrategy.h>

#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>

#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>

#include <llvm/Support/CommandLine.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Debug.h>
#include <llvm/CodeGen/RegAllocRegistry.h>
#include "Application.hh"
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

#include <llvm/Bitcode/BitcodeReader.h>

#include <llvm/IR/Verifier.h>

#include <llvm-c/Core.h> // LLVMGetGlobalContext()

// tce_config.h defines these. this undef to avoid warning.
// TODO: how to do this in tce_config.h???
#ifdef LLVM_LIBDIR
#undef LLVM_LIBDIR
#endif
#include "tce_config.h" // to get llvm version

#include <llvm/Support/TargetRegistry.h>
#include "llvm/Support/FileSystem.h"

#include <llvm/InitializePasses.h>

// cheat llvm's multi-include-protection
#define CONFIG_H

#include <cstdlib> // system()
#include <fstream>

#include "LLVMBackend.hh"
#include "LLVMTCECmdLineOptions.hh"
#include "TDGen.hh"

#include "passes/InnerLoopFinder.hh"
#include "Environment.hh"
#include "Conversion.hh"
#include "FileSystem.hh"
#include "TCETargetMachine.hh"
#include "TCEStubTargetMachine.hh"
#include "TCETargetMachinePlugin.hh"
#include "LLVMPOMBuilder.hh"
#include "Program.hh"
#include "ADFSerializer.hh"
#include "MachineValidator.hh"
#include "MachineValidatorResults.hh"
#include "Instruction.hh"
#include "ProgramAnnotation.hh"
#include "TCEString.hh"
#include "InterPassData.hh"
#include "InterPassDatum.hh"
#include "LLVMTCEIRBuilder.hh"
#include "Machine.hh"
#include "MachineInfo.hh"
#include "ConstantTransformer.hh"
//#define DEBUG_TDGEN

#define DS TCEString(FileSystem::DIRECTORY_SEPARATOR)

using namespace llvm;

#include <llvm/IR/IRPrintingPasses.h>

#include "llvm/IR/DataLayout.h"
typedef llvm::DataLayout TargetData;

POP_COMPILER_DIAGS

const std::string LLVMBackend::TBLGEN_INCLUDES = "";
const std::string LLVMBackend::PLUGIN_PREFIX = "tcecc-";
const std::string LLVMBackend::PLUGIN_SUFFIX = ".so";
const TCEString LLVMBackend::CXX0X_FLAG = "-std=c++0x";
const TCEString LLVMBackend::CXX11_FLAG = "-std=c++11";
const TCEString LLVMBackend::CXX14_FLAG = "-std=c++14";

Pass* createWorkItemAliasAnalysisPass();

/**
 * Returns minimum opset that is required by llvm.
 *
 * @return Minimumn opset that is required for llvm.
 */
OperationDAGSelector::OperationSet 
LLVMBackend::llvmRequiredOpset(bool includeFloatOps, bool littleEndian, bool bits64) {
    OperationDAGSelector::OperationSet requiredOps;

    if (littleEndian) {
        requiredOps.insert("LD32");
        requiredOps.insert("LD16");
        requiredOps.insert("LDU16");
        requiredOps.insert("LD8");
        requiredOps.insert("LDU8");
        requiredOps.insert("ST32");
        requiredOps.insert("ST16");
        requiredOps.insert("ST8");
    } else {
        requiredOps.insert("LDW");
        requiredOps.insert("LDH");
        requiredOps.insert("LDHU");
        requiredOps.insert("LDQ");
        requiredOps.insert("LDQU");
        requiredOps.insert("STW");
        requiredOps.insert("STH");
        requiredOps.insert("STQ");
    }
   
    // -- Floating point operations --
    if (includeFloatOps) {
        requiredOps.insert("ADDF");
        requiredOps.insert("SUBF");
        requiredOps.insert("MULF");
        requiredOps.insert("DIVF");
        requiredOps.insert("NEGF");
        requiredOps.insert("SQRTF");

        requiredOps.insert("CFI");
        requiredOps.insert("CFIU");
        requiredOps.insert("CIF");
        requiredOps.insert("CIFU");

        // Ordered FP comparison operations
        requiredOps.insert("EQF");
        requiredOps.insert("NEF");
        requiredOps.insert("LTF");
        requiredOps.insert("LEF");
        requiredOps.insert("GTF");
        requiredOps.insert("GEF");

        // Unordered FP comparison operations
        requiredOps.insert("EQUF");
        requiredOps.insert("NEUF");
        requiredOps.insert("LTUF");
        requiredOps.insert("LEUF");
        requiredOps.insert("GTUF");
        requiredOps.insert("GEUF");

        // Ordered/unordered operations
        requiredOps.insert("ORDF");
        requiredOps.insert("UORDF");

        if (bits64) {
            requiredOps.insert("ADDD");
            requiredOps.insert("SUBD");
            requiredOps.insert("MULD");
            requiredOps.insert("DIVD");
            requiredOps.insert("NEGD");
            requiredOps.insert("SQRTD");

            requiredOps.insert("CDL");
            requiredOps.insert("CDLU");
            requiredOps.insert("CLD");
            requiredOps.insert("CLDU");

            requiredOps.insert("CFD");
            requiredOps.insert("CDF");

            // Ordered FP comparison operations
            requiredOps.insert("EQD");
            requiredOps.insert("NED");
            requiredOps.insert("LTD");
            requiredOps.insert("LED");
            requiredOps.insert("GTD");
            requiredOps.insert("GED");

            // Unordered FP comparison operations
            requiredOps.insert("EQUD");
            requiredOps.insert("NEUD");
            requiredOps.insert("LTUD");
            requiredOps.insert("LEUD");
            requiredOps.insert("GTUD");
            requiredOps.insert("GEUD");

            // Ordered/unordered operations
            requiredOps.insert("ORDD");
            requiredOps.insert("UORDD");
        }
    }

    if (bits64) {
        requiredOps.insert("LDU32"); // TODO not really needed?
        requiredOps.insert("LD64");
        requiredOps.insert("ST64");

        requiredOps.insert("ADD64");
        requiredOps.insert("SUB64");
        requiredOps.insert("MUL64");
        requiredOps.insert("DIV64");
        requiredOps.insert("DIVU64");
        requiredOps.insert("DIV64");
        requiredOps.insert("MOD64");
        requiredOps.insert("MODU64");

        requiredOps.insert("SXH64");
        requiredOps.insert("SXQ64");

        requiredOps.insert("AND64");
        requiredOps.insert("XOR64");
        requiredOps.insert("IOR64");

        requiredOps.insert("SHL64");
        requiredOps.insert("SHR64");
        requiredOps.insert("SHRU64");

        requiredOps.insert("EQ64");
        requiredOps.insert("NE64");
        requiredOps.insert("LT64");
        requiredOps.insert("LTU64");
        requiredOps.insert("LE64");
        requiredOps.insert("LEU64");
        requiredOps.insert("GT64");
        requiredOps.insert("GTU64");
        requiredOps.insert("GE64");
        requiredOps.insert("GEU64");

        return requiredOps;
    } else {
        requiredOps.insert("ADD");
        requiredOps.insert("SUB");
        requiredOps.insert("MUL");
        requiredOps.insert("DIV");
        requiredOps.insert("DIVU");
        requiredOps.insert("DIV");
        requiredOps.insert("MOD");
        requiredOps.insert("MODU");

        requiredOps.insert("SXHW");
        requiredOps.insert("SXQW");

        requiredOps.insert("AND");
        requiredOps.insert("XOR");
        requiredOps.insert("IOR");

        requiredOps.insert("SHL");
        requiredOps.insert("SHR");
        requiredOps.insert("SHRU");

        requiredOps.insert("EQ");
        requiredOps.insert("NE");
        requiredOps.insert("LT");
        requiredOps.insert("LTU");
        requiredOps.insert("LE");
        requiredOps.insert("LEU");
        requiredOps.insert("GT");
        requiredOps.insert("GTU");
        requiredOps.insert("GE");
        requiredOps.insert("GEU");
    }
    return requiredOps;
}
/**
 * Constructor.
 *
 * @param useInstalledVersion Should be true in case we are running an
 * installed TCE (not from the source/build tree).
 * @param tempDir An existing directory where to store temporary files.
 *                The directory should be removed by the caller after use.
 */
LLVMBackend::LLVMBackend(bool useInstalledVersion, TCEString tempDir) : 
    useInstalledVersion_(useInstalledVersion), tempDir_(tempDir) {

    cachePath_ = Environment::llvmtceCachePath();

    options_ =
        dynamic_cast<LLVMTCECmdLineOptions*>(Application::cmdLineOptions());

    if (options_ != NULL)
        cachePath_ = options_->backendCacheDir();

    PassRegistry &Registry = *llvm::PassRegistry::getPassRegistry();
    llvm::initializeCore(Registry);
    llvm::initializeScalarOpts(Registry);
    llvm::initializeIPO(Registry);
    llvm::initializeAnalysis(Registry);
    llvm::initializeTransformUtils(Registry);
    llvm::initializeInstCombine(Registry);
    llvm::initializeTarget(Registry);
}

/**
 * Destructor.
 */
LLVMBackend::~LLVMBackend() {
}

/**
 * Compiles bytecode for the given target machine.
 *
 * @param bytecodeFile Full path to the llvm bytecode file to compile.
 * @param target Target machine to compile the bytecode for.
 * @param optLevel Optimization level.
 * @param debug If true, enable LLVM debug printing.
 */
TTAProgram::Program*
LLVMBackend::compile(
    const std::string& bytecodeFile, const std::string& emulationBytecodeFile,
    TTAMachine::Machine& target, int optLevel, bool debug,
    InterPassData* ipData) {
    // Check target machine
    MachineValidator validator(target);
    std::set<MachineValidator::ErrorCode> checks;
    checks.insert(MachineValidator::GCU_MISSING);
    checks.insert(MachineValidator::GCU_AS_MISSING);
    checks.insert(MachineValidator::USED_IO_NOT_BOUND);
    checks.insert(MachineValidator::PC_PORT_MISSING);
    checks.insert(MachineValidator::RA_PORT_MISSING);
    checks.insert(MachineValidator::FU_PORT_MISSING);
    MachineValidatorResults* res = validator.validate(checks);

    if (res->errorCount() > 0) {
        std::string msg;
        for (int i = 0; i < res->errorCount(); i++) {
            msg += res->error(i).second + "\n";
        }
        delete res; res = NULL;
        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    // Load bytecode file.
    std::string errMsgParse;
    LLVMContext context;

    std::unique_ptr<llvm::Module> m;

    ErrorOr<std::unique_ptr<MemoryBuffer>> bufferPtr =
        MemoryBuffer::getFileOrSTDIN(bytecodeFile.c_str());

    if (std::error_code ec = bufferPtr.getError()) {
        std::string msg = "Error reading bytecode file: " + bytecodeFile +
            "\n" + ec.message();
        throw CompileError(__FILE__, __LINE__, __func__, msg);
    } 

    // todo: what are these buffers..
    std::unique_ptr<MemoryBuffer> buffer = std::move(bufferPtr.get());
    Expected<std::unique_ptr<llvm::Module> > module =
        parseBitcodeFile(buffer.get()->getMemBufferRef(), context);
    if (Error E = module.takeError()) {
        THROW_EXCEPTION(CompileError, "Error parsing bytecode file: "
            + bytecodeFile);
    }

    // TODO: why does this work? it should not?
//    m.reset(module.get().get());
    m = std::move(module.get());

    if (m.get() == 0) {
        std::string msg = "Error parsing bytecode file: " + bytecodeFile +
            "\n" + errMsgParse;
        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    std::unique_ptr<Module> emuM;
   
    if (!emulationBytecodeFile.empty()) {
        ErrorOr<std::unique_ptr<MemoryBuffer>> emuBufferPtr =
            MemoryBuffer::getFileOrSTDIN(emulationBytecodeFile.c_str());

        if (std::error_code ec = emuBufferPtr.getError()) {
            std::string msg = "Error reading bytecode file: " + 
                emulationBytecodeFile +
                " of emulation library:\n" + ec.message();
            throw CompileError(__FILE__, __LINE__, __func__, msg);
        }

        std::unique_ptr<MemoryBuffer> emuBuffer = 
            std::move(emuBufferPtr.get());
        Expected<std::unique_ptr<Module> > module =
            parseBitcodeFile(emuBuffer.get()->getMemBufferRef(), context);
        if (Error E = module.takeError()) {
            THROW_EXCEPTION(CompileError, "Error parsing bytecode file: " +
                emulationBytecodeFile + " of emulation library \n"
                + errMsgParse);
        }

        emuM = std::move(module.get());
        if (emuM.get() == 0) {
            std::string msg = "Error parsing bytecode file: " + 
                emulationBytecodeFile + " of emulation library \n" 
                + errMsgParse;
            throw CompileError(__FILE__, __LINE__, __func__, msg);
        }
    }

    // Create target machine plugin.
#if (!defined(HAVE_CXX11) && !defined(HAVE_CXX0X))
    std::auto_ptr<TCETargetMachinePlugin> plugin(createPlugin(target));
#else
    std::unique_ptr<TCETargetMachinePlugin> plugin(createPlugin(target));
#endif

    TTAProgram::Program* result = NULL;
    try {
        // Compile.
        result =
            compile(*m.release(), emuM.release(), *plugin, target, optLevel,
                    debug, ipData);
    } catch (...) {
        // delete the backend plugin if we don't want to save it
        // Let's hope this doesn't crash as the plugin is loaded to the
        // current process. TCETargetMachinePlugin dtor should unload it.
        if (!options_->saveBackendPlugin()) {
            TCEString pluginPath = 
                cachePath_ + DS + pluginFilename(target);
            FileSystem::removeFileOrDirectory(pluginPath);
        }
        delete res; res = NULL;
        
        throw;
    }

    // delete the backend plugin if we don't want to save it
    // Let's hope this doesn't crash as the plugin is loaded to the
    // current process. TCETargetMachinePlugin dtor should unload it.
    if (!options_->saveBackendPlugin()) {
        TCEString pluginPath = 
            cachePath_ + DS + pluginFilename(target);
        FileSystem::removeFileOrDirectory(pluginPath);
    }
    delete res; res = NULL;

    return result;
}

/**
 * Ripped from LLVMTargetMachine.cpp
 
static void printAndVerify(PassManagerBase &PM,
                           bool allowDoubleDefs = false,
                           bool printMF = false) {
    if (printMF) {
        PM.add(createMachineFunctionPrinterPass(cerr));
    }
    PM.add(createMachineVerifierPass(allowDoubleDefs));
}
*/

/**
 * Finds the maximum alignment of an alloca in the module.
 *
 * Used to align the stack for the program image.
 */
unsigned
LLVMBackend::maxAllocaAlignment(const llvm::Module& mod) const {
    unsigned maxAlignment = 4;
    for (llvm::Module::const_iterator f = mod.begin(), 
             fe = mod.end(); f != fe; ++f) {
        for (llvm::Function::const_iterator bb = f->begin(), be = f->end();
             bb != be; ++bb) {
            const llvm::BasicBlock* basicBlock = &(*bb);
            for (llvm::BasicBlock::const_iterator i = basicBlock->begin(),
                     ie = basicBlock->end(); i != ie; ++i) {
                if (!isa<const llvm::AllocaInst>(i)) continue;
                const llvm::AllocaInst* alloca = 
                    dyn_cast<const llvm::AllocaInst>(i);
                maxAlignment = std::max(maxAlignment, alloca->getAlignment());
            }
        }
    }
    return maxAlignment;
}

#ifndef LLVM_OLDER_THAN_12
static MCRegisterInfo*
createTCEMCRegisterInfo(const Triple& TT) {
    MCRegisterInfo* X = new MCRegisterInfo();
    return X;
}

static MCInstrInfo*
createTCEMCInstrInfo() {
    MCInstrInfo* X = new MCInstrInfo();
    return X;
}

static MCSubtargetInfo*
createTCEMCSubtargetInfo(const Triple& TT, StringRef CPU, StringRef FS) {
    const MCWriteProcResEntry WPR[] = {{0, 0}};
    const MCWriteLatencyEntry WL[] = {{0, 0}};
    const MCReadAdvanceEntry RA[] = {{0, 0, 0}};
    ArrayRef<SubtargetFeatureKV> PF;
    ArrayRef<SubtargetSubTypeKV> PD;

    MCSubtargetInfo* X = new MCSubtargetInfo(
        TT, CPU, /*TuneCPU*/ "", FS, PF, PD, WPR, WL, RA, nullptr, nullptr,
        nullptr);

    return X;
}
#endif

/**
 * Compiles given llvm program module for target machine using the given
 * target machine plugin.
 *
 * @param module LLVM module to compile.
 * @param plugin Target architecture compiler plugin.
 * @param target Target architecture as Machine object.
 * @param optLevel Optimization level.
 * @param debug If true, enable LLVM debug printing.
 * @return Module compiled to program for the target architecture.
 */
TTAProgram::Program*
LLVMBackend::compile(
    llvm::Module& module, llvm::Module* emulationModule,
    TCETargetMachinePlugin& plugin, TTAMachine::Machine& target, int optLevel,
    bool /*debug*/, InterPassData* ipData) {
    ipData_ = ipData;
    // TODO: fixme
    std::string targetStr = "tce-llvm";
    if (target.isLittleEndian()) {
        if (target.is64bit()) {
            targetStr = "tcele64-llvm";
        } else {
            targetStr = "tcele-llvm";
        }
    }

    std::string errorStr;

    std::string featureString ="";

    // run registering code, which should have been done by LLVM
    
    // Initialize targets first. needs 
    // #include <llvm/Target/TargetSelecty.h>, 
    // whose defines collide with tce_config.h

    //InitializeAllTargets();
    //InitializeAllAsmPrinters();

    // Register target to llvm for using lookupTarget
    LLVMInitializeTCETargetInfo();
    LLVMInitializeTCETarget();

    // get registered target machine and set plugin.
    const Target* tceTarget =
        TargetRegistry::lookupTarget(targetStr, errorStr);
#ifndef LLVM_OLDER_THAN_12
    Target* nonconst_target = const_cast<Target*>(tceTarget);
#endif

    if (!tceTarget) {
        errs() << errorStr << "\n";
        return NULL;
    }

#ifndef LLVM_OLDER_THAN_12
    TargetRegistry::RegisterMCRegInfo(
        *nonconst_target, createTCEMCRegisterInfo);
    TargetRegistry::RegisterMCInstrInfo(
        *nonconst_target, createTCEMCInstrInfo);
    TargetRegistry::RegisterMCSubtargetInfo(
        *nonconst_target, createTCEMCSubtargetInfo);
#endif

    std::string cpuStr = "tce";

    TargetOptions Options;
#ifdef LLVM_OLDER_THAN_12
    Options.PrintMachineCode = false; //PrintCode;
#endif
    Options.UnsafeFPMath = false; //EnableUnsafeFPMath;
    Options.NoInfsFPMath = false; //EnableNoInfsFPMath;
    Options.NoNaNsFPMath = false; //EnableNoNaNsFPMath;
    Options.HonorSignDependentRoundingFPMathOption = false;
    // the stack alignment depends on the widest aligned
    // memory operations supported by the machine and
    // on the maximum alignment of any stack object in
    // the program, recompute this now as the llvm::Module has
    // been loaded
    Options.StackAlignmentOverride =
        (unsigned)MachineInfo::maxMemoryAlignment(target);
    if (options_->assumeADFStackAlignment()) {

        if (maxAllocaAlignment(module) > Options.StackAlignmentOverride) {
            abortWithError(
                "Alloca object requires larger stack alignment than widest "
                "memory operation. "
                "We were hoping this wouldn't happen, because now stack "
                "alignment cannot be "
                "figured out just from adf-file. ATM (5/20) this assumption is "
                "made here, "
                "at tcecc::getStackAlignment and at CodeGenerator.cc "
                "constructor.");
        }
    } else {
        Options.StackAlignmentOverride = std::max((unsigned)MachineInfo::maxMemoryAlignment(target), maxAllocaAlignment(module));
    }
    Options.GuaranteedTailCallOpt = true; //EnableGuaranteedTailCallOpt;

    TCETargetMachine* targetMachine =
        static_cast<TCETargetMachine*>(
            tceTarget->createTargetMachine(
                targetStr, cpuStr, featureString, Options,
                Reloc::Model::Static));

    if (!targetMachine) {
        errs() << "Could not create tce target machine" << "\n";
        return NULL;
    }

    // This hack must be cleaned up before adding TCE target to llvm upstream
    // these are needed by TCETargetMachine::addInstSelector passes
    targetMachine->setTargetMachinePlugin(plugin, target);
    targetMachine->setEmulationModule(emulationModule);

    /**
     * This is quite straight copy how llc actually creates passes for target.
     */       

    llvm::legacy::PassManager Passes;
#define addPass(P) Passes.add(P)    
    
    llvm::raw_fd_ostream sos(STDOUT_FILENO, false);

    targetMachine->addPassesToEmitFile(
        Passes, sos, nullptr, CGFT_AssemblyFile);


    // Add alias analysis pass that is distributed with pocl library.
    if (options_->isWorkItemAAFileDefined()) {
        ImmutablePass* (*creator)();
        std::string file = options_->workItemAAFile();
        bool foundAA = true;
        try {
            pluginTool_.importSymbol(
                "create_workitem_aa_plugin", creator, file);       
        } catch(Exception& e) {
            std::string msg = std::string() +
                "Unable to load plugin file '" +
                file + "'. Error: " + e.errorMessageStack();
            if (Application::verboseLevel() > 0) {
                Application::logStream()
                    << msg << std::endl;
                Application::logStream() << 
                    "Most likely too old version of POCL. \
                    TCE will continue without work item alias analysis." << 
                    std::endl;
            }
            foundAA = false;
        }
        if (foundAA)
            addPass(creator());
    }
    if (ipData_ != NULL) {
        // Stack pointer datum.
        RegDatum* spReg = new RegDatum;
        spReg->first = plugin.rfName(plugin.spDRegNum());
        spReg->second = plugin.registerIndex(plugin.spDRegNum());
        ipData_->setDatum("STACK_POINTER", spReg);

        // FP register datum.
        RegDatum* fpReg = new RegDatum;
        fpReg->first = plugin.rfName(plugin.fpDRegNum());
        fpReg->second = plugin.registerIndex(plugin.fpDRegNum());
        ipData_->setDatum("FRAME_POINTER", fpReg);

        // Return value register datum.
        RegDatum* rvReg = new RegDatum;
        rvReg->first = plugin.rfName(plugin.rvDRegNum());
        rvReg->second = plugin.registerIndex(plugin.rvDRegNum());
        ipData_->setDatum("RV_REGISTER", rvReg);

        std::vector<unsigned> paramRegs = plugin.getParamDRegNums();
        for (unsigned int i = 0; i < paramRegs.size(); i++) {
            RegDatum* p = new RegDatum;
            p->first = plugin.rfName(paramRegs[i]);
            p->second = plugin.registerIndex(paramRegs[i]);
            TCEString datumName = "IPARAM";
            datumName << i+2;
            ipData_->setDatum(datumName, p);
        }

        std::vector<unsigned> vectorRVRegs = plugin.getVectorRVDRegNums();
        for (unsigned int i = 0; i < vectorRVRegs.size(); i++) {
            RegDatum* p = new RegDatum;
            p->first = plugin.rfName(vectorRVRegs[i]);
            p->second = plugin.registerIndex(vectorRVRegs[i]);
            TCEString datumName = "VRV_REGISTER";
            datumName << i;
            ipData_->setDatum(datumName, p);
        }
        
        // TODO: add datums for vector RV registers.
    }

    // Find the inner loops. Must be executed with a separate
    // PassManager as mixing module passes and loop passes does
    // not seem to magically work.
    // TODO: is it safe to trust the llvm::BasicBlock pointers are
    // intact until we run the actual code generation? The loop info
    // is stored using those as indices.
    llvm::legacy::PassManager FPasses;
    InnerLoopFinder* loopFinder = new InnerLoopFinder();
    FPasses.add(loopFinder);
    FPasses.run(module);

    LLVMTCEBuilder* builder = NULL;

    // This is not actuall LLVM pass so we can not get actual AA.
    // It will be picked later, for now just passing NULL.
    // When LLVMTCEIRBuilder is called from TCEScheduler it will require
    // AA parameter.
    AliasAnalysis* AA = NULL;
    LLVMTCEIRBuilder* b = 
        new LLVMTCEIRBuilder(*targetMachine, &target, *ipData, AA);
    b->setInnerLoopFinder(loopFinder);
    builder = b;

    if (options_ != NULL && options_->isInitialStackPointerValueSet()) {
        builder->setInitialStackPointerValue(
            options_->initialStackPointerValue());
    }
    addPass(new ConstantTransformer(target));
    addPass(builder);
    Passes.run(module);

    if (ipData_ != NULL) {
        if (builder->isProgramUsingRestrictedPointers()) {
            ipData_->setDatum("RESTRICTED_POINTERS_FOUND", NULL);
        }
    }
    builder->deleteDeadProcedures();
    return builder->result();
}

/**
 * Creates TCETargetMachinePlugin for target architecture.
 *
 * @param target Target machine to build plugin for.
 */
TCETargetMachinePlugin*
LLVMBackend::createPlugin(const TTAMachine::Machine& target) {
    std::string pluginFile = pluginFilename(target);
    std::string pluginFileName;
    std::string tempPluginFileName;

    // Create cache directory if it doesn't exist.
    if (!FileSystem::fileIsDirectory(cachePath_)) {
        FileSystem::createDirectory(cachePath_);
    }

    pluginFileName = cachePath_ + DS + pluginFile;
    tempPluginFileName = cachePath_ + DS + pluginFile + ".%%_%%_%%_%%";

    llvm::SmallString<128> ResultPath;
    llvm::sys::fs::createUniqueFile(llvm::Twine(tempPluginFileName), ResultPath);
    tempPluginFileName = ResultPath.str().str();

    // Static plugin source files path.
    std::string srcsPath = "";
    std::string pluginIncludeFlags = "";
    if (useInstalledVersion_) {
        srcsPath = Application::installationDir() +  DS + "include" + DS;
        pluginIncludeFlags = " -I" + srcsPath;
    } else {
        srcsPath = std::string(TCE_SRC_ROOT) + DS +
            "src" + DS + "applibs" + DS + "LLVMBackend" + DS + "plugin" + DS;

        pluginIncludeFlags =
            " -I" + srcsPath + 
            " -I" + std::string(TCE_SRC_ROOT) + DS + " " +
            " -I" + std::string(TCE_SRC_ROOT) + DS + "src" + DS + "tools" +
            " -I" + std::string(TCE_SRC_ROOT) + DS + "src" + DS + "base" +
            DS + "mach"
            " -I" + std::string(TCE_SRC_ROOT) + DS + "src" + DS + 
            "applibs" + DS + "LLVMBackend" + DS + " " +

            " -I" + std::string(TCE_SRC_ROOT) + DS + "src" + DS + 
            "applibs" + DS + "mach" + " " +

            " -I" + std::string(TCE_SRC_ROOT) + DS + "src" + DS +
            "applibs" + DS + "Scheduler" + DS + " " +

            " -I" + std::string(TCE_SRC_ROOT) + DS + "src" + DS +
            "applibs" + DS + "Scheduler" + DS + "Algorithms" + " " +

            " -I`" LLVM_CONFIG " --includedir`" + DS + "llvm" + DS +
            "Target" + DS;

    }


    if (FileSystem::fileExists(pluginFileName) &&
        FileSystem::fileIsReadable(pluginFileName)) {

        try {
            pluginTool_.addSearchPath(cachePath_);
            pluginTool_.registerModule(pluginFile);
            TCETargetMachinePlugin* (*creator)();
            pluginTool_.importSymbol(
                "create_tce_backend_plugin", creator, pluginFile);

            return creator();
        } catch(Exception& e) {
            if (Application::verboseLevel() > 0) {
                Application::logStream()
                    << "Unable to load plugin file " << pluginFileName 
                    << ": " << e.errorMessage() << ", "
                    << "regenerating..." << std::endl;
            }
        }
    }

    if (!options_->useOldBackendSources()) {
        // Create target instruction and register definitions in .td files.
        TDGen* pluginGenPtr = new TDGen(target);

        try {
            pluginGenPtr->generateBackend(tempDir_);
            delete pluginGenPtr;
        } catch(Exception& e) {
            std::string msg =
                "Failed to build compiler plugin for target architecture: ";
            msg += e.errorMessage();
            CompileError ne(__FILE__, __LINE__, __func__, msg);
            ne.setCause(e);
            throw ne;
        }
    }
    std::string tblgenbin = "llvm-tblgen";

    // Generate TCEGenRegisterNames.inc
    std::string tblgenCmd;

    if (useInstalledVersion_) {
        // This is quite ugly. LLVM include dir is determined by
        // executing llvm-config in the commandline. This doesn't
        // work if llvm-config is not found in path.
        // First check that llvm-config is found in path.
        if (system(LLVM_CONFIG " --version")) {
            std::string msg = "Unable to determine llvm include dir. "
                LLVM_CONFIG " not found in path";

            throw CompileError(__FILE__, __LINE__, __func__, msg);
        }
        // /usr/include needs to be last in case there is old llvm installation
        // from packages
        tblgenCmd = tblgenbin + " " + TBLGEN_INCLUDES +
            pluginIncludeFlags +
            " -I" + tempDir_ +
            " -I`" LLVM_CONFIG " --includedir`" +
            " -I`" LLVM_CONFIG " --includedir`/Target" +
            " -I`" LLVM_CONFIG " --includedir`/llvm/Target" +
            " -I/usr/include ";
    } else {
        tblgenCmd = tblgenbin + " " + TBLGEN_INCLUDES +
            pluginIncludeFlags +
            " -I" + tempDir_ + 
            " -I" + LLVM_INCLUDE_DIR +
            " -I" + LLVM_INCLUDE_DIR + "/Target" +
            " -I" + LLVM_INCLUDE_DIR + "/llvm/Target"; 
    }

    tblgenCmd += " " + tempDir_ + FileSystem::DIRECTORY_SEPARATOR + "TCE.td";

    // Generate TCEGenRegisterInfo.inc
    std::string cmd = tblgenCmd +
        " -gen-register-info" +
        " -o " + tempDir_ + FileSystem::DIRECTORY_SEPARATOR +
        "TCEGenRegisterInfo.inc";

    if (Application::verboseLevel() > 0) {
        Application::logStream() << "LLVMBackend: " << cmd << std::endl;
    }
    int ret = system(cmd.c_str());
    if (ret) {
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    // Generate TCEGenInstrInfo.inc
    cmd = tblgenCmd +
        " -gen-instr-info" +
        " -o " + tempDir_ + FileSystem::DIRECTORY_SEPARATOR +
        "TCEGenInstrInfo.inc";

    ret = system(cmd.c_str());
    if (ret) {
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    // Generate TCEGenDAGISel.inc
    cmd = tblgenCmd +
        " -gen-dag-isel" +
        " -o " + tempDir_ + FileSystem::DIRECTORY_SEPARATOR +
        "TCEGenDAGISel.inc";

    ret = system(cmd.c_str());
    if (ret) { 
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    
    // Generate TCEGenCallingConv.inc
    cmd = tblgenCmd +
        " -gen-callingconv" +
        " -o " + tempDir_ + FileSystem::DIRECTORY_SEPARATOR +
        "TCEGenCallingConv.inc";

    ret = system(cmd.c_str());
    if (ret) { 
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    // Generate TCESuBTargetInfo.inc
    cmd = tblgenCmd +
        " -gen-subtarget" +
        " -o " + tempDir_ + FileSystem::DIRECTORY_SEPARATOR +
        "TCEGenSubTargetInfo.inc";

    ret = system(cmd.c_str());
    if (ret) {
        std::string msg =
            std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    // Generate TCEDFAPacketizer.inc
    cmd = tblgenCmd + " -gen-dfa-packetizer" + " -o " + tempDir_ +
          FileSystem::DIRECTORY_SEPARATOR + "TCEGenDFAPacketizer.inc";

    ret = system(cmd.c_str());
    if (ret) {
        std::string msg =
            std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    // NOTE: this could be get from Makefile.am
    TCEString pluginSources = srcsPath + "PluginCompileWrapper.cc ";

    TCEString endianOption = target.isLittleEndian() ?
        "-DLITTLE_ENDIAN_TARGET" : "";

    TCEString bitnessOption = target.is64bit() ? "-DTARGET64BIT" : "";
    // Compile plugin to cache.
    // CXX and SHARED_CXX_FLAGS defined in tce_config.h
    cmd = std::string(CXX) +
        " -I" + tempDir_ +
        pluginIncludeFlags +
        " " + SHARED_CXX_FLAGS +
        " " + LLVM_CPPFLAGS;

    if (useInstalledVersion_)
        cmd += " -I`" LLVM_CONFIG " --includedir`";

    cmd +=
        " " + CXX14_FLAG +
        " " + endianOption +
        " " + bitnessOption +
        " " + pluginSources +
        " -o " + tempPluginFileName;

    if (Application::verboseLevel() > 0) {
        Application::logStream() << "LLVMBackend: " << cmd << std::endl;
    }
    ret = system(cmd.c_str());
    if (ret) {
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    // move plugin to final location
    llvm::sys::fs::rename(llvm::Twine(tempPluginFileName), llvm::Twine(pluginFileName));

    // Load plugin.
    TCETargetMachinePlugin* (*creator)();
    try {
        pluginTool_.addSearchPath(cachePath_);
        pluginTool_.registerModule(pluginFile);
        pluginTool_.importSymbol(
            "create_tce_backend_plugin", creator, pluginFile);       
    } catch(Exception& e) {
        std::string msg = std::string() +
            "Unable to load plugin file '" +
            pluginFileName + "'. Error: " + e.errorMessage();

        IOException ne(__FILE__, __LINE__, __func__, msg);
        throw ne;
    }

    return creator();
}

/**
 * Returns (hopefully) unique plugin filename for target architecture.
 *
 * The filename includes also the TCE version string to avoid problems with 
 * incompatible backend plugins between TCE revisions.
 *  The filename is used for cached plugins.
 *
 * @param target Target architecture.
 * @return Filename for the target architecture.
 */
std::string
LLVMBackend::pluginFilename(
    const TTAMachine::Machine& target) {
    TCEString fileName = target.hash();
    fileName += "-" + Application::TCEVersionString();
    fileName += PLUGIN_SUFFIX;

    return fileName;
}
