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
 * @file LLVMTCECmdLineOptions.hh
 *
 * Implementation of LLVMTCECmdLineOptions class.
 *
 * @author Veli-Pekka J��skel�inen 2008 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "LLVMTCECmdLineOptions.hh"

const std::string LLVMTCECmdLineOptions::SWL_TARGET_MACHINE = "adf";
const std::string LLVMTCECmdLineOptions::SWS_TARGET_MACHINE = "a";
const std::string LLVMTCECmdLineOptions::SWL_OUTPUT_FILE = "output";
const std::string LLVMTCECmdLineOptions::SWS_OUTPUT_FILE = "o";
const std::string LLVMTCECmdLineOptions::SWL_SCHEDULER_CONFIG = "config";
const std::string LLVMTCECmdLineOptions::SWS_SCHEDULER_CONFIG = "c";
const std::string LLVMTCECmdLineOptions::SWL_EMULATION_LIB = "emulation";
const std::string LLVMTCECmdLineOptions::SWS_EMULATION_LIB = "e";
const std::string LLVMTCECmdLineOptions::SWL_DEBUG_FLAG = "debug";
const std::string LLVMTCECmdLineOptions::SWL_OPT_LEVEL = "optimize";
const std::string LLVMTCECmdLineOptions::SWS_OPT_LEVEL = "O";
const std::string LLVMTCECmdLineOptions::VERBOSE_SWITCH = "verbose";

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

    addOption(
        new StringCmdLineOptionParser(
            SWL_EMULATION_LIB, "Emulation library bitcode file.",
            SWS_EMULATION_LIB));

    addOption(
        new IntegerCmdLineOptionParser(
            SWL_OPT_LEVEL, "Optimization level (0-2)",
            SWS_OPT_LEVEL));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_DEBUG_FLAG, "Print LLVM debug data."));

    addOption(
        new BoolCmdLineOptionParser(
            VERBOSE_SWITCH, "The verbose switch", "v"));

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

std::string
LLVMTCECmdLineOptions::standardEmulationLib() const {
    return findOption(SWS_EMULATION_LIB)->String();
}

bool
LLVMTCECmdLineOptions::isStandardEmulationLibDefined() const {
    return findOption(SWS_EMULATION_LIB)->isDefined();
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

/**
 * Returns true if optimization level switch was given.
 *
 * @return True, if optimization level was given on command line.
 */
bool
LLVMTCECmdLineOptions::isOptLevelDefined() const {
    return findOption(SWS_OPT_LEVEL)->isDefined();
}


/**
 * Returns optimization level defined with the -O switch.
 *
 * @return Optimization level.
 */
int
LLVMTCECmdLineOptions::optLevel() const {
    return findOption(SWS_OPT_LEVEL)->integer();
}


/**
 * Returns true if the debug flag option was given.
 *
 * @return True, if debug mode switch was given.
 */
bool
LLVMTCECmdLineOptions::debugFlag() const {

    if (findOption(SWL_DEBUG_FLAG)->isDefined() &&
        findOption(SWL_DEBUG_FLAG)->isFlagOn()) {

        return true;
    }
    return false;
}

/**
 * Return true if the verbose switch was defined in the command line.
 *
 * @return True if the verbose switch was defined in the command line.
 */
bool
LLVMTCECmdLineOptions::isVerboseSwitchDefined() const {
    return findOption(VERBOSE_SWITCH)->isDefined();
}
