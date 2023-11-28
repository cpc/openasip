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
 * @file GenerateProcessor.cc
 *
 * Implementation of GenerateProcessor class and the main program of 
 * "generateprocessor".
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @author Pekka Jaaskelainen 2011
 * @author Vinogradov Viacheslav(added Verilog generating) 2012 
 * @note rating: red
 */

#include <string>
#include <iostream>

#include "GenerateProcessor.hh"
#include "ProGeCmdLineOptions.hh"
#include "FileSystem.hh"
#include "Environment.hh"
#include "PluginTools.hh"
#include "ICDecoderGeneratorPlugin.hh"
#include "Machine.hh"
#include "BinaryEncoding.hh"
#include "Stratix2DSPBoardIntegrator.hh"
#include "Stratix3DevKitIntegrator.hh"
#include "KoskiIntegrator.hh"
#include "AvalonIntegrator.hh"
#include "AlmaIFIntegrator.hh"
#include "MemoryGenerator.hh"
#include "StringTools.hh"

using namespace ProGe;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

int const DEFAULT_IMEMWIDTH_IN_MAUS = 1;

/**
 * The main program of generateprocessor application.
 */
int main(int argc, char* argv[]) {
    GenerateProcessor ui;
    bool successful = ui.generateProcessor(argc, argv);
    if (successful) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}


/**
 * The constructor.
 */
GenerateProcessor::GenerateProcessor() {
}


/**
 * The destructor.
 */
GenerateProcessor::~GenerateProcessor() {
}


/**
 * Parses the command line arguments and generates the processor.
 *
 * @return True if the generation of the processor was succesful, otherwise
 *         false.
 */
