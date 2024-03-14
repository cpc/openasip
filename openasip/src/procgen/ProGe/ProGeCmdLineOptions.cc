/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file ProGeCmdLineOptions.cc
 *
 * Implementation of ProGeCmdLineOptions class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <sstream>
#include "ProGeCmdLineOptions.hh"
#include "tce_config.h"

using std::string;
using std::cout;
using std::endl;

const string BEM_PARAM_NAME = "bem";
const string IDF_PARAM_NAME = "idf";
const string ICDECODER_PARAM_NAME = "gen";
const string HDL_PARAM_NAME = "hdl";
const string OUTPUTDIR_PARAM_NAME = "output";
const string SHARED_OUTPUTDIR_PARAM_NAME = "shared-files-dir";
const string PLUGIN_PARAMETERS_PARAM_NAME = "pluginparameters";
const string INTEGRATOR_NAME = "integrator";
const string IMEM_TYPE = "imem";
const string DMEM_TYPE = "dmem";
const string CLK_FREQUENCY = "clock-frequency";
const string TPEF_NAME = "program";
const string ENTITY_NAME = "entity-name";
const string USE_ABSOLUTE_PATHS = "absolute-paths";
const string LIST_INTEGRATORS = "list-integrators";
const string DEVICE_FAMILY = "device-family";
const string DEVICE_NAME = "device-name";
const string GENERATE_TESTBENCH = "generate-testbench";
const string SIMULATION_RUNTIME = "sim-runtime";
const string FORCE_OUTPUT = "force-output";
const string FU_ICGATE_LIST = "fu-ic-gate";
const string SYNC_RESET = "sync-reset";
const string HDB_LIST = "hdb-list";
const string ICD_ARG_LIST = "icd-arg-list";
const string PREFER_GEN = "prefer-generation";
const string RF_ICGATE_LIST = "rf-ic-gate";
const string DONT_RESET_ALL = "dont-reset-all";
const string FU_BACKREGISTER_LIST = "fu-back-register";
const string FU_FRONTREGISTER_LIST = "fu-front-register";
const string FU_MIDDLEREGISTER_LIST = "fu-middle-register";
const string DONT_CARE_INIT = "dont-care-init";

/**
 * The constructor.
 */
