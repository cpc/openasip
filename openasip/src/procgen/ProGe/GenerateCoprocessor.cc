/*
    Copyright (c) 2002-2025 Tampere University.

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
 * @file GenerateCoprocessor.cc
 *
 * Implementation of GenerateCoprocessor class from GenerateProcessor.
 *
 * @author Tharaka Sampath
 */

#include <string>
#include <iostream>

#include "GenerateCoprocessor.hh"
#include "CoprocessorCmdLineOptions.hh"
#include "FileSystem.hh"
#include "Environment.hh"
#include "Machine.hh"
#include "BinaryEncoding.hh"
#include "StringTools.hh"

using namespace ProGe;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

int const DEFAULT_IMEMWIDTH_IN_MAUS = 1;

/**
 * The main program of generatecoprocessor application.
 */
int main(int argc, char* argv[]) {
    GenerateCoprocessor ui;
    bool successful = ui.generateCoprocessor(argc, argv);
    if (successful) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}


/**
 * The constructor.
 */
GenerateCoprocessor::GenerateCoprocessor() {
}


/**
 * The destructor.
 */
GenerateCoprocessor::~GenerateCoprocessor() {
}


/**
 * Parses the command line arguments and generates the coprocessor with FUs.
 *
 * @return True if the generation of the coprocessor was succesful, otherwise
 *         false.
 */
bool
GenerateCoprocessor::generateCoprocessor(int argc, char* argv[]) {

    CoprocessorCmdLineOptions options;
    string entity = "";

    try {

        options.parse(argv, argc);
        ProGeOptions progeOptions(options, true);

        if (options.isVerboseSwitchDefined()) {
            Application::setVerboseLevel(
                Application::VERBOSE_LEVEL_INCREASED);
        }

        if (options.isVerboseSpamSwitchDefined()) {
            Application::setVerboseLevel(Application::VERBOSE_LEVEL_SPAM);
        }
        
        if (options.numberOfArguments() == 0) {
            options.printHelp();
            return false;
        }

        if (FileSystem::fileExists(progeOptions.outputDirectory)) {
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

        int imemWidthInMAUs = 4;
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

    ProGeOptions progeOptions(options, true);

    return true;
}

/** 
 * Generates the output directory name.
 *
 * @param options Proge command line options.
 * @param outputDir String where output directory name is to be stored. 
 */
void
GenerateCoprocessor::getOutputDir(
    const CoprocessorCmdLineOptions& options,
    std::string& outputDir) {

    outputDir = options.outputDirectory();

    if (outputDir == "") {
        outputDir = FileSystem::currentWorkingDir() + 
            FileSystem::DIRECTORY_SEPARATOR + "_out";
    } else {
        outputDir = FileSystem::expandTilde(outputDir);
        outputDir = FileSystem::absolutePathOf(outputDir);
    }
}
