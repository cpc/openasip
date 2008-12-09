/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
