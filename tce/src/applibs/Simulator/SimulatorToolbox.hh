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
 * @file SimulatorToolbox.hh
 *
 * Declaration of SimulatorToolbox class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATOR_TOOLBOX_HH
#define TTA_SIMULATOR_TOOLBOX_HH

#include <string>
#include <vector>
#include "SimulatorTextGenerator.hh"
#include "boost/regex.hpp"

class OperationPool;
class SimulationEventHandler;


/**
 * Class that offers global services to Simulator classes.
 *
 * Services provided by this utility class are provided as singleton
 * instances. Some of the singleton instances are meant to be "per simulation".
 * After simulation has ended, the per simulation instances should be cleared 
 * by calling clearSimulationSpecific(). Such an instance is the 
 * SimulationEventHandler which contains listener references that are 
 * connected to a specific simulation.
 */
class SimulatorToolbox {
public:
    /// The severities of runtime errors.
    typedef enum {
        RES_MINOR, ///< Minor runtime error, no abort necessary.
        RES_FATAL  ///< Fatal runtime error, there is a serious error in the
                   /// simulated program, thus it makes no sense to go on
                   /// with the simulation.
    } RuntimeErrorSeverity;

    static OperationPool& operationPool();
    static SimulatorTextGenerator& textGenerator();
    
    // NOTE: Moved to SimulatorFrontend !
    //static SimulationEventHandler& eventHandler();

    static boost::regex sequentialRegisterRegex();
    static boost::regex fuPortRegex();

    static void clearSimulationSpecific();
    static void clearAll();

    static void reportSimulatedProgramError(
        SimulationEventHandler& eventHandler,
        RuntimeErrorSeverity severity, const std::string& description);
    static std::string programErrorReport(
        RuntimeErrorSeverity severity, std::size_t index);
    static std::size_t programErrorReportCount(
        RuntimeErrorSeverity severity);
    static void clearProgramErrorReports();

protected:
    /// Instantiation not allowed.
    SimulatorToolbox();

private:
    /// A type for storing a program error description.
    typedef std::pair<RuntimeErrorSeverity, std::string>
    ProgramErrorDescription;
    /// Container for simulated program error descriptions.
    typedef std::vector<ProgramErrorDescription> ProgramErrorDescriptionList;
    /// Copying not allowed.
    SimulatorToolbox(const SimulatorToolbox&);
    /// Assignment not allowed.
    SimulatorToolbox& operator=(const SimulatorToolbox&);

    /// Global instance of OperationPool.
    static OperationPool* pool_;
    /// Global instance of SimulatorTextGenerator.
    static SimulatorTextGenerator textGenerator_;
    /// Per simulation instance of SimulationEventHandler.
    static SimulationEventHandler* eventHandler_;

    /// Runtime error reports.
    static ProgramErrorDescriptionList programErrorReports_;
    
};

#include "SimulatorToolbox.icc"

#endif
