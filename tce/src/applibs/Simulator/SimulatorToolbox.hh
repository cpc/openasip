/**
 * @file SimulatorToolbox.hh
 *
 * Declaration of SimulatorToolbox class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
