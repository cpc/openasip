/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file LLVMBackend.cc
 *
 * TCE compiler backend 
 *
 * @author Veli-Pekka J‰‰skel‰inen 2008 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <cstdlib> // system()
#include <boost/functional/hash.hpp>
#include <llvm/Module.h>
#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/CodeGen/Passes.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/ModuleProvider.h>
#include <llvm/Support/Debug.h>
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

#include <llvm/Target/TargetLowering.h>
volatile TargetLowering dummy(TargetMachine());

#include <llvm/Assembly/PrintModulePass.h>
Pass* createLowerMissingInstructionsPass(const TTAMachine::Machine& mach);
Pass* createLinkBitcodePass(Module& inputCode);
Pass* createMachineDCE();

// TODO: uncomment with llvm 2.4
// extern const PassInfo* UnreachableMachineBlockElimID;

const std::string LLVMBackend::TBLGEN_INCLUDES = "";
const std::string LLVMBackend::PLUGIN_PREFIX = "tcecc-";
const std::string LLVMBackend::PLUGIN_SUFFIX = ".so";
    
/**
 * Constructor.
 */
LLVMBackend::LLVMBackend(bool useCache, bool useInstalledVersion):
    useCache_(useCache), useInstalledVersion_(useInstalledVersion) {

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
        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    // Load bytecode file.
    std::string errorMessage;

    // TODO: refactor
    std::auto_ptr<Module> m;
    std::auto_ptr<MemoryBuffer> buffer(
        MemoryBuffer::getFileOrSTDIN(bytecodeFile, &errorMessage));
    if (buffer.get()) {
        m.reset(ParseBitcodeFile(buffer.get(), &errorMessage));
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
            emuM.reset(ParseBitcodeFile(emuBuffer.get(), &errorMessage));
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
        compile(*m.get(), emuM.get(), *plugin, target, optLevel, debug, ipData);

    delete plugin;
    plugin = NULL;

    return result;
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
    std::string fs = "";

    TCETargetMachine targetMachine(module, fs, plugin);

    llvm::FunctionPassManager fpm(new ExistingModuleProvider(&module));
    fpm.add(new TargetData(*targetMachine.getTargetData()));

    llvm::PassManager pm;
    pm.add(new TargetData(*targetMachine.getTargetData()));

    // TODO:
    // Loop strength reduction pass?
    // Garbage collection?
    // lower invoke/unwind

    // LOWER MISSING
    pm.add(createLowerMissingInstructionsPass(target));

    // DCE
    pm.add(createUnreachableBlockEliminationPass());

    // Global DCE
    pm.add(createGlobalDCEPass());

    // this should use scan data to link in needed emulation functions.
    // emulationModule will be useless after this (linker is not very gentle)..
    if (emulationModule != NULL) {
        pm.add(createLinkBitcodePass(*emulationModule));
    }

    // to allow machine dead basic block emlination...
    pm.add(createInternalizePass(true));

    // just for debugging... disable if needed... 
    // (TODO: check if pm automatically frees passes.. if not create stack object)
    // pm.add(new PrintModulePass());
    
    // Instruction selection.
    targetMachine.addInstSelector(fpm, fast);

    // Machine DCE pass. TODO: fix to following in llvm 2.4
    fpm.add(createMachineDCE());
    // fpm.add(UnreachableMachineBlockElimID->createPass());
    
    // Register allocation.
    fpm.add(createRegisterAllocator());

    // Insert prolog/epilog code.
    fpm.add(createPrologEpilogCodeInserter());
    //fpm.add(createBranchFoldingPass());
    fpm.add(createDebugLabelFoldingPass());

    LLVMPOMBuilder* pomBuilder = new LLVMPOMBuilder(targetMachine, &target);
    fpm.add(pomBuilder);

    // Module passes.
    pm.run(module);

    // Function passes.
    fpm.doInitialization();
    for (Module::iterator i = module.begin(), e = module.end(); i != e; ++i) {
        if (!i->isDeclaration()) {
            fpm.run(*i);
        }
    }
    fpm.doFinalization();

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
    TTAMachine::Machine& target,
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
        throw IOException(
            __FILE__, __LINE__, __func__, exception.errorMessage());
    } 

    // call tcecc to compile, link and schedule the program
    std::vector<std::string> tceccOutputLines;
    std::string debugParams = "";
    if (debug > 1) {
        debugParams = "-v ";
        if (debug > 2) {
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
        return NULL;    
    } 

    TTAProgram::Program* prog = NULL;
    try {
        prog = TTAProgram::Program::loadFromTPEF(tpef, target);
    } catch (const Exception& e) {
        IOException error(__FILE__, __LINE__,__func__, e.errorMessage());
        error.setCause(e);
        throw error;
    }
    FileSystem::removeFileOrDirectory(tmpDir);

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
    std::string pluginFileName;
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
    std::string srcsPath;
    std::string pluginIncludeFlags;
    if (useInstalledVersion_) {
        srcsPath = std::string(TCE_INSTALLATION_ROOT) +  DS + "include" + DS;
        pluginIncludeFlags = " -I" + srcsPath;
    } else {
        srcsPath = std::string(TCE_SRC_ROOT) + DS +
            "src" + DS + "applibs" + DS + "LLVMBackend" + DS + "plugin" + DS;

        pluginIncludeFlags =
            " -I" + srcsPath +
            " -I" + std::string(TCE_SRC_ROOT) + DS + " " +
            " -I" + std::string(TCE_SRC_ROOT) + DS +
            "src" + DS + "ext" + DS + "llvm" + DS + LLVM_VERSION + DS + "TableGen" + DS +
            "td" + DS + " -I" + std::string(TCE_SRC_ROOT) + DS +
            "src" + DS + "applibs" + DS + "LLVMBackend" + DS + " " +
            " -I" + std::string(TCE_INSTALLATION_ROOT) +  DS + "include" + DS;

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
            std::string msg = "Unable to load plugin file '" +
                pluginFileName + "'.";

            IOException ne(__FILE__, __LINE__, __func__, msg);
            ne.setCause(e);
            throw ne;
        }
    }

    // Create target instruciton and register definitions in .td files.
    TDGen plugingen(target);
    try {
        plugingen.generateBackend(tmpDir);
    } catch(Exception& e) {
        std::string msg =
            "Failed to build compiler plugin for target architecture.";

        CompileError ne(__FILE__, __LINE__, __func__, msg);
        ne.setCause(e);
        throw ne;
    }

    std::string tblgenbin;
    if (useInstalledVersion_) {
        tblgenbin = std::string(TCE_INSTALLATION_ROOT) + DS + "bin" + DS +
            "tblgen";
    } else {
        tblgenbin = std::string(TCE_SRC_ROOT) + DS +
            "src" + DS + "ext" + DS + "llvm" + DS + LLVM_VERSION + DS +
            "TableGen" + DS + "tblgen";
    }
       
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

        tblgenCmd = tblgenbin + " " + TBLGEN_INCLUDES +
            " -I/usr/include " +
            " -I" + tmpDir +
            " -I`llvm-config --includedir`";
    } else {
        tblgenCmd = tblgenbin + " " + TBLGEN_INCLUDES +
            " -I" + tmpDir + " -I" + LLVM_INCLUDEDIR;
    }

    tblgenCmd += pluginIncludeFlags;
    tblgenCmd += " " + tmpDir + FileSystem::DIRECTORY_SEPARATOR + "TCE.td";

    std::string cmd = tblgenCmd + " -gen-register-enums" +
        " -o " + tmpDir + FileSystem::DIRECTORY_SEPARATOR +
        "TCEGenRegisterNames.inc";

    int ret = system(cmd.c_str());
    if (ret) {
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
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    // Generate TCEGenAsmWriter.inc
    cmd = tblgenCmd +
        " -gen-asm-writer" +
        " -o " + tmpDir + FileSystem::DIRECTORY_SEPARATOR +
        "TCEGenAsmWriter.inc";

    ret = system(cmd.c_str());
    if (ret) {
        std::string msg = std::string() +
            "Failed to build compiler plugin for target architecture.\n" +
            "Failed command was: " + cmd;

        throw CompileError(__FILE__, __LINE__, __func__, msg);
    }

    std::string pluginSources =
        srcsPath + "TCERegisterInfo.cc " +
        srcsPath + "TCEInstrInfo.cc " +
        srcsPath + "TCETargetLowering.cc " +
        srcsPath + "TCEDAGToDAGISel.cc " +
        srcsPath + "TCEAsmPrinter.cc " +
        srcsPath + "TCETargetMachinePlugin.cc";

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
            pluginFileName + "'.";

        IOException ne(__FILE__, __LINE__, __func__, msg);
        throw ne;
    }

    FileSystem::removeFileOrDirectory(tmpDir);
    return creator();
}

/**
 * Returns (hopefully) unique plugin filename for target architecture.
 *
 * The filename consist of a 32bit hash of the .adf xml data and the .adf data
 * length as a hex string. The filename is used for cached plugins.
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
    fileName += PLUGIN_SUFFIX;

    return fileName;
}
