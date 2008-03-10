/**
 * @file BEMGeneratorCLITextGenerator.cc
 *
 * Implementation of BEMGeneratorCLITextGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "BEMGeneratorCLITextGenerator.hh"
#include "config.h"

/**
 * The constructor.
 */
BEMGeneratorCLITextGenerator::BEMGeneratorCLITextGenerator() {
    addText(
        TXT_CLI_TITLE, 
        "createbem - TCE Binary Encoding Map Generator command line "
        "interface");
    addText(TXT_CLI_VERSION, VERSION);
}


/**
 * The destructor.
 */
BEMGeneratorCLITextGenerator::~BEMGeneratorCLITextGenerator() {
}

