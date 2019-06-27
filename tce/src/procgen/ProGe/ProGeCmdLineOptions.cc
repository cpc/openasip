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
const string GENERATE_TESTBENCH = "generate-testbench";


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

    StringCmdLineOptionParser* imemType =
        new StringCmdLineOptionParser(
            IMEM_TYPE, "Instruction memory type. Available types depends on "
            "the platform integrator. Types are 'vhdl_array', 'onchip', "
            "'sram' and 'dram'", "f");
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

    StringCmdLineOptionParser* entityName = 
        new StringCmdLineOptionParser(
            ENTITY_NAME, 
            "String to use to make the generated VHDL entities unique. This is "
            "also used in the name of the top level entity platform integrator "
            "creates. Default is 'tta0' for the core, thus 'tta0_toplevel' for "
            "the platform integrator top level component.", "e");
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


/**
 * Prints the version of the application.
 */
void
ProGeCmdLineOptions::printVersion() const {
    std::cout << "generateprocessor - TCE Processor Generator "
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
