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
 * @file BEMGeneratorCmdLineOptions.cc
 *
 * Implementation of BEMGeneratorCmdLineOptions class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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