ProGeCmdLineOptions::ProGeCmdLineOptions() : 
    CmdLineOptions("") {

    StringCmdLineOptionParser* bemFile = new StringCmdLineOptionParser(
        BEM_PARAM_NAME, "The BEM file", "b");
    addOption(bemFile);

    StringCmdLineOptionParser* idfFile = new StringCmdLineOptionParser(
        IDF_PARAM_NAME, "The IDF file", "i");
    addOption(idfFile);

    StringCmdLineOptionParser* hdlParam = new StringCmdLineOptionParser(
        HDL_PARAM_NAME, "The HDL to generate. 'vhdl' = VHDL", "l");
    addOption(hdlParam);

    StringCmdLineOptionParser* outputDirectory = 
        new StringCmdLineOptionParser(
            OUTPUTDIR_PARAM_NAME, "The output directory", "o");
    addOption(outputDirectory);

    StringCmdLineOptionParser* sharedOutputDirectory = 
        new StringCmdLineOptionParser(
            SHARED_OUTPUTDIR_PARAM_NAME, 
            "The directory for HDL files that are potentially shared between "
            "multiple generated processors.", "s");
    addOption(sharedOutputDirectory);

    StringCmdLineOptionParser* pluginParameters = 
        new StringCmdLineOptionParser(
            PLUGIN_PARAMETERS_PARAM_NAME, "List plugin parameters for an "
            "IC/Decoder generator plugin file.", "u");
    addOption(pluginParameters);

    StringCmdLineOptionParser* integratorName =
        new StringCmdLineOptionParser(
            INTEGRATOR_NAME, "Select the target for platform integration.",
            "g");
    addOption(integratorName);

    StringCmdLineOptionParser* imemType = new StringCmdLineOptionParser(
        IMEM_TYPE,
        "Instruction memory type. Available types depends on "
        "the platform integrator. Types are 'vhdl_array', 'onchip', "
        "'sram', 'dram' and 'none'.",
        "f");
    addOption(imemType);

    StringCmdLineOptionParser* dmemType =
        new StringCmdLineOptionParser(
            DMEM_TYPE, "Data memory type. Available types depends on the "
            "platform integrator. Types are 'vhdl_array', 'onchip', 'sram',"
            " 'dram' and 'none'", "d");
    addOption(dmemType);

    IntegerCmdLineOptionParser* fmax =
        new IntegerCmdLineOptionParser(
            CLK_FREQUENCY, "Defines the target clock frequency.", "c");
    addOption(fmax);

    StringCmdLineOptionParser* programName =
        new StringCmdLineOptionParser(
            TPEF_NAME, "Name of tpef program.", "p");
    addOption(programName);

    StringCmdLineOptionParser* entityName = new StringCmdLineOptionParser(
        ENTITY_NAME,
        "String to use to make the generated VHDL entities unique. This "
        "is also used in the name of the top level entity platform "
        "integrator creates. Default is 'tta0' for the core, thus "
        "'tta0_toplevel' for the platform integrator top level "
        "component.",
        "e");
    addOption(entityName);

    BoolCmdLineOptionParser* useAbsolutePaths = 
        new BoolCmdLineOptionParser(
            USE_ABSOLUTE_PATHS, "Use absolute paths in generated platform "
            "integrator files.", "a");
    addOption(useAbsolutePaths);
    
    BoolCmdLineOptionParser* generateTestbench = 
        new BoolCmdLineOptionParser(
            GENERATE_TESTBENCH, "Generate testbench.", "t");
    addOption(generateTestbench);

    BoolCmdLineOptionParser* listIntegrators = 
        new BoolCmdLineOptionParser(
            LIST_INTEGRATORS, "List available integrators and information "
            "about them.", "n");
    addOption(listIntegrators);

    StringCmdLineOptionParser* deviceFamilyName = 
        new StringCmdLineOptionParser(
            DEVICE_FAMILY, 
            "Set FPGA device family for integration. Stand-alone integrators "
            "may ignore this parameter. Example: \"Stratix II\" or "
            "Stratix\\ II", "m");
    addOption(deviceFamilyName);

    StringCmdLineOptionParser* deviceName = new StringCmdLineOptionParser(
        DEVICE_NAME,
        "Set FPGA device family for integration. Stand-alone integrators "
        "may ignore this parameter. Example: \"xc7z020clg400-1\"");
    addOption(deviceName);

    IntegerCmdLineOptionParser* simTime = new IntegerCmdLineOptionParser(
        SIMULATION_RUNTIME,
        "The runtime of the simulation in nanoseconds. Default: 52390 ns",
        "r");
    addOption(simTime);

    BoolCmdLineOptionParser* forceOutput = new BoolCmdLineOptionParser(
        FORCE_OUTPUT, "Forces output writing into existing directory.", "F");
    addOption(forceOutput);

    BoolCmdLineOptionParser* syncReset = new BoolCmdLineOptionParser(
        SYNC_RESET, "Generate Synchronous reset (default async).");
    addOption(syncReset);

    StringCmdLineOptionParser* hdbList = new StringCmdLineOptionParser(
        HDB_LIST, "Comma separated list of HDBs for automated generation.",
        "h");
    addOption(hdbList);

    StringCmdLineOptionParser* icdArgList = new StringCmdLineOptionParser(
        ICD_ARG_LIST,
        "Comma separated list of IC decoder plugin arguments "
        "for automated generation.");
    addOption(icdArgList);

    BoolCmdLineOptionParser* preferGen = new BoolCmdLineOptionParser(
        PREFER_GEN,
        "Prefer HDL generation over existing HDB implementations.");
    addOption(preferGen);

    BoolCmdLineOptionParser* resetAll = new BoolCmdLineOptionParser(
        DONT_RESET_ALL,
        "Doesn't reset unnecessary registers (default false).");
    addOption(resetAll);

    StringCmdLineOptionParser* rfIcGateList = new StringCmdLineOptionParser(
        RF_ICGATE_LIST, "Comma separated list of RFs to IC-Gate.");
    addOption(rfIcGateList);

    StringCmdLineOptionParser* fuIcGateList = new StringCmdLineOptionParser(
        FU_ICGATE_LIST, "Comma separated list of FUs to IC-Gate.");
    addOption(fuIcGateList);

    StringCmdLineOptionParser* fuBackRegList = new StringCmdLineOptionParser(
        FU_BACKREGISTER_LIST,
        "Comma separated list of FUs to back-register.");
    addOption(fuBackRegList);

    StringCmdLineOptionParser* fuFrontRegList = new StringCmdLineOptionParser(
        FU_FRONTREGISTER_LIST,
        "Comma separated list of FUs to front-register.");
    addOption(fuFrontRegList);

    StringCmdLineOptionParser* fuMiddleRegList =
        new StringCmdLineOptionParser(
            FU_MIDDLEREGISTER_LIST,
            "Comma separated list of FUs to middle-register.");
    addOption(fuMiddleRegList);

    BoolCmdLineOptionParser* dontCareInit = new BoolCmdLineOptionParser(
        DONT_CARE_INIT,
        "Initialize FUGen generated signals as don't care. E.g. some FPGA tool optimizations prefer these.");
    addOption(dontCareInit);

}