bool
GenerateProcessor::generateProcessor(int argc, char* argv[]) {

    ProGeCmdLineOptions options;
    string entity = "";

    try {

        options.parse(argv, argc);
        ProGeOptions progeOptions(options);

        std::string pluginParamQuery = options.pluginParametersQuery();
        if (pluginParamQuery != "") {
            return listICDecPluginParameters(pluginParamQuery);
        }

        if (options.isVerboseSwitchDefined()) {
            Application::setVerboseLevel(
                Application::VERBOSE_LEVEL_INCREASED);
        }

        if (options.isVerboseSpamSwitchDefined()) {
            Application::setVerboseLevel(Application::VERBOSE_LEVEL_SPAM);
        }

        if (options.listAvailableIntegrators()) {
            listIntegrators();
            return true;
        }
        
        if (options.numberOfArguments() == 0) {
            options.printHelp();
            return false;
        }

        if (!validIntegratorParameters(options)) {
            options.printHelp();
            return false;
        }

        if (!options.forceOutputDirectory() &&
            FileSystem::fileExists(progeOptions.outputDirectory)) {
            cerr << "Error: Output directory " << progeOptions.outputDirectory
                 << " already exists." << endl;
            return false;
        }

        string processorDefinition = options.processorToGenerate();
        if (FileSystem::fileExtension(processorDefinition) == ".adf") {
            loadMachine(processorDefinition);
        } else if (FileSystem::fileExtension(processorDefinition) == 
                   ".pcf") {
            loadProcessorConfiguration(processorDefinition);
        } else {
            cerr << "Unknown file: " << processorDefinition 
                 << ". The given file must be either an ADF or PCF file." 
                 << endl;
            throw IllegalCommandLine(__FILE__, __LINE__, __func__);
        }
        
        string bem = options.bemFile();
        string idf = options.idfFile();
        string hdl = options.hdl();

        int imemWidthInMAUs = DEFAULT_IMEMWIDTH_IN_MAUS;
        if (machine_->isRISCVMachine()) {
            imemWidthInMAUs = 4;
        }

        if (bem != "") {
            loadBinaryEncoding(bem);
        }
        if (idf != "") {
            loadMachineImplementation(idf);
        }

        ProGeUI::generateProcessor(
            progeOptions, imemWidthInMAUs, std::cerr, std::cerr, std::cerr);
    } catch (ParserStopRequest const&) {
        return false;
    } catch (const IllegalCommandLine& exception) {
        cerr << exception.errorMessage() << endl;
        return false;
    } catch (const Exception& e) {
        cerr << e.errorMessage() << endl;
        cerr << "Exception thrown at: " << e.fileName() << ":" 
             << e.lineNum() << endl;
        cerr << "  message: " << e.errorMessage() << endl;
        return false;
    }

    ProGeOptions progeOptions(options);
    
    string integrator = options.integratorName();
    if (!integrator.empty()) {
        if (progeOptions.language == Verilog) {
            std::cerr << "Verilog is not yet supported by Platform Integrator"
                      << std::endl;
            return false;
        }

        string progeOutDir = progeOptions.outputDirectory;
        string sharedOutDir = progeOptions.sharedOutputDirectory;
        if (!options.useAbsolutePaths()) {
            string cwd = FileSystem::currentWorkingDir();
            FileSystem::relativeDir(cwd, progeOutDir);
            FileSystem::relativeDir(cwd, sharedOutDir);
        }

        string platformDir = progeOutDir + FileSystem::DIRECTORY_SEPARATOR +
        "platform";
        string program = 
            StringTools::chopString(options.tpefName(), ".tpef").at(0);
        MemType imem = string2MemType(options.imemType());
        MemType dmem = string2MemType(options.dmemType());
        int fmax = options.clockFrequency();
        string devFamily = options.deviceFamilyName();
        string devName = options.deviceName();
        bool syncReset = options.syncReset();

        try {
            ProGeUI::integrateProcessor(
                std::cout, std::cerr, progeOutDir, sharedOutDir, integrator,
                progeOptions.entityName, program, devFamily, devName, imem,
                dmem, progeOptions.language, fmax, syncReset,
                options.generateTestbench());
        } catch (const Exception& e) {
            std::cerr << "Processor integration failed: "
                      << e.procedureName() << ": "
                      << e.errorMessage() << endl;
            return false;
        }
    }

    if (options.generateTestbench()) {
        string testBenchDir = progeOptions.outputDirectory +
                              FileSystem::DIRECTORY_SEPARATOR + "tb";
        try {
            ProGeUI::generateTestBench(
                progeOptions.language, testBenchDir,
                progeOptions.outputDirectory);
        } catch (const Exception& e) {
            std::cerr << "Warning: Processor Generator failed to "
                      << "generate testbench." << std::endl;
            std::cerr << e.errorMessage() << std::endl;
        }

        try {
            ProGeUI::generateScripts(
                progeOptions.language, progeOptions.outputDirectory,
                progeOptions.outputDirectory,
                progeOptions.sharedOutputDirectory, testBenchDir,
                progeOptions.simulationRuntime);
        } catch (const Exception& e) {
            std::cerr << "Warning: Processor Generator failed to "
                      << "generate simulation/compilation scripts."
                      << std::endl;
            std::cerr << e.errorMessage() << std::endl;
        }
    }

    return true;
}

/** 
 * Generates the output directory name.
 *
 * @param options Proge command line options.
 * @param outputDir String where output directory name is to be stored. 
 */
void
GenerateProcessor::getOutputDir(
    const ProGeCmdLineOptions& options,
    std::string& outputDir) {

    outputDir = options.outputDirectory();

    if (outputDir == "") {
        outputDir = FileSystem::currentWorkingDir() + 
            FileSystem::DIRECTORY_SEPARATOR + "proge-output";
    } else {
        outputDir = FileSystem::expandTilde(outputDir);
        outputDir = FileSystem::absolutePathOf(outputDir);
    }
}

/**
 * Prints listing of IC/Decoder generator plugin parameters to stdout.
 *
 * @param pluginFile Full path to the plugin file.
 */
