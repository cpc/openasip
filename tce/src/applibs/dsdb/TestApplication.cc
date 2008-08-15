/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file TestApplication.cc
 *
 * Implementation of TestApplication class.
 *
 * @author Jari M‰ntyneva 2007 (jari.mantyneva@tut.fi)
 * @author Esa M‰‰tt‰ 2008 (esa.maatta@tut.fi)
 * @note rating: red
 */

#include <cstdio>
#include <fstream>

#include "TestApplication.hh"
#include "FileSystem.hh"
#include "Conversion.hh"


using std::string;

const string TestApplication::DESCRIPTION_FILE_NAME_ = "description.txt";
const string TestApplication::APPLICATION_FILE_NAME_ = "program.bc";
const string TestApplication::SETUP_FILE_NAME_ = "setup.sh";
const string TestApplication::SIMULATE_TTASIM_FILE_NAME_ = "simulate.ttasim";
const string 
TestApplication::CORRECT_OUTPUT_FILE_NAME_ = "correct_simulation_output";
const string TestApplication::VERIFY_FILE_NAME_ = "verify.sh";
const string TestApplication::CLEANUP_FILE_NAME_ = "cleanup.sh";
const string TestApplication::MAX_RUNTIME_ = "max_runtime";
const int TestApplication::MAX_LINE_LENGTH_ = 512;

/**
 * Constructor.
 *
 * @param testApplicationPath Path of the test application directory.
 */
TestApplication::TestApplication(const string& testApplicationPath) 
    throw (IOException) :
    testApplicationPath_(testApplicationPath), maxRuntime_(0) {

    string maxRuntimeFile = testApplicationPath_ 
        + FileSystem::DIRECTORY_SEPARATOR
        + MAX_RUNTIME_;
    if (FileSystem::fileExists(maxRuntimeFile)) {
        char line[MAX_LINE_LENGTH_];
        FILE* file = fopen(maxRuntimeFile.c_str(), "r");
        fgets(line, MAX_LINE_LENGTH_, file);
        try {
            maxRuntime_ = Conversion::toDouble(line);
        } catch (const NumberFormatException& exception) {
            fclose(file);
            throw IOException(
                __FILE__, __LINE__, __func__, exception.errorMessage());
        }
        fclose(file);
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
 *
 * @return The maximum runtime of the test application.
 */
TestApplication::Runtime
TestApplication::maxRuntime() const {
    return maxRuntime_;
}

/**
 * Returns true if 'sequential_program' file is in the test application
 * directory.
 *
 * @return True if 'sequential_program' file is in the test application
 * directory.
 */
bool
TestApplication::hasApplication() const {

    string applicationFile =
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        APPLICATION_FILE_NAME_;
    return FileSystem::fileExists(applicationFile);
}

/**
 * Returns true if 'setup.sh' file is in the test application directory.
 *
 * @return True if 'setup.sh' file is in the test application directory.
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
 *
 * @return True if 'simulate.ttasim' file is in the test application directory.
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
 *
 * @return True if 'correct_simulation_output' file is in the test 
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
 *
 * @return True if 'verify.sh' file is in the test application directory.
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
 *
 * @return True if 'cleanup.sh' file is in the test application directory.
 */
bool
TestApplication::hasCleanupSimulation() const {

    string cleanupFile =
        testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
        CLEANUP_FILE_NAME_;
    return FileSystem::fileExists(cleanupFile);
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
        string applicationFile =
            testApplicationPath_ + FileSystem::DIRECTORY_SEPARATOR +
            APPLICATION_FILE_NAME_;
        if (FileSystem::fileExists(applicationFile)) {
            return applicationFile;
        }
    }
    return "";
}
