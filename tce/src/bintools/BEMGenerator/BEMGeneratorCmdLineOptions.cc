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
 * @file BEMGeneratorCmdLineOptions.cc
 *
 * Implementation of BEMGeneratorCmdLineOptions class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "BEMGeneratorCmdLineOptions.hh"
#include "BEMGeneratorCLITextGenerator.hh"

const std::string BEMGeneratorCmdLineOptions::ADF_PARAM_NAME = "adf";
const std::string BEMGeneratorCmdLineOptions::OUTPUT_FILE_PARAM_NAME = 
    "output";

/**
 * The constructor.
 */
BEMGeneratorCmdLineOptions::BEMGeneratorCmdLineOptions() : 
    CmdLineOptions("") {

    StringCmdLineOptionParser* outputFile = new StringCmdLineOptionParser(
        OUTPUT_FILE_PARAM_NAME, "The name of the output file", "o");
    addOption(outputFile);
}


/**
 * The destructor.
 */
BEMGeneratorCmdLineOptions::~BEMGeneratorCmdLineOptions() {
}


/**
 * Returns the name of the ADF file given as command line parameter.
 *
 * @return The name of the ADF file.
 */
std::string
BEMGeneratorCmdLineOptions::adfFile() const {
    if (numberOfArguments() < 1) {
        return "";
    } else {
        return argument(numberOfArguments());
    }
}


/**
 * Returns the name of the output file given as command line parameter.
 *
 * @return The name of the output file.
 */
std::string
BEMGeneratorCmdLineOptions::outputFile() const {
    return findOption(OUTPUT_FILE_PARAM_NAME)->String();
}


/**
 * Prints the version of the user interface.
 */
void
BEMGeneratorCmdLineOptions::printVersion() const {
    BEMGeneratorCLITextGenerator textGenerator;
    std::cout 
        << textGenerator.text(BEMGeneratorCLITextGenerator::TXT_CLI_TITLE).
        str() 
        << " " 
        << textGenerator.text(
            BEMGeneratorCLITextGenerator::TXT_CLI_VERSION).str() 
        << std::endl;
}


/**
 * Prints the help.
 */
void
BEMGeneratorCmdLineOptions::printHelp() const {
    printVersion();
    std::cout << "Usage: createbem [OPTION] <ADF File>";
    CmdLineOptions::printHelp();
}
