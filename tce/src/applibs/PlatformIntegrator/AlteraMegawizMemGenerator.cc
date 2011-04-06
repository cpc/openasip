/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file AlterMegaWizRamGenerator.hh
 *
 * Implementation of AlteraMegawizMemGenerator class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen
 * @note rating: red
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <boost/format.hpp>
#include "Exception.hh"
#include "StringTools.hh"
#include "FileSystem.hh"
#include "Conversion.hh"
#include "Application.hh"
#include "MemoryGenerator.hh"
#include "AlteraMegawizMemGenerator.hh"
using std::string;
using std::endl;

const std::string AlteraMegawizMemGenerator::VERSION_CMD = "quartus_sh -v";
const std::string AlteraMegawizMemGenerator::VERSION_TAG = "Version";

AlteraMegawizMemGenerator::AlteraMegawizMemGenerator(
    int memMauWidth,
    int widthInMaus,
    int addrWidth,
    std::string initFile,
    const PlatformIntegrator* integrator,
    std::ostream& warningStream,
    std::ostream& errorStream):
    MemoryGenerator(memMauWidth, widthInMaus, addrWidth, initFile,
                    integrator, warningStream, errorStream) {

}


AlteraMegawizMemGenerator::~AlteraMegawizMemGenerator() {
}

std::vector<std::string>
AlteraMegawizMemGenerator::runMegawizard(std::string outputFile) {
    
    std::vector<string> componentFiles;
    string tempDir = FileSystem::createTempDirectory();
    if (tempDir.empty()) {
        string msg = "Couldn't create temp directory";
        IOException exc(__FILE__, __LINE__, "AlteraMegawizMemGenerator",
                        msg);
        throw exc;
    }
    string parameterFile = tempDir + FileSystem::DIRECTORY_SEPARATOR + 
        "mem.parameters";

    std::ofstream file;
    file.open(parameterFile.c_str());
    if (!file) {
        string msg = "Couldn't open file " + parameterFile + " for writing";
        IOException exc(__FILE__, __LINE__, "AlteraMeqawizMemGenerator",
                        msg);
        throw exc;
    }
    file << createMemParameters();
    file.close();
    

    TCEString command = "";
    string alteraLibPath =
        Environment::environmentVariable(
            "ALTERA_LIBRARY_PATH");

    if (alteraLibPath != "") {
        command << "export LD_LIBRARY_PATH=" << alteraLibPath << ";";
    }
    // execute "Altera MegaWizard Plug-In Manager(c)"
    command += "qmegawiz -silent module=altsyncram -f:" +
        parameterFile + " " + outputFile + " 2>&1";
    std::vector<string> output;
    int rv = Application::runShellCommandAndGetOutput(command, output);
    
    if (rv != 0 || output.size() != 0) {
        errorStream() 
            << "Failed to create memory component. Make sure 'qmegawiz' "
            << "is in PATH" << endl;
        errorStream()
            << "The command was:" << endl
            << command << endl;
        errorStream()
            << "Error:" << endl;
        for (unsigned int i = 0; i < output.size(); i++) {
            errorStream() << output.at(i) << endl;
        }
    } else {
        componentFiles.push_back(outputFile);
    }

    // clean up
    FileSystem::removeFileOrDirectory(tempDir);
    return componentFiles;
}

/**
 * Defines the default device family which qmegawizard should use if no
 * specific device is specified.
 */
std::string
AlteraMegawizMemGenerator::defaultDeviceFamily() const {

    return "Stratix II";
}

/**
 * Tries to find out quartus II major version
 *
 * quartus_sh outputs following version string:
 * Version 8.0 Build 215 05/29/2008 SJ Full Version
 *
 * @return Major version number. Returns -1 if not successful
 */
int
AlteraMegawizMemGenerator::quartusMajorVersion() const {


    std::vector<string> output;
    int rv = Application::runShellCommandAndGetOutput(VERSION_CMD, output);
    if (rv != 0) {
        return -1;
    }
    int versionNumber = -1;
    for (unsigned int i = 0; i < output.size(); i++) {
        string::size_type pos = output.at(i).find(VERSION_TAG);
        // Version string must be at the beginning of line
        if (pos != string::npos && pos == 0) {

            // remove chars before version number
            string version = output.at(i).substr(VERSION_TAG.size());
            version = StringTools::trim(version);

            // chop from major number
            std::vector<string> chopped;
            StringTools::chopString(version, ".", chopped);
            if (chopped.at(0).size() < 1) {
                continue;
            }
            try {
                versionNumber = Conversion::toInt(chopped.at(0));
                break;
            } catch (Exception& e) {
                // couldn't convert
                continue;
            }
        }
    }
    return versionNumber;
}
