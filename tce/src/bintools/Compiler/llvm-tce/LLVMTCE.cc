/**
 * @file LLVMTCE.cc
 *
 * LLVM/TCE compiler command line interface.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2008 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <iostream>
#include "Application.hh"
#include "LLVMBackend.hh"
#include "LLVMTCECmdLineOptions.hh"
#include "Program.hh"
#include "ADFSerializer.hh"
#include "SchedulingPlan.hh"
#include "SchedulerFrontend.hh"
#include "FileSystem.hh"
#include "InterPassData.hh"

const std::string DEFAULT_OUTPUT_FILENAME = "out.tpef";
const int DEFAULT_OPT_LEVEL = 2;

/**
 * Main function of the CLI.
 *
 * Checks command line options and runs compiler accordingly.
 *
 * @param argc Argument count.
 * @param argv Command line arguments.
 * @return Exit status code for the OS.
 */
int
main(int argc, char* argv[]) {

    Application::initialize();

    LLVMTCECmdLineOptions options;


    // Parse command line options.
    try {
        options.parse(argv, argc);
    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& e) {
        std::cerr << e.errorMessageStack() << std::endl;
        return EXIT_FAILURE;
    }

    if (options.numberOfArguments() != 1) {
        options.printHelp();
        return EXIT_FAILURE;
    }

    if (options.isVerboseSwitchDefined()) {
        Application::setVerboseLevel(Application::VERBOSE_LEVEL_INCREASED);
    }

    // --- Target ADF ---
    if (!options.isMachineFileDefined()) {
        std::cerr << "ERROR: No target architecture (.adf) defined."
                  << std::endl;

        return EXIT_FAILURE;
    }

    TTAMachine::Machine* mach = NULL;
    std::string targetADF = options.machineFile();

    if (!FileSystem::fileExists(targetADF) ||
        !FileSystem::fileIsReadable(targetADF) ||
        FileSystem::fileIsDirectory(targetADF)) {

        std::cerr << "ERROR: Target architecture file '"
                  << targetADF << "' doesn't exist or isn't readable."
                  << std::endl;

        return EXIT_FAILURE;
    }

    try {
        ADFSerializer serializer;
        serializer.setSourceFile(targetADF);
        mach = serializer.readMachine();
    } catch (Exception& e) {
        std::cerr << "Error loading target architecture file '"
                  << targetADF << "':" << std::endl
                  << e.errorMessageStack() << std::endl;

        return EXIT_FAILURE;
    }

    SchedulingPlan* plan = NULL;
    if (options.isSchedulerConfigFileDefined()) {
        std::string schedulerConf = options.schedulerConfigFile();

        try {
            plan = SchedulingPlan::loadFromFile(schedulerConf);
        } catch (Exception& e) {
            std::cerr << "Error loading scheduler configuration file '"
                      << schedulerConf << "':" << std::endl
                      << e.errorMessageStack() << std::endl;

            return EXIT_FAILURE;
        }
    }
   
    // --- Output file name ---
    std::string outputFileName = DEFAULT_OUTPUT_FILENAME;
    if (options.isOutputFileDefined()) {
        // Test if output file can be opened
        outputFileName = options.outputFile();
    }
    if (!FileSystem::fileIsWritable(outputFileName) &&
        !FileSystem::fileIsCreatable(outputFileName)) {
        std::cerr << "Output file '"
            << outputFileName << "' can not be opened for writing!"
            << std::endl;
        return EXIT_FAILURE;
    }

    // --- optimization level ---
    int optLevel = DEFAULT_OPT_LEVEL;
    if (options.isOptLevelDefined()) {
        optLevel = options.optLevel();
    }

    std::string bytecodeFile = options.argument(1);

    bool debug = options.debugFlag();
    
    //--- check if program was ran in src dir or from install dir ---
    std::string runPath = string(argv[0]);
    bool useInstalledVersion = 
        runPath.find("bintools/Compiler/llvm-tce/.libs") == string::npos &&
        runPath.find("lt-llvm-tce") == string::npos;
    
    // ---- Run compiler ----
    try {
        InterPassData* ipData = new InterPassData;
        
        LLVMBackend compiler(true, useInstalledVersion);
        TTAProgram::Program* seqProg =
            compiler.compile(bytecodeFile, *mach, optLevel, debug, ipData);

        if (plan != NULL) {
            SchedulerFrontend scheduler;
            TTAProgram::Program* prog;
            prog = scheduler.schedule(*seqProg, *mach, *plan, ipData);
            TTAProgram::Program::writeToTPEF(*prog, outputFileName);

            delete prog;
            prog = NULL;
        } else {
            TTAProgram::Program::writeToTPEF(*seqProg, outputFileName);
        }

        delete seqProg;
        seqProg = NULL;

        delete ipData;
        ipData = NULL;

    } catch (Exception& e) {
        std::cerr << "Error compiling '" << bytecodeFile << "':" << std::endl
                  << e.errorMessageStack() << std::endl;

        return EXIT_FAILURE;
    }

    delete mach;
    mach = NULL;
    delete plan;
    plan = NULL;

    return EXIT_SUCCESS;
}
