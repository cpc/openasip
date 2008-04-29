/**
 * @file LLVMTCECmdLineOptions.hh
 *
 * Declaration of LLVMTCECmdLineOptions class.
 *
 * @author Veli-Pekka J��skel�inen 2008 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */


#ifndef LLVM_TCE_CMD_LINE_OPTIONS_HH
#define LLVM_TCE_CMD_LINE_OPTIONS_HH

#include <string>
#include "CmdLineOptions.hh"
#include "config.h" // VERSION

/**
 * Command line options class for LLVMTCE compiler CLI.
 */
class LLVMTCECmdLineOptions : public CmdLineOptions {
public:
    LLVMTCECmdLineOptions();
    ~LLVMTCECmdLineOptions();

    bool isMachineFileDefined() const;
    std::string machineFile() const;

    bool isOutputFileDefined() const;
    std::string outputFile() const;

    bool isSchedulerConfigFileDefined() const;
    std::string schedulerConfigFile() const;

    bool isOptLevelDefined() const;
    int optLevel() const;

    bool isVerboseSwitchDefined() const;
    bool debugFlag() const;

    virtual void printVersion() const {
        std::cout << "llvmtce - TCE LLVM compiler " << VERSION
                  << std::endl;
    }

private:
    // Command line switches.
    static const std::string SWL_TARGET_MACHINE;
    static const std::string SWS_TARGET_MACHINE;
    static const std::string SWL_OUTPUT_FILE;
    static const std::string SWS_OUTPUT_FILE;
    static const std::string SWL_SCHEDULER_CONFIG;
    static const std::string SWS_SCHEDULER_CONFIG;
    static const std::string SWL_DEBUG_FLAG;
    static const std::string SWL_OPT_LEVEL;
    static const std::string SWS_OPT_LEVEL;
    /// Switch for verbose output listing scheduler modules
    static const std::string VERBOSE_SWITCH;

    static const std::string USAGE;

};

#endif
