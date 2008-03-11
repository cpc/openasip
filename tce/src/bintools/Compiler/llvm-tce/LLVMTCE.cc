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
        std::cerr << e.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }

    if (options.numberOfArguments() != 1) {
        options.printHelp();
        return EXIT_FAILURE;
    }

    // --- Target ADF ---
    if (!options.isMachineFileDefined()) {
        std::cerr << "ERROR: No target architecture (.adf) defined."
                  << std::endl;

        return EXIT_FAILURE;
    }

    TTAMachine::Machine* mach = NULL;
    std::string targetADF = options.machineFile();
    try {
        ADFSerializer serializer;
        serializer.setSourceFile(targetADF);
        mach = serializer.readMachine();
    } catch (Exception& e) {
        std::cerr << "Error loading target architecture file '"
                  << targetADF << "':" << std::endl
                  << e.errorMessage() << std::endl;

        return EXIT_FAILURE;
    }

    bool schedule = false;
    SchedulingPlan* plan = NULL;
    if (options.isSchedulerConfigFileDefined()) {
        std::string schedulerConf = options.schedulerConfigFile();
        try {
            plan = SchedulingPlan::loadFromFile(schedulerConf);
        } catch (Exception& e) {
            std::cerr << "Error loading scheduler configuration file '"
                      << schedulerConf << "':" << std::endl
                      << e.errorMessage() << std::endl;

            return EXIT_FAILURE;
        }
        schedule = true;
    }

    // --- Output file name ---
    std::string outputFileName = DEFAULT_OUTPUT_FILENAME;
    if (options.isOutputFileDefined()) {
        outputFileName = options.outputFile();
    }

    // --- optimization level ---
    int optLevel = DEFAULT_OPT_LEVEL;
    if (options.isOptLevelDefined()) {
        optLevel = options.optLevel();
    }

    std::string bytecodeFile = options.argument(1);

    bool debug = options.debugFlag();

    // ---- Run compiler ----
    try {
        LLVMBackend compiler;
        TTAProgram::Program* seqProg =
            compiler.compile(bytecodeFile, *mach, optLevel, debug);

        if (schedule) {
            SchedulerFrontend scheduler;
            TTAProgram::Program* prog =
                scheduler.schedule(*seqProg, *mach, *plan);

            TTAProgram::Program::writeToTPEF(*prog, outputFileName);

            delete prog;
            prog = NULL;
        } else {
            TTAProgram::Program::writeToTPEF(*seqProg, outputFileName);
        }

        delete seqProg;
        seqProg = NULL;

    } catch (Exception& e) {
        std::cerr << "Error compiling '" << bytecodeFile << "':" << std::endl
                  << e.errorMessage() << std::endl;

        return EXIT_FAILURE;
    }

    delete mach;
    mach = NULL;
    delete plan;
    plan = NULL;

    return EXIT_SUCCESS;
}
