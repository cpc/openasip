/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @author Ari Mets‰halme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2010
 * @note rating: red
 */

#include <iostream>
#include "SchedulerCmdLineOptions.hh"
#include "tce_config.h"

using std::cout;
using std::endl;

const std::string SchedulerCmdLineOptions::ADF_PARAM_NAME = "adf";
const std::string SchedulerCmdLineOptions::OUTPUT_PARAM_NAME = "output";
const std::string SchedulerCmdLineOptions::VERBOSE_SWITCH = "verbose";
const std::string SchedulerCmdLineOptions::SWL_LOOP_FLAG = "loop";
const std::string SchedulerCmdLineOptions::SWL_RENAME_REGISTERS = "rename-registers";
const std::string SchedulerCmdLineOptions::SWL_KILL_DEAD_RESULTS = "kill-dead-results";
const std::string SchedulerCmdLineOptions::SWL_BYPASS_DISTANCE = "bypass-distance";
const std::string SchedulerCmdLineOptions::SWL_NO_DRE_BYPASS_DISTANCE = "bypass-distance-nodre";
const std::string SchedulerCmdLineOptions::SWL_OPERAND_SHARE_DISTANCE = "operand-share-distance";


const std::string SchedulerCmdLineOptions::USAGE =
    "Usage: schedule [OPTION]... SOURCE\n"
    "Schedule SOURCE binary with the given options\n"
    "Example: schedule -a mach.adf hello.tpef";

static const std::string SWL_RESOURCE_CONSTRAINT_PRINTING = 
    "print-resource-constraints";

/**
 * Constructor.
 */
SchedulerCmdLineOptions::SchedulerCmdLineOptions(): CmdLineOptions(USAGE) {
    StringCmdLineOptionParser* adfFile = new StringCmdLineOptionParser(
        ADF_PARAM_NAME, "The ADF file of the target architecture", "a");
    addOption(adfFile);
    BoolCmdLineOptionParser* verboseSwitch = new BoolCmdLineOptionParser(
        VERBOSE_SWITCH, "The verbose switch", "v");
    addOption(verboseSwitch);
    StringCmdLineOptionParser* outputFile = new StringCmdLineOptionParser(
        OUTPUT_PARAM_NAME, "The output file", "o");
    addOption(outputFile);
    addOption(
        new BoolCmdLineOptionParser(
            SWL_RESOURCE_CONSTRAINT_PRINTING,
            "Print out the resource constraints that potentially limit the "
            "basic block's schedule."));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_LOOP_FLAG, "Use loop scheduler."));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_RENAME_REGISTERS, 
            "Rename already allocated registers during scheduler"));
   
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
}

/**
 * Destructor.
 */
SchedulerCmdLineOptions::~SchedulerCmdLineOptions() {
}

/**
 * Return true if the target ADF was defined in the command line.
 *
 * @return True if the target ADF was defined in the command line.
 */
bool
SchedulerCmdLineOptions::isTargetADFDefined() const {
    return findOption(ADF_PARAM_NAME)->isDefined();
}

/**
 * Return the name of the target ADF file.
 *
 * @return The name of the target ADF file.
 * @exception NotAvailable if the target ADF was not defined.
 */
std::string
SchedulerCmdLineOptions::targetADF() const {
    return findOption(ADF_PARAM_NAME)->String();
}

/**
 * Return true if the verbose switch was defined in the command line.
 *
 * @return True if the verbose switch was defined in the command line.
 */
bool
SchedulerCmdLineOptions::isVerboseSwitchDefined() const {
    return findOption(VERBOSE_SWITCH)->isDefined();
}

/**
 * Return true if the output file was defined in the command line.
 *
 * @return True if the output file was defined in the command line.
 */
bool
SchedulerCmdLineOptions::isOutputFileDefined() const {
    return findOption(OUTPUT_PARAM_NAME)->isDefined();
}

/**
 * Return the name of the output file.
 *
 * @return The name of the output file.
 * @exception NotAvailable if the output file was not defined.
 */
std::string
SchedulerCmdLineOptions::outputFile() const {
    return findOption(OUTPUT_PARAM_NAME)->String();
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
SchedulerCmdLineOptions::printResourceConstraints() const {
    if (!optionGiven(SWL_RESOURCE_CONSTRAINT_PRINTING)) {
        return false;
    } 
    return findOption(SWL_RESOURCE_CONSTRAINT_PRINTING)->isFlagOn();
}

/**
 * Print the version of the user interface.
 */
void
SchedulerCmdLineOptions::printVersion() const {
    cout << "schedule - TCE Scheduler command line interface "
         <<  Application::TCEVersionString() << endl;
}

bool
SchedulerCmdLineOptions::renameRegisters() const {
    if (!optionGiven(SWL_RENAME_REGISTERS)) {
        return true;
    } 
    return findOption(SWL_RENAME_REGISTERS)->isFlagOn();
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

/**
 * Returns the distance how far from operands can be shared,
 * set by --operand-share-distance parameter.
 */
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
