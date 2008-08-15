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
 * @file PIGCmdLineOptions.cc
 *
 * Implementation of PIGCmdLineOptions class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "PIGCmdLineOptions.hh"
#include "PIGCLITextGenerator.hh"

using std::string;

const std::string PIGCmdLineOptions::BEM_PARAM_NAME = "bem";
const std::string PIGCmdLineOptions::TPEF_PARAM_NAME = "program";
const std::string PIGCmdLineOptions::PI_FORMAT_PARAM_NAME = "piformat";
const std::string PIGCmdLineOptions::DI_FORMAT_PARAM_NAME = "diformat";
const std::string PIGCmdLineOptions::COMPRESSOR_PARAM_NAME = "compressor";
const std::string PIGCmdLineOptions::DATA_IMG_PARAM_NAME = "dataimages";
const std::string PIGCmdLineOptions::GEN_DECOMP_PARAM_NAME = "decompressor";
const std::string PIGCmdLineOptions::DMEM_WIDTH_IN_MAUS_PARAM_NAME = 
    "dmemwidthinmaus";
const std::string PIGCmdLineOptions::IMEM_WIDTH_IN_MAUS_PARAM_NAME =
    "imemwidthinmaus";
const std::string PIGCmdLineOptions::COMPRESSOR_PARAMS_PARAM_NAME = 
    "compressor param";
const std::string PIGCmdLineOptions::SHOW_COMPRESSORS_PARAM_NAME =
    "showcompressors";

/**
 * The constructor.
 */
PIGCmdLineOptions::PIGCmdLineOptions() : CmdLineOptions("") {
    StringListCmdLineOptionParser* tpefFile = 
        new StringListCmdLineOptionParser(
            TPEF_PARAM_NAME, "The TPEF program file(s)", "p");
    addOption(tpefFile);
    StringCmdLineOptionParser* bemFile = new StringCmdLineOptionParser(
        BEM_PARAM_NAME, "The BEM file", "b");
    addOption(bemFile);
    StringCmdLineOptionParser* piOutputMode = new StringCmdLineOptionParser(
        PI_FORMAT_PARAM_NAME,
        "The output format of program image(s) ('ascii', 'array' or "
        "'binary'). Default is 'ascii'.", "f");
    addOption(piOutputMode);
    StringCmdLineOptionParser* diOutputMode = new StringCmdLineOptionParser(
        DI_FORMAT_PARAM_NAME, "The output format of data image(s) "
        "('ascii', 'array' or 'binary'). Default is 'ascii'.", "o");
    addOption(diOutputMode);
    StringCmdLineOptionParser* pluginFile = new StringCmdLineOptionParser(
        COMPRESSOR_PARAM_NAME, "Name of the code compressor plugin file.", 
        "c");
    addOption(pluginFile);
    BoolCmdLineOptionParser* createDataImages = new BoolCmdLineOptionParser(
        DATA_IMG_PARAM_NAME, 
        "Create data images.",
        "d");
    addOption(createDataImages);
    BoolCmdLineOptionParser* createDecompressor = 
        new BoolCmdLineOptionParser(
            GEN_DECOMP_PARAM_NAME, "Generate decompressor block.", "g");
    addOption(createDecompressor);
    IntegerCmdLineOptionParser* dmemMAUsPerLine = 
        new IntegerCmdLineOptionParser(
            DMEM_WIDTH_IN_MAUS_PARAM_NAME,
            "Width of data memory in MAUs. Default is 1.", "w");
    addOption(dmemMAUsPerLine);
    IntegerCmdLineOptionParser* imemMAUsPerLine =
        new IntegerCmdLineOptionParser(
            IMEM_WIDTH_IN_MAUS_PARAM_NAME,
            "Width of instruction memory in MAUs. Affects the ASCII output "
            "formats. If not given, each instruction is printed on "
            "different line.", "i");
    addOption(imemMAUsPerLine);
    StringListCmdLineOptionParser* compressorParams = 
        new StringListCmdLineOptionParser(
            COMPRESSOR_PARAMS_PARAM_NAME,
            "Parameter to the code compressor in form 'name=value'.", "u");
    addOption(compressorParams);
    BoolCmdLineOptionParser* showCompressors = new BoolCmdLineOptionParser(
        SHOW_COMPRESSORS_PARAM_NAME, "Show compressor plugin descriptions.",
        "s");
    addOption(showCompressors);
}


/**
 * The destructor.
 */
PIGCmdLineOptions::~PIGCmdLineOptions() {
}


