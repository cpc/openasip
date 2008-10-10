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
 * @file EstimatorCmdLineOptions.cc
 *
 * Declaration of EstimatorCmdLineOptions.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "CmdLineOptions.hh"
#include "EstimatorCmdLineOptions.hh"
#include "config.h"


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
              << VERSION << std::endl;
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

