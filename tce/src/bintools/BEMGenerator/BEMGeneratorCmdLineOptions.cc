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
