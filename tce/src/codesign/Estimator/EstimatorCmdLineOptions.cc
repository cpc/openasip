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
 * @file EstimatorCmdLineOptions.cc
 *
 * Declaration of EstimatorCmdLineOptions.
 *
 * @author Pekka J��skel�inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "CmdLineOptions.hh"
#include "EstimatorCmdLineOptions.hh"
#include "tce_config.h"


/// Long switch string for setting the TPEF (program).
const std::string SWL_TPEF = "program";
/// Short switch string for setting the TPEF (program).
const std::string SWS_TPEF = "p";
/// Long switch string for setting the TraceDB.
const std::string SWL_TRACE = "trace";
/// Short switch string for setting the TraceDB.
const std::string SWS_TRACE = "t";

/// Long switch string for setting the total area estimation.
const std::string SWL_TOTALAREA = "total-area";
/// Short switch string for setting the total area estimation.
const std::string SWS_TOTALAREA = "a";
/// Long switch string for setting the longest path estimation.
const std::string SWL_LONGESTPATH = "longest-path";
/// Short switch string for setting the longest path estimation.
const std::string SWS_LONGESTPATH = "l";
/// Long switch string for setting the total energy consumed estimation.
const std::string SWL_TOTALENERGY = "total-energy";
/// Short switch string for setting the total energy consumed estimation.
const std::string SWS_TOTALENERGY = "e";

/**
 * Constructor.
 */
EstimatorCmdLineOptions::EstimatorCmdLineOptions() : CmdLineOptions("") {
    addOption(
        new StringCmdLineOptionParser(
            SWL_TPEF, 
            "sets the TTA program exchange format file (TPEF) from which to "
            "load the estimated program (required for energy estimation only)",
            SWS_TPEF));
    addOption(
        new StringCmdLineOptionParser(
            SWL_TRACE, 
            "sets the simulation trace database (TraceDB) from which to "
            "load the simulation data of the estimated program (required for "
            "energy estimation only)",
            SWS_TRACE));
    addOption(
        new BoolCmdLineOptionParser(
            SWL_TOTALAREA, 
            "run total area estimation",
            SWS_TOTALAREA));
    addOption(
        new BoolCmdLineOptionParser(
            SWL_LONGESTPATH, 
            "run longest path estimation",
            SWS_LONGESTPATH));
    addOption(
        new BoolCmdLineOptionParser(
            SWL_TOTALENERGY, 
            "run total energy consumption estimation",
            SWS_TOTALENERGY));
}

/**
 * Destructor.
 */
EstimatorCmdLineOptions::~EstimatorCmdLineOptions() {
}

/**
 * Prints the version of the program.
 */
void
EstimatorCmdLineOptions::printVersion() const {
    std::cout << "estimate - TTA Processor Cost Estimator " 
              <<  Application::TCEVersionString() << std::endl;
}

/**
 * Prints the help menu of the program.
 */
void
EstimatorCmdLineOptions::printHelp() const {
    printVersion();
    std::cout << std::endl 
              << "usage: estimate [options] ADF IDF "
              << std::endl
              << std::endl
              << "ADF and IDF are required always, TPEF and TraceDB only for "
              << "energy estimation, which is not performed in case they are "
              << "not given."
              << std::endl;
    CmdLineOptions::printHelp();
}

/**
 * Returns the TPEF file name given by the user.
 *
 * If no value is given in the parsed command line, returns an empty string.
 *
 * @return The TPEF file name.
 */
std::string
EstimatorCmdLineOptions::TPEF() {
    return findOption(SWL_TPEF)->String();
}

/**
 * Returns the TraceDB file name given by the user.
 *
 * If no value is given in the parsed command line, returns an empty string.
 *
 * @return The TraceDB file name.
 */
std::string
EstimatorCmdLineOptions::traceDB() {
    return findOption(SWL_TRACE)->String();
}

/** 
 * Test if estimation selection flags are given, returns true is so.
 *
 * @return True if estimation selection flags are given.
 */
bool
EstimatorCmdLineOptions::runOnlyEstimations() const {
   if (findOption(SWL_TOTALAREA)->isDefined() ||
       findOption(SWL_LONGESTPATH)->isDefined() ||
       findOption(SWL_LONGESTPATH)->isDefined()) {

       return true;
   }
   return false;
}

/**
 * Checks if run total area estimation option was given.
 *
 * @return True if option was given and was not "no" prefixed
 */
bool
EstimatorCmdLineOptions::totalArea() const {
    if (findOption(SWL_TOTALAREA)->isDefined()) {
        return findOption(SWL_TOTALAREA)->isFlagOn();
    }
    return false;
}

/**
 * Checks if run longest path estimation option was given.
 *
 * @return True if option was given and was not "no" prefixed
 */
bool
EstimatorCmdLineOptions::longestPath() const {
    if (findOption(SWL_LONGESTPATH)->isDefined()) {
        return findOption(SWL_LONGESTPATH)->isFlagOn();
    }
    return false;
}

/**
 * Checks if run total energy consumption estimation option was given.
 *
 * @return True if option was given and was not "no" prefixed
 */
bool
EstimatorCmdLineOptions::totalEnergy() const {
    if (findOption(SWL_TOTALENERGY)->isDefined()) {
        return findOption(SWL_TOTALENERGY)->isFlagOn();
    }
    return false;
}

