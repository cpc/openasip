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
 * @file LLVMBackend.cc
 *
 * TCE compiler backend 
 *
 * @author Veli-Pekka Jääskeläinen 2008 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2009-2011
 * @note rating: red
 */

#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/LoopPass.h>
#include <llvm/Analysis/Dominators.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/PassManager.h>
#include <llvm/Pass.h>
//#include <llvm/ModuleProvider.h>

#include <llvm/CodeGen/AsmPrinter.h>
#include <llvm/CodeGen/Passes.h>
#include <llvm/CodeGen/GCStrategy.h>
#include <llvm/CodeGen/MachineFunctionAnalysis.h>

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
#include "tce_config.h"
#ifdef LLVM_3_2
#include <llvm/Module.h>
#include <llvm/LLVMContext.h>
#else
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#endif

#include <llvm/Bitcode/ReaderWriter.h>

#include <llvm/Analysis/Verifier.h>

// tce_config.h defines these. this undef to avoid warning.
// TODO: how to do this in tce_config.h???
#ifdef LLVM_LIBDIR
#undef LLVM_LIBDIR
#endif
#include "tce_config.h" // to get llvm version

#include <llvm/Support/TargetRegistry.h>

// cheat llvm's multi-include-protection
#define CONFIG_H
#include "llvm/Support/system_error.h"

#include <cstdlib> // system()
#include <fstream>

#include "LLVMBackend.hh"
#include "LLVMTCECmdLineOptions.hh"
#include "TDGen.hh"

#include "Environment.hh"
#include "Conversion.hh"
#include "FileSystem.hh"
#include "TCETargetMachine.hh"
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

//#define DEBUG_TDGEN

#define DS TCEString(FileSystem::DIRECTORY_SEPARATOR)

using namespace llvm;

#include <llvm/Assembly/PrintModulePass.h>

#ifdef LLVM_3_2

#include "llvm/DataLayout.h"
typedef llvm::DataLayout TargetData;

#else

#include "llvm/IR/DataLayout.h"
typedef llvm::DataLayout TargetData;

#endif

const std::string LLVMBackend::TBLGEN_INCLUDES = "";
const std::string LLVMBackend::PLUGIN_PREFIX = "tcecc-";
const std::string LLVMBackend::PLUGIN_SUFFIX = ".so";

/**
 * Returns minimum opset that is required by llvm.
 *
 * @return Minimumn opset that is required for llvm.
 */
