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
 * @file TTAUnitTesterCmdLineOptions.cc
 *
 * Implementation of TestHDBCmdLineOptions class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <string>
#include "CmdLineOptions.hh"
#include "TTAUnitTesterCmdLineOptions.hh"

const std::string TTAUnitTesterCmdLineOptions::VERBOSE_PARAM_NAME = "verbose";
const std::string TTAUnitTesterCmdLineOptions::DIRTY_PARAM_NAME = 
    "leave-dirty";
const std::string TTAUnitTesterCmdLineOptions::SIM_PARAM_NAME = "simulator";
const std::string TTAUnitTesterCmdLineOptions::ADF_PARAM_NAME = "adf";

TTAUnitTesterCmdLineOptions::TTAUnitTesterCmdLineOptions(): 
    CmdLineOptions("") {

    BoolCmdLineOptionParser* verbose =
        new BoolCmdLineOptionParser(
            VERBOSE_PARAM_NAME, "Enable verbose output", "v");
    addOption(verbose);
    BoolCmdLineOptionParser* dirty =
        new BoolCmdLineOptionParser(
            DIRTY_PARAM_NAME, "Don't delete created files", "d");
    addOption(dirty);
    StringCmdLineOptionParser* simulator = 
        new StringCmdLineOptionParser(
            SIM_PARAM_NAME, "HDL simulator used to simulate testbench. "
            "Accepted values are 'ghdl' and 'modelsim'. Default is ghdl",
            "s");
    addOption(simulator);
    StringCmdLineOptionParser* adfFile =
        new StringCmdLineOptionParser(
            ADF_PARAM_NAME, "If ADF file is given IDF will be validated",
            "a");
    addOption(adfFile);
            
}

TTAUnitTesterCmdLineOptions::~TTAUnitTesterCmdLineOptions() {
}


bool
TTAUnitTesterCmdLineOptions::verbose() const {
    CmdLineOptionParser* option = findOption(VERBOSE_PARAM_NAME);
    return option->isFlagOn();
}


bool
TTAUnitTesterCmdLineOptions::leaveDirty() const {
    CmdLineOptionParser* option = findOption(DIRTY_PARAM_NAME);
    return option->isFlagOn();
}


std::string 
TTAUnitTesterCmdLineOptions::vhdlSim() const {
    CmdLineOptionParser* option = findOption(SIM_PARAM_NAME);
    return option->String();
}


std::string
TTAUnitTesterCmdLineOptions::adfFileName() const {
    CmdLineOptionParser* option = findOption(ADF_PARAM_NAME);
    return option->String();
}


std::string
TTAUnitTesterCmdLineOptions::idfFileName() const {
     return argument(numberOfArguments());
}

void
TTAUnitTesterCmdLineOptions::printVersion() const {
    std::cout << "ttaunittester "
              << Application::TCEVersionString() << std::endl;
}


void
TTAUnitTesterCmdLineOptions::printHelp() const {
    printVersion();
    std::cout << "Usage: ttaunittester <options> IDF-file" << std::endl;
    CmdLineOptions::printHelp();
}

