/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file TestGenCmdLineOptions.cc
 *
 * Implementation of TestGenCmdLineOptions class.
 *
 * Created on: 24.2.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "TestGenCmdLineOptions.hh"

#include "TCEString.hh"
#include "FileSystem.hh"

const TCEString TestGenCmdLineOptions::PROGEOUTDIR_OPT = "hdl-dir";
const TCEString TestGenCmdLineOptions::OUTPUTDIR_OPT = "output";
const TCEString TestGenCmdLineOptions::PROGRAMS_OPT = "program";
const TCEString TestGenCmdLineOptions::LIST_GENERATORS = "list-generators";
const TCEString TestGenCmdLineOptions::ENABLE_LIST_OPT = "enable";
const TCEString TestGenCmdLineOptions::DISABLE_LIST_OPT = "disable";
const TCEString TestGenCmdLineOptions::DISABLE_ALL_OPT = "disable-all";
const TCEString TestGenCmdLineOptions::SEED_OPT = "seed";
const TCEString TestGenCmdLineOptions::ADF_OPT = "adf";
const TCEString TestGenCmdLineOptions::IDF_OPT = "idf";
const TCEString TestGenCmdLineOptions::IMAGE_FORMAT_OPT = "format";
const TCEString TestGenCmdLineOptions::DMEM_WIDTH_IN_MAUS_OPT =
    "dmemwidthinmaus";

/**
 * Constructor. Adds command line parameters.
 */
TestGenCmdLineOptions::TestGenCmdLineOptions()
    : CmdLineOptions("") {

    addOption(new StringCmdLineOptionParser(
        ADF_OPT,
        "The target architecture for tests. The option is Required. ",
        "a"));
    addOption(new StringCmdLineOptionParser(
        IDF_OPT,
        "The implementation definition for target architecture. The option is "
        "optional but implicitly disables some test generators if left out",
        "i"));
    addOption(new BoolCmdLineOptionParser(
        LIST_GENERATORS,
        "Lists the test generators with their names, descriptions "
        "and activation status.",
        "l"));
    addOption(new StringListCmdLineOptionParser(
        ENABLE_LIST_OPT,
        "Enables test generators. Format of LIST is "
        "comma separated of names of test generators.", "e"));
    addOption(new StringListCmdLineOptionParser(
        DISABLE_LIST_OPT,
        "Disables test generators. Format of LIST is "
        "Format of LIST is comma separated of names of "
        "test generators.", "d"));
    addOption(new BoolCmdLineOptionParser(
        DISABLE_ALL_OPT,
        "Disables all test generators. This option can be chained with -e.",
        "D"));
    addOption(new OptionalStringCmdLineOptionParser(
        SEED_OPT,
        "Uses VALUE as seed for random number generator "
        "to generate determined set of test cases. "
        "If VALUE is empty the seed is generated randomly. "
        "If option is not specified a default "
        "seed value is used instead.", "s"));
    addOption(new StringCmdLineOptionParser(
        OUTPUTDIR_OPT,
        "Additional output directory where assembly files of "
        "generated tests are placed.", "o"));
    addOption(new StringCmdLineOptionParser(
        PROGEOUTDIR_OPT,
        "Directory root where ProGe generated HDL files. "
        "GenerateTests will place test files under its tb/ directory", "x"));
    addOption(new StringListCmdLineOptionParser(
        IMAGE_FORMAT_OPT,
        "Output format of program and data images."
        "Choices are 'ascii', 'array', 'mif', 'coe', 'vhdl' or 'binary'. "
        "Default is 'ascii' ", "f"));
    addOption(new StringListCmdLineOptionParser(
        PROGRAMS_OPT,
        "Additional test programs to be included in tests.",
        "p"));
    addOption(new BoolCmdLineOptionParser(
        "help", "Print this text.", "h"));
    addOption(new IntegerCmdLineOptionParser(
        DMEM_WIDTH_IN_MAUS_OPT,
        "Width of data memory in MAUs. Default is 1.", "w"));

}

/**
 * Destructor.
 */
TestGenCmdLineOptions::~TestGenCmdLineOptions() {
}


/**
 * Defines options rules for this class.
 *
 * Checks that given options are valid and required options are present.
 *
 * @exception IllegalCommandLine If there are invalid or missing options.
 */
void
TestGenCmdLineOptions::validate() const {
    std::string msg;
    bool allOk = true;

    if (!adfDefined()) {
        allOk &= adfDefined();
        msg += "ADF is required. Use -a ADF option.\n";
    }

    //todo check for valid formats

    if (!allOk) {
        throw IllegalCommandLine(__FILE__, __LINE__,
            "TestGenCmdLineOptions::validate()", msg);
    }
}

/**
 * Prints the version of the application.
 */
void TestGenCmdLineOptions::printVersion() const {
    std::cout << "generatetests - Automated Test Generator Utility "
              << Application::TCEVersionString() << std::endl;
}

/**
 * Returns true if ADF option is defined.
 */
bool
TestGenCmdLineOptions::adfDefined() const {
    if (!optionGiven(ADF_OPT)) {
        return false;
    }
    return true;
}

/**
 * Returns file path to the ADF.
 */
Path TestGenCmdLineOptions::adfFilePath() const {
    assert(adfDefined());
    Path adfPath(findOption(ADF_OPT)->String(0));
    return adfPath;
}

/**
 * Returns true if IDF option is defined.
 */
bool
TestGenCmdLineOptions::idfDefined() const {
    return optionGiven(IDF_OPT);
}

/**
 * Returns file path to the IDF.
 */
