/*
    Copyright (c) 2002-2013 Tampere University of Technology.

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
 * @file OperationBuilder.cc
 *
 * Definition of OperationBuilder class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005-2013 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "OperationBuilder.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "tce_config.h"
#include "OperationSerializer.hh"
#include "ObjectState.hh"
#include "Application.hh"

using std::vector;
using std::string;
using std::cerr;
using std::endl;

const string SCHEMA_FILE_NAME = "Operation_Schema.xsd";

OperationBuilder* OperationBuilder::instance_ = NULL;

/**
 * Constructor.
 */
OperationBuilder::OperationBuilder() {
}

/**
 * Destructor.
 */
OperationBuilder::~OperationBuilder() {
}

/**
 * Returns an instance of OperationBuilder.
 *
 * @return An instance of OperationBuilder.
 */
OperationBuilder&
OperationBuilder::instance() {
    if (instance_ == NULL) {
        instance_ = new OperationBuilder();
    }
    return *instance_;
}

/**
 * Finds the path for XML file.
 *
 * File is searched only from certain locations. If file is not found,
 * returns an empty string.
 *
 * @param xmlFile The name of the XML file.
 * @return The path to XML file.
 */
string
OperationBuilder::xmlFilePath(const std::string& xmlFile) {

    vector<string> searchPaths = Environment::osalPaths();
    searchPaths.push_back(FileSystem::currentWorkingDir());

    string filePath = "";
    for (unsigned int i = 0; i < searchPaths.size(); i++) {
        string file = searchPaths[i] + FileSystem::DIRECTORY_SEPARATOR +
            xmlFile;
        if (FileSystem::fileExists(file)) {
            filePath = searchPaths[i];
            break;
        }
    }
    return filePath;
}

/**
 * Searches for the path of behavior file.
 *
 * If path for the file is not given, it is searched from the current
 * working directory. If file is not found, an empty string is returned.
 *
 * @param baseName The name of behavior file.
 * @param path The path where the file is located.
 */
string
OperationBuilder::behaviorFile(
    const std::string& baseName,
    std::string& path) {

    string behFile = baseName + ".cc";
    if (path == "") {
        path = FileSystem::currentWorkingDir();
    }
    string pathToFile = path + FileSystem::DIRECTORY_SEPARATOR + behFile;
    if (FileSystem::fileExists(pathToFile)) {
        return pathToFile;
    }
    return "";
}

/**
 * Builds dynamic module and copies it to the same location where
 * XML property file is located.
 *
 * @param baseName The base name for the module.
 * @param behaviorFile The full path for behavior file.
 * @param path Path where built module is put.
 * @param output Output of compilation.
 * @return True if everything is successful, false otherwise.
 */