/**
 * The destructor.
 */
ProGeCmdLineOptions::~ProGeCmdLineOptions() {
}


/**
 * Returns the ADF or PCF file given as last argument.
 *
 * @return The name of the file.
 */
std::string
ProGeCmdLineOptions::processorToGenerate() const {
    return argument(numberOfArguments());
}


/**
 * Returns the given BEM file.
 *
 * @return The name of the file.
 */
std::string
ProGeCmdLineOptions::bemFile() const {
    return findOption(BEM_PARAM_NAME)->String();
}


/**
 * Returns the given IDF file.
 *
 * @return The name of the file.
 */
std::string
ProGeCmdLineOptions::idfFile() const {
    return findOption(IDF_PARAM_NAME)->String();
}


/**
 * Returns the given HDL parameter.
 *
 * @return The HDL parameter.
 */
std::string
ProGeCmdLineOptions::hdl() const {
    return findOption(HDL_PARAM_NAME)->String();
}


/**
 * Returns the given output directory.
 *
 * @return The given output directory.
 */
std::string
ProGeCmdLineOptions::outputDirectory() const {
    return findOption(OUTPUTDIR_PARAM_NAME)->String();
}

/**
 * Returns the given output directory.
 *
 * @return The given output directory.
 */
std::string
ProGeCmdLineOptions::sharedOutputDirectory() const {
    return findOption(SHARED_OUTPUTDIR_PARAM_NAME)->String();
}


/**
 * Returns the IC/decoder generator plugin parameter list query.
 *
 * @return The plugin parameter query.
 */
std::string
ProGeCmdLineOptions::pluginParametersQuery() const {
    return findOption(PLUGIN_PARAMETERS_PARAM_NAME)->String();
}


std::string
ProGeCmdLineOptions::integratorName() const {
    return findOption(INTEGRATOR_NAME)->String();
}


std::string
ProGeCmdLineOptions::imemType() const {
    return findOption(IMEM_TYPE)->String();
}


std::string
ProGeCmdLineOptions::dmemType() const {
    return findOption(DMEM_TYPE)->String();
}


int
ProGeCmdLineOptions::clockFrequency() const {

    int freq = 0;
    if (findOption(CLK_FREQUENCY)->isDefined()) {
        freq = findOption(CLK_FREQUENCY)->integer();
    }
    return freq;
}


std::string
ProGeCmdLineOptions::tpefName() const {
    return findOption(TPEF_NAME)->String();
}


std::string
ProGeCmdLineOptions::entityName() const {
    return findOption(ENTITY_NAME)->String();
}

bool
ProGeCmdLineOptions::useAbsolutePaths() const {
    return findOption(USE_ABSOLUTE_PATHS)->isFlagOn();
}

bool
ProGeCmdLineOptions::generateTestbench() const {
    return findOption(GENERATE_TESTBENCH)->isFlagOn();
}

bool
ProGeCmdLineOptions::listAvailableIntegrators() const {
    return findOption(LIST_INTEGRATORS)->isFlagOn();
}

std::string
ProGeCmdLineOptions::deviceFamilyName() const {
    string deviceFamily = "";
    if (findOption(DEVICE_FAMILY)->isDefined()) {
        deviceFamily = findOption(DEVICE_FAMILY)->String();
    }
    return deviceFamily;
}

std::string
ProGeCmdLineOptions::deviceName() const {
    string devicePart = "";
    if (findOption(DEVICE_NAME)->isDefined()) {
        devicePart = findOption(DEVICE_NAME)->String();
    }
    return devicePart;
}

/**
 * Gets the HDL simulation time from Cmd line options. If none given, the
 * legacy MAGICAL RUNTIME CONSTANT value 52390ns will be used.
 */
