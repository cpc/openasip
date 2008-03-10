/**
 * @file DesignSpaceExplorer.hh
 *
 * Declaration of DesignSpaceExplorer class.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_DESIGN_SPACE_EXPLORER_HH
#define TTA_DESIGN_SPACE_EXPLORER_HH

#include <set>
#include <istream>
#include "Application.hh"
#include "Exception.hh"
#include "SimulatorConstants.hh"
#include "SchedulingPlan.hh"
#include "SchedulerFrontend.hh"
#include "PluginTools.hh"
#include "Estimator.hh"
#include "DSDBManager.hh"
#include "TestApplication.hh"
#include "BaseLineReader.hh"

class CostEstimates;
class ExecutionTrace;
class DesignSpaceExplorerPlugin;

namespace TTAMachine {
    class Machine;
}

namespace TTAProgram {
    class Program;
}

namespace IDF {
    class MachineImplementation;
}

/**
 * Design space explorer interface provides methods to automatically evaluate
 * machine configurations and select best implementations to the processor
 * components according the test applications set in Design Space Database
 * (DSDB).
 */
class DesignSpaceExplorer {
public:
    DesignSpaceExplorer();
    virtual ~DesignSpaceExplorer();

    virtual void setDSDB(DSDBManager& dsdb);

    virtual bool evaluate(
        const DSDBManager::MachineConfiguration& configuration,
        CostEstimates& results, bool estimate);
    virtual DSDBManager& db();
    static DesignSpaceExplorerPlugin* loadExplorerPlugin(
        const std::string& pluginName, DSDBManager& dsdb)
        throw (FileNotFound, DynamicLibraryException);
protected:
    TTAProgram::Program* schedule(
        const TTAProgram::Program& sequentialProgram,
        const TTAMachine::Machine& machine)
        throw (Exception);
    
    const ExecutionTrace* simulate(
        const TTAProgram::Program& program,
        const TTAMachine::Machine& machine,
        const TestApplication& testApplication,
        const ClockCycleCount& maxCycles,
        ClockCycleCount& runnedCycles,
        const bool tracing)
        throw (Exception);

private:
    /// Design space database where results are stored.
    DSDBManager* dsdb_;
    /// Scheduling plan.
    SchedulingPlan* schedulingPlan_;
    /// The scheduler frontend.
    SchedulerFrontend scheduler_;
    /// The plugin tool.
    static PluginTools pluginTool_;
    /// The estimator frontend.
    CostEstimator::Estimator estimator_;
    /// Output stream.
    std::ostringstream* oStream_;
};

#endif
