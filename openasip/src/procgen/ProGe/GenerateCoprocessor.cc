/*
    Copyright (C) 2025 Tampere University.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301 USA
 */
/**
 * @file GenerateCoprocessor.cc
 *
 * Implementation of GenerateCoprocessor class from GenerateProcessor.
 *
 * @author Tharaka Sampath
 */

#include "GenerateCoprocessor.hh"

#include <iostream>
#include <string>

#include "BinaryEncoding.hh"
#include "CoprocessorCmdLineOptions.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "Machine.hh"
#include "StringTools.hh"

using namespace ProGe;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::vector;

/**
 * The main program of generatecoprocessor application.
 */
int
main(int argc, char* argv[]) {
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
GenerateCoprocessor::GenerateCoprocessor() {}

/**
 * The destructor.
 */
GenerateCoprocessor::~GenerateCoprocessor() {}

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
        } else if (FileSystem::fileExtension(processorDefinition) == ".pcf") {
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
        cerr << "Exception thrown at: " << e.fileName() << ":" << e.lineNum()
             << endl;
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
    const CoprocessorCmdLineOptions& options, std::string& outputDir) {
    outputDir = options.outputDirectory();

    if (outputDir == "") {
        outputDir = FileSystem::currentWorkingDir() +
                    FileSystem::DIRECTORY_SEPARATOR + "_out";
    } else {
        outputDir = FileSystem::expandTilde(outputDir);
        outputDir = FileSystem::absolutePathOf(outputDir);
    }
}
