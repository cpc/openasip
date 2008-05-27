/**
 * @file GenerateProcessor.cc
 *
 * Implementation of GenerateProcessor class and the main program of 
 * "generateprocessor".
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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

using namespace ProGe;
using std::string;
using std::cerr;
using std::endl;
using std::cout;

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
    string outputDirectory = "";
    
    try {

        options.parse(argv, argc);
        
        getOutputDir(options, outputDirectory);
        
        if(FileSystem::fileExists(outputDirectory)) {
            cerr << "Error: Output directory " << outputDirectory
                 << " already exists." << endl;
            return false;
        }
        
        std::string pluginParamQuery = options.pluginParametersQuery();
        if (pluginParamQuery != "") {
            return listICDecPluginParameters(pluginParamQuery);
        }
        
        if (options.numberOfArguments() == 0) {
            options.printHelp();
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
        int imemWidthInMAUs = options.imemWidthInMAUs();

        if (bem != "") {
            loadBinaryEncoding(bem);
        }
        if (idf != "") {
            loadMachineImplementation(idf);
        }

        ProGe::HDL language;
        if (hdl == "vhdl" || hdl == "") {
            language = ProGe::VHDL;
        } else {
            cerr << "Unknown HDL given: " << hdl << endl;
            return false;
        }
        
        ProGeUI::generateProcessor(
            imemWidthInMAUs, language, outputDirectory, std::cout);
    } catch (ParserStopRequest) {
        return false;
    } catch (const Exception& e) {
        cerr << e.errorMessage() << endl;
        cerr << "Exception thrown at: " << e.fileName() << ":" 
             << e.lineNum() << endl;
        return false;
    }

    string testBenchDir = outputDirectory + FileSystem::DIRECTORY_SEPARATOR +
        "tb";
    try {
        ProGeUI::generateTestBench(testBenchDir, outputDirectory);
    } catch (const Exception& e) {
        std::cerr << "Warning: Processor Generator failed to "
                  << "generate a test bench." << std::endl;
        std::cerr << e.errorMessage() << std::endl;
    }

    try {
        ProGeUI::generateScripts(outputDirectory, outputDirectory, 
            testBenchDir);
    } catch (const Exception& e) {
        std::cerr << "Warning: Processor Generator failed to "
                  << "generate a simulation/compilation scripts."
                  << std::endl;
        std::cerr << e.errorMessage() << std::endl;
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
    vector<string> pluginPaths = Environment::icDecoderPluginPaths();
    for (vector<string>::const_iterator iter = pluginPaths.begin();
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
