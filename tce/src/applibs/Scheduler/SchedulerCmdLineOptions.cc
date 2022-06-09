/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file SchedulerCmdLineOptions.cc
 *
 * Implementation of SchedulerCmdLineOptions class.
 *
 * @author Ari Metsï¿½halme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2010
 * @note rating: red
 */

#include <iostream>
#include "SchedulerCmdLineOptions.hh"
#include "tce_config.h"

using std::cout;
using std::endl;

const std::string SchedulerCmdLineOptions::USAGE =
    "Usage: schedule [OPTION]... SOURCE\n"
    "Schedule SOURCE binary with the given options\n"
    "Example: schedule -a mach.adf hello.tpef";

const std::string SchedulerCmdLineOptions::SWL_TARGET_MACHINE = "adf";
const std::string SchedulerCmdLineOptions::SWS_TARGET_MACHINE = "a";
const std::string SchedulerCmdLineOptions::SWL_OUTPUT_FILE = "output";
const std::string SchedulerCmdLineOptions::SWS_OUTPUT_FILE = "o";
const std::string SchedulerCmdLineOptions::SWL_LOOP_FLAG = "loop";
const std::string SchedulerCmdLineOptions::SWL_SCHEDULING_WINDOW = 
    "scheduling-window-size";
const std::string SchedulerCmdLineOptions::SWL_IF_CONVERSION_THRESHOLD = 
    "if-conversion-threshold";
const std::string SchedulerCmdLineOptions::SWL_LOWMEM_MODE_THRESHOLD = 
    "lowmem-mode-threshold";
const std::string SchedulerCmdLineOptions::SWL_RESTRICTED_AA = "restricted-aa";
const std::string SchedulerCmdLineOptions::SWL_STACK_AA = "stack-aa";
const std::string SchedulerCmdLineOptions::SWL_OFFSET_AA = "offset-aa";
const std::string SchedulerCmdLineOptions::SWL_RENAME_REGISTERS = "rename-registers";
const std::string SchedulerCmdLineOptions::SWL_RESOURCE_CONSTRAINT_PRINTING = 
    "print-resource-constraints";

const std::string SchedulerCmdLineOptions::SWL_DUMP_IFCONVERSION_CFGS =
    "dump-ifconversion-cfgs";

const std::string SchedulerCmdLineOptions::SWL_KILL_DEAD_RESULTS = "kill-dead-results";
const std::string SchedulerCmdLineOptions::SWL_BYPASS_DISTANCE = "bypass-distance";
const std::string SchedulerCmdLineOptions::SWL_NO_DRE_BYPASS_DISTANCE = "bypass-distance-nodre";
const std::string SchedulerCmdLineOptions::SWL_OPERAND_SHARE_DISTANCE = "operand-share-distance";
const std::string SchedulerCmdLineOptions::SWL_NOALIAS_FUNCTIONS = "noalias-functions";

/**
 * Constructor.
 */
