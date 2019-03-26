/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file PIGCmdLineOptions.cc
 *
 * Implementation of PIGCmdLineOptions class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011
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
const std::string PIGCmdLineOptions::COMPRESSOR_PARAMS_PARAM_NAME = 
    "compressor param";
const std::string PIGCmdLineOptions::SHOW_COMPRESSORS_PARAM_NAME =
    "showcompressors";
const std::string PIGCmdLineOptions::HDL_OUTPUT_DIR = "hdl-dir";
const string ENTITY_NAME = "entity-name";

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
        "The output format of program image(s) ('ascii', 'array', 'mif', "
        "'coe', 'vhdl', 'hex' or 'binary'). Default is 'ascii'.", "f");
    addOption(piOutputMode);

    StringCmdLineOptionParser* diOutputMode = new StringCmdLineOptionParser(
        DI_FORMAT_PARAM_NAME, "The output format of data image(s) "
        "('ascii', 'array', 'mif', 'coe', 'vhdl', 'hex' or 'binary'). Default "
        "is 'ascii'.", "o");
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

    StringListCmdLineOptionParser* compressorParams = 
        new StringListCmdLineOptionParser(
            COMPRESSOR_PARAMS_PARAM_NAME,
            "Parameter to the code compressor in form 'name=value'.", "u");
    addOption(compressorParams);

    BoolCmdLineOptionParser* showCompressors = new BoolCmdLineOptionParser(
        SHOW_COMPRESSORS_PARAM_NAME, "Show compressor plugin descriptions.",
        "s");
    addOption(showCompressors);

    string hdlDirDesc("Directory root where ProGe generated HDL files. "
                      "Generatebits will write imem_mau_pkg and "
                      "decompressor, if it is needed, under the given "
                      "directory. Otherwise they are written to cwd.");

    StringCmdLineOptionParser* hdlDir = new StringCmdLineOptionParser(
        HDL_OUTPUT_DIR, hdlDirDesc, "x");
    addOption(hdlDir);

    StringCmdLineOptionParser* entityName = 
        new StringCmdLineOptionParser(
            ENTITY_NAME, 
            "String to use to make the generated VHDL entities unique. This "
            "should be the same which was given to ProGe when the processor "
            "was generated (default is 'tta0').", "e");
    addOption(entityName);
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
 * Returns the proge output directory. Empty if not given
 *
 * @return proge output directory
 */
std::string
PIGCmdLineOptions::progeOutputDirectory() const {
    return findOption(HDL_OUTPUT_DIR)->String();
}

std::string
PIGCmdLineOptions::entityName() const {
    return findOption(ENTITY_NAME)->String();
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
