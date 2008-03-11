/**
 * @file LLVMTCECmdLineOptions.hh
 *
 * Implementation of LLVMTCECmdLineOptions class.
 *
 * @author Veli-Pekka Jääskeläinen 2008 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "LLVMTCECmdLineOptions.hh"

const std::string LLVMTCECmdLineOptions::SWL_TARGET_MACHINE = "adf";
const std::string LLVMTCECmdLineOptions::SWS_TARGET_MACHINE = "a";
const std::string LLVMTCECmdLineOptions::SWL_OUTPUT_FILE = "output";
const std::string LLVMTCECmdLineOptions::SWS_OUTPUT_FILE = "o";
const std::string LLVMTCECmdLineOptions::SWL_SCHEDULER_CONFIG = "config";
const std::string LLVMTCECmdLineOptions::SWS_SCHEDULER_CONFIG = "c";

const std::string LLVMTCECmdLineOptions::USAGE =
    "Usage: llvmtce [OPTION]... BYTECODE\n"
    "Compile LLVM bytecode for target TCE architecture.\n";

/**
 * Constructor.
 */
LLVMTCECmdLineOptions::LLVMTCECmdLineOptions() :
    CmdLineOptions(USAGE) {

    addOption(
        new StringCmdLineOptionParser(
            SWL_TARGET_MACHINE, "Target machine (.adf file)",
            SWS_TARGET_MACHINE));

    addOption(
        new StringCmdLineOptionParser(
            SWL_OUTPUT_FILE, "Output file name.",
            SWS_OUTPUT_FILE));

    addOption(
        new StringCmdLineOptionParser(
            SWL_SCHEDULER_CONFIG, "Scheduler configuration file.",
            SWS_SCHEDULER_CONFIG));

}

/**
 * Destructor.
 */
LLVMTCECmdLineOptions::~LLVMTCECmdLineOptions() {
}


/**
 * Returns path to the target .adf file defined with the -a switch.
 *
 * @return Target adf file defined in the command line.
 */
std::string
LLVMTCECmdLineOptions::machineFile() const {
    return findOption(SWS_TARGET_MACHINE)->String();
}

/**
 * Returns true if the target machine file is defined, false otherwise.
 *
 * @return True if the target machine file is defined.
 */
bool
LLVMTCECmdLineOptions::isMachineFileDefined() const {
    return findOption(SWS_TARGET_MACHINE)->isDefined();
}


/**
 * Returns scheduler configuration file defined with the -c switch.
 *
 * @return Scheduler configuration file defined in the command line.
 */
std::string
LLVMTCECmdLineOptions::schedulerConfigFile() const {
    return findOption(SWS_SCHEDULER_CONFIG)->String();
}

/**
 * Returns true if the scheduler config file is defined, false otherwise.
 *
 * @return True if the scheduler config file is defined.
 */
bool
LLVMTCECmdLineOptions::isSchedulerConfigFileDefined() const {
    return findOption(SWS_SCHEDULER_CONFIG)->isDefined();
}


/**
 * Returns path to the output file defined with the -o switch.
 *
 * @return Output file name defined in the command line.
 */
std::string
LLVMTCECmdLineOptions::outputFile() const {
    return findOption(SWS_OUTPUT_FILE)->String();
}

/**
 * Returns true if the output file name is defined, false otherwise.
 *
 * @return True if the output file name is defined.
 */
bool
LLVMTCECmdLineOptions::isOutputFileDefined() const {
    return findOption(SWS_OUTPUT_FILE)->isDefined();
}