SchedulerCmdLineOptions::SchedulerCmdLineOptions(): CmdLineOptions(USAGE) {

    addOption(
        new StringCmdLineOptionParser(
            SWL_TARGET_MACHINE, "Target machine (.adf file)",
            SWS_TARGET_MACHINE));

    addOption(
        new StringCmdLineOptionParser(
            SWL_OUTPUT_FILE, "Output file name.",
            SWS_OUTPUT_FILE));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_LOOP_FLAG, "Use loop optimizations."));

    addOption(
        new IntegerCmdLineOptionParser(
            SWL_SCHEDULING_WINDOW, 
            "The scheduling window size in TTA instructions."));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_RESTRICTED_AA, 
            "Use restricted C language keyword in alias analyzis."));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_STACK_AA, "Enable stack alias analyzer. On by default."));


    addOption(
        new BoolCmdLineOptionParser(
            SWL_RENAME_REGISTERS, 
            "Rename already allocated registers during scheduler"));

    addOption(
        new IntegerCmdLineOptionParser(
            SWL_IF_CONVERSION_THRESHOLD,
            "The maximum number of moves in an if-structure to predicate "
            "when if-converting."));

    addOption(
        new IntegerCmdLineOptionParser(
            SWL_LOWMEM_MODE_THRESHOLD,
            "Move count of procedure which activates low-mem mode"
            "which saves memory from scheduler but "
            "disables some optimizations."));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_OFFSET_AA, "Enable constant offset alias analyzer. On by default."));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_RENAME_REGISTERS, 
            "Rename already allocated registers during scheduler. "
            "On by default."));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_RESOURCE_CONSTRAINT_PRINTING,
            "Print out the resource constraints that potentially limit the "
            "basic block's schedule."));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_DUMP_IFCONVERSION_CFGS,
            "Dumps the control flow graphs before and after if-conversion."));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_KILL_DEAD_RESULTS,
            "Kill dead results after bypass. On by default."));

    addOption(
        new IntegerCmdLineOptionParser(
            SWL_BYPASS_DISTANCE,
            "Bypass distance when dead result elimination can be used"));

    addOption(
        new IntegerCmdLineOptionParser(
            SWL_NO_DRE_BYPASS_DISTANCE,
            "Bypass distance when dead result elimination cannot be used"));

    addOption(
        new IntegerCmdLineOptionParser(
            SWL_OPERAND_SHARE_DISTANCE,
            "Operand sharing max distance"));

    addOption(
        new StringListCmdLineOptionParser(
            SWL_NOALIAS_FUNCTIONS,
            "List of functions which are scheduled with assumptions "
            "that there are no aliased memory accesses in these function "
            "This allows much more aggressive instruction scheduling, "
            "but will cause broken code if the code contains memory "
            "aliasing. Use with extreme caution!"));
}

/**
 * Destructor.
 */
SchedulerCmdLineOptions::~SchedulerCmdLineOptions() {
}

/**
 * Print the version of the user interface.
 */
void
SchedulerCmdLineOptions::printVersion() const {
    cout << "schedule - TCE Scheduler command line interface "
         <<  Application::TCEVersionString() << endl;
}

/**
 * Returns path to the target .adf file defined with the -a switch.
 *
 * @return Target adf file defined in the command line.
 */
std::string
SchedulerCmdLineOptions::machineFile() const {
    return findOption(SWS_TARGET_MACHINE)->String();
}

/**
 * Returns true if the target machine file is defined, false otherwise.
 *
 * @return True if the target machine file is defined.
 */
bool
SchedulerCmdLineOptions::isMachineFileDefined() const {
    return findOption(SWS_TARGET_MACHINE)->isDefined();
}

/**
 * Returns path to the output file defined with the -o switch.
 *
 * @return Output file name defined in the command line.
 */
std::string
SchedulerCmdLineOptions::outputFile() const {
    return findOption(SWS_OUTPUT_FILE)->String();
}

/**
 * Returns true if the output file name is defined, false otherwise.
 *
 * @return True if the output file name is defined.
 */
bool
SchedulerCmdLineOptions::isOutputFileDefined() const {
    return findOption(SWS_OUTPUT_FILE)->isDefined();
}

/**
 * Return true if the loop opt switch was defined in the command line.
 *
 * @return True if the loop opt switch was defined in the command line.
 */
bool
SchedulerCmdLineOptions::isLoopOptDefined() const {
    return findOption(SWL_LOOP_FLAG)->isDefined();
}

bool
SchedulerCmdLineOptions::useRestrictedAliasAnalyzer() const {
    return findOption(SWL_RESTRICTED_AA)->isDefined();
}

bool
SchedulerCmdLineOptions::enableStackAA() const {
    if (!optionGiven(SWL_STACK_AA)) {
        return true;
    } 
    return findOption(SWL_STACK_AA)->isFlagOn();
}