OperationDAGSelector::OperationSet 
LLVMBackend::llvmRequiredOpset(bool includeFloatOps) {
    OperationDAGSelector::OperationSet requiredOps;

    requiredOps.insert("ADD");
    requiredOps.insert("SUB");
    requiredOps.insert("MUL");
    requiredOps.insert("DIV");
    requiredOps.insert("DIVU");
    requiredOps.insert("DIV");
    requiredOps.insert("MOD");
    requiredOps.insert("MODU");

    requiredOps.insert("LDW");
    requiredOps.insert("LDH");
    requiredOps.insert("LDHU");
    requiredOps.insert("LDQ");
    requiredOps.insert("LDQU");
    requiredOps.insert("STW");
    requiredOps.insert("STH");
    requiredOps.insert("STQ");

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
    initializeCore(Registry);
    initializeScalarOpts(Registry);
    initializeIPO(Registry);
    initializeAnalysis(Registry);
    initializeIPA(Registry);
    initializeTransformUtils(Registry);
    initializeInstCombine(Registry);
    initializeTarget(Registry);
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
    const std::string& bytecodeFile,
    const std::string& emulationBytecodeFile,
    TTAMachine::Machine& target,
    int optLevel,
    bool debug,
    InterPassData* ipData) 
    throw (Exception) {

    // Check target machine
    MachineValidator validator(target);
    std::set<MachineValidator::ErrorCode> checks;
    checks.insert(MachineValidator::GCU_MISSING);
    checks.insert(MachineValidator::GCU_AS_MISSING);
    checks.insert(MachineValidator::USED_IO_NOT_BOUND);
    checks.insert(MachineValidator::PC_PORT_MISSING);
    checks.insert(MachineValidator::RA_PORT_MISSING);
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
    LLVMContext &context = getGlobalContext();

    std::auto_ptr<Module> m;
    OwningPtr<MemoryBuffer> buffer;
    if (error_code ec = MemoryBuffer::getFileOrSTDIN(
            bytecodeFile.c_str(), buffer)) {
        std::string msg = "Error reading bytecode file: " + bytecodeFile +
            "\n" + ec.message();
        throw CompileError(__FILE__, __LINE__, __func__, msg);
    } else {
        m.reset(ParseBitcodeFile(buffer.get(), context, &errMsgParse));
    }
    if (m.get() == 0) {
        std::string msg = "Error parsing bytecode file: " + bytecodeFile +
	    "\n" + errMsgParse;
        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    std::auto_ptr<Module> emuM;
    if (!emulationBytecodeFile.empty()) {
        OwningPtr<MemoryBuffer> emuBuffer;
        if (error_code ec = MemoryBuffer::getFileOrSTDIN(
                emulationBytecodeFile.c_str(), emuBuffer)) {
            std::string msg = "Error reading bytecode file: " + 
                emulationBytecodeFile +
                " of emulation library:\n" + ec.message();
            throw CompileError(__FILE__, __LINE__, __func__, msg);
        }
        else {
            emuM.reset(
                ParseBitcodeFile(emuBuffer.get(), context, &errMsgParse));
        }
        if (emuM.get() == 0) {
            std::string msg = "Error parsing bytecode file: " + 
                emulationBytecodeFile + " of emulation library \n" 
                + errMsgParse;
            throw CompileError(__FILE__, __LINE__, __func__, msg);
        }
    }
   
    // Create target machine plugin.
    std::auto_ptr<TCETargetMachinePlugin> plugin(createPlugin(target));

    TTAProgram::Program* result = NULL;
    try {
        // Compile.
        result =
            compile(*m.release(), emuM.release(), *plugin, target, optLevel, debug, 
                    ipData);
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
    llvm::Module& module,
    llvm::Module* emulationModule,
    TCETargetMachinePlugin& plugin,
    TTAMachine::Machine& target,
    int optLevel,
    bool /*debug*/,
    InterPassData* ipData)
    throw (Exception) {

    ipData_ = ipData;
    std::string targetStr = "tce-llvm";
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

    // get registered target machine and set plugin.
    const Target* tceTarget = 
	TargetRegistry::lookupTarget(targetStr, errorStr); 
    
    if (!tceTarget) {
        errs() << errorStr << "\n";
        errs() << "Available targets:\n";
        for (TargetRegistry::iterator i = TargetRegistry::begin(); 
             i != TargetRegistry::end(); i++) {
            errs() << i->getName() << " : " 
                   << i->getShortDescription() << "\n";
        }        
        return NULL;
    }
    
    // TODO: what should this be? revision 134127 of llvm added this.
    std::string cpuStr = "tce";
    // TODO: are these default options, ripped from llc, sensible?

  TargetOptions Options;
  Options.LessPreciseFPMADOption = true; //EnableFPMAD;
  Options.PrintMachineCode = false; //PrintCode;
  Options.NoFramePointerElim = false; // DisableFPElim;
#if (defined (LLVM_3_2) || defined(LLVM_3_3))
  Options.NoFramePointerElimNonLeaf = false; //DisableFPElimNonLeaf;
#endif
  // TODO: just commented this out..
//  Options.NoExcessFPPrecision = true; //DisableExcessPrecision;
  Options.UnsafeFPMath = false; //EnableUnsafeFPMath;
  Options.NoInfsFPMath = false; //EnableNoInfsFPMath;
  Options.NoNaNsFPMath = false; //EnableNoNaNsFPMath;
  Options.HonorSignDependentRoundingFPMathOption = false;
//      EnableHonorSignDependentRoundingFPMath;
  Options.UseSoftFloat = false; //GenerateSoftFloatCalls; 
//  if (FloatABIForCalls != FloatABI::Default)
//    Options.FloatABIType = FloatABIForCalls;
//  Options.NoZerosInBSS = DontPlaceZerosInBSS;
//  Options.JITExceptionHandling = EnableJITExceptionHandling;
//  Options.JITEmitDebugInfo = EmitJitDebugInfo;
//  Options.JITEmitDebugInfoToDisk = EmitJitDebugInfoToDisk;
  Options.GuaranteedTailCallOpt = true; //EnableGuaranteedTailCallOpt;
  Options.StackAlignmentOverride = false; //OverrideStackAlignment;
#if (defined(LLVM_3_2) || defined(LLVM_3_3))
  Options.RealignStack = false; //EnableRealignStack;
#endif
//TODO: new llvm removed/renamed this
//  Options.DisableJumpTables = false; //DisableSwitchTables;
//  Options.TrapFuncName = TrapFuncName;
  Options.EnableSegmentedStacks = false; //SegmentedStacks;

    TCETargetMachine* targetMachine = 
        static_cast<TCETargetMachine*>(
            tceTarget->createTargetMachine(
		targetStr, cpuStr, featureString, Options));

    if (!targetMachine) {
        errs() << "Could not create tce target machine" << "\n";
        return NULL;
    }

    // This hack must be cleaned up before adding TCE target to llvm upstream
    // these are needed by TCETargetMachine::addInstSelector passes
    targetMachine->setTargetMachinePlugin(plugin);
    targetMachine->setTTAMach(&target);
    targetMachine->setEmulationModule(emulationModule);

    /**
     * This is quite straight copy how llc actually creates passes for target.
     */       

    // if opt level is less than 2 we will not run extra optimization passes
    // after linking emulation function code
    CodeGenOpt::Level OptLevel = 
        (optLevel < 2) ? (CodeGenOpt::None) : (CodeGenOpt::Aggressive);

//    ExistingModuleProvider provider(&module);    
    llvm::PassManager Passes;
    
    const TargetData *TD = targetMachine->getDataLayout();
    assert(TD);

    Passes.add(new TargetData(*TD));

    targetMachine->addPassesToEmitFile(
        Passes, fouts(), TargetMachine::CGFT_AssemblyFile, OptLevel);
//    targetMachine->addPassesToEmitFileFinish(Passes, OCE, OptLevel);
// TODO: should the above be on===

    // inlined from old, removed addPassesToEmitFileFinish();
//    setCodeModelForStatic();
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
            Passes.add(creator());
    }
#ifdef LLVM_3_2
    Passes.add(createGCInfoDeleter());
    // LLVM 3.3 doesn't have this. Maybe it's not even needed anymore?
#endif
    if (ipData_ != NULL) {
        // Stack pointer datum.
        RegDatum* spReg = new RegDatum;
        spReg->first = plugin.rfName(plugin.spDRegNum());
        spReg->second = plugin.registerIndex(plugin.spDRegNum());
        ipData_->setDatum("STACK_POINTER", spReg);

        // Return value register datum.
        RegDatum* rvReg = new RegDatum;
        rvReg->first = plugin.rfName(plugin.rvDRegNum());
        rvReg->second = plugin.registerIndex(plugin.rvDRegNum());
        ipData_->setDatum("RV_REGISTER", rvReg);

        // high-part of 64-bit RV datum
        RegDatum* rvHighReg = new RegDatum;
        rvHighReg->first = plugin.rfName(plugin.rvHighDRegNum());
        rvHighReg->second = plugin.registerIndex(plugin.rvHighDRegNum());
        ipData_->setDatum("RV_HIGH_REGISTER", rvHighReg);
    }

    LLVMTCEBuilder* builder = NULL;

    // This is not actuall LLVM pass so we can not get actual AA.
    // It will be picked later, for now just passing NULL.
    // When LLVMTCEIRBuilder is called from TCEScheduler it will require
    // AA parameter.
    AliasAnalysis* AA = NULL;
    builder = new LLVMTCEIRBuilder(*targetMachine, &target, *ipData, AA);

    Passes.add(builder);
    Passes.run(module);
    // get and write out pom
    TTAProgram::Program* prog = builder->result();
    assert(prog != NULL);
    if (ipData_ != NULL) {
        if (builder->isProgramUsingRestrictedPointers()) {
            ipData_->setDatum("RESTRICTED_POINTERS_FOUND", NULL);
        }
        if (builder->isProgramUsingAddressSpaces()) {
            ipData_->setDatum("MULTIPLE_ADDRESS_SPACES_FOUND", NULL);
        }
    }
    return prog;
}

/**
 * Creates TCETargetMachinePlugin for target architecture.
 *
 * @param target Target machine to build plugin for.
 */
TCETargetMachinePlugin*
LLVMBackend::createPlugin(const TTAMachine::Machine& target)
    throw (Exception) {

    std::string pluginFile = pluginFilename(target);
    std::string pluginFileName = "";

    // Create cache directory if it doesn't exist.
    if (!FileSystem::fileIsDirectory(cachePath_)) {
        FileSystem::createDirectory(cachePath_);
    }
    pluginFileName = cachePath_ + DS + pluginFile;

    // Static plugin source files path.
    std::string srcsPath = "";
    std::string pluginIncludeFlags = "";
    if (useInstalledVersion_) {
        srcsPath = std::string(TCE_INSTALLATION_ROOT) +  DS + "include" + DS;
        pluginIncludeFlags = " -I" + srcsPath;
    } else {
        srcsPath = std::string(TCE_SRC_ROOT) + DS +
            "src" + DS + "applibs" + DS + "LLVMBackend" + DS + "plugin" + DS;

        pluginIncludeFlags =
            " -I" + srcsPath + 
            " -I" + std::string(TCE_SRC_ROOT) + DS + " " +
            " -I" + std::string(TCE_SRC_ROOT) + DS + "src" + DS + "tools" +              
            " -I" + std::string(TCE_SRC_ROOT) + DS + "src" + DS + 
            "applibs" + DS + "LLVMBackend" + DS + " " +

            " -I" + std::string(TCE_SRC_ROOT) + DS + "src" + DS + 
            "applibs" + DS + "Scheduler" + DS + " " +

            " -I" + std::string(TCE_SRC_ROOT) + DS + "src" + DS + 
            "applibs" + DS + "Scheduler" + DS + "Algorithms" + " " +
            
            " -I" + std::string(TCE_INSTALLATION_ROOT) +  DS + 
            "include" + DS +
            
            " -I`llvm-config --includedir`" + DS + "llvm" + DS + 
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
        TDGen plugingen(target);
        try {
            plugingen.generateBackend(tempDir_);
        } catch(Exception& e) {
            std::string msg =
                "Failed to build compiler plugin for target architecture.";
            
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
        if (system("llvm-config --version")) {
            std::string msg = "Unable to determine llvm include dir. "
                "llvm-config not found in path";

            throw CompileError(__FILE__, __LINE__, __func__, msg);
        }
        // /usr/include needs to be last in case there is old llvm installation
        // from packages
        tblgenCmd = tblgenbin + " " + TBLGEN_INCLUDES +
            pluginIncludeFlags + 
            " -I" + tempDir_ +
            " -I`llvm-config --includedir`" + 
            " -I`llvm-config --includedir`/Target" + 
            " -I`llvm-config --includedir`/llvm/Target" +
            " -I/usr/include ";
    } else {
        tblgenCmd = tblgenbin + " " + TBLGEN_INCLUDES +
            pluginIncludeFlags +
            " -I" + tempDir_ + 
            " -I" + LLVM_INCLUDEDIR +
            " -I" + LLVM_INCLUDEDIR + "/Target" +
            " -I" + LLVM_INCLUDEDIR + "/llvm/Target"; 
    }

    tblgenCmd += " " + tempDir_ + FileSystem::DIRECTORY_SEPARATOR + "TCE.td";

    // Generate TCEGenRegisterInfo.inc

    std::string cmd = tblgenCmd +
        " -gen-register-info" +
        " -o " + tempDir_ + FileSystem::DIRECTORY_SEPARATOR +
        "TCEGenRegisterInfo.inc";

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
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    // NOTE: this could be get from Makefile.am
    std::string pluginSources =
        srcsPath + "TCERegisterInfo.cc " +
        srcsPath + "TCEInstrInfo.cc " +
        srcsPath + "TCEISelLowering.cc " +
        srcsPath + "TCEDAGToDAGISel.cc " +
        srcsPath + "TCETargetObjectFile.cc " +
        srcsPath + "TCEFrameInfo.cc " +
        srcsPath + "TCETargetMachinePlugin.cc " +
        srcsPath + "TCESubtarget.cc ";

    // Compile plugin to cache.
    // CXX and SHARED_CXX_FLAGS defined in tce_config.h
    cmd = std::string(CXX) +
        " -I" + tempDir_ +
        pluginIncludeFlags +
        " " + SHARED_CXX_FLAGS +
        " " + LLVM_CPPFLAGS +
        " " + pluginSources +
        " -o " + pluginFileName;

    // TODO: whether vectors are used or not stored in has of the
    // plugin. this is a temporary solution
    if (options_->useVectorBackend()) {
        cmd += " -DUSE_VECTOR_REGS";
    }
    ret = system(cmd.c_str());
    if (ret) {
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

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
LLVMBackend::pluginFilename(const TTAMachine::Machine& target) {
    TCEString fileName = target.hash();
    fileName += "-" + Application::TCEVersionString();
    fileName += PLUGIN_SUFFIX;

    return fileName;
}
