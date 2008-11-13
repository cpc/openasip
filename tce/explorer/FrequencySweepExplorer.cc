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
 * @file FrequencySweepExplorer.cc
 *
 * Explorer plugin that that uses algorithm that sets one target frequency as
 * requirement and sweeps all frequencies given by user.
 *
 * @author Jari M‰ntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @author Esa M‰‰tt‰ 2008 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#include <vector>
#include <string>
#include <set>
#include <map>
#include <sstream>
#include <boost/format.hpp>

#include "DesignSpaceExplorerPlugin.hh"
#include "ExplorerPluginParameter.hh"
#include "Conversion.hh"
#include "DSDBManager.hh"
#include "Machine.hh"
#include "TestApplication.hh"
#include "Program.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "Operation.hh"
#include "ComponentImplementationSelector.hh"
#include "HDBRegistry.hh"
#include "ICDecoderEstimatorPlugin.hh"
#include "ControlUnit.hh"
#include "HWOperation.hh"
#include "StringTools.hh"
#include "ADFSerializer.hh"
#include "IDFSerializer.hh"
#include "Segment.hh"
#include "RFPort.hh"
#include "FullyConnectedCheck.hh"
#include "TemplateSlot.hh"
#include "CostEstimates.hh"
#include "Application.hh"
#include "Guard.hh"
#include "Exception.hh"
#include "FrequencySweep.hh"
#include "MachineResourceModifier.hh"
#include "MachineImplementation.hh"
#include "FUImplementationLocation.hh"


using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using namespace IDF;
using std::endl;
using std::map;
using std::vector;
using std::set;

/**
 * Explorer plugin for finding processor configurations that
 * satisfy the runtime requirements with certain clock frequencies.
 *
 * Sweeps the user-set allowed frequency range with the given
 * steps and collects all configurations that are suitable for
 * running the given applications in the given clock frequencies.
 *
 * Uses all HDBs found in the HDB registry to create the configurations. 
 *
 * Supported parameters:
 *  - start_freq_mhz, lowest frequency in the swept range in MHz
 *  - end_freq_mhz, highest frequency in the range in MHz
 *  - step_freq_mhz, frequency step in MHz
 *
 *  - ic_dec, name of the ic decoder plugin, default is DefaultICDecoder
 *  - ic_hdb, name of the HDB that is used in IC estimation
 */
