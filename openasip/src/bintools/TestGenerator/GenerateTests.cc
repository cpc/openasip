/*
    Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file GenerateTests.cc
 *
 *
 *
 * Created on: 24.2.2015
 * @author: Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <cstdlib>
#include <string>
#include <iterator>
#include <set>

#include "Application.hh"
#include "TestGenCmdLineOptions.hh"
#include "FileSystem.hh"
#include "Machine.hh"
#include "Program.hh"
#include "ProgramWriter.hh"
#include "TPEFWriter.hh"
#include "BinaryStream.hh"
#include "TestCase.hh"
#include "VerificationDataGenerator.hh"
#include "TestFileWriter.hh"
#include "Exception.hh"
#include "MachineImplementation.hh"

#include "TestGeneratorCollection.hh"
#include "TestGeneratorBase.hh"

using std::cout;
using std::cerr;
using std::endl;
using namespace TTAProgram;
using namespace TTAMachine;

template<class ContainerType>
void loadPrograms(
    const std::set<Path>& sourceFilePaths,
    const TTAMachine::Machine& targetArch,
    ContainerType& programContainer);
bool getOutputDirectory(const TestGenCmdLineOptions& options, Path& outputDir);
void writeTPEF(const TestCase& testCase, Path& outputDir);
void printGeneratorList(const TestGeneratorCollection& generators);
RandomNumberGenerator::SeedType getSeed(const TestGenCmdLineOptions& options);

/**
 * The main function.
 */
