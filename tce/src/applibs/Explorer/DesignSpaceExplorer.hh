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
#include <vector>
#include <istream>
#include "Application.hh"
#include "Exception.hh"
#include "SimulatorConstants.hh"
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
    void buildMinimalOpSet(const TTAMachine::Machine* machine = NULL);
    std::set<std::string> minimalOpSet() const;
    bool checkMinimalOpSet(const TTAMachine::Machine& machine) const;
    void missingOperations(
        const TTAMachine::Machine& machine,
        std::vector<std::string>& missingOps) const;

protected:
    TTAProgram::Program* schedule(
        const std::string applicationFile,
        TTAMachine::Machine& machine);
    
    const ExecutionTrace* simulate(
        const TTAProgram::Program& program,
        const TTAMachine::Machine& machine,
        const TestApplication& testApplication,
        const ClockCycleCount& maxCycles,
        ClockCycleCount& runnedCycles,
        const bool tracing,
        const bool useCompiledSimulation = false)
        throw (Exception);

private:
    /// Design space database where results are stored.
    DSDBManager* dsdb_;
    /// The plugin tool.
    static PluginTools pluginTool_;
    /// The estimator frontend.
    CostEstimator::Estimator estimator_;
    /// Output stream.
    std::ostringstream* oStream_;
    // minimal opset
    std::set<std::string> minimalOpSet_;
};

#endif
