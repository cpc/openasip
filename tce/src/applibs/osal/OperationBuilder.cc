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
 * @file OperationBuilder.cc
 *
 * Definition of OperationBuilder class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
        const string CPPFLAGS = Environment::environmentVariable("CPPFLAGS");
        const string CXXFLAGS = Environment::environmentVariable("CXXFLAGS");
        const string CXXCOMPILER = Environment::environmentVariable("CXX");
        vector<string> includes = Environment::includeDirPaths();

        string INCLUDES = makeIncludeString(includes);
        
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

        string COMPILE_FLAGS = \
            CXXFLAGS + " " + CPPFLAGS + " " + INCLUDES + " " + 
            CONFIGURE_CPPFLAGS + " " + CONFIGURE_LDFLAGS;

        string module = path + FileSystem::DIRECTORY_SEPARATOR +
            baseName + ".opb";

        string command = (CXXCOMPILER == "") ? (string(CXX)) : (CXXCOMPILER);
        command += " " + COMPILE_FLAGS + " " + behaviorFile + " " +
            string(SHARED_CXX_FLAGS) + " -o " + module + " 2>&1";
        
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
