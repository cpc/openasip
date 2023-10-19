/*
    Copyright (c) 2002-2017 Tampere University.

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
 * @file AutoExplorer.cc
 *
 * Drives the whole automated exploration process by calling several plugins.
 *
 * @author Alex Hirvonen 2017 (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */

#include <chrono>
#include <ctime>
#include <string>
#include <vector>

#include "ADFSerializer.hh"
#include "Conversion.hh"
#include "DSDBManager.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "Exception.hh"
#include "FileSystem.hh"
#include "HDBRegistry.hh"
#include "Machine.hh"

using namespace TTAMachine;
using namespace HDB;

const int ALLOPERATIONAMCHINE = 0;
const int PRUNEUNUSEDUNITS = 1;
const int VLIWCONNECTIC = 2;
const int FUMERGEMINIMIZER = 3;
const int BUSMERGEMINIMIZER = 4;
const int RFPORTMERGEMINIMIZER = 5;
const int SHRINKREGISTERCOUNTS = 6;
const int SHORTIMMEDIATEOPTIMIZER = 7;
const int IMMEDIATEGENERATOR = 8;

class AutoExplorer : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION(
        "Drives the whole automated exploration process by "
        "calling several plugins.");

    AutoExplorer()
        : DesignSpaceExplorerPlugin(),
          targetFrequency_(0),
          targetCycleCount_(0),
          resultSize_(10),
          skeleton_(""),
          mode_("scalar"),
          wipeRegisterFile_(false),
          shortImmediateWidth_(32),
          longImmediateBusCount_(1),
          numLSUs_(1) {
        // TODO: update descriptions
        addParameter(
            targetFrequencyPN_, UINT, false, "0",
            "Target operating frequency.");
        addParameter(
            targetCycleCountPN_, UINT, false, "0", "Target clockcycle count");
        addParameter(
            resultSizePN_, UINT, false, "10",
            "Maximum number of found architectures.");
        addParameter(
            skeletonPN_, STRING, false, "",
            "Skeleton IO. List of predefined FUs in starting arch.");
        addParameter(
            modePN_, STRING, false, "scalar",
            "Scalar or vector architecture mode.");
        addParameter(
            wipeRegisterFilePN_, BOOL, false, "true",
            "Generate a VLIW-style register file.");
        addParameter(
            shortImmediateWidthPN_, INT, false, "32",
            "Short immediate width for each bus.");
        addParameter(
            longImmediateBusCountPN_, INT, false, "1",
            "Number of dummy buses for long immediates. (power-of-2)");
        addParameter(
            numLSUsPN_, UINT, false, "1",
            "Minimum number of LSU to be created.");
    }

    virtual bool
    requiresStartingPointArchitecture() const {
        return true;
    }
    virtual bool
    producesArchitecture() const {
        return true;
    }
    virtual bool
    requiresHDB() const {
        return false;
    }
    virtual bool
    requiresSimulationData() const {
        return false;
    }
    virtual bool
    requiresApplication() const {
        return false;
    }

    virtual std::vector<RowID>
    explore(const RowID& configurationID, const unsigned int&) {
        readParameters();

        auto expStart = std::chrono::system_clock::now();
        std::time_t startTime =
            std::chrono::system_clock::to_time_t(expStart);
        if (Application::verboseLevel() > 0) {
            Application::logStream()
                << "Starting exploration at: " << std::ctime(&startTime);
        }
        // Exploration pipeline and plugin parameters
        pluginPipeline_ = {"AllOperationMachine",  "PruneUnusedUnits",
                           "VLIWConnectIC",        "FUMergeMinimizer",
                           "BusMergeMinimizer",    "RFPortMergeMinimizer",
                           "ShrinkRegisterCounts", "ShortImmediateOptimizer",
                           "ImmediateGenerator"};

        pluginParameters_ = {
            {
                // AllOperationMachine
                {"skeleton", skeleton_},
                {"mode", Conversion::toString(mode_)},
            },
            {
                // PruneuUusedUnits
                {"skeleton", skeleton_},
                {"mode", Conversion::toString(mode_)},
            },
            {
                // VLIWConnectIC
                {"wipe_register_file",
                 Conversion::toString(wipeRegisterFile_)},
                {"simm_width", Conversion::toString(shortImmediateWidth_)},
                {"limm_bus_count",
                 Conversion::toString(longImmediateBusCount_)},
            },
            {
                // FUMergeMinimizer
                {"num_lsu", Conversion::toString(numLSUs_)},
                {"dont_merge", skeleton_},
            },
            {
                // BusMergeMinimizer
            },
            {// RFPortMergeMinimizer
             {"cc_threshold", Conversion::toString(targetCycleCount_)}},
            {// ShrinkRegisterCounts
             {"cc_threshold", Conversion::toString(targetCycleCount_)}},
            {
                // ShortImmediateOptimizer
            },
            {// ImmediateGenerator
             {"remove_it_name", "limm"},
             {"add_it_name", "limm32"},
             {"width", "32"},
             {"split", "true"}},
        };

        std::vector<RowID> result;
        std::vector<RowID> finishedResults;
        DSDBManager& dsdb = db();
        DSDBManager::MachineConfiguration conf;
        conf.hasImplementation = false;
        TTAMachine::Machine* mach = NULL;

        // load the adf from file or from dsdb
        try {
            conf = dsdb.configuration(configurationID);
            mach = dsdb.architecture(conf.architectureID);
        } catch (const Exception& e) {
            std::ostringstream msg(std::ostringstream::out);
            Application::errorStream()
                << "Error loading the adf." << std::endl;
            return result;
        }
        assert(mach != NULL);

        std::vector<std::vector<RowID>> generatedConfs(
            pluginPipeline_.size());

        unsigned int pid = 0;
        RowID currentConfID = configurationID;

        // Execute the exploration pipeline
        while (pid < pluginPipeline_.size()) {
            conf = dsdb.configuration(currentConfID);
            mach = dsdb.architecture(conf.architectureID);

            if (Application::verboseLevel() > 1) {
                Application::logStream() << "PID: " << pid << std::endl;
                Application::logStream()
                    << "Current config ID = " << currentConfID << std::endl;
            }

            result = runPlugin(pid, currentConfID);

            if (Application::verboseLevel() > 1) {
                Application::logStream()
                    << "Result size before prune: " << result.size()
                    << std::endl;
            }
            std::vector<RowID> prunedResult =
                pruneConfigurations(result, targetCycleCount_);

            if (pid == IMMEDIATEGENERATOR) {
                printConfInfo(result.back());
            }

            // wait untill all immediate optimizations are made
            if (prunedResult.size() == 0 && pid == SHORTIMMEDIATEOPTIMIZER) {
                prunedResult = result;
            }

            if (Application::verboseLevel() > 1) {
                Application::logStream()
                    << "Result size after prune: " << prunedResult.size()
                    << std::endl;
            }
            generatedConfs[pid] = prunedResult;
            //            std::cout << "1.Gen confs size:" <<
            //            generatedConfs[pid].size() << std::endl;

            // pruned conf. list is empty -> try to fallback in pipeline
            if (prunedResult.size() == 0) {
                verboseLog("Pruned result is empty, trying fallback.");
                if (pid > PRUNEUNUSEDUNITS) {
                    // error, cant fallback & CC budjet failed
                    if (pid == VLIWCONNECTIC || pid == FUMERGEMINIMIZER) {
                        verboseLog("CC budjet failed: ");
                        return prunedResult;
                    }

                    while (pid > FUMERGEMINIMIZER) {
                        if (Application::verboseLevel() > 1) {
                            Application::logStream()
                                << "pid: " << pid << std::endl;
                        }
                        // cc budjet overflow in prev. conf -> remove it
                        //                        std::cout << "Pid sizes: "
                        //                        <<
                        //                        generatedConfs[pid].size()
                        //                        << " " <<
                        //                        generatedConfs[pid-1].size()
                        //                        << std::endl;
                        generatedConfs[pid - 1].pop_back();
                        //                        std::cout << "2\n";
                        // no more confs -> fallback to prev. stage
                        //                        std::cout << "2.Gen confs
                        //                        size:" <<
                        //                        generatedConfs[pid].size()
                        //                        << std::endl;

                        if (generatedConfs[pid - 1].empty()) {
                            if (Application::verboseLevel() > 1) {
                                Application::logStream()
                                    << "Fall back to pid: " << (pid - 1)
                                    << std::endl;
                            }
                            pid--;
                            continue;
                            // try next conf with more hw resources
                        } else {
                            currentConfID = generatedConfs[pid - 1].back();
                            if (Application::verboseLevel() > 1) {
                                Application::logStream()
                                    << "Trying next conf: " << currentConfID
                                    << std::endl;
                            }
                            break;
                        }
                    }
                    // just pick conf before vliwconnectic
                } else {
                    currentConfID = result.back();
                    pid++;
                }
                // pruned conf list not empty
            } else {
                //                std::cout << "pruned not empty\n";
                currentConfID = prunedResult.back();

                if (pid == IMMEDIATEGENERATOR) {
                    // TODO: imm ready, exp finished
                    conf = dsdb.configuration(currentConfID);
                    mach = dsdb.architecture(conf.architectureID);

                    if (archFitsTiming(*mach, targetFrequency_)) {
                        // save, all ok
                        // TODO: try other confs for more performance
                        verboseLog(
                            "Adding to list: " +
                            Conversion::toString(currentConfID));
                        finishedResults.push_back(currentConfID);
                        // growing reg could inc the CC but still satisfy
                        // freq, need more testing
                        // pid = SHRINKREGISTERCOUNTS; // dont try bigger RFs

                    } else {
                        // fall back?
                        verboseLog("FREQ FAILED");
                        pid = RFPORTMERGEMINIMIZER;  // dont grow rf ports any
                                                     // more

                        // if minimal rf ports conf fails increasing buses
                        // will fail also
                        if (mach->registerFileNavigator()
                                .item("RF_32")
                                ->portCount() == 2) {
                            verboseLog(
                                "Increasing ports will fail, going to "
                                "BusMerge");
                            pid = BUSMERGEMINIMIZER;
                        }
                    }

                    if (finishedResults.size() > resultSize_ - 1) {
                        verboseLog("Got enough archs, finishing");
                        break;
                    }

                    while (pid > FUMERGEMINIMIZER) {
                        if (Application::verboseLevel() > 1) {
                            Application::logStream()
                                << "pid: " << pid << std::endl;
                        }
                        // cc budjet overflow in prev. conf -> remove it
                        //                        std::cout << "zPid sizes: "
                        //                        <<
                        //                        generatedConfs[pid].size()
                        //                        << " " <<
                        //                        generatedConfs[pid-1].size()
                        //                        << std::endl;
                        generatedConfs[pid - 1].pop_back();
                        // no more confs -> fallback to prev. stage
                        //                        std::cout << "z2.Gen confs
                        //                        size:" <<
                        //                        generatedConfs[pid].size()
                        //                        << std::endl;

                        if (generatedConfs[pid - 1].empty()) {
                            if (Application::verboseLevel() > 1) {
                                Application::logStream()
                                    << "Fall back to pid: " << (pid - 1)
                                    << std::endl;
                            }
                            pid--;
                            continue;
                            // try next conf with more hw resources
                        } else {
                            currentConfID = generatedConfs[pid - 1].back();
                            if (Application::verboseLevel() > 1) {
                                Application::logStream()
                                    << "Trying next conf: " << currentConfID
                                    << std::endl;
                            }
                            break;
                        }
                    }
                    pid--;
                }
                pid++;
            }
            if (pid == IMMEDIATEGENERATOR &&
                mach->functionUnitNavigator().count() > 4) {
                verboseLog("FU limit reached!");
                break;
            }

        }  //<-- while pid < pipeline_size
        verboseLog("end of pipeline");

        for (unsigned int cid = 0; cid < finishedResults.size(); cid++) {
            currentConfID = finishedResults[cid];
            printConfInfo(currentConfID);
        }

        auto expEnd = std::chrono::system_clock::now();
        std::time_t endTime = std::chrono::system_clock::to_time_t(expEnd);
        verboseLog(
            TCEString("Finished exploration at ") + std::ctime(&endTime));

        return finishedResults;
    }

