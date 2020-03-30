/*
    Copyright (c) 2002-2014 Tampere University.

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
 * @file LLVMTCECmdLineOptions.hh
 *
 * Implementation of LLVMTCECmdLineOptions class.
 *
 * @author Veli-Pekka Jääskeläinen 2008 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2008-2014
 * @note rating: red
 */

#include "LLVMTCECmdLineOptions.hh"
#include "Environment.hh"

const std::string LLVMTCECmdLineOptions::SWL_TARGET_MACHINE = "adf";
const std::string LLVMTCECmdLineOptions::SWS_TARGET_MACHINE = "a";
const std::string LLVMTCECmdLineOptions::SWL_OUTPUT_FILE = "output";
const std::string LLVMTCECmdLineOptions::SWS_OUTPUT_FILE = "o";
const std::string LLVMTCECmdLineOptions::SWL_EMULATION_LIB = "emulation";
const std::string LLVMTCECmdLineOptions::SWS_EMULATION_LIB = "e";
const std::string LLVMTCECmdLineOptions::SWL_DEBUG_FLAG = "debug";
const std::string LLVMTCECmdLineOptions::SWL_OPT_LEVEL = "optimize";
const std::string LLVMTCECmdLineOptions::SWS_OPT_LEVEL = "O";
const std::string LLVMTCECmdLineOptions::VERBOSE_SWITCH = "verbose";
const std::string LLVMTCECmdLineOptions::DISABLE_LLVMAA = "disable-llvmaa";
const std::string LLVMTCECmdLineOptions::DISABLE_DSF = "disable-dsf";
const std::string LLVMTCECmdLineOptions::CONSERVATIVE_PRE_RA_SCHEDULER= 
    "conservative-pre-ra-scheduler";

const std::string LLVMTCECmdLineOptions::SWL_DUMP_DDGS_DOT = "dump-ddgs-dot";
const std::string LLVMTCECmdLineOptions::SWL_DUMP_DDGS_XML = "dump-ddgs-xml";
const std::string LLVMTCECmdLineOptions::SWL_SAVE_BACKEND_PLUGIN = 
    "save-backend-plugin";
const std::string LLVMTCECmdLineOptions::SWL_BU_SCHEDULER = 
    "bottom-up-scheduler";
const std::string LLVMTCECmdLineOptions::SWL_TD_SCHEDULER = "td-scheduler";
const std::string LLVMTCECmdLineOptions::SWL_USE_OLD_BACKEND_SOURCES = 
    "use-old-backend-src";

const std::string LLVMTCECmdLineOptions::SWL_ANALYZE_INSTRUCTION_PATTERNS = 
    "analyze-instruction-patterns";

const std::string LLVMTCECmdLineOptions::SWL_TEMP_DIR = "temp-dir";

const std::string LLVMTCECmdLineOptions::SWL_ENABLE_VECTOR_BACKEND = 
    "vector-backend";
const std::string LLVMTCECmdLineOptions::SWL_WORK_ITEM_AA_FILE = "wi-aa-filename";
const std::string LLVMTCECmdLineOptions::SWL_BACKEND_CACHE_DIR = "backend-cache-dir";
const std::string LLVMTCECmdLineOptions::SWL_INIT_SP = "init-sp";

const std::string LLVMTCECmdLineOptions::USAGE =
    "Usage: llvmtce [OPTION]... BYTECODE\n"
    "Compile LLVM bytecode for target TCE architecture.\n";

/**
 * Constructor.
 */
