/**
 * @file PIGCLITextGenerator.cc
 *
 * Implementation of PIGCLITextGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "PIGCLITextGenerator.hh"
#include "config.h"

/**
 * The constructor.
 */
PIGCLITextGenerator::PIGCLITextGenerator() {
    addText(
        TXT_CLI_TITLE,
        "generatebits - TCE Program Image Generator command line interface");
    addText(TXT_CLI_VERSION, VERSION);
    addText(TXT_CLI_USAGE, "Usage: generatebits <options> ADF");
    addText(
        TXT_GENERATING_BEM, 
        "BEM file was not given --> Generating default BEM..");
    addText(TXT_ADF_REQUIRED, "ADF file is required.");
    addText(TXT_ILLEGAL_ARGS, "Illegal command line arguments.");
}


/**
 * The destructor.
 */
PIGCLITextGenerator::~PIGCLITextGenerator() {
}

