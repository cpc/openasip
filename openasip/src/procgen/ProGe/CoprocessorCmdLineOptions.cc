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
 * @file CoprocessorCmdLineOptions.cc
 *
 * Implementation of CoprocessorCmdLineOptions class from ProGeCmdLineOptions
 *
 * @author Tharaka Sampath
 */

#include "CoprocessorCmdLineOptions.hh"

#include <iostream>
#include <sstream>

#include "tce_config.h"

using std::cout;
using std::endl;
using std::string;

const string InterF = "coproInterface";
const string BEM_PARAM_NAME = "bem";
const string IDF_PARAM_NAME = "idf";
const string HDL_PARAM_NAME = "hdl";
const string OUTPUTDIR_PARAM_NAME = "output";
const string ENTITY_NAME = "entity-name";
const string FORCE_OUTPUT = "force-output";
const string PREFER_GEN = "prefer-generation";
const string HDB_LIST = "hdb-list";

/**
 * The constructor.
 */
CoprocessorCmdLineOptions::CoprocessorCmdLineOptions() : CmdLineOptions("") {
    StringCmdLineOptionParser* Interface = new StringCmdLineOptionParser(
        InterF, "Select the Coprocessor Interface :- 'cvx' OR 'rocc'", "c");
    addOption(Interface);

    StringCmdLineOptionParser* bemFile =
        new StringCmdLineOptionParser(BEM_PARAM_NAME, "The BEM file ", "b");
    addOption(bemFile);

    StringCmdLineOptionParser* hdlParam = new StringCmdLineOptionParser(
        HDL_PARAM_NAME, "The language of the HDL to generate. 'vhdl' = VHDL",
        "l");
    addOption(hdlParam);

    StringCmdLineOptionParser* idfFile =
        new StringCmdLineOptionParser(IDF_PARAM_NAME, "The IDF file", "i");
    addOption(idfFile);

    StringCmdLineOptionParser* outputDirectory =
        new StringCmdLineOptionParser(
            OUTPUTDIR_PARAM_NAME, "The output directory", "o");
    addOption(outputDirectory);

    StringCmdLineOptionParser* entityName = new StringCmdLineOptionParser(
        ENTITY_NAME, "Coprocessor TOP name as a String.", "e");
    addOption(entityName);

    BoolCmdLineOptionParser* preferGen = new BoolCmdLineOptionParser(
        PREFER_GEN,
        "Prefer HDL generation over existing HDB implementations.");
    addOption(preferGen);

    StringCmdLineOptionParser* hdbList = new StringCmdLineOptionParser(
        HDB_LIST, "Comma separated list of HDBs for automated generation.",
        "h");
    addOption(hdbList);
}

/**
 * The destructor.
 */
CoprocessorCmdLineOptions::~CoprocessorCmdLineOptions() {}

/**
 * Returns the ADF or PCF file given as last argument.
 *
 * @return The name of the file.
 */
std::string
CoprocessorCmdLineOptions::processorToGenerate() const {
    return argument(numberOfArguments());
}

/**
 * Selects the Interface for the Coprocessor
 */
std::string
CoprocessorCmdLineOptions::interFace() const {
    return findOption(InterF)->String();
}

/**
 * Returns the given BEM file.
 *
 * @return The name of the file.
 */
std::string
CoprocessorCmdLineOptions::bemFile() const {
    return findOption(BEM_PARAM_NAME)->String();
}

/**
 * Returns the given IDF file.
 *
 * @return The name of the file.
 */
std::string
CoprocessorCmdLineOptions::idfFile() const {
    return findOption(IDF_PARAM_NAME)->String();
}

/**
 * Returns the given HDL parameter.
 *
 * @return The HDL parameter.
 */
std::string
CoprocessorCmdLineOptions::hdl() const {
    return findOption(HDL_PARAM_NAME)->String();
}

/**
 * Returns the given output directory.
 *
 * @return The given output directory.
 */
std::string
CoprocessorCmdLineOptions::outputDirectory() const {
    return findOption(OUTPUTDIR_PARAM_NAME)->String();
}

std::string
CoprocessorCmdLineOptions::entityName() const {
    return findOption(ENTITY_NAME)->String();
}

/**
 * Returns true if preferring HDL Generation.
 */
bool
CoprocessorCmdLineOptions::preferHDLGeneration() const {
    return findOption(PREFER_GEN)->isFlagOn();
}

/**
 * Helper for arguments with comma-separated arguments
 */
std::vector<std::string>
CoprocessorCmdLineOptions::commaSeparatedList(
    const std::string argumentName) const {
    std::vector<std::string> list;
    std::string str;
    if (findOption(argumentName)->isDefined()) {
        str = findOption(argumentName)->String();
    }
    std::stringstream ss(str);
    while (ss.good()) {
        std::string sub;
        std::getline(ss, sub, ',');
        if (sub.size() > 1) {
            list.emplace_back(sub);
        }
    }
    return list;
}

/**
 * Return list of HDBs.
 */
std::vector<std::string>
CoprocessorCmdLineOptions::hdbList() const {
    return commaSeparatedList(HDB_LIST);
}

/**
 * Prints the version of the application.
 */
void
CoprocessorCmdLineOptions::printVersion() const {
    std::cout << "generatecoprocessor -  coprocessor generator "
              << Application::TCEVersionString() << std::endl;
}

/**
 * Prints help of the application.
 */
void
CoprocessorCmdLineOptions::printHelp() const {
    printVersion();
    cout << "Usage: generatecoprocessor [options] <coprocessor>" << endl
         << "where <coprocessor> means an ADF file." << endl;
    CmdLineOptions::printHelp();
}