std::string
ProGeCmdLineOptions::simulationRuntime() const {
    int simTime = 52390;
    if (findOption(SIMULATION_RUNTIME)->isDefined()) {
        simTime = findOption(SIMULATION_RUNTIME)->integer();
    }
    std::ostringstream s;
    s << simTime;

    return s.str();
}

/**
 * Returns true if ProGe is allowed to write the processor files to an
 * existing directory.
 */
bool
ProGeCmdLineOptions::forceOutputDirectory() const {
    return findOption(FORCE_OUTPUT)->isFlagOn();
}

/**
 * Returns true if synchronous reset.
 */
bool
ProGeCmdLineOptions::syncReset() const {
    return findOption(SYNC_RESET)->isFlagOn();
}

/**
 * Returns true if asynchronous reset.
 */
bool
ProGeCmdLineOptions::asyncReset() const {
    return !findOption(SYNC_RESET)->isFlagOn();
}

/**
 * Helper for arguments with comma-separated arguments
 */
std::vector<std::string>
ProGeCmdLineOptions::commaSeparatedList(
    const std::string argumentName) const {
    std::vector<std::string> list;
    std::string str;
    if (findOption(argumentName)->isDefined()) {
        str = findOption(argumentName)->String();
    }
    std::stringstream ss(str);
    while (ss.good()) {
        std::string sub;
        std::getline(ss, sub, ',');
        if (sub.size() > 1) {
            list.emplace_back(sub);
        }
    }
    return list;
}

/**
 * Return list of HDBs.
 */
std::vector<std::string>
ProGeCmdLineOptions::hdbList() const {
    return commaSeparatedList(HDB_LIST);
}

/**
 * Return list of RFs to IC-Gate.
 */
std::vector<std::string>
ProGeCmdLineOptions::rfIcGateList() const {
    return commaSeparatedList(RF_ICGATE_LIST);
}

/**
 * Return list of option/argument pairs for ICDecoder plugin
 */
std::vector<std::pair<std::string, std::string>>
ProGeCmdLineOptions::icdArgList() const {
    auto raw_pairs = commaSeparatedList(ICD_ARG_LIST);
    std::vector<std::pair<std::string, std::string>> parsed_pairs;

    for (std::string key_val_pair : raw_pairs) {
        if (key_val_pair.size() > 1) {
            std::stringstream kvp(key_val_pair);
            std::string key;
            std::string value;
            std::getline(kvp, key, ':');
            std::getline(kvp, value, ':');
            parsed_pairs.emplace_back(key, value);
        }
    }
    return parsed_pairs;
}

/**
 * Return list of FUs to IC-Gate.
 */
std::vector<std::string>
ProGeCmdLineOptions::fuIcGateList() const {
    return commaSeparatedList(FU_ICGATE_LIST);
}

std::vector<std::string>
ProGeCmdLineOptions::fuBackRegistered() const {
    return commaSeparatedList(FU_BACKREGISTER_LIST);
}

std::vector<std::string>
ProGeCmdLineOptions::fuFrontRegistered() const {
    return commaSeparatedList(FU_FRONTREGISTER_LIST);
}

std::vector<std::string>
ProGeCmdLineOptions::fuMiddleRegistered() const {
    return commaSeparatedList(FU_MIDDLEREGISTER_LIST);
}

/**
 * Returns true if preferring HDL Generation.
 */
bool
ProGeCmdLineOptions::preferHDLGeneration() const {
    return findOption(PREFER_GEN)->isFlagOn();
}

/**
 * Returns true if all registers should be reseted.
 */
bool
ProGeCmdLineOptions::resetAllRegisters() const {
    return !findOption(DONT_RESET_ALL)->isFlagOn();
}

/**
 * Returns true if HDL Generated signals should initialize as don't care.
 */
bool
ProGeCmdLineOptions::dontCareInitialization() const {
    return findOption(DONT_CARE_INIT)->isFlagOn();
}


/**
 * Prints the version of the application.
 */
void
ProGeCmdLineOptions::printVersion() const {
    std::cout << "generateprocessor - OpenASIP Processor Generator "
              << Application::TCEVersionString() << std::endl;
}


/**
 * Prints help of the application.
 */
void
ProGeCmdLineOptions::printHelp() const {
    printVersion();
    cout << "Usage: generateprocessor [options] <processor>" << endl
         << "where <processor> means either an ADF or PCF file." << endl; 
    CmdLineOptions::printHelp();
}
