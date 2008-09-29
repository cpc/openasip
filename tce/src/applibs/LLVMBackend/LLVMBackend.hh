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
 * @file LLVMBackend.hh
 *
 * Declaration of LLVMBackend class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2008 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef LLVM_TCE_HH
#define LLVM_TCE_HH

#include <string>
#include "Exception.hh"
#include "PluginTools.hh"
#include "SchedulingPlan.hh"

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

/**
 * LLVM compiler backend and compiler interface for TCE.
 */
class LLVMBackend {
public:

    LLVMBackend(bool useCache = true, bool useInstalledVersion = false);
    virtual ~LLVMBackend();

    TTAProgram::Program* compile(
        const std::string& bytecodeFile,
        const std::string& emulationBytecodeFile,
        TTAMachine::Machine& target,
        int optLevel,
        bool debug = false,
        InterPassData* ipData = NULL)
        throw (Exception);

    TTAProgram::Program* compile(
        llvm::Module& module,
        llvm::Module* emulationModule,
        llvm::TCETargetMachinePlugin& plugin,
        TTAMachine::Machine& target,
        int optLevel,
        bool debug = false,
        InterPassData* ipData = NULL)
        throw (Exception);

    TTAProgram::Program* compileAndSchedule(
        const std::string& bytecodeFile,
        const TTAMachine::Machine& target,
        int optLevel = 2,
        const unsigned int debug = 0)
        throw (Exception);

    TTAProgram::Program* schedule(
        const std::string& bytecodeFile,
        const std::string& emulationBytecodeFile,
        TTAMachine::Machine& target,
        int optLevel = 2,
        bool debug = false,
        SchedulingPlan* plan = NULL)
        throw (Exception);

    llvm::TCETargetMachinePlugin* createPlugin(
        const TTAMachine::Machine& target)
        throw (Exception);

private:

    std::string pluginFilename(const TTAMachine::Machine& target);

    /// Plugin tool for loading target machine plugin.
    PluginTools pluginTool_;
    /// Path to the cache where precompiled plugins are stored.
    std::string cachePath_;

    bool useCache_;
    bool useInstalledVersion_;
    InterPassData* ipData_;

    static const std::string TCEPLUGINGEN_BIN;
    static const std::string TBLGEN_INCLUDES;
    static const std::string LLVM_CXX_FLAGS;
    static const std::string LLVM_LD_FLAGS;
    static const std::string LLVM_INCLUDE_DIR;

    static const std::string PLUGIN_PREFIX;
    static const std::string PLUGIN_SUFFIX;
};

#endif