bool
OperationBuilder::buildObject(
    const std::string& baseName,
    const std::string& behaviorFile,
    const std::string& path,
    std::vector<std::string>& output) {

    if (behaviorFile != "") {
        TCEString CPPFLAGS = Environment::environmentVariable("CPPFLAGS");
        TCEString CXXFLAGS = Environment::environmentVariable("CXXFLAGS")
            + " " + CONFIGURE_CPPFLAGS + " ";
        TCEString LDFLAGS = Environment::environmentVariable("LDFLAGS")
            + " " + CONFIGURE_LDFLAGS + " ";
        TCEString CXXCOMPILER = Environment::environmentVariable("CXX");
        vector<string> includes = Environment::includeDirPaths();

        TCEString INCLUDES = makeIncludeString(includes);
        
        // Ugly fix. When compiling TCE the base opset is compiled in the 
        // source directory. When distributed version is enabled buildopset
        // needs to know some include paths from the source tree because these
        // headers aren't resident in places that includeDirPaths() returns.
        if (DISTRIBUTED_VERSION) {
            string cwd = FileSystem::currentWorkingDir();
            string DS = FileSystem::DIRECTORY_SEPARATOR;
            string srcOpsetDir = TCE_SRC_ROOT + DS + "opset" + DS + "base";
            if (cwd == srcOpsetDir) {
                vector<string> extraIncludes = Environment::opsetIncludeDir();
                INCLUDES += makeIncludeString(extraIncludes);
            }
         }

        if (Application::isInstalled()) {
            /* Add a dependency to the installed libtce.so just in case
               the .opb will be loaded through a libtce.so that is loaded
               through a dlopen() with the RTLD_LOCAL flag. In that case
               it can happen the loading of libtce.so to access some 
               specific functionality might not import all the symbols
               required by the .opb loaded by libtce later. */
            CXXFLAGS += " -L" + Application::installationDir() + "/lib -ltce ";
        }

        // Add user defined CXXFLAGS + CPPFLAGS to the end because they
        // might point to paths with incompatible TCE headers.
        string COMPILE_FLAGS = \
            INCLUDES + " " + 
            CONFIGURE_CPPFLAGS + " " + CONFIGURE_LDFLAGS + " " +
            CXXFLAGS + " " + CPPFLAGS + " ";

        string module = path + FileSystem::DIRECTORY_SEPARATOR +
            baseName + ".opb";

        string command = (CXXCOMPILER == "") ? (string(CXX)) : (CXXCOMPILER);
        command += " " + COMPILE_FLAGS + " " + behaviorFile + " " +
            string(SHARED_CXX_FLAGS) + " " + LDFLAGS + " -o " + module + " 2>&1";

        if (Application::verboseLevel() > Application::VERBOSE_LEVEL_DEFAULT) {
            Application::logStream() << command << std::endl;
        }
        
        if (Application::runShellCommandAndGetOutput(command, output) != 0) {
            return false;
        }
    }
    return true;
}

/**
 * Installs the data file containing operation property declarations.
 *
 * Creates the destination path if it does not exist. Overwrites the
 * existing file, if it is already installed in given destination.
 *
 * Installation can fail for several reasons: no rights to create
 * destination directory, no rights to overwrite a previously installed
 * version of the data file, no rights to read the source data file.
 *
 * @param path Source path of data file.
 * @param xmlFile Name of the data file.
 * @param destination Destination path where the data file is to be
 *     installed.
 * @return True, if installation is completed successfully, false otherwise.
 */
bool
OperationBuilder::installDataFile(
    const std::string& sourcePath,
    const std::string& xmlFile,
    const std::string& destinationPath) {

    const string source = 
        sourcePath + FileSystem::DIRECTORY_SEPARATOR + xmlFile;

    // an unreadable or nonexisting file should have been caught earlier on
    // (for example, during validation)
    assert(FileSystem::fileExists(source) && 
           FileSystem::fileIsReadable(source));

    if (!FileSystem::fileExists(destinationPath)) {
        if (!FileSystem::createDirectory(destinationPath)) {
            return false;
        }
    }

    const string dest = 
        destinationPath + FileSystem::DIRECTORY_SEPARATOR + xmlFile;

    if (FileSystem::fileExists(dest)) {
        if (FileSystem::removeFileOrDirectory(dest)) {
            return false;
        }
    }

    FileSystem::copy(source, dest);
    return true;
}


/**
 * Verifies the correctness of XML file.
 *
 * @param file Absolute path of XML file.
 */
bool
OperationBuilder::verifyXML(const std::string file) {
    OperationSerializer serializer;
    serializer.setSourceFile(file);

    try {
        ObjectState* root = serializer.readState();
        delete root;
    } catch (const SerializerException& s) {
        cerr << "Invalid XML file: " << s.errorMessage() << endl;
        return false;
    }
    return true;
}

/**
 * Makes an include string for compiler (-I<path1> -I<path2> etc) from the
 * paths in the vector
 *
 * @param Vector containing the include paths
 */
string
OperationBuilder::makeIncludeString(const vector<string>& paths) {
    string includes = "";
    for (size_t i = 0; i < paths.size(); i++) {
        includes += "-I" + paths.at(i) + " ";
    }
    return includes;
}