class FrequencySweepExplorer : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION("Frequency sweep algorithm.");
    
    FrequencySweepExplorer(): DesignSpaceExplorerPlugin(), 
        icDec_("DefaultICDecoder"),
        icDecHDB_("asic_130nm_1.5V.hdb"),
        superiority_(10) {

        // compulsory parameters
        addParameter(startMHzPN_, UINT);
        addParameter(endMHzPN_, UINT);
        addParameter(stepMHzPN_, UINT);

        // parameters that have a default value
        addParameter(icDecPN_, STRING, false, icDec_);
        addParameter(icDecHDBPN_, STRING, false, icDecHDB_);
        addParameter(superiorityPN_, UINT, false, 
                Conversion::toString(superiority_));
    }

    /**
     * Explores from the given start configuration.
     *
     * @param startPointConfigurationID Configuration ID to start the
     * exploration from.
     * @return Returns a set of best found configuration IDs.
     */
    virtual std::vector<RowID>
    explore(const RowID& startPointConfigurationID, const unsigned int&) {

        checkParameters();
        readParameters();

        openHDBs();
        std::vector<RowID> result;
        
        RowID startPointConfID = startPointConfigurationID;
        if (startPointConfID == 0) {
            // Starting point is going to be needed, or the new componentAdder
            // called. That would add FUs that support basic opset if needed.
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error: No starting configuration specified." << endl;
            verboseLog(msg.str())
            return result;
        }

        // other explorer plugins used
        DesignSpaceExplorerPlugin* icOptimizer =
            DesignSpaceExplorer::loadExplorerPlugin(
                    "SimpleICOptimizer", &db());
        DesignSpaceExplorerPlugin* minimizeMachine =
            DesignSpaceExplorer::loadExplorerPlugin(
                    "MinimizeMachine", &db());
        DesignSpaceExplorerPlugin* growMachine =
            DesignSpaceExplorer::loadExplorerPlugin(
                    "GrowMachine", &db());

        DSDBManager& dsdb = db();

        // Sweep is always done from the lowest frequency towards the highest
        // frequency.
        if (startMHz_ > endMHz_) {
            int tempMHz = startMHz_;
            startMHz_ = endMHz_;
            endMHz_ = tempMHz;
        }

        // helper for returning the stepped frequencies in order
        FrequencySweep sweeper(startMHz_, endMHz_, stepMHz_);

        // parameters for GrowMachine plugin
        growMachine->giveParameter("superiority", Conversion::toString(superiority_));

        // find new configurations by adding components until the cycle
        // count stops going down
        vector<RowID> cycleOptimizedConfs = 
            growMachine->explore(startPointConfID);
        if (Application::verboseLevel() > 1) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "GrowMachine plugin produced initial configs: ";
            for (unsigned int i = 0; i < cycleOptimizedConfs.size(); ++i) {
                msg << cycleOptimizedConfs.at(i) << " ";
            }
            msg << endl;
            verboseLog(msg.str())
        }
        delete growMachine;
        growMachine = NULL;

        int currentFrequencyMHz = sweeper.nextFrequency();
        vector<RowID>::const_iterator archIter;
        while (currentFrequencyMHz != 0) {

            verboseLogC("Testing frequency: " + Conversion::toString(
                        currentFrequencyMHz), 3)
            /* Find the configurations that are fast enough for the
               real time requirements of the applications at the
               currently examined frequency. */
            for (archIter = cycleOptimizedConfs.begin();
                 archIter != cycleOptimizedConfs.end();
                 archIter++) {

                verboseLogC("Testing (fast enough) init config: " +
                        Conversion::toString(*archIter), 3)
                // if is fast enough for all apps
                if (fastEnough(*archIter, currentFrequencyMHz, dsdb)) {
                    verboseLogC("Calling minimize machine for init config: " +
                            Conversion::toString(*archIter), 3)

                    // calling MimimizeMachine plugin with confToMinimize 
                    // (architer) and currentFrequencyMHz
                    minimizeMachine->giveParameter("frequency", 
                            Conversion::toString(currentFrequencyMHz));
                    DSDBManager::MachineConfiguration minConf = 
                        callPlugin(minimizeMachine, *archIter, dsdb);

                    // create implementation for configuration
                    RowID selectedConf = createImplementationAndStore(minConf, 
                            currentFrequencyMHz, 0, true, icDec_, icDecHDB_);

                    // check if component selection failed
                    if (selectedConf == 0) {
                        verboseLogC("Component selection failed for minimized"
                            " arch: " + Conversion::toString(
                                minConf.architectureID), 3)
                        continue;
                    }
                    
                    // IC optimization with SimpleICOptimizer plugin
                    vector<RowID> icOptimizedResult = 
                        icOptimizer->explore(selectedConf);
                    if (icOptimizedResult.size() == 1) {
                        if (Application::verboseLevel()) {
                            std::ostringstream msg(std::ostringstream::out);
                            msg << "Config " << icOptimizedResult.at(0) 
                                << " created for frequency " 
                                << currentFrequencyMHz << "." << endl;
                            verboseLogC(msg.str(), 1)
                        }
                        result.push_back(icOptimizedResult.at(0));
                    } else {
                        // simpleICOptimizer can make a machine not fully
                        // connected, and so, register file requirements can
                        // change. Meaning evaluating the machine can fail in
                        // the plugin.
                        verboseLogC("SimpleICOptimzer failed for arch: "
                            + Conversion::toString(selectedConf), 3)
                        continue;
                    }

                    /// @todo Optimization of the instruction size
                    /// not required for 1st version!

                    /// @todo Final optimization/tuning
                    /// not required for 1st version!

                } else {
                    verboseLogC("Init config was too slow.", 3)
                    // the architecture was too slow
                }

            }
            // advance to next frequency
            currentFrequencyMHz = sweeper.nextFrequency();
        }

        delete minimizeMachine;
        minimizeMachine = NULL;
        delete icOptimizer;
        icOptimizer = NULL;
        
        // Idea:
        // All results will be given to a result explorer that returns
        // the given number of best configuration id:s that can be returned
        // to the user.
        // BestResultExplorer resultExplorer(dsdb, explorer.results());
        // result = resultExplorer.find(5);

        return result;
    }

