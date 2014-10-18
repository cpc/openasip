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
 * @file BuildOpset.cc
 *
 * Program that builds operation behavior module and locates
 * them to the target directory.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2013
 * @note rating: red
 */

#include <string>
#include <iostream>
#include <vector>

#include "BuildOpset.hh"
#include "FileSystem.hh"
#include "CmdLineOptions.hh"
#include "Environment.hh"
#include "Conversion.hh"
#include "tce_config.h"
#include "OperationBuilder.hh"

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;

const string SCHEMA_FILE_NAME = "Operation_Schema.xsd";

// probably following enumeration should be in a domain-wide OSAL header

/// Enumeration of all reachable paths to operation definitions. The order
/// of paths is hardcoded in Environment::osalPaths().
enum { BASE, USER, CUSTOM, NO_PATH };


//////////////////////////////////////////////////////////////////////////////
// BuildOpsetOptions
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
BuildOpsetOptions::BuildOpsetOptions() :
    CmdLineOptions("Usage: buildopset [options] module_name") {

    string desc = "\n\tInstall the data file and the built dynamic module\n";
    desc += "\tinto one of the allowed paths. Paths are identified by the\n";
    desc += "\tfollowing keywords: base, custom, user.";
    StringCmdLineOptionParser* install =
        new StringCmdLineOptionParser("install", desc, "k");
    addOption(install);

    desc = "\n\tIgnore the case whereby the source file containing the\n";
    desc += "\toperation behavior model code are not found. By default, the\n";
    desc += "\tOSAL Builder aborts if it cannot build the dynamic module.\n";
    desc += "\tThis option may be used in combination with install option\n";
    desc += "\tto install XML data even if operation behavior definitions\n";
    desc += "\tdo not exist.";
    BoolCmdLineOptionParser* ignore = 
	 new BoolCmdLineOptionParser("ignore", desc, "b");
    addOption(ignore);

    desc = "\n\tEnter explicit directory where the behavior definition\n";
    desc += "\tsource file to be used is found.\n";
    StringCmdLineOptionParser* sourceDir =
        new StringCmdLineOptionParser("source-dir", desc, "s");
    addOption(sourceDir);
}

/**
 * Destructor
 */
BuildOpsetOptions::~BuildOpsetOptions() {
}

/**
 * Prints the version of the application.
 */
void
BuildOpsetOptions::printVersion() const {
    cout << "buildopset - Operation behavior module builder " 
         << Application::TCEVersionString() << endl;
}

/**
 * Returns the value of the install option.
 *
 * @return The value of the install option.
 */
string
BuildOpsetOptions::install() const {
    return findOption("install")->String();
}

/**
 * Returns the value of the source-dir option.
 *
 * @return The value of the source-dir option.
 */
string
BuildOpsetOptions::sourceDir() const {
    return findOption("source-dir")->String();
}

/**
 * Returns the value of the ignore option.
 *
 * @return The value of the ignore option.
 */
bool
BuildOpsetOptions::ignore() const {
    return findOption("ignore")->isFlagOn();
}

/**
 * Main program.
 *
 * Searches for XML file given to it as a parameter. Then searches for a
 * corresponding operation behavior source file. It is compiled and then
 * installed.
 */
int main(int argc, char* argv[]) {

    Application::initialize(argc, argv);

    try {
        OperationBuilder& builder = OperationBuilder::instance();
        BuildOpsetOptions *options = new BuildOpsetOptions;

        options->parse(argv, argc);
        Application::setCmdLineOptions(options);

        int arguments = options->numberOfArguments();
        string module = "";
        // first argument is XML data file
        if (arguments == 0 || arguments > 1) {
            options->printHelp();
            return EXIT_FAILURE;
        } else {
            module = options->argument(1);
        }

        // get the path for the files
        if (!FileSystem::isAbsolutePath(module)) {
            module = FileSystem::currentWorkingDir()
                + FileSystem::DIRECTORY_SEPARATOR + module;
        }
        string path = FileSystem::directoryOfPath(module);

        // get the base name of the module
        string moduleName = FileSystem::fileOfPath(module);

        // get the path for the behavior file
        string behaviorPath = options->sourceDir();
        if (behaviorPath == "") {
            behaviorPath = path;
        }

        // verify that the operation properties file is legal
        if (!builder.verifyXML(module + ".opp")) {
            return EXIT_FAILURE;
        }

        // get the file where stuff gets installed
        string installDir = options->install();
        if (installDir == "base") {
            installDir = Environment::osalPaths()[BASE];
        } else if (installDir == "custom") {
            installDir = Environment::osalPaths()[CUSTOM];
        } else if (installDir == "user") {
            installDir = Environment::osalPaths()[USER];
        } else if (installDir == "") {
            installDir = path;
        } else {
            cerr << "Illegal install option: " 
                 << installDir << endl;
            return EXIT_FAILURE;
        }

        // install data file, if destination directory is different
        // than the current directory of data file
        if (installDir != path) {
            if(!builder.installDataFile(path, moduleName + ".opp",
                                        installDir)) {
                string errorMessage = "Cannot install property data file\n'";
                errorMessage += path + FileSystem::DIRECTORY_SEPARATOR
                    + moduleName + ".opp" + "'\n" + "into path\n"
                    + "'" + installDir + "'";
                cerr << errorMessage << endl;
                return EXIT_FAILURE;
            }
        }

        // find the behavior source file and check if it should be ignored
        // if it's not found
        string behFile = builder.behaviorFile(moduleName, behaviorPath);
        if (behFile == "" && !options->ignore()) {
            string errorMessage = "Behavior source file '";
            errorMessage += behaviorPath + FileSystem::DIRECTORY_SEPARATOR
                + moduleName + ".cc" + "' not found";
            cerr << errorMessage << endl;
            return EXIT_FAILURE;
        }

        // build and install the behavior module
        vector<string> output;
        builder.buildObject(moduleName, behFile, installDir, output);
        if (output.size() > 0) {
            cerr << "Error building shared objects:" << endl;
            for (size_t i = 0; i < output.size(); i++) {
                cerr << output[i] << endl;
            }
            return EXIT_FAILURE;
        }

    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& i) { 
        cerr << i.errorMessage() << endl;
        return EXIT_FAILURE;
    } catch (const Exception& e) {
        string linenum = Conversion::toString(e.lineNum());
        cerr << "Exception thrown: " << e.fileName() << ": "
             << linenum << ": " << e.procedureName() << ": "
             << e.errorMessage() << endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
