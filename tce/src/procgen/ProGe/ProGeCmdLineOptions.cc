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
 * @file ProGeCmdLineOptions.cc
 *
 * Implementation of ProGeCmdLineOptions class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include "ProGeCmdLineOptions.hh"
#include "config.h"

using std::string;
using std::cout;
using std::endl;

const string BEM_PARAM_NAME = "bem";
const string IDF_PARAM_NAME = "idf";
const string ICDECODER_PARAM_NAME = "gen";
const string HDL_PARAM_NAME = "hdl";
const string DECOMPRESSOR_PARAM_NAME = "decomp";
const string OUTPUTDIR_PARAM_NAME = "output";
const string IMEM_WIDTH_IN_MAUS_PARAM_NAME = "imemwidthinmaus";
const string PLUGIN_PARAMETERS_PARAM_NAME = "pluginparameters";

/**
 * The constructor.
 */
ProGeCmdLineOptions::ProGeCmdLineOptions() : 
    CmdLineOptions("") {

    StringCmdLineOptionParser* bemFile = new StringCmdLineOptionParser(
        BEM_PARAM_NAME, "The BEM file", "b");
    addOption(bemFile);
    StringCmdLineOptionParser* idfFile = new StringCmdLineOptionParser(
        IDF_PARAM_NAME, "The IDF file", "i");
    addOption(idfFile);
    StringCmdLineOptionParser* hdlParam = new StringCmdLineOptionParser(
        HDL_PARAM_NAME, "The HDL to generate. 'vhdl' = VHDL", "l");
    addOption(hdlParam);
    StringCmdLineOptionParser* outputDirectory = 
        new StringCmdLineOptionParser(
            OUTPUTDIR_PARAM_NAME, "The output directory", "o");
    addOption(outputDirectory);
    IntegerCmdLineOptionParser* imemWidth = new IntegerCmdLineOptionParser(
        IMEM_WIDTH_IN_MAUS_PARAM_NAME, 
        "Width of instruction memory in MAUs. Default value is 1.", "w");
    addOption(imemWidth);
    StringCmdLineOptionParser* pluginParameters = 
        new StringCmdLineOptionParser(
            PLUGIN_PARAMETERS_PARAM_NAME, "List plugin parameters for an "
            "IC/Decoder generator plugin file.", "u");
    addOption(pluginParameters);
}


/**
 * The destructor.
 */
ProGeCmdLineOptions::~ProGeCmdLineOptions() {
}


/**
 * Returns the ADF or PCF file given as last argument.
 *
 * @return The name of the file.
 */
std::string
ProGeCmdLineOptions::processorToGenerate() const {
    return argument(numberOfArguments());
}


/**
 * Returns the given BEM file.
 *
 * @return The name of the file.
 */
std::string
ProGeCmdLineOptions::bemFile() const {
    return findOption(BEM_PARAM_NAME)->String();
}


/**
 * Returns the given IDF file.
 *
 * @return The name of the file.
 */
std::string
ProGeCmdLineOptions::idfFile() const {
    return findOption(IDF_PARAM_NAME)->String();
}


/**
 * Returns the given HDL parameter.
 *
 * @return The HDL parameter.
 */
std::string
ProGeCmdLineOptions::hdl() const {
    return findOption(HDL_PARAM_NAME)->String();
}


/**
 * Returns the given instruction memory width in MAUs.
 *
 * @return The width.
 */
int
ProGeCmdLineOptions::imemWidthInMAUs() const {
    CmdLineOptionParser* option = findOption(
        IMEM_WIDTH_IN_MAUS_PARAM_NAME);
    if (option->isDefined()) {
        return option->integer();
    } else {
        return 1;
    }
}

/**
 * Returns the given output directory.
 *
 * @return The given output directory.
 */
std::string
ProGeCmdLineOptions::outputDirectory() const {
    return findOption(OUTPUTDIR_PARAM_NAME)->String();
}

/**
 * Returns the IC/decoder generator plugin parameter list query.
 *
 * @return The plugin parameter query.
 */
std::string
ProGeCmdLineOptions::pluginParametersQuery() const {
    return findOption(PLUGIN_PARAMETERS_PARAM_NAME)->String();
}


/**
 * Prints the version of the application.
 */
void
ProGeCmdLineOptions::printVersion() const {
    std::cout << "generateprocessor - TCE Processor Generator "
              << Application::TCEVersionString() << std::endl;
}


/**
 * Prints help of the application.
 */
void
ProGeCmdLineOptions::printHelp() const {
    printVersion();
    cout << "Usage: generateprocessor [options] <processor>" << endl
         << "where <processor> means either an ADF or PCF file." << endl; 
    CmdLineOptions::printHelp();
}
