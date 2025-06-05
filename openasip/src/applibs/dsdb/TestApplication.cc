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
 * @file TestApplication.cc
 *
 * Implementation of TestApplication class.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @author Esa Määttä 2008 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#include <cstdio>
#include <fstream>

#include "TestApplication.hh"
#include "FileSystem.hh"
#include "Conversion.hh"


using std::string;

const string TestApplication::DESCRIPTION_FILE_NAME_ = "description.txt";
const string TestApplication::APPLICATION_BASE_FILE_NAME_ = "program";
const string TestApplication::SETUP_FILE_NAME_ = "setup.sh";
const string TestApplication::SIMULATE_TTASIM_FILE_NAME_ = "simulate.ttasim";
const string TestApplication::CORRECT_OUTPUT_FILE_NAME_ =
    "correct_simulation_output";
const string TestApplication::VERIFY_FILE_NAME_ = "verify.sh";
const string TestApplication::CLEANUP_FILE_NAME_ = "cleanup.sh";
const string TestApplication::FUNCTIONS_OF_INTEREST_FILE_NAME_ =
    "functions_of_interest";
const string TestApplication::MAX_RUNTIME_ = "max_runtime";
const int TestApplication::MAX_LINE_LENGTH_ = 512;

/**
 * Constructor.
 *
 * @param testApplicationPath Path of the test application directory.
 */
TestApplication::TestApplication(const string& testApplicationPath)
    : testApplicationPath_(testApplicationPath), maxRuntime_(0) {
    string maxRuntimeFile = testApplicationPath_ 
        + FileSystem::DIRECTORY_SEPARATOR
        + MAX_RUNTIME_;
    if (FileSystem::fileExists(maxRuntimeFile)) {
        char line[MAX_LINE_LENGTH_];
        FILE* file = fopen(maxRuntimeFile.c_str(), "r");
        if (fgets(line, MAX_LINE_LENGTH_, file) == NULL)
            abort();
        try {
            maxRuntime_ = Conversion::toDouble(line);
        } catch (const NumberFormatException& exception) {
            fclose(file);
            throw IOException(
                __FILE__, __LINE__, __func__, exception.errorMessage());
        }
        fclose(file);
    }

    if (hasFunctionsOfInterest()) {
        std::ifstream f(
            testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
            FUNCTIONS_OF_INTEREST_FILE_NAME_);
        std::string str(
            (std::istreambuf_iterator<char>(f)),
            std::istreambuf_iterator<char>());
        functionsOfInterest_ = TCEString(str).split(",");
    }
}

/**
 * Destructor.
 */
TestApplication::~TestApplication() {
}

/**
 * Returns the test application description.
 *
 * The description is given in the description.txt file of the test
 * application directory.
 *
 * @return The test application description. Returns an empty vector if the
 *         desctription.txt file was not found.
 */
std::vector<std::string>
TestApplication::description() const {

    std::vector<string> description;
    string descriptionFile = 
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        DESCRIPTION_FILE_NAME_;
    if (FileSystem::fileExists(descriptionFile)) {
        char line[MAX_LINE_LENGTH_];
        FILE* file = fopen(descriptionFile.c_str(), "r");
        while (fgets(line, MAX_LINE_LENGTH_, file)) {
            description.push_back(line);
        }
        fclose(file);
    }
    return description;
}

/**
 * Verifies the previous simulation run.
 *
 * Uses 'verify.sh' to verify the previous simulation run.
 *
 * @return True if the simulation result was verified as correct.
 */
bool
TestApplication::verifySimulation() const {

    string verifyFile =
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        VERIFY_FILE_NAME_;

    if (FileSystem::fileExists(verifyFile)) {
        return FileSystem::runShellCommand(verifyFile);            
    }
    return false;
}

/**
 * Returns the correct output from file 'correct_simulation_output' as string.
 *
 * @return Correct output as string.
 */
const std::string
TestApplication::correctOutput() const {

    string correctOutputFile =
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        CORRECT_OUTPUT_FILE_NAME_;
    if (!FileSystem::fileExists(correctOutputFile)) {
        return "";
    }
    std::string buf;
    std::string line;
    std::ifstream in(correctOutputFile.c_str());

    if (std::getline(in,line)) {
        buf += line;
    }
    while(std::getline(in,line)) {
        buf += "\n";
        buf += line;
    }
    return buf;
}