int main(int argc, char* argv[]) {

    TestGeneratorCollection testGenerators;
    TestGenCmdLineOptions options;
    try {
        options.parse(argv, argc);
        if (options.listGeneratorsDefined()) {
            printGeneratorList(testGenerators);
            return EXIT_SUCCESS;
        }
        options.validate();
    } catch (const ParserStopRequest&) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& exception) {
        cerr << exception.errorMessage() << endl;
        return EXIT_FAILURE;
    }

    if (options.isVerboseSpamSwitchDefined()) {
        Application::setVerboseLevel(2);
    } else if (options.isVerboseSwitchDefined()) {
        Application::setVerboseLevel(1);
    } else {
        Application::setVerboseLevel();
    }

    // Initialize test generators //
    const Machine& targetMachine =
        *Machine::loadFromADF(options.adfFilePath());
    testGenerators.setMachine(targetMachine);
    const IDF::MachineImplementation* targetImpl = nullptr;
    if (options.idfDefined()) {
        targetImpl = IDF::MachineImplementation::loadFromIDF(
            options.idfFilePath().string());
        testGenerators.setImplementation(*targetImpl);
    }
    //todo Do enabling/disabling in order of arguments i.e. allow
    // "-D -e some-test" => disable all first then enable some-test.
    if (options.disableAllTestGenerators()) {
        for (auto& generator : testGenerators.getGenerators()) {
            testGenerators.setEnabled(generator.second->title(), false);
        }
    }

    for (size_t i = 0; i < options.toDisabledListSize(); i++) {
        testGenerators.setEnabled(options.toDisabled(i), false);
    }
    for (size_t i = 0; i < options.toEnabledListSize(); i++) {
        testGenerators.setEnabled(options.toEnabled(i));
    }

    std::set<Path> userSourceFiles = options.programFiles();
    std::set<Path>::const_iterator prog_it;

    // Load user defined test programs //
    std::vector<TestCase> userTestCases;
    loadPrograms(userSourceFiles, targetMachine, userTestCases);

    Path outputDir("");
    if (!getOutputDirectory(options, outputDir)) {
        return EXIT_FAILURE;
    }

    if (Application::spamVerbose()) {
        cout << "Loaded user test code(s): ";
        std::vector<TestCase>::const_iterator tc_it;
        for (tc_it = userTestCases.begin(); tc_it != userTestCases.end();
            tc_it++) {
            cout << tc_it->testName() << ", ";
        }
        if (userTestCases.empty()) {
            cout << "none";
        }
        cout << endl;
    }

    TestFileWriter testFileWriter;
    testFileWriter.setOutput(outputDir);
    testFileWriter.setMachine(targetMachine);
    testFileWriter.setImageFormat(options.programImageFormat());
    testFileWriter.setDataMemoryWidthInMAUs(options.dataMemoryWidthInMAUs());

    // Call Test Generators to produce test programs //
    RandomNumberGenerator::SeedType seed = getSeed(options);
    std::vector<TestCase> testcases;
    testcases = testGenerators.generateTestCases(seed);
    if (Application::increasedVerbose()) {
        cout << "Generated test case count: "
             << Conversion::toString(testcases.size()) << endl;
    }

    // Write ouput files //
    std::vector<TestCase>::const_iterator tc_it;
    for (tc_it = testcases.begin();
        tc_it != testcases.end(); tc_it++) {
        testFileWriter.setTestCase(*tc_it);
        if (tc_it->generateVerificationData()) {
            try {
                testFileWriter.makeVerificationData();
            } catch (Exception& e) {
                if (Application::increasedVerbose()) {
                    std::cerr << "Error from verification data generator. "
                        << "Message: " << endl << e.errorMessage() << endl;
                }
                continue;
            }
        }
        try {
            testFileWriter.makeTPEF();
            testFileWriter.makeDisassembly();
            testFileWriter.makeProgramImages();
        } catch (Exception& e) {
            if (Application::increasedVerbose()) {
                std::cerr << "Error from test file writer."
                          << "Message: " << endl << e.errorMessage() << endl;
            }
            continue;
        }
    }

    for (tc_it = userTestCases.begin();
        tc_it != userTestCases.end(); tc_it++) {
        testFileWriter.setTestCase(*tc_it);
        if (tc_it->generateVerificationData()) {
            try {
                testFileWriter.makeVerificationData();
            } catch (IOException& e) {
                if (Application::increasedVerbose()) {
                    std::cerr << "Error from verification data generator. "
                        << "Message: " << endl << e.errorMessage() << endl;
                }
                continue;
            }
        }
        testFileWriter.makeTPEF();
        testFileWriter.makeDisassembly();
        testFileWriter.makeProgramImages();
    }

    if (options.progeOutputDirDefined()) {
        testFileWriter.makeTestRunnerScript(options.progeOutputDir());
    } else {
        testFileWriter.makeTestRunnerScript(outputDir);
    }

    return EXIT_SUCCESS;
}

/**
 * Loads valid code files it recognizes by file extension as TestCases.
 *
 * @param sourceFilePaths The source files.
 * @param targetArch The target machine.
 * @param programInserter The container in which TestCases are inserted. The
 *                        container must have STL-style insert()-function.
 */
template<class ContainerType>
void loadPrograms(
    const std::set<Path>& sourceFilePaths,
    const TTAMachine::Machine& targetArch,
    ContainerType& programContainer) {

    std::insert_iterator<ContainerType> programInserter =
       std::inserter(programContainer, programContainer.end());

    std::set<Path>::const_iterator src_it;
    for (src_it = sourceFilePaths.begin(); src_it != sourceFilePaths.end();
        src_it++) {

        if (!FileSystem::fileExists(*src_it)) {
            if (Application::increasedVerbose()) {
                cout << "Skipped non-existent file: " << src_it->string()
                     << endl;
            }
            continue;
        }

        if (src_it->extension() == ".tpef") {
            Program* prog = NULL;
            try {
                prog = Program::loadFromTPEF(*src_it, targetArch);
            } catch (Exception& e) {
                if (Application::increasedVerbose()) {
                    cout << "Skipped invalid TPEF file. "
                        << "Loader message: " << endl << e.errorMessage()
                        << endl;
                }
                continue;
            }
            TestCase testcase(prog, src_it->stem().string());
            programInserter = testcase;

        } else {
            // Otherwise ignore unresolved file.
            if (Application::increasedVerbose()) {
                cerr << "Skipped unresolved user code: "
                     << src_it->string() << endl;
            }
        }
    }
}

