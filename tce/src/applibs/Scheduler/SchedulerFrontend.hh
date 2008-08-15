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

class InterPassData;

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
        const SchedulingPlan& schedulingPlan,
        InterPassData* interPassData = NULL)
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
