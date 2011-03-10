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
 * @file DesignSpaceExplorer.hh
 *
 * Declaration of DesignSpaceExplorer class.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
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
        CostEstimates& results=dummyEstimate_, bool estimate=false);

    virtual DSDBManager& db();
    static DesignSpaceExplorerPlugin* loadExplorerPlugin(
        const std::string& pluginName, DSDBManager* dsdb = NULL)
        throw (FileNotFound, DynamicLibraryException);
    
    std::vector<DesignSpaceExplorerPlugin*> getPlugins();

    RowID createImplementationAndStore(
        const DSDBManager::MachineConfiguration& conf,
        const double& frequency = 0.0,
        const double& maxArea = 0.0,
        const bool& createEstimates = true,
        const std::string& icDec = "DefaultICDecoder",
        const std::string& icDecHDB = "asic_130nm_1.5V.hdb");
    bool createImplementation(
        const DSDBManager::MachineConfiguration& conf,
        DSDBManager::MachineConfiguration& newConf,
        const double& frequency = 0.0,
        const double& maxArea = 0.0,
        const bool& createEstimates = true,
        const std::string& icDec = "DefaultICDecoder",
        const std::string& icDecHDB = "asic_130nm_1.5V.hdb");
    IDF::MachineImplementation*  selectComponents(
        const TTAMachine::Machine& mach,
        const double& frequency = 0.0,
        const double& maxArea = 0.0,
        const std::string& icDec = "DefaultICDecoder",
        const std::string& icDecHDB = "asic_130nm_1.5V.hdb") const;
    void createEstimateData(
        const TTAMachine::Machine& mach,
        const IDF::MachineImplementation& idf,
        CostEstimator::AreaInGates& area,
        CostEstimator::DelayInNanoSeconds& longestPathDelay);
    RowID addConfToDSDB(
        const DSDBManager::MachineConfiguration& conf);

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
    /// Used for the default evaluate() argument.
    static CostEstimates dummyEstimate_;

};

#endif
