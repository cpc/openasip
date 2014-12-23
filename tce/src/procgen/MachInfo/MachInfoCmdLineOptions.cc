/*
    Copyright (c) 2014 Tampere University of Technology.

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
 * @file MachInfoCmdLineOptions.cc
 *
 * @author Pekka Jääskeläinen 2014
 */

#include <iostream>
#include "MachInfoCmdLineOptions.hh"
#include "tce_config.h"

using std::string;
using std::cout;
using std::endl;

// Defines to avoid issues with global object (string) initialization order.
#define OUTPUTFN_PARAM_NAME  "output-suffix"
#define OUTPUT_FORMAT_PARAM_NAME "output-format"

MachInfoCmdLineOptions::MachInfoCmdLineOptions() : 
    CmdLineOptions("") {

    addOption(
        new StringCmdLineOptionParser(
            OUTPUTFN_PARAM_NAME,
            "The suffix of the filenames of the produced documentation files.",
            "s"));
    addOption(
        new StringCmdLineOptionParser(
            OUTPUT_FORMAT_PARAM_NAME,
            "The format of the produced documentation (only 'latex' supported "
            "for now).", "f"));

}


MachInfoCmdLineOptions::~MachInfoCmdLineOptions() {
}

TCEString
MachInfoCmdLineOptions::outputFileNameSuffix() const {
    if (!findOption(OUTPUTFN_PARAM_NAME)->isDefined()) {
        if (numberOfArguments() == 1) return argument(1);
    }
    return findOption(OUTPUTFN_PARAM_NAME)->String();
}

TCEString
MachInfoCmdLineOptions::outputFormat() const {
    if (!findOption(OUTPUT_FORMAT_PARAM_NAME)->isDefined())
        return "latex";
    return findOption(OUTPUT_FORMAT_PARAM_NAME)->String();
}


/**
 * Prints the version of the application.
 */
void
MachInfoCmdLineOptions::printVersion() const {
    std::cout << "machinfo - processor design documentation generator "
              << Application::TCEVersionString() << std::endl;
}


void
MachInfoCmdLineOptions::printHelp() const {
    printVersion();
    cout << "Usage: machinfo [options] ADF" << endl;
    CmdLineOptions::printHelp();
}