private:
    /// Selector used by the plugin.
    ComponentImplementationSelector selector_;
    /// Number of buses in the machine.
    int busCount_;
    /// Default value of busCount_
    static const int busCountDefault_ = 4;

    static const unsigned int immSlotBusIndexDefault_ = 0;
    unsigned int immSlotBusIndex_;

    static const int registerFileSizeDefault_ = 4;
    static const int maxNumberOfRegisterFilesDefault_ = 16;
    static const int rfReadPortsDefault_ = 1;
    static const int rfWritePortsDefault_ = 1;
    int registerFileSize_;
    int maxNumberOfRegisterFiles_;
    int rfReadPorts_;
    int rfWritePorts_;
    
    // parameter names
    const static std::string startMHzPN_;
    const static std::string endMHzPN_;
    const static std::string stepMHzPN_;

    const static std::string icDecPN_;
    const static std::string icDecHDBPN_;
    const static std::string superiorityPN_;

    // parameters
    unsigned int startMHz_;
    unsigned int endMHz_;
    unsigned int stepMHz_;

    /// name of the ic decoder plugin for idf
    std::string icDec_;
    /// name of the hdb used by ic decoder
    std::string icDecHDB_;
    /// Superirity percentage for the GrowMachine plugin
    unsigned int superiority_;

    static const std::string talo;

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        // compulsory parameters
        readCompulsoryParameter(startMHzPN_, startMHz_);
        readCompulsoryParameter(endMHzPN_, endMHz_);
        readCompulsoryParameter(stepMHzPN_, stepMHz_);

        // optional parameters
        readOptionalParameter(icDecPN_, icDec_);
        readOptionalParameter(icDecHDBPN_, icDecHDB_);
        readOptionalParameter(superiorityPN_, superiority_);
    }

    
    /**
     * Loads HDBs that are used into the registry.
     */
    void openHDBs() {
        HDBRegistry& hdbRegistry = HDBRegistry::instance();

        // if HDBRegistry contains no HDBManagers load from default paths
        if (hdbRegistry.hdbCount() == 0) {
            hdbRegistry.loadFromSearchPaths();
        }
    }

    
    /**
     * Check if architecture is fast enough.
     *
     * @param id Row id of the architecture.
     * @param freq Frequency in MHz for testing the run time.
     * @param dsdb Desing Space Explorer database.
     */
    bool fastEnough(const RowID& id, const int& freq, DSDBManager& dsdb) {
        set<RowID>::const_iterator applicationIter;
        set<RowID> applicationIDs = dsdb.applicationIDs();
        DSDBManager::MachineConfiguration configuration = 
            dsdb.configuration(id);
        for (applicationIter = applicationIDs.begin();
                applicationIter != applicationIDs.end();
                applicationIter++) {

            ClockCycleCount cycleCount = 
                dsdb.cycleCount(
                        *applicationIter, configuration.architectureID);
            TestApplication testApplication(
                    dsdb.applicationPath(*applicationIter));

            // test if application max runtime is set
            if (testApplication.maxRuntime() < 1) {
                continue;
            }   

            if ((cycleCount / (freq * 100000)) >
                    testApplication.maxRuntime()) {
                // we can skip this architecture since it won't
                // meet the speed requirements
                return false;
            }
        }
        return true;
    }


    /**
     * Calls an explorer plugin.
     *
     * @param plugin The plugin to be called.
     * @param arch Row id of the architechture to be passed to the plugin.
     * @return dsdb Design space database to be used.
     */
    DSDBManager::MachineConfiguration callPlugin(
        DesignSpaceExplorerPlugin* plugin,
        const RowID& arch,
        DSDBManager& dsdb) {
        
        vector<RowID> resultConfs = plugin->explore(arch);

        DSDBManager::MachineConfiguration resultConf;
        if (resultConfs.size() == 1) {
            resultConf = dsdb.configuration(resultConfs.at(0));
        } else {
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                (boost::format(
                    "%s failed to optimize "
                    "configuration %d. Possible bug in Optimizer,"
                    " Estimator or missing data from HDB."
                    ) % arch % plugin->name()).str());
        }

        if (Application::verboseLevel() > 2) {
            std::ostringstream msg(std::ostringstream::out);
            msg << plugin->name()
                << " plugin produced config: "
                << resultConfs.at(0) << endl;
            verboseLog(msg.str())
        }

        return resultConf;
    }
};

// parameter names
const std::string FrequencySweepExplorer::startMHzPN_("start_freq_mhz");
const std::string FrequencySweepExplorer::endMHzPN_("end_freq_mhz");
const std::string FrequencySweepExplorer::stepMHzPN_("step_freq_mhz");
const std::string FrequencySweepExplorer::icDecPN_("ic_dec");
const std::string FrequencySweepExplorer::icDecHDBPN_("ic_hdb");
const std::string FrequencySweepExplorer::superiorityPN_("superiority");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(FrequencySweepExplorer)
