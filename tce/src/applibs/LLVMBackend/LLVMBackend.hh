/*
    Copyright (c) 2002-2012 Tampere University.

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
 * @file LLVMBackend.hh
 *
 * Declaration of LLVMBackend class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2008 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2008-2012
 * @note rating: red
 */

#ifndef LLVM_TCE_HH
#define LLVM_TCE_HH

#include <string>
#include "Exception.hh"
#include "PluginTools.hh"
#include "OperationDAGSelector.hh"

namespace TTAProgram {
    class Program;
}

namespace TTAMachine {
    class Machine;
}

namespace llvm {
    class Module;
    class TCETargetMachinePlugin;
}

class InterPassData;
class SchedulingPlan;
class LLVMTCECmdLineOptions;

/**
 * LLVM compiler backend and compiler interface for TCE.
 */
class LLVMBackend {
public:
    static OperationDAGSelector::OperationSet 
    llvmRequiredOpset(bool includeFloatOps, bool isLittleEndian);

    LLVMBackend(bool useInstalledVersion, TCEString tempDir);
    virtual ~LLVMBackend();

    TTAProgram::Program* compile(
        const std::string& bytecodeFile,
        const std::string& emulationBytecodeFile, TTAMachine::Machine& target,
        int optLevel, bool debug = false, InterPassData* ipData = NULL);

    TTAProgram::Program* compile(
        llvm::Module& module, llvm::Module* emulationModule,
        llvm::TCETargetMachinePlugin& plugin, TTAMachine::Machine& target,
        int optLevel, bool debug = false, InterPassData* ipData = NULL);

    TTAProgram::Program* schedule(
        const std::string& bytecodeFile,
        const std::string& emulationBytecodeFile, TTAMachine::Machine& target,
        int optLevel = 2, bool debug = false, SchedulingPlan* plan = NULL);

    llvm::TCETargetMachinePlugin* createPlugin(
        const TTAMachine::Machine& target);

private:
    std::string pluginFilename(const TTAMachine::Machine& target);

    /// Assume we are running an installed TCE version.
    bool useInstalledVersion_;

    /// Plugin tool for loading target machine plugin.
    PluginTools pluginTool_;
    /// Path to the cache where precompiled plugins are stored.
    TCEString cachePath_;
    /// Directory to store temporary files.
    TCEString tempDir_;

    LLVMTCECmdLineOptions* options_;

    InterPassData* ipData_;

    static const std::string TBLGEN_INCLUDES;
    static const std::string PLUGIN_PREFIX;
    static const std::string PLUGIN_SUFFIX;
    static const TCEString CXX0X_FLAG;
    static const TCEString CXX11_FLAG;
    static const TCEString CXX14_FLAG;
};

#endif