private:
    // parameter names
    static const TCEString targetFrequencyPN_;
    static const TCEString targetCycleCountPN_;
    static const TCEString resultSizePN_;
    // Initial machine has premade FU(s), list of their names
    static const TCEString skeletonPN_;
    static const TCEString modePN_;
    static const TCEString wipeRegisterFilePN_;
    static const TCEString shortImmediateWidthPN_;
    static const TCEString longImmediateBusCountPN_;
    static const TCEString numLSUsPN_;

    int targetFrequency_;
    int targetCycleCount_;
    int resultSize_;
    TCEString skeleton_;
    TCEString mode_;
    /// VLIWConnectIC parameters
    bool wipeRegisterFile_;
    int shortImmediateWidth_;
    int longImmediateBusCount_;
    // Number of parallel LSUs to create in FUMergeMinimizer plugin
    int numLSUs_;

    std::vector<std::string> pluginPipeline_;
    std::vector<std::vector<std::vector<std::string>>> pluginParameters_;

    /**
     * Reads the parameters given to the plugin.
     */
    void
    readParameters() {
        // compulsory parameters
        readCompulsoryParameter(targetFrequencyPN_, targetFrequency_);
        readCompulsoryParameter(targetCycleCountPN_, targetCycleCount_);
        readOptionalParameter(resultSizePN_, resultSize_);
        readOptionalParameter(skeletonPN_, skeleton_);
        readOptionalParameter(modePN_, mode_);
        readOptionalParameter(wipeRegisterFilePN_, wipeRegisterFile_);
        readOptionalParameter(shortImmediateWidthPN_, shortImmediateWidth_);
        readOptionalParameter(
            longImmediateBusCountPN_, longImmediateBusCount_);
        readOptionalParameter(numLSUsPN_, numLSUs_);
    }

    /**
     * Prints machine information from given configuration id.
     *
     * @param confId Configuration ID from the exploration database.
     */
    void
    printConfInfo(RowID confID) {
        DSDBManager::MachineConfiguration conf = db().configuration(confID);
        TTAMachine::Machine* mach = db().architecture(conf.architectureID);
        TTAMachine::Machine::RegisterFileNavigator rfNavi =
            mach->registerFileNavigator();

        if (Application::verboseLevel() > 0) {
            Application::logStream()
                << "Configuration: " << Conversion::toString(confID)
                << "\n\tFUs: "
                << Conversion::toString(mach->functionUnitNavigator().count())
                << ", Buses: "
                << Conversion::toString(mach->busNavigator().count());
        }
        for (int i = 0; i < rfNavi.count(); i++) {
            // TODO: print read/write ports counts, cycle counts
            if (rfNavi.item(i)->width() > 1) {
                if (Application::verboseLevel() > 0) {
                    Application::logStream()
                        << "\n\tRF(" << rfNavi.item(i)->width()
                        << "b): ports: "
                        << Conversion::toString(rfNavi.item(i)->portCount())
                        << ", size: "
                        << Conversion::toString(
                               rfNavi.item(i)->numberOfRegisters());
                }
            }
        }
        verboseLog("\n");
    }

    /**
     * Prunes configurations from the list, which are below threshold.
     *
     * @param result List of configuration IDs.
     * @param threshold Clockcycle threshold number.
     * @return Pruned list of configuration IDs.
     */
    std::vector<RowID>
    pruneConfigurations(std::vector<RowID> result, int threshold) {
        std::vector<RowID> prunedConfIDs;

        for (unsigned int i = 0; i < result.size(); i++) {
            // calculate average cc in case of many apps
            ClockCycleCount cc = calcAvgClockCycles(result[i]);

            if (cc <= threshold) {
                prunedConfIDs.push_back(result[i]);
            }
        }
        return prunedConfIDs;
    }

    /**
     * Loads exploration plugin and runs it.
     *
     * @param Plugin.
     * @param Starting configuration.
     * @return List of produced configurations by plugin.
     */
    std::vector<RowID>
    runPlugin(int pid, RowID confID) {
        DesignSpaceExplorerPlugin* plugin =
            loadExplorerPlugin(pluginPipeline_[pid], &db());
        if (Application::verboseLevel() > 0) {
            Application::logStream()
                << "\nRunning " << plugin->name() << " plugin." << std::endl;
        }
        for (auto param : pluginParameters_[pid]) {
            if (Application::verboseLevel() > 0) {
                Application::logStream() << "\t Param: " << param[0] << ": "
                                         << param[1] << std::endl;
            }
            plugin->giveParameter(param[0], param[1]);
        }
        std::vector<RowID> result = plugin->explore(confID);
        delete plugin;
        plugin = NULL;
        return result;
    }

    /**
     * Calculates average clock cycle count in case of multiple apps in dsdb.
     *
     * @param Configuration ID.
     * @return Calculated average clock cycle count.
     */
    ClockCycleCount
    calcAvgClockCycles(RowID confID) {
        if (Application::verboseLevel() > 0) {
            Application::logStream()
                << "Checking conf cc: " << confID << std::endl;
        }

        DSDBManager& dsdb = db();
        // machine configuration for clock cycle information
        DSDBManager::MachineConfiguration conf = dsdb.configuration(confID);
        ClockCycleCount cycleSum = 0;

        for (ClockCycleCount cc : dsdb.cycleCounts(conf)) {
            if (Application::verboseLevel() > 0) {
                Application::logStream() << "cycles: " << cc << std::endl;
            }
            cycleSum += cc;
        }

        if (cycleSum == 0) {
            return 0;
        }

        return cycleSum / dsdb.cycleCounts(conf).size();
    }

    /**
     * Checks if architecture fits frequency input parameter.
     *
     * @param Architecture.
     * @param Frequency.
     * @return true/false.
     */
    bool
    archFitsTiming(Machine& mach, int frequency) {
        const std::string DS = FileSystem::DIRECTORY_SEPARATOR;

        if (frequency == 0) {
            verboseLog("Frequency check disabled...");
            return true;
        }
        // create temp directory for the target machine
        std::string tmpDir = FileSystem::createTempDirectory();

        mach.addressSpaceNavigator().item("instructions")->setWidth(32);
        mach.addressSpaceNavigator().item("data")->setAddressBounds(0, 32767);
        mach.addressSpaceNavigator()
            .item("instructions")
            ->setAddressBounds(0, 32767);

        // write machine to a file for tcecc
        std::string adf = "test-mach.adf";
        ADFSerializer serializer;
        serializer.setDestinationFile(adf);
        try {
            serializer.writeMachine(mach);
        } catch (const SerializerException& exception) {
            //        FileSystem::removeFileOrDirectory(tmpDir);
            throw IOException(
                __FILE__, __LINE__, __func__, exception.errorMessage());
        }

        // call tcecc to compile, link and schedule the program
        std::string command =
            "generateprocessor -g AlmaIFIntegrator "
            "--icd-arg-list=\"debugger:minimal;synchronous-reset:yes\" "
            "-d onchip -f onchip --dont-reset-all --fu-middle-register=all "
            "--sync-reset -e tta_core -p dummy.tpef "
            "--hdb-list=xilinx_series7.hdb,generate_base32.hdb,"
            "generate_rf_iu.hdb,generate_lsu_32.hdb --output=";
        command += tmpDir + DS + "synth " + adf +
                   "; generatebits -e tta_core -x " + tmpDir + DS + "synth " +
                   adf;

        verboseLog(command);
        Application::runShellCommandSilently(command);

        command = "python " + tmpDir + DS + "synth/find_fmax.py --tf " +
                  Conversion::toString(frequency);
        std::vector<std::string> output;
        verboseLog(command);
        Application::runShellCommandAndGetOutput(command, output);

        verboseLog(output.size());

        for (unsigned int z = 0; z < output.size(); z++) {
            if (Application::verboseLevel() > 0) {
                Application::logStream()
                    << "(" << output.at(z) << ")" << std::endl;
            }
        }

        if (output.size() == 1) {
            if (output.at(0) == "passed\n") {
                verboseLog("Frequency passed!");
                return true;
            } else {
                verboseLog("Frequency failed");
                return false;
            }
        } else {
            verboseLog("Frequency failed");
            return false;
        }

        // FileSystem::removeFileOrDirectory(tmpDir);
        // FileSystem::removeFileOrDirectory(adf);
    }
};

const TCEString AutoExplorer::targetFrequencyPN_("target_f");
const TCEString AutoExplorer::targetCycleCountPN_("target_cc");
const TCEString AutoExplorer::resultSizePN_("result_size");
const TCEString AutoExplorer::skeletonPN_("skeleton");
const TCEString AutoExplorer::modePN_("mode");
const TCEString AutoExplorer::wipeRegisterFilePN_("wipe_register_file");
const TCEString AutoExplorer::shortImmediateWidthPN_("simm_width");
const TCEString AutoExplorer::longImmediateBusCountPN_("limm_bus_count");
const TCEString AutoExplorer::numLSUsPN_("num_lsu");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(AutoExplorer)