/**
 * Resolves target directory for the generated tests.
 *
 * @param options Queries output options.
 * @param outputDirOut The target directory
 * @return True, if resolution was successful. Otherwise returns false.
 */
bool
getOutputDirectory(const TestGenCmdLineOptions& options, Path& outputDirOut) {
    if (options.progeOutputDirDefined()) {
        outputDirOut /= options.progeOutputDir();
        if (!FileSystem::fileExists(outputDirOut)) {
            std::cerr << "Error: Could not found proge output directory "
                << outputDirOut.string()
                << endl;
            return false;
        }
        outputDirOut /= "tests";
    } else if (options.outputDirDefined()) {
        outputDirOut /= options.outputDir();
    } else {
        outputDirOut /= "tests";
    }
    if (!FileSystem::isAbsolutePath(outputDirOut)) {
        outputDirOut = Path(FileSystem::currentWorkingDir()) /= outputDirOut;
    }
    if (!FileSystem::fileExists(outputDirOut)) {
        assert(FileSystem::createDirectory(outputDirOut));
    }
    if (Application::increasedVerbose()) {
        cerr << "Output directory set to: "
             << "[" << outputDirOut.string() << "]" << endl;
    }
    assert(FileSystem::fileExists(outputDirOut));

    return true;
}

/**
 * For debugging.
 */
void
writeTPEF(const TestCase& testCase, Path& outputDir) {
    using namespace TPEF;
    Path tpefFile = outputDir;
    tpefFile /= testCase.testName() + ".tpef";
    ProgramWriter progWriter(*testCase.program());
    BinaryStream tpefStream(tpefFile);
    TPEFWriter::instance().writeBinary(tpefStream, progWriter.createBinary());
}


void
printGeneratorList(const TestGeneratorCollection& collection) {
    const std::string IDENT = "    ";

    TestGeneratorCollection::GeneratorMap::const_iterator it;
    const TestGeneratorCollection::GeneratorMap& testGenerators =
        collection.getGenerators();

    for (it = testGenerators.begin(); it != testGenerators.end(); it++) {
        const TestGeneratorBase& gen = *(it->second);
        cout << gen.title() << TCEString::applyIf(gen.isEnabled(),
            " [enabled]", " [disabled]") << endl;
        std::stringstream desc(gen.description());
        std::string word;
        size_t charsInLine = IDENT.size();
        cout << IDENT;
        while (desc >> word) {
            if (charsInLine + word.size() > 79) {
                cout << endl << IDENT << word << " ";
                charsInLine = IDENT.size() + word.size() + 1;

            } else {
                cout << word << " ";
                charsInLine += word.size() + 1;
            }
        }
        cout << endl;
    }
    cout << endl
         << "Use -e COMMA-SEPARATED-LIST to enable test generators." << endl
         << "Use -d COMMA-SEPARATED-LIST to disable test generators." << endl
         << "COMMA-SEPARATED-LIST is list of test generators listed above."
         << endl;
}


/**
 * Returns seed value for random number generator.
 */
RandomNumberGenerator::RNGValueType
getSeed(const TestGenCmdLineOptions& options) {
    if (options.seedDefined()) {
        const std::string& seedString = options.getSeed();
        if (seedString.empty()) {
            RandomNumberGenerator::RNGValueType newSeed = RandomNumberGenerator::generateNewSeed();
            cout << "Generated seed: " << newSeed << endl;
            return newSeed;
        } else {
            return RandomNumberGenerator::convertToSeed(seedString);
        }
    } else {
        return RandomNumberGenerator::DEFAULTSEED;
    }
}