bool
SchedulerCmdLineOptions::enableOffsetAA() const {
    if (!optionGiven(SWL_OFFSET_AA)) {
        return true;
    } 
    return findOption(SWL_OFFSET_AA)->isFlagOn();
}

bool
SchedulerCmdLineOptions::renameRegisters() const {
    if (!optionGiven(SWL_RENAME_REGISTERS)) {
        return true;
    } 
    return findOption(SWL_RENAME_REGISTERS)->isFlagOn();
}

bool
SchedulerCmdLineOptions::printResourceConstraints() const {
    if (!optionGiven(SWL_RESOURCE_CONSTRAINT_PRINTING)) {
        return false;
    } 
    return findOption(SWL_RESOURCE_CONSTRAINT_PRINTING)->isFlagOn();
}

bool
SchedulerCmdLineOptions::dumpIfConversionCFGs() const {
    if (!optionGiven(SWL_DUMP_IFCONVERSION_CFGS)) {
        return false;
    } 
    return findOption(SWL_DUMP_IFCONVERSION_CFGS)->isFlagOn();
}

/**
 * Returns the maximum number of moves in an if-structure which is still
 * if-converted.
 *
 * By default returns -1 which results in using heuristics to define the
 * maximum values.
 */
int
SchedulerCmdLineOptions::ifConversionThreshold() const {
    if (!findOption(SWL_IF_CONVERSION_THRESHOLD)->isDefined()) {
        return -1;
    } else {
        return findOption(SWL_IF_CONVERSION_THRESHOLD)->integer();
    }
}

/**
 * Returns the maximum number of moves in an if-structure which is still
 * if-converted.
 *
 * By default returns -1 which results in using heuristics to define the
 * maximum values.
 */
int
SchedulerCmdLineOptions::lowMemModeThreshold() const {
    if (!findOption(SWL_LOWMEM_MODE_THRESHOLD)->isDefined()) {
        return -1;
    } else {
        return findOption(SWL_LOWMEM_MODE_THRESHOLD)->integer();
    }
}



/**
 * Returns the bypass limit when dead result elimination can be used.
 *
 * By default returns -1 which results in using heuristics to define the
 * maximum values.
 */
int
SchedulerCmdLineOptions::bypassDistance() const {
    if (!findOption(SWL_BYPASS_DISTANCE)->isDefined()) {
        return -1;
    } else {
        return findOption(SWL_BYPASS_DISTANCE)->integer();
    }
}

int
SchedulerCmdLineOptions::operandShareDistance() const {
    if (!findOption(SWL_OPERAND_SHARE_DISTANCE)->isDefined()) {
        return -1;
    } else {
        return findOption(SWL_OPERAND_SHARE_DISTANCE)->integer();
    }
}


/**
 * Returns the bypass limit when dead result elimination cannot be used.
 *
 * By default returns -1 which results in using heuristics to define the
 * maximum values.
 */
int
SchedulerCmdLineOptions::noDreBypassDistance() const {
    if (!findOption(SWL_NO_DRE_BYPASS_DISTANCE)->isDefined()) {
        return -1;
    } else {
        return findOption(SWL_NO_DRE_BYPASS_DISTANCE)->integer();
    }
}

bool
SchedulerCmdLineOptions::killDeadResults() const {
    if (!optionGiven(SWL_KILL_DEAD_RESULTS)) {
        return true;
    } 
    return findOption(SWL_KILL_DEAD_RESULTS)->isFlagOn();
}

/**
 * Returns the list of "noalias functions" that are scheduled
 * with assumption of no aliasing memory accesses.
 */
FunctionNameList*
SchedulerCmdLineOptions::noaliasFunctions() const {
    FunctionNameList* funcs = new FunctionNameList();
    CmdLineOptionParser* opt = findOption(SWL_NOALIAS_FUNCTIONS);
    if (opt == NULL)
        return funcs;
    for (int i = 1; i <= opt->listSize(); ++i) {
        funcs->insert(opt->String(i));
    }
    return funcs;
}