/**
 * Returns the name of the BEM file given as command line parameter.
 *
 * @return The name of the BEM file.
 */
std::string
PIGCmdLineOptions::bemFile() const {
    return findOption(BEM_PARAM_NAME)->String();
}


/**
 * Returns the number of TPEF files given.
 *
 * @return The number of TPEF files.
 */
int
PIGCmdLineOptions::tpefFileCount() const {
    return findOption(TPEF_PARAM_NAME)->listSize();
}


/**
 * Returns the name of the TPEF file given as command line parameter.
 *
 * @return The name of the TPEF file.
 */
std::string
PIGCmdLineOptions::tpefFile(int index) const
    throw (OutOfRange) {

    CmdLineOptionParser* option = findOption(TPEF_PARAM_NAME);
    if (index < 0 || index >= tpefFileCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return option->String(index + 1);
}


/**
 * Returns the program image output format given as command line parameter.
 *
 * @return The output format.
 */
std::string
PIGCmdLineOptions::programImageOutputFormat() const {
    return findOption(PI_FORMAT_PARAM_NAME)->String();
}


/**
 * Returns the data image output format given as command line parameter.
 *
 * @return The output format.
 */
std::string
PIGCmdLineOptions::dataImageOutputFormat() const {
    return findOption(DI_FORMAT_PARAM_NAME)->String();
}


/**
 * Returns the code compressor plugin file name.
 *
 * @return The plugin file.
 */
std::string
PIGCmdLineOptions::compressorPlugin() const {
    return findOption(COMPRESSOR_PARAM_NAME)->String();
}


/**
 * Returns the given width of data memory in MAUs.
 *
 * @return The width.
 */
int
PIGCmdLineOptions::dataMemoryWidthInMAUs() const {
    CmdLineOptionParser* option = findOption(
        DMEM_WIDTH_IN_MAUS_PARAM_NAME);
    if (option->isDefined()) {
        return option->integer();
    } else {
        return 1;
    }
}


/**
 * Returns the given width of instruction memory in MAUs.
 *
 * @return The width.
 */
int
PIGCmdLineOptions::instructionMemoryWidthInMAUs() const {
    CmdLineOptionParser* option = findOption(IMEM_WIDTH_IN_MAUS_PARAM_NAME);
    if (option->isDefined()) {
        return option->integer();
    } else {
        return 0;
    }
}


/**
 * Tells whether to create data images or not.
 */
bool
PIGCmdLineOptions::generateDataImages() const {
    return findOption(DATA_IMG_PARAM_NAME)->isFlagOn();
}


/**
 * Tells whether to generate decompressor block or not.
 */
bool
PIGCmdLineOptions::generateDecompressor() const {
    return findOption(GEN_DECOMP_PARAM_NAME)->isFlagOn();
}


/**
 * Returns the number of parameters given to compressor.
 *
 * @return The number of parameters.
 */
int
PIGCmdLineOptions::compressorParameterCount() const {
    return findOption(COMPRESSOR_PARAMS_PARAM_NAME)->listSize();
}


/**
 * By the given index, returns a parameter given to code compressor.
 *
 * @param index The index.
 * @exception OutOfRange If the given index is negative or not smaller than
 *                       the number of parameters given to compressor.
 */
std::string
PIGCmdLineOptions::compressorParameter(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= compressorParameterCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    return findOption(COMPRESSOR_PARAMS_PARAM_NAME)->String(index + 1);
}


/**
 * Tells whether to show code compressor descriptions or not.
 */
bool
PIGCmdLineOptions::showCompressors() const {
    return findOption(SHOW_COMPRESSORS_PARAM_NAME)->isFlagOn();
}


/**
 * Prints the version of the user interface.
 */
void
PIGCmdLineOptions::printVersion() const {
    PIGCLITextGenerator textGenerator;
    std::cerr << 
        textGenerator.text(PIGCLITextGenerator::TXT_CLI_TITLE).str() 
              << " " 
              << textGenerator.text(PIGCLITextGenerator::TXT_CLI_VERSION).
        str() << std::endl;
}


/**
 * Prints the help menu of the program.
 */
void
PIGCmdLineOptions::printHelp() const {
    printVersion();
    printUsage();
    CmdLineOptions::printHelp();
}


/**
 * Prints the usage of the CLI.
 */
void
PIGCmdLineOptions::printUsage() const {
    PIGCLITextGenerator textGen;
    std::cerr << textGen.text(PIGCLITextGenerator::TXT_CLI_USAGE).str() 
              << std::endl;
}
