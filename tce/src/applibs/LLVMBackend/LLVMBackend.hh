/**
 * @file LLVMBackend.hh
 *
 * Declaration of LLVMBackend class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2008 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef LLVM_TCE_HH
#define LLVM_TCE_HH

#include <string>
#include "Exception.hh"
#include "PluginTools.hh"

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

/**
 * LLVM compiler backend and compiler interface for TCE.
 */
class LLVMBackend {
public:

    LLVMBackend(bool useCache = true);
    virtual ~LLVMBackend();

    TTAProgram::Program* compile(
        const std::string& bytecodeFile,
        TTAMachine::Machine& target,
	int optLevel,
        bool debug = false)
        throw (Exception);

    TTAProgram::Program* compile(
        llvm::Module& module,
        llvm::TCETargetMachinePlugin& plugin,
        TTAMachine::Machine& target,
        int optLevel,
        bool debug = false)
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

    static const std::string TCEPLUGINGEN_BIN;
    static const std::string TBLGEN_BIN;
    static const std::string TBLGEN_INCLUDES;
    static const std::string LLVM_CXX_FLAGS;
    static const std::string LLVM_LD_FLAGS;
    static const std::string LLVM_INCLUDE_DIR;

    static const std::string PLUGIN_PREFIX;
    static const std::string PLUGIN_SUFFIX;
};

#endif
