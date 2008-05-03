/**
 * @file SchedulerFrontend.hh
 *
 * Declaration of SchedulerFrontend class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @author Vladimír Guzma 2008 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SCHEDULER_FRONTEND_HH
#define TTA_SCHEDULER_FRONTEND_HH

#include "Exception.hh"
#include "SchedulerPluginLoader.hh"
#include "SchedulingPlan.hh"
#include "OperationPool.hh"
#include "UniversalMachine.hh"

class SchedulerCmdLineOptions;
class BaseSchedulerModule;
class UniversalMachine;

namespace TTAProgram {
    class Program;
}

namespace TTAMachine {
    class Machine;
}

namespace TPEF {
    class Binary;
}

/**
 * The top-level component of the Scheduler that the clients use to
 * setup and launch the scheduling process.
 */
class SchedulerFrontend {
public:
    SchedulerFrontend();
    virtual ~SchedulerFrontend();

    void schedule(SchedulerCmdLineOptions& options)
        throw (Exception);
    TTAProgram::Program* schedule(
        const TTAProgram::Program& source, const TTAMachine::Machine& target,
        const SchedulingPlan& schedulingPlan)
        throw (Exception);
private:
    /// Copying forbidden.
    SchedulerFrontend(const SchedulerFrontend&);
    /// Assignment forbidden.
    SchedulerFrontend& operator=(const SchedulerFrontend&);
    void prepareModule(
        BaseSchedulerModule& module, TTAProgram::Program& source,
        const TTAMachine::Machine& target)
        throw (IOException, Exception);

    SchedulerPluginLoader* pluginLoader_;
};

#endif
