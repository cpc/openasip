/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file ProGeCmdLineOptions.cc
 *
 * Implementation of ProGeCmdLineOptions class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
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
const string OUTPUTDIR_PARAM_NAME = "output";
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
