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
 * @file LLVMBackend.cc
 *
 * TCE compiler backend 
 *
 * @author Veli-Pekka Jääskeläinen 2008 (vjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2009 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#include <llvm/PassManager.h>
#include <llvm/Pass.h>
#include <llvm/ModuleProvider.h>

#include <llvm/CodeGen/AsmPrinter.h>
#include <llvm/CodeGen/Passes.h>
#include <llvm/CodeGen/GCStrategy.h>
#include <llvm/CodeGen/MachineFunctionAnalysis.h>

#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetAsmInfo.h>
#include <llvm/Target/TargetRegistry.h>

#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/Scalar.h>

#include <llvm/Support/CommandLine.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Debug.h>

#include <llvm/Bitcode/ReaderWriter.h>

#include <llvm/Analysis/Verifier.h>

#include <cstdlib> // system()
#include <boost/functional/hash.hpp>

#include "LLVMBackend.hh"
#include "TDGen.hh"

#include "tce_config.h" // CXX, SHARED_CXX_FLAGS, LLVM LD&CPP flags

#include "Environment.hh"
#include "Conversion.hh"
#include "FileSystem.hh"
#include "TCETargetMachine.hh"
#include "TCETargetMachinePlugin.hh"
#include "LLVMPOMBuilder.hh"
#include "Program.hh"
#include "ADFSerializer.hh"
#include "SchedulingPlan.hh"
#include "SchedulerFrontend.hh"
#include "MachineValidator.hh"
#include "MachineValidatorResults.hh"

#include "InterPassData.hh"
#include "InterPassDatum.hh"

using namespace llvm;

#include <llvm/Assembly/PrintModulePass.h>

Pass* createLowerMissingInstructionsPass(const TTAMachine::Machine& mach);
Pass* createLinkBitcodePass(Module& inputCode);
Pass* createMachineDCE();

const std::string LLVMBackend::TBLGEN_INCLUDES = "";
const std::string LLVMBackend::PLUGIN_PREFIX = "tcecc-";
const std::string LLVMBackend::PLUGIN_SUFFIX = ".so";
    
/**
 * Constructor.
 */
LLVMBackend::LLVMBackend(bool useCache, bool useInstalledVersion, bool removeTempFiles):
    useCache_(useCache), useInstalledVersion_(useInstalledVersion), removeTmp_(removeTempFiles) {
    cachePath_ = Environment::llvmtceCachePath();    
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
    InterPassData* ipData) throw (Exception){

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
    std::string errorMessage;
    LLVMContext &context = getGlobalContext();

    // TODO: refactor
    std::auto_ptr<Module> m;
    std::auto_ptr<MemoryBuffer> buffer(
        MemoryBuffer::getFileOrSTDIN(bytecodeFile, &errorMessage));
    if (buffer.get()) {
        m.reset(ParseBitcodeFile(buffer.get(), context, &errorMessage));
    }
    if (m.get() == 0) {
        std::string msg = "Error reading bytecode file:\n" + errorMessage;
        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    
    std::auto_ptr<Module> emuM;

    if (!emulationBytecodeFile.empty()) {
        std::auto_ptr<MemoryBuffer> emuBuffer(
            MemoryBuffer::getFileOrSTDIN(emulationBytecodeFile, &errorMessage));
        if (emuBuffer.get()) {
            emuM.reset(ParseBitcodeFile(emuBuffer.get(), context, &errorMessage));
        }
        if (emuM.get() == 0) {
            std::string msg = "Error reading bytecode file:\n" + errorMessage;
            throw CompileError(__FILE__, __LINE__, __func__, msg);
        }
    }

    // Create target machine plugin.
    TCETargetMachinePlugin* plugin = createPlugin(target);

    // Compile.
    TTAProgram::Program* result =
        compile(*m.release(), emuM.release(), *plugin, target, optLevel, debug, ipData);

    delete plugin;
    plugin = NULL;

    delete res; res = NULL;

    return result;
}

/**
 * Ripped from LLVMTargetMachine.cpp
 */
static void printAndVerify(PassManagerBase &PM,
                           bool allowDoubleDefs = false,
                           bool printMF = false) {
    if (printMF) {
        PM.add(createMachineFunctionPrinterPass(cerr));
    }
//    PM.add(createMachineVerifierPass(allowDoubleDefs));
}


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
    int /* optLevel */,
    bool debug,
    InterPassData* ipData)
    throw (Exception) {

    llvm::DebugFlag = debug;
    ipData_ = ipData;
    bool fast = false;
    std::string targetStr = "tce-llvm";
    std::string errorStr;

    std::string featureString ="";

    // run registering code, which should have been done by LLVM
    
    // Initialize targets first. needs 
    // #include <llvm/Target/TargetSelecty.h>, 
    // whose defines collide with tce_config.h

    //InitializeAllTargets();
    //InitializeAllAsmPrinters();

    LLVMInitializeTCETargetInfo();

    // get registered target machine and set plugin.
    const Target* tceTarget = TargetRegistry::lookupTarget(targetStr, errorStr); 
    
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
    
    TCETargetMachine* targetMachine = 
        dynamic_cast<TCETargetMachine*>(
            tceTarget->createTargetMachine(targetStr, featureString));

    if (!targetMachine) {
        errs() << "Could not create tce target machine" << "\n";
        return NULL;
    }

    // NOTE: maybe this should be done by passing info in 
    //       featureString to TCETargetMachine or Subtarget
    targetMachine->setTargetMachinePlugin(plugin);

    /**
     * If there is problems with pass constructions 
     *
     * check passes which must be generated  from llc.cpp and
     * llvm/lib/CodeGen/LLVMTargetMachine.cpp
     * 
     * Ultimately would be nice that llc could be used to run
     * the whole pom generation. 
     * (pass creation should be moved to TCETargetMachine)

     * passes ran in addCommonCodeGenPasses (llvm 2.6)
     
     createLoopStrengthReducePass(getTargetLowering());
     createLowerInvokePass(getTargetLowering());
     createGCLoweringPass();
     createUnreachableBlockEliminationPass();
     createCodeGenPreparePass(getTargetLowering());
     createStackProtectorPass(getTargetLowering());
     new MachineFunctionAnalysis(*this, OptLevel);
     addInstSelector(PM, OptLevel); // overload in TargetMachine...
     createMachineLICMPass();
     createMachineSinkingPass()
     addPreRegAlloc(PM, OptLevel); // overload in TargetMachine...
     createRegisterAllocator()
     createStackSlotColoringPass(false)
     addPostRegAlloc(PM, OptLevel); // overload in TargetMachine...
     createLowerSubregsPass();
     createPrologEpilogCodeInserter();
     createPostRAScheduler();
     createBranchFoldingPass(getEnableTailMergeDefault());
     createGCMachineCodeAnalysisPass();

     * ------  TCE extra passes.. ------
     createLowerMissingInstructionsPass(target) // emulation code
     createLinkBitcodePass(*emulationModule) // link emulation code
     createMachineDCE() // remove unneeded emulation code
     // remove debug GlobalValues
     createStripSymbolsPass(bool OnlyDebugInfo=true) 
     */

    CodeGenOpt::Level OptLevel = CodeGenOpt::Aggressive;

    const TargetData *TD = targetMachine->getTargetData();
    assert(TD);

    ExistingModuleProvider provider(&module);
    
    llvm::PassManager pm1;
    pm1.add(new TargetData(*TD));

    // lower floating point stuff..
    pm1.add(createLowerMissingInstructionsPass(target));
      

    // Some passes from addCommonCodeGen
    pm1.add(createLoopStrengthReducePass(targetMachine->getTargetLowering()));
    pm1.add(createLowerInvokePass(targetMachine->getTargetLowering()));
    pm1.add(createGCLoweringPass());
    pm1.add(createUnreachableBlockEliminationPass());
    pm1.add(createCodeGenPreparePass(targetMachine->getTargetLowering()));
    pm1.add(createStackProtectorPass(targetMachine->getTargetLowering()));

    // Some TCE specific passes

    // this should use scan data to link in needed emulation functions.
    // emulationModule object will be useless after this 
    // (linker is not very gentle).
    if (emulationModule != NULL) {
        pm1.add(createLinkBitcodePass(*emulationModule));
    }
  
      // to allow machine dead basic block elimination...
    pm1.add(createInternalizePass(true));

    // NOTE: This must be added before Machine function analysis pass..
    // needed by POMBuilder to prevent writing debug data to data section
    // might be good to disable when printing out machine function code...
    pm1.add(createStripSymbolsPass(/*bool OnlyDebugInfo=*/true));

    // More passes from addCommonCodeGen
    pm1.add(new MachineFunctionAnalysis(*targetMachine, OptLevel));

    targetMachine->addInstSelector(pm1, OptLevel);
    printAndVerify(pm1, /* allowDoubleDefs= */ true, /* printMF=*/ true);

    // Yet some more addCommonCodceGen passes
    pm1.add(createMachineLICMPass());
    printAndVerify(pm1, /* allowDoubleDefs= */ true);

    pm1.add(createMachineSinkingPass());
    printAndVerify(pm1, /* allowDoubleDefs= */ true);

    targetMachine->addPreRegAlloc(pm1, OptLevel);
    printAndVerify(pm1, /* allowDoubleDefs= */ true, /* printMF=*/ true);

    pm1.add(createRegisterAllocator());
    printAndVerify(pm1, /* allowDoubleDefs= */ true, /* printMF=*/ true);

    pm1.add(createStackSlotColoringPass(false));
    printAndVerify(pm1, /* allowDoubleDefs= */ true);

    targetMachine->addPostRegAlloc(pm1, OptLevel);
    printAndVerify(pm1, /* allowDoubleDefs= */ true);

    pm1.add(createLowerSubregsPass());
    printAndVerify(pm1, /* allowDoubleDefs= */ true);

    pm1.add(createPrologEpilogCodeInserter());
    printAndVerify(pm1, /* allowDoubleDefs= */ true, /* printMF=*/ true);

    // Following pass breaks something ... 
    // and also causes invalid results.
    // e.g.
    //*** Bad machine code: Using an undefined physical register ***
    //- function:    float32_sqrt
    //- basic block: bb16 0xa1a1d8c (#18)
    //- instruction: %I10<def> = SHLri %I8, 17 
    //[dbg: newlib/libc/sys/tce/float_emulation.c,426,0]
    //- operand 1:   %I8
    //LLVM ERROR: Found 1 machine code errors.
    //]  
    //pm1.add(createPostRAScheduler());    
    //printAndVerify(pm1, /* allowDoubleDefs= */ true);
    
    pm1.add(createBranchFoldingPass(
                targetMachine->getEnableTailMergeDefault()));
    
    pm1.add(createGCMachineCodeAnalysisPass());
    printAndVerify(pm1, /* allowDoubleDefs= */ true);
    
    // and a pass from addPassesToEmitFile
    pm1.add(createDebugLabelFoldingPass());
    printAndVerify(pm1, /* allowDoubleDefs= */ true);

    LLVMPOMBuilder* pomBuilder = new LLVMPOMBuilder(*targetMachine, &target);
    pm1.add(pomBuilder);

    // run passes (runs automatically initializers and finalize). 
    pm1.run(module);

    TTAProgram::Program* prog = pomBuilder->result();
    assert(prog != NULL);

    if (ipData_ != NULL) {
        typedef SimpleInterPassDatum<std::pair<std::string, int> > RegData;

        // Stack pointer datum.
        RegData* spReg = new RegData;
        spReg->first = plugin.rfName(plugin.spDRegNum());
        spReg->second = plugin.registerIndex(plugin.spDRegNum());
        ipData_->setDatum("STACK_POINTER", spReg);
    }

    return prog;
}


/**
 * Compiles bytecode for the given target machine and calls scheduler.
 *
 * @param bytecodeFile Full path to the llvm bytecode file to compile.
 * @param target Target machine to compile the bytecode for.
 * @param optLevel Optimization level.
 * @param debug Debug level as integer.
 * @return Scheduled program or NULL if no plan given.
 */
TTAProgram::Program* 
LLVMBackend::compileAndSchedule(
    const std::string& bytecodeFile,
    const TTAMachine::Machine& target,
    int optLevel,
    const unsigned int debug)
    throw (Exception) {

    static const std::string DS = FileSystem::DIRECTORY_SEPARATOR;

    // create temp directory for the target machine
    std::string tmpDir = FileSystem::createTempDirectory();
   
    // write machine to a file for tcecc
    std::string adf = tmpDir + DS + "mach.adf";
    std::string tpef = tmpDir + DS + "program.tpef";
    ADFSerializer serializer;
    serializer.setDestinationFile(adf);
    try {
        serializer.writeMachine(target);
    } catch (const SerializerException& exception) {
        if (removeTmp_) {
            FileSystem::removeFileOrDirectory(tmpDir);
        }
        throw IOException(
            __FILE__, __LINE__, __func__, exception.errorMessage());
    } 

    // call tcecc to compile, link and schedule the program
    std::vector<std::string> tceccOutputLines;
    std::string debugParams = "";
    if (debug > 3) {
        debugParams = "-v ";
        if (debug > 4) {
            debugParams.append("--debug ");
        }
    }
    std::string tceccPath = Environment::tceCompiler();
    std::string tceccCommand = tceccPath + " " + debugParams 
        + "-O " + Conversion::toString(optLevel) + " -a " + adf + " -o " 
        + tpef + " " + bytecodeFile + " 2>&1";

    Application::runShellCommandAndGetOutput(tceccCommand, tceccOutputLines);

    if (debug && tceccOutputLines.size() > 0) {
        for (unsigned int i = 0; i < tceccOutputLines.size(); ++i) {
            std::cout << tceccOutputLines.at(i) << std::endl;
        }
    }

    // check if tcecc produced any tpef output
    if (!(FileSystem::fileExists(tpef) && FileSystem::fileIsReadable(tpef))) {
        if (removeTmp_) {
            FileSystem::removeFileOrDirectory(tmpDir);
        }
        return NULL;    
    } 

    TTAProgram::Program* prog = NULL;
    try {
        prog = TTAProgram::Program::loadFromTPEF(tpef, target);
    } catch (const Exception& e) {
        if (removeTmp_) {
            FileSystem::removeFileOrDirectory(tmpDir);
        }
        IOException error(__FILE__, __LINE__,__func__, e.errorMessage());
        error.setCause(e);
        throw error;
    }

    if (removeTmp_) {
        FileSystem::removeFileOrDirectory(tmpDir);
    }

    return prog;
}


/**
 * Compiles bytecode for the given target machine and calls scheduler.
 *
 * @param bytecodeFile Full path to the llvm bytecode file to compile.
 * @param target Target machine to compile the bytecode for.
 * @param optLevel Optimization level.
 * @param plan Scheduling plan.
 * @param debug If true, enable LLVM debug printing.
 * @return Scheduled program.
 */
TTAProgram::Program* 
LLVMBackend::schedule(
    const std::string& bytecodeFile,
    const std::string& emulationBytecodeFile,
    TTAMachine::Machine& target,
    int optLevel,
    bool debug,
    SchedulingPlan* plan)
    throw (Exception) {

    TTAProgram::Program* prog = 
        compile(bytecodeFile, emulationBytecodeFile, target, optLevel, debug);

    // load default scheduler plan if no plan given
    if (plan == NULL) {
        try {
            plan = SchedulingPlan::loadFromFile(
                    Environment::defaultSchedulerConf());
        } catch (const Exception& e) {
            std::string msg = "Unable to load default scheduler config '" +
                Environment::defaultSchedulerConf() + "'.";

            IOException ne(__FILE__, __LINE__, __func__, msg);
            ne.setCause(e);
            throw ne;
        }
    }

    SchedulerFrontend scheduler;
    prog = scheduler.schedule(*prog, target, *plan);

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
    std::string DS = FileSystem::DIRECTORY_SEPARATOR;

    // Create temp directory for building the target machine plugin.
    std::string tmpDir = FileSystem::createTempDirectory();

    if (useCache_) {
        // Create cache directory if it doesn't exist.
        if (!FileSystem::fileIsDirectory(cachePath_)) {
            FileSystem::createDirectory(cachePath_);
        }
        pluginFileName = cachePath_ + DS + pluginFile;

    } else {
        pluginFileName = tmpDir + DS + pluginFile;
    }

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
            
            " -I" + std::string(TCE_SRC_ROOT) + DS + "src" + DS + 
            "applibs" + DS + "LLVMBackend" + DS + " " +
            
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

            if (removeTmp_) {
                FileSystem::removeFileOrDirectory(tmpDir);
            }
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

    // Create target instruction and register definitions in .td files.
    TDGen plugingen(target);
    try {
        plugingen.generateBackend(tmpDir);
    } catch(Exception& e) {
        if (removeTmp_) {
            FileSystem::removeFileOrDirectory(tmpDir);
        }
        std::string msg =
            "Failed to build compiler plugin for target architecture.";

        CompileError ne(__FILE__, __LINE__, __func__, msg);
        ne.setCause(e);
        throw ne;
    }
    
    // since llvm-2.6 tblgen is installed
    std::string tblgenbin = "tblgen";
       
    // Generate TCEGenRegisterNames.inc
    std::string tblgenCmd;
    
    if (useInstalledVersion_) {
        // This is quite ugly. LLVM include dir is determined by
        // executing llvm-config in the commandline. This doesn't
        // work if llvm-config is not found in path.
        // First check that llvm-config is found in path.
        if (system("llvm-config --version")) {
            if (removeTmp_) {
                FileSystem::removeFileOrDirectory(tmpDir);
            }
            std::string msg = "Unable to determine llvm include dir. "
                "llvm-config not found in path";

            throw CompileError(__FILE__, __LINE__, __func__, msg);
        }
        // /usr/include needs to be last in case there is old llvm instalation
        // from packages
        tblgenCmd = tblgenbin + " " + TBLGEN_INCLUDES +
            " -I" + tmpDir +
            " -I`llvm-config --includedir`" + 
            " -I`llvm-config --includedir`/Target" + 
            " -I`llvm-config --includedir`/llvm/Target" +
            " -I/usr/include ";
    } else {
        tblgenCmd = tblgenbin + " " + TBLGEN_INCLUDES +
            " -I" + tmpDir + 
	    " -I" + LLVM_INCLUDEDIR +
	    " -I" + LLVM_INCLUDEDIR + "/Target" +
	    " -I" + LLVM_INCLUDEDIR + "/llvm/Target"; 
    }

    tblgenCmd += pluginIncludeFlags;
    tblgenCmd += " " + tmpDir + FileSystem::DIRECTORY_SEPARATOR + "TCE.td";

    std::string cmd = tblgenCmd + " -gen-register-enums" +
        " -o " + tmpDir + FileSystem::DIRECTORY_SEPARATOR +
        "TCEGenRegisterNames.inc";

    int ret = system(cmd.c_str());
    if (ret) {
        if (removeTmp_) {
            FileSystem::removeFileOrDirectory(tmpDir);
        }
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    // Generate TCEGenRegisterInfo.inc
    cmd = tblgenCmd +
        " -gen-register-desc" +
        " -o " + tmpDir + FileSystem::DIRECTORY_SEPARATOR +
        "TCEGenRegisterInfo.inc";

    ret = system(cmd.c_str());
    if (ret) {
        if (removeTmp_) {
            FileSystem::removeFileOrDirectory(tmpDir);
        }
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    // Generate TCEGenRegisterInfo.h.inc
    cmd = tblgenCmd +
        " -gen-register-desc-header" +
        " -o " + tmpDir + FileSystem::DIRECTORY_SEPARATOR +
        "TCEGenRegisterInfo.h.inc";

    ret = system(cmd.c_str());
    if (ret) {
        if (removeTmp_) {
            FileSystem::removeFileOrDirectory(tmpDir);
        }
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    // Generate TCEGenInstrNames.inc
    cmd = tblgenCmd +
        " -gen-instr-enums" +
        " -o " + tmpDir + FileSystem::DIRECTORY_SEPARATOR +
        "TCEGenInstrNames.inc";

    ret = system(cmd.c_str());
    if (ret) {
        if (removeTmp_) {
            FileSystem::removeFileOrDirectory(tmpDir);
        }
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    // Generate TCEGenInstrInfo.inc
    cmd = tblgenCmd +
        " -gen-instr-desc" +
        " -o " + tmpDir + FileSystem::DIRECTORY_SEPARATOR +
        "TCEGenInstrInfo.inc";

    ret = system(cmd.c_str());
    if (ret) {
        if (removeTmp_) {
            FileSystem::removeFileOrDirectory(tmpDir);
        }
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    // Generate TCEGenDAGISel.inc
    cmd = tblgenCmd +
        " -gen-dag-isel" +
        " -o " + tmpDir + FileSystem::DIRECTORY_SEPARATOR +
        "TCEGenDAGISel.inc";

    ret = system(cmd.c_str());
    if (ret) { 
        if (removeTmp_) {
            FileSystem::removeFileOrDirectory(tmpDir);
        }
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    
    // Generate TCEGenCallingConv.inc
    cmd = tblgenCmd +
        " -gen-callingconv" +
        " -o " + tmpDir + FileSystem::DIRECTORY_SEPARATOR +
        "TCEGenCallingConv.inc";

    ret = system(cmd.c_str());
    if (ret) { 
        if (removeTmp_) {
            FileSystem::removeFileOrDirectory(tmpDir);
        }
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    ret = system(cmd.c_str());
    if (ret) {
        if (removeTmp_) {
            FileSystem::removeFileOrDirectory(tmpDir);
        }
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
        srcsPath + "TCETargetMachinePlugin.cc ";

    // Compile plugin to cache.
    // CXX and SHARED_CXX_FLAGS defined in tce_config.h
    cmd = std::string(CXX) +
        " -I" + tmpDir +
        pluginIncludeFlags +
        " " + SHARED_CXX_FLAGS +
        " " + LLVM_CPPFLAGS +
        " " + pluginSources +
        " -o " + pluginFileName;

    ret = system(cmd.c_str());
    if (ret) {
        if (removeTmp_) {
            FileSystem::removeFileOrDirectory(tmpDir);
        }
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
        if (removeTmp_) {
            FileSystem::removeFileOrDirectory(tmpDir);
        }
        std::string msg = std::string() +
            "Unable to load plugin file '" +
            pluginFileName + "'. Error: " + e.errorMessage();

        IOException ne(__FILE__, __LINE__, __func__, msg);
        throw ne;
    }

    if (removeTmp_) {
        FileSystem::removeFileOrDirectory(tmpDir);
    }
    return creator();
}

/**
 * Returns (hopefully) unique plugin filename for target architecture.
 *
 * The filename consist of a 32bit hash of the .adf xml data and the .adf data
 * length as a hex string. Also the TCE version string is part of the hash
 * to avoid problems with incompatible backend plugins between TCE revisions.
 * The filename is used for cached plugins.
 *
 * @param target Target architecture.
 * @return Filename for the target architecture.
 */
std::string
LLVMBackend::pluginFilename(const TTAMachine::Machine& target) {
    ADFSerializer serializer;
    std::string buffer;
    serializer.setDestinationString(buffer);
    serializer.writeMachine(target);

    boost::hash<std::string> string_hasher;
    size_t h = string_hasher(buffer);

    std::string fileName =
        (Conversion::toHexString(buffer.length())).substr(2);

    fileName += "_";
    fileName += (Conversion::toHexString(h)).substr(2);
    fileName += "-" + Application::TCEVersionString();
    fileName += PLUGIN_SUFFIX;

    return fileName;
}