Path
TestGenCmdLineOptions::idfFilePath() const {
    assert(idfDefined());
    Path idfPath(findOption(IDF_OPT)->String(0));
    return idfPath;
}

/**
 * Prints help of the application.
 */
void TestGenCmdLineOptions::printHelp() const {
    printVersion();
    std::cout << "Usage: generatetests [options]... -a ADF";
    std::cout << std::endl;
    CmdLineOptions::printHelp();
}

/**
 * Returns the given width of data memory in MAUs.
 *
 * @return The width.
 */
int
TestGenCmdLineOptions::dataMemoryWidthInMAUs() const {
    CmdLineOptionParser* option = findOption(
        DMEM_WIDTH_IN_MAUS_OPT);
    if (option->isDefined()) {
        return option->integer();
    } else {
        return 1;
    }
}

/**
 * returns true if output directory option is defined (-o).
 */
bool
TestGenCmdLineOptions::outputDirDefined() const {
    return optionGiven(OUTPUTDIR_OPT);
}

/**
 * returns output directory (-o).
 */
Path
TestGenCmdLineOptions::outputDir() const {
    assert(outputDirDefined());
    return Path(findOption(OUTPUTDIR_OPT)->String(0));
}

/**
 * Returns true if proge output directory option is defined (-x).
 */
bool
TestGenCmdLineOptions::progeOutputDirDefined() const {
    return optionGiven(PROGEOUTDIR_OPT);
}

/**
 * Returns base directory path of ProGe output.
 */
Path
TestGenCmdLineOptions::progeOutputDir() const {
    assert(progeOutputDirDefined());
    Path adfPath(findOption(PROGEOUTDIR_OPT)->String(0));
    return adfPath;
}

/**
 * Returns set of custom test program filepaths provided by user.
 */
std::set<Path>
TestGenCmdLineOptions::programFiles() const {
    const CmdLineOptionParser& progOptParser = *findOption(PROGRAMS_OPT);
    std::set<Path> progSet;
    for (int i = 1; i <= progOptParser.listSize(); i++) {
        Path progFile(progOptParser.String(i));
        if (progFile.has_filename()) {
            progSet.insert(progFile);
        }
    }
    return progSet;
}

/**
 * True if application should print all test generator descriptions.
 */
bool
TestGenCmdLineOptions::listGeneratorsDefined() const {
    return optionGiven(LIST_GENERATORS);
}

/**
 * Item count of test generators to be enabled.
 */
size_t
TestGenCmdLineOptions::toEnabledListSize() const {
    if (optionGiven(ENABLE_LIST_OPT)) {
        return findOption(ENABLE_LIST_OPT)->listSize();
    } else {
        return 0;
    }
}

/**
 * Return name of test generator to be enabled at index
 * in range [0, toEnabledListSize()].
 */
std::string
TestGenCmdLineOptions::toEnabled(size_t index) const {
    assert(index < toEnabledListSize());
    assert(optionGiven(ENABLE_LIST_OPT));
    return findOption(ENABLE_LIST_OPT)->String(index + 1);
}

/**
 * Item count of test generators to be disabled.
 */
size_t
TestGenCmdLineOptions::toDisabledListSize() const {
    if (optionGiven(DISABLE_LIST_OPT)) {
        return findOption(DISABLE_LIST_OPT)->listSize();
    } else {
        return 0;
    }
}

/**
 * Return name of test generator to be disabled at index
 * in range [0, toDisabledListSize()].
 */
std::string
TestGenCmdLineOptions::toDisabled(size_t index) const {
    assert(index < toDisabledListSize());
    assert(optionGiven(DISABLE_LIST_OPT));
    return findOption(DISABLE_LIST_OPT)->String(index + 1);
}

/**
 * Return true if all test generators should be disabled.
 */
bool
TestGenCmdLineOptions::disableAllTestGenerators() const {
    return optionGiven(DISABLE_ALL_OPT);
}

/**
 * True if user provides seed value.
 */
bool
TestGenCmdLineOptions::seedDefined() const {
    return optionGiven(SEED_OPT);
}

/**
 * The given seed value that may be non-numerical.
 */
std::string
TestGenCmdLineOptions::getSeed() const {
    return findOption(SEED_OPT)->String(0);
}

/**
 * Returns program image format.
 */
ProgramImageGenerator::OutputFormat
TestGenCmdLineOptions::programImageFormat() const {
    if (!optionGiven(IMAGE_FORMAT_OPT)) {
        return ProgramImageGenerator::ASCII;
    } else {
        const std::string format = findOption(IMAGE_FORMAT_OPT)->String(1);
        if (format == "ascii") {
            return ProgramImageGenerator::ASCII;
        } else if (format == "array") {
            return ProgramImageGenerator::ARRAY;
        } else if (format == "mif") {
            return ProgramImageGenerator::MIF;
        } else if (format == "coe") {
            return ProgramImageGenerator::COE;
        } else if (format == "vhdl") {
            return ProgramImageGenerator::VHDL;
        } else if (format == "binary") {
            return ProgramImageGenerator::BINARY;
        } else {
            throw IllegalCommandLine(__FILE__, __LINE__, __func__,
                "Unknown image output format: " + format);
        }
    }
}

/**
 * Returns program image format.
 */
ProgramImageGenerator::OutputFormat
TestGenCmdLineOptions::dataImageFormat() const {
    try {
        return programImageFormat(); //note: placeholder
    } catch (IllegalCommandLine& e) {
        throw IllegalCommandLine(__FILE__, __LINE__, __func__,
            e.errorMessage());
    }
}
