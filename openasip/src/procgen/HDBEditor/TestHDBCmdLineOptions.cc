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
 * @file TestHDBCmdLineOptions.cc
 *
 * Implementation of TestHDBCmdLineOptions class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <string>
#include "CmdLineOptions.hh"
#include "TestHDBCmdLineOptions.hh"

using std::string;
const std::string TestHDBCmdLineOptions::FU_ID_PARAM_NAME = "fuid";
const std::string TestHDBCmdLineOptions::RF_ID_PARAM_NAME = "rfid";
const std::string TestHDBCmdLineOptions::VERBOSE_PARAM_NAME = "verbose";
const std::string TestHDBCmdLineOptions::DIRTY_PARAM_NAME = "leave-dirty";
const std::string TestHDBCmdLineOptions::SIM_PARAM_NAME = "simulator";

/**
 * The constructor.
 */
TestHDBCmdLineOptions::TestHDBCmdLineOptions(): CmdLineOptions("") {
    IntegerCmdLineOptionParser* fuEntryID = 
        new IntegerCmdLineOptionParser(
            FU_ID_PARAM_NAME, "Entry id of FU component to be tested. If this"
            " is and RF ID are not defined whole HDB will be tested.", "f");
    addOption(fuEntryID);
    IntegerCmdLineOptionParser* rfEntryID = 
        new IntegerCmdLineOptionParser(
            RF_ID_PARAM_NAME, "Entry id of RF component to be tested. If this"
            " is and FU ID are not defined whole HDB will be tested.", "r");
    addOption(rfEntryID);
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
}

/**
 * The destructor.
 */
TestHDBCmdLineOptions::~TestHDBCmdLineOptions() {
}

/**
 * Returns information whether this option was given
 *
 * @return Is FU entryID defined
 */
bool TestHDBCmdLineOptions::isFUEntryIDGiven() const {
    CmdLineOptionParser* option = findOption(FU_ID_PARAM_NAME);
    return option->isDefined();
}

/**
 * Returns information whether this option was given
 *
 * @return Is RF entryID defined
 */
bool TestHDBCmdLineOptions::isRFEntryIDGiven() const {
    CmdLineOptionParser* option = findOption(RF_ID_PARAM_NAME);
    return option->isDefined();
}

/**
 * Returns information whether this option was given
 *
 * @return Is verbose output defined
 */
bool TestHDBCmdLineOptions::verbose() const {
    CmdLineOptionParser* option = findOption(VERBOSE_PARAM_NAME);
    return option->isFlagOn();
}

/**
 * Returns information whether this option was given
 *
 * @return Is leave dirty defined
 */
bool TestHDBCmdLineOptions::leaveDirty() const {
    CmdLineOptionParser* option = findOption(DIRTY_PARAM_NAME);
    return option->isFlagOn();
}

/** 
 * Returns FU entryID
 *
 * @return The FU entryID
 */
int TestHDBCmdLineOptions::fuEntryID() const {
    CmdLineOptionParser* option = findOption(FU_ID_PARAM_NAME);
    return option->integer();
}

/** 
 * Returns RF entryID
 *
 * @return The RF entryID
 */
int TestHDBCmdLineOptions::rfEntryID() const {
    CmdLineOptionParser* option = findOption(RF_ID_PARAM_NAME);
    return option->integer();
}


/** 
 * Returns HDB file name
 *
 * @return The HDB file name
 */
std::string TestHDBCmdLineOptions::hdbFile() const {
    return argument(numberOfArguments());
}

/** 
 * Returns HDL simulator to be used
 *
 * @return The HDL simulator name
 */
std::string TestHDBCmdLineOptions::vhdlSim() const {
    CmdLineOptionParser* option = findOption(SIM_PARAM_NAME);
    return option->String();
}

/**
 * Prints the version of the application.
 */
void TestHDBCmdLineOptions::printVersion() const {
    std::cout << "testhdb - HDB test utility "
              << Application::TCEVersionString() << std::endl;
}

/**
 * Prints help of the application.
 */
void TestHDBCmdLineOptions::printHelp() const {
    printVersion();
    std::cout << "Usage: testhdb <options> HDB-file" << std::endl;
    CmdLineOptions::printHelp();
}