/**
 * Set up the simulation run.
 *
 * Uses 'setup.sh' to set up the simulation run.
 */
void
TestApplication::setupSimulation() const {

    string setupFile =
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        SETUP_FILE_NAME_;
    if (FileSystem::fileExists(setupFile)) {
        FileSystem::runShellCommand(setupFile);        
    }
}

/**
 * Returns a istream to 'simulate.ttasim' file.
 *
 * Client takes responsibility of destroying the stream.
 *
 * @return istream to 'simulate.ttasim' file.
 */
std::istream*
TestApplication::simulateTTASim() const {

    std::ifstream* ifStream = new std::ifstream;
    if (!hasSimulateTTASim()) {
        return ifStream;
    }
    delete ifStream;
    ifStream = NULL;
    
    string simulateFile =
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        SIMULATE_TTASIM_FILE_NAME_;
    return new std::ifstream(simulateFile.c_str());
}


/**
 * Cleans up the previous simulation run.
 *
 * Uses 'cleanup.sh' script if it exists.
 */
void
TestApplication::cleanupSimulation() const {
    string cleanupFile =
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        CLEANUP_FILE_NAME_;
    if (FileSystem::fileExists(cleanupFile)) {
        FileSystem::runShellCommand(cleanupFile);        
    }
}

/**
 * Returns the maximum runtime that is set in file "max_runtime".
 */
TestApplication::Runtime
TestApplication::maxRuntime() const {
    return maxRuntime_;
}

/**
 * Returns true if 'sequential_program' file is in the test application
 * directory.
 */
bool
TestApplication::hasApplication() const {

    string applicationBCFile =
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        APPLICATION_BASE_FILE_NAME_ + ".bc";
    string applicationLLFile =
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        APPLICATION_BASE_FILE_NAME_ + ".ll";

    return FileSystem::fileExists(applicationBCFile) ||
        FileSystem::fileExists(applicationLLFile);
}

/**
 * Returns true if 'setup.sh' file is in the test application directory.
 */
bool
TestApplication::hasSetupSimulation() const {

    string setupFile =
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        SETUP_FILE_NAME_;
    return FileSystem::fileExists(setupFile);
}

/**
 * Returns true if 'simulate.ttasim' file is in the test application directory.
 */
bool
TestApplication::hasSimulateTTASim() const {

    string simulateFile =
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        SIMULATE_TTASIM_FILE_NAME_;
    return FileSystem::fileExists(simulateFile);
}

/**
 * Returns true if 'correct_simulation_output' file is in the test 
 * application directory.
 */
bool
TestApplication::hasCorrectOutput() const {

    string correctOutputFile =
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        CORRECT_OUTPUT_FILE_NAME_;
    return FileSystem::fileExists(correctOutputFile);
}

/**
 * Returns true if 'verify.sh' file is in the test application directory.
 */
bool
TestApplication::hasVerifySimulation() const {

    string verifyFile =
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        VERIFY_FILE_NAME_;
    return FileSystem::fileExists(verifyFile);
}

/**
 * Returns true if 'cleanup.sh' file is in the test application directory.
 */
bool
TestApplication::hasCleanupSimulation() const {

    string cleanupFile =
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        CLEANUP_FILE_NAME_;
    return FileSystem::fileExists(cleanupFile);
}

/**
 * @return true if 'functions_of_interest' file is found in the application
 * directory.
 */
bool
TestApplication::hasFunctionsOfInterest() const {
    return FileSystem::fileIsReadable(
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        FUNCTIONS_OF_INTEREST_FILE_NAME_);
}

/**
 * Returns the application path in the test application directory.
 *
 * If the file 'sequential_program' does not exists returns an empty string.
 *
 * @return The path of the 'sequential_program' file.
 */
const std::string
TestApplication::applicationPath() const {

    if (hasApplication()) {
        string applicationBCFile =
            testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
            APPLICATION_BASE_FILE_NAME_ + ".bc";
        string applicationLLFile =
            testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
            APPLICATION_BASE_FILE_NAME_ + ".ll";

        // prioritize the .ll file as it's more portable across
        // LLVM versions than the bitcode
        if (FileSystem::fileExists(applicationLLFile)) {
            return applicationLLFile;
        }
        if (FileSystem::fileExists(applicationBCFile)) {
            return applicationBCFile;
        }

    }
    return "";
}