LLVMTCECmdLineOptions::LLVMTCECmdLineOptions() {

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

    addOption(
        new BoolCmdLineOptionParser(
            DISABLE_LLVMAA, "Disable use of LLVM alias analysis.", 
            DISABLE_LLVMAA));

    addOption(
        new BoolCmdLineOptionParser(
            DISABLE_DSF, "Disable post-pass delay slot filler optimizer.",
            DISABLE_DSF));

    addOption(
        new BoolCmdLineOptionParser(
            CONSERVATIVE_PRE_RA_SCHEDULER, 
            "Conservative pre-ra-scheduler. May decrease register usage but "
            "limit ILP. good for machines with low amount of registers."));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_DUMP_DDGS_DOT, 
            "Dump DDGs in dot format before and after scheduling."));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_DUMP_DDGS_XML, 
            "Dump DDGs in XML format before and after scheduling."));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_SAVE_BACKEND_PLUGIN,
            "Save the generated backend plugin for the architecture. "
            "This avoid the regeneration of the backend plugin when calling "
            "tcecc for the same architecture."));
    addOption(
        new BoolCmdLineOptionParser(
            SWL_BU_SCHEDULER, 
            "Use an experiment Bottom Up scheduler."));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_TD_SCHEDULER,
            "Use the old top-down instruction scheduler(previous default)."));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_USE_OLD_BACKEND_SOURCES, 
            "Use the existing backend sources from the given temporary directory."));

    addOption(
        new StringCmdLineOptionParser(
            SWL_TEMP_DIR, 
            "The temporary directory to use for files needed during the code generation."));

    addOption(
        new StringCmdLineOptionParser(
            SWL_WORK_ITEM_AA_FILE, 
            "The filename with Work Item Alias Analysis - this is filled automatically "
            "if tcecc finds path to installed pocl."));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_ENABLE_VECTOR_BACKEND,
            "Enable backend support for vector registers split between multiple RF's"));

    addOption(
        new BoolCmdLineOptionParser(
            SWL_ANALYZE_INSTRUCTION_PATTERNS,
            "Analyze the instruction patterns after instruction selection."));

    addOption(
        new StringCmdLineOptionParser(
            SWL_BACKEND_CACHE_DIR,
            "The directory to use for caching LLVM backend plugins."));

    addOption(
        new UnsignedIntegerCmdLineOptionParser(
            SWL_INIT_SP,
            "Initialize the stack pointer of the program to the given value."));
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

bool
LLVMTCECmdLineOptions::disableLLVMAA() const {
    return findOption(DISABLE_LLVMAA)->isDefined();
}

bool
LLVMTCECmdLineOptions::disableDelaySlotFiller() const {
    return findOption(DISABLE_DSF)->isDefined();
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

bool
LLVMTCECmdLineOptions::conservativePreRAScheduler() const {
    if (findOption(CONSERVATIVE_PRE_RA_SCHEDULER)->isDefined() &&
        findOption(CONSERVATIVE_PRE_RA_SCHEDULER)->isFlagOn()) {
        return true;
    }
    return false;
}

bool
LLVMTCECmdLineOptions::dumpDDGsDot() const {
    return findOption(SWL_DUMP_DDGS_DOT)->isDefined();
}

bool
LLVMTCECmdLineOptions::dumpDDGsXML() const {
    return findOption(SWL_DUMP_DDGS_XML)->isDefined();
}

bool
LLVMTCECmdLineOptions::saveBackendPlugin() const {
    return !(findOption(SWL_SAVE_BACKEND_PLUGIN)->isDefined() &&
             !findOption(SWL_SAVE_BACKEND_PLUGIN)->isFlagOn());
}
bool
LLVMTCECmdLineOptions::useBUScheduler() const {
    return findOption(SWL_BU_SCHEDULER)->isDefined();
}

bool
LLVMTCECmdLineOptions::useTDScheduler() const {
    return findOption(SWL_TD_SCHEDULER)->isDefined();
}

bool
LLVMTCECmdLineOptions::useOldBackendSources() const {
    return findOption(SWL_USE_OLD_BACKEND_SOURCES)->isDefined();
}

TCEString
LLVMTCECmdLineOptions::tempDir() const {
    return findOption(SWL_TEMP_DIR)->String();
}

bool
LLVMTCECmdLineOptions::useVectorBackend() const {
    return findOption(SWL_ENABLE_VECTOR_BACKEND)->isDefined();
}

bool
LLVMTCECmdLineOptions::isWorkItemAAFileDefined() const {
    return findOption(SWL_WORK_ITEM_AA_FILE)->isDefined();
}

std::string
LLVMTCECmdLineOptions::workItemAAFile() const {
    return findOption(SWL_WORK_ITEM_AA_FILE)->String();
}

bool
LLVMTCECmdLineOptions::analyzeInstructionPatterns() const {
    return findOption(SWL_ANALYZE_INSTRUCTION_PATTERNS)->isDefined();
}

std::string
LLVMTCECmdLineOptions::backendCacheDir() const {
    if (findOption(SWL_BACKEND_CACHE_DIR)->isDefined())
        return findOption(SWL_BACKEND_CACHE_DIR)->String();
    return Environment::llvmtceCachePath();
}

bool
LLVMTCECmdLineOptions::isInitialStackPointerValueSet() const {
   return findOption(SWL_INIT_SP)->isDefined();
}

unsigned
LLVMTCECmdLineOptions::initialStackPointerValue() const {
    return findOption(SWL_INIT_SP)->unsignedInteger();
}
