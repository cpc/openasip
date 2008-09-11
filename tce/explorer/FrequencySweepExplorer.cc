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
using std::string;
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
public:
    DESCRIPTION("Frequency sweep algorithm.");
    
    /**
     * Explores from the given start configuration.
     *
     * @param startPointConfigurationID Configuration ID to start the
     * exploration from.
     * @return Returns a set of best found configuration IDs.
     */
    virtual std::vector<RowID>
    explore(const RowID& startPointConfigurationID, const unsigned int&) {

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
                    "SimpleICOptimizer", db());
        DesignSpaceExplorerPlugin* minimizeMachine =
            DesignSpaceExplorer::loadExplorerPlugin(
                    "MinimizeMachine", db());
        DesignSpaceExplorerPlugin* growMachine =
            DesignSpaceExplorer::loadExplorerPlugin(
                    "GrowMachine", db());

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
        DesignSpaceExplorerPlugin::ParameterTable growMachineParameters;
        DesignSpaceExplorerPlugin::Parameter superiorityPar;
        superiorityPar.name = "superiority";
        superiorityPar.value = "2";
        growMachineParameters.push_back(superiorityPar);
        growMachine->setParameters(growMachineParameters);

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

        int currentFrequencyMHz = sweeper.nextFrequency();
        vector<RowID>::const_iterator archIter;
        DesignSpaceExplorerPlugin::ParameterTable minMachineParameters;
        DesignSpaceExplorerPlugin::Parameter frequencyPar;
        frequencyPar.name = "frequency";
        while (currentFrequencyMHz != 0) {

            // for minimizeMachine plugin
            frequencyPar.value = Conversion::toString(currentFrequencyMHz);
            minMachineParameters.clear();
            minMachineParameters.push_back(frequencyPar);

            /* Find the configurations that are fast enough for the
               real time requirements of the applications at the
               currently examined frequency. */
            for (archIter = cycleOptimizedConfs.begin();
                 archIter != cycleOptimizedConfs.end();
                 archIter++) {

                // if is fast enough for all apps
                if (fastEnough(*archIter, currentFrequencyMHz, dsdb)) {
                    // calling MimimizeMachine plugin with confToMinimize 
                    // (architer) and currentFrequencyMHz
                    minimizeMachine->setParameters(minMachineParameters);
                    vector<RowID> minimizedConfs = 
                        minimizeMachine->explore(*archIter);

                    DSDBManager::MachineConfiguration minConf;
                    if (minimizedConfs.size() == 1) {
                        minConf = dsdb.configuration(minimizedConfs.at(0));
                    } else {
                        throw InvalidData(
                            __FILE__, __LINE__, __func__,
                            (boost::format(
                                "MinimizeMachine failed to optimize "
                                "configuration %d. Possible bug in Optimizer,"
                                " Estimator or missing data from HDB."
                            ) % *archIter).str());
                    }

                    if (Application::verboseLevel() > 2) {
                        std::ostringstream msg(std::ostringstream::out);
                        msg << "MinimizeMachine plugin produced config: "
                            << minimizedConfs.at(0) << " (" 
                            << currentFrequencyMHz << " Mhz)" << endl;
                        verboseLog(msg.str())
                    }

                    // create implementation for configuration
                    RowID selectedConf = createImplementationAndStore(minConf, 
                            currentFrequencyMHz, 0, true, icDec_, icDecHDB_);

                    // check if component selection failed
                    if (selectedConf == 0) {
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
                        continue;
                    }

                    /// @todo Optimization of the instruction size
                    /// not required for 1st version!

                    /// @todo Final optimization/tuning
                    /// not required for 1st version!

                } else {
                    // the architecture was too slow
                }

            }
            // advance to next frequency
            currentFrequencyMHz = sweeper.nextFrequency();
        }
        
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

    int startMHz_;
    int endMHz_;
    unsigned int stepMHz_;

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

    /// name of the ic decoder plugin for idf
    std::string icDec_;
    /// name of the hdb used by ic decoder
    std::string icDecHDB_;


    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        const std::string startMHz = "start_freq_mhz";
        const std::string endMHz = "end_freq_mhz";
        const std::string stepMHz = "step_freq_mhz";
        const std::string icDec = "ic_dec";
        const std::string icDecoderDefault = "DefaultICDecoder";
        const std::string icDecHDB = "ic_hdb";
        const std::string icDecHDBDefault = "asic_130nm_1.5V.hdb";

        if (hasParameter(startMHz)) {
            try {
                startMHz_ = Conversion::toInt(parameterValue(startMHz));
            } catch (const Exception& e) {
                parameterError(startMHz, "integer");
                string msg = "'start_freq_mhz' parameter needed";
                throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
            }
        } else {
            string msg = "'start_freq_mhz' parameter needed";
                throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
        }

        if (hasParameter(endMHz)) {
            try {
                endMHz_ = Conversion::toInt(parameterValue(endMHz));
            } catch (const Exception& e) {
                parameterError(endMHz, "integer");
                string msg = "'end_freq_mhz' parameter needed";
                throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
            }
        } else {
            string msg = "'end_freq_mhz' parameter needed";
                throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
        }
        
        if (hasParameter(stepMHz)) {
            try {
                stepMHz_ = Conversion::toInt(parameterValue(stepMHz));
            } catch (const Exception& e) {
                parameterError(stepMHz, "integer");
                string msg = "'step_freq_mhz' parameter needed";
                throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
            }
        } else {
            string msg = "'step_freq_mhz' parameter needed";
            throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
        }

        if (hasParameter(icDec)) {
            try {
                icDec_ = parameterValue(icDec);
            } catch (const Exception& e) {
                parameterError(icDec, "String");
                icDec_ = icDecoderDefault;
            }
        } else {
            // set defaut value to icDec
            icDec_ = icDecoderDefault;
        }

        if (hasParameter(icDecHDB)) {
            try {
                icDecHDB_ = parameterValue(icDecHDB);
            } catch (const Exception& e) {
                parameterError(icDecHDB, "String");
                icDecHDB_ = icDecHDBDefault;
            }
        } else {
            // set defaut value to icDecHDB
            icDecHDB_ = icDecHDBDefault;
        }
    }

    
    /**
     * Print error message of invalid parameter to plugin error stream.
     *
     * @param param Name of the parameter that has invalid value.
     * @param type Type of the parameter ought to be.
     */
    void parameterError(const std::string& param, const std::string& type) {
        std::ostringstream msg(std::ostringstream::out);
        msg << "Invalid parameter value '" << parameterValue(param)
            << "' on parameter '" << param << "'. " << type 
            << " value expected." << std::endl;
        errorOuput(msg.str());
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
};

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(FrequencySweepExplorer)