bool
GenerateProcessor::listICDecPluginParameters(
    const std::string& pluginFile) const {

    // An ugly way to determine the plugin name which should be the
    // file name minus the "Plugin.so" ending.
    string pluginName = FileSystem::fileOfPath(pluginFile);
    pluginName = FileSystem::fileNameBody(pluginFile);
    if (pluginName.length() < 6 ||
        pluginName.substr(pluginName.length() - 6) != "Plugin") {
        
        cerr << "Unable to determine plugin name. Plugin file must be named "
             << "'<plugin name>Plugin.so'." << endl;;
        return false;
    }

    pluginName = pluginName.substr(0, pluginName.length() - 6);

    // initialize plugin tool
    PluginTools pluginTool;
    std::vector<string> pluginPaths = Environment::icDecoderPluginPaths();
    for (std::vector<string>::const_iterator iter = pluginPaths.begin();
         iter != pluginPaths.end(); iter++) {
        try {
            pluginTool.addSearchPath(*iter);
        } catch (const FileNotFound&) {
        }
    }

    try {
        pluginTool.registerModule(pluginFile);
    } catch (const FileNotFound&) {
        cerr << "Plugin file '" << pluginFile << "' not found." << endl;
        return false;
    } catch (Exception& e) {
        cerr << "Error loading plugin file '" << pluginFile << "': "
             << e.errorMessage() << endl;

        return false;
    }

    
    ICDecoderGeneratorPlugin* (*creator)(
        TTAMachine::Machine&, BinaryEncoding&);

    ICDecoderGeneratorPlugin* plugin;

    TTAMachine::Machine machine;
    BinaryEncoding bem;

    try {
        pluginTool.importSymbol(
            "create_generator_plugin_" + pluginName, creator, pluginFile);

        plugin = creator(machine, bem);
    } catch (Exception& e) {
        cerr << "Error loading plugin '" << pluginName << "' from '"
             << pluginFile << "':" << endl;
        cerr << e.errorMessage() << endl;
        return false;
    }

    assert(plugin != NULL);

    cout << pluginName << ":" << endl;
    cout << plugin->pluginDescription() << endl << endl;
    cout << "Recognized parameters:" << endl
         << "----------------------" << endl;

    for (int i = 0; i < plugin->recognizedParameterCount(); i++) {
        std::string paramName = plugin->recognizedParameter(i);
        cout << paramName << endl;
        cout << "   " << plugin->parameterDescription(paramName) << endl;
        cout << endl;
    }
    cout << "----------------------" << endl;

    delete plugin;
    return true;
}


void
GenerateProcessor::listIntegrators() const {
    
    std::vector<PlatformIntegrator*> integrators;
    // append new integrators here
    integrators.push_back(new Stratix2DSPBoardIntegrator());
    integrators.push_back(new Stratix3DevKitIntegrator());
    integrators.push_back(new KoskiIntegrator());
    integrators.push_back(new AvalonIntegrator());
    integrators.push_back(new AlmaIFIntegrator());

    for (unsigned int i = 0; i < integrators.size(); i++) {
        integrators.at(i)->printInfo(std::cout);
        delete integrators.at(i);
    }
    std::cout << "Please refer to the user manual for more information on "
              << "Platform Integrators." << std::endl;
}


bool
GenerateProcessor::validIntegratorParameters(
    const ProGeCmdLineOptions& options) const {

    if (options.integratorName().empty()) {
        return true;
    }
    string entity = options.entityName();
    if (entity.empty()) {
        std::cerr << "Entity name must be given" << endl;
        return false;
    }
    string program = options.tpefName();
    if (program.empty()) {
        std::cerr
            << "Tpef is required for platform integration" << endl;
        return false;
    }
    if (!StringTools::endsWith(program, ".tpef")) {
        std::cerr << "Program does not have '.tpef' ending" << endl;
        return false;
    }

    string imem = options.imemType();
    if (imem.empty()) {
        std::cerr << "Instruction memory type is required for platform "
                  << "integration" << endl;
        return false;
    }
    string dmem = options.dmemType();
    if (dmem.empty()) {
        std::cerr << "Data memory type is required for platform integration"
                  << endl;
        return false;
    }
    if (string2MemType(imem) == UNKNOWN) {
        std::cerr
            << "Invalid instruction memory type " << imem << endl;
        return false;
    }
    if (string2MemType(dmem) == UNKNOWN) {
        std::cerr << "Invalid data memory type " << dmem << endl;
        return false;
    }
    return true;
}


MemType
GenerateProcessor::string2MemType(const std::string& memoryString) const {

    MemType memory = UNKNOWN;
    if (memoryString == "none") {
        memory = NONE;
    } else if (memoryString == "vhdl_array") {
        memory = VHDL_ARRAY;
    } else if (memoryString == "onchip") {
        memory = ONCHIP;
    } else if (memoryString == "sram") {
        memory = SRAM;
    } else if (memoryString == "dram") {
        memory = DRAM;
    }
    return memory;
}
