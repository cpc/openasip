/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file LLVMTCE.cc
 *
 * LLVM/TCE compiler command line interface.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2008 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */
#include <iostream>
#include "Application.hh"
#include "LLVMBackend.hh"
#include "LLVMTCECmdLineOptions.hh"
#include "Program.hh"
#include "ADFSerializer.hh"
#include "FileSystem.hh"
#include "InterPassData.hh"
#include "Machine.hh"

#ifndef LLVM_3_1
#ifndef __STDC_LIMIT_MACROS
#define __STDC_LIMIT_MACROS
#endif
#include <llvm/Support/CommandLine.h>
#endif

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

    LLVMTCECmdLineOptions* options = new LLVMTCECmdLineOptions();

    // Parse command line options.
    try {
        options->parse(argv, argc);
    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& e) {
        std::cerr << e.errorMessageStack() << std::endl;
        return EXIT_FAILURE;
    }

    if (options->numberOfArguments() != 1) {
        options->printHelp();
        return EXIT_FAILURE;
    }

    if (options->tempDir() == "") {
        Application::errorStream() 
            << "Temporary directory required." << std::endl;
        options->printHelp();
        return EXIT_FAILURE;
    }

    Application::setCmdLineOptions(options);

    if (options->isVerboseSwitchDefined()) {
        Application::setVerboseLevel(Application::VERBOSE_LEVEL_INCREASED);
    }

    // --- Target ADF ---
    if (!options->isMachineFileDefined()) {
        std::cerr << "ERROR: No target architecture (.adf) defined."
                  << std::endl;

        return EXIT_FAILURE;
    }

    TTAMachine::Machine* mach = NULL;
    std::string targetADF = options->machineFile();

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

    // --- Output file name ---
    std::string outputFileName = DEFAULT_OUTPUT_FILENAME;
    if (options->isOutputFileDefined()) {
        // Test if output file can be opened
        outputFileName = options->outputFile();
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
    if (options->isOptLevelDefined()) {
        optLevel = options->optLevel();
    }

    std::string bytecodeFile = options->argument(1);

    bool debug = options->debugFlag();
    
    //--- check if program was ran in src dir or from install dir ---
    std::string runPath = std::string(argv[0]);
    bool useInstalledVersion = 
        runPath.find("bintools/Compiler/llvm-tce/.libs") == std::string::npos &&
        runPath.find("lt-llvm-tce") == std::string::npos;
    
    // All emulation code which cannot be linked in before last global dce is
    // executed, for emulation of instructions which are generated during 
    // lowering. Unnecessary functions are removed by the MachineDCE pass.
    std::string emulationCode;    
    if (options->isStandardEmulationLibDefined()) {
        emulationCode = options->standardEmulationLib();
    }
            
    // ---- Run compiler ----
    try {
        InterPassData* ipData = new InterPassData;

	#ifndef LLVM_3_1
	const char* argv[] = {"llvm-tce", "--no-stack-coloring"};
	llvm::cl::ParseCommandLineOptions(2, argv, "llvm linker\n");
	#endif
        
        LLVMBackend compiler(useInstalledVersion, options->tempDir());
        TTAProgram::Program* seqProg =
            compiler.compile(bytecodeFile, emulationCode, *mach, optLevel, debug, ipData);

        TTAProgram::Program::writeToTPEF(*seqProg, outputFileName);
        delete seqProg;
        seqProg = NULL;

        delete ipData;
        ipData = NULL;

    } catch (const CompileError& e) {
        // CompilerErrors are related to the user program input and
        // should be communicated to the programmer in a clean fashion.
        Application::errorStream() << e.errorMessage() << std::endl;
    } catch (const Exception& e) {
        // Assume other Exceptions are due to like lack of more
        // user friendly CompilerError or actual internal compiler errors.
        std::cerr << "Error compiling '" << bytecodeFile << "':" << std::endl
                  << e.errorMessageStack() << std::endl;

        return EXIT_FAILURE;
    }

    delete mach;
    mach = NULL;

    return EXIT_SUCCESS;
}
