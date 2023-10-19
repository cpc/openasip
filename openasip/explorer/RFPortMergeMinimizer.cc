/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file RFPortMergeMinimizer.cc
 *
 * Explorer plugin that greedily merges RF ports which are rarely
 * used simultaneously.
 *
 * @author Timo Viitanen 2014
 * @author Pekka Jääskeläinen 2021
 * @note rating: red
 */

#include <vector>

#include "DSDBManager.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "ExplorationTools.hh"
#include "FUPort.hh"
#include "Instruction.hh"
#include "InstructionExecution.hh"
#include "Machine.hh"
#include "Move.hh"
#include "Port.hh"
#include "Program.hh"
#include "Segment.hh"
#include "TerminalImmediate.hh"
#include "TerminalRegister.hh"

using namespace TTAProgram;
using namespace TTAMachine;

class RFPortMergeMinimizer : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION("Shrinks RF by merging RF ports with low covariance.");

    RFPortMergeMinimizer()
        : DesignSpaceExplorerPlugin(),
          ccThreshold_(0),
          rfToMerge_(""),
          stopPortCount_(2) {
        addParameter(
            ccThresholdPN_, INT, false, "0",
            "Cycle count threshold to stop merging Register File ports.");
        addParameter(
            rfToMergePN_, STRING, false, "",
            "The name of the Register File which ports will be merged.");
        addParameter(
            stopPortCountPN_, INT, false, "2",
            "The amount of RF ports at which merging will stop");
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
        return true;
    }

    virtual void
    benchmark(RegisterFile* rf, std::vector<double>& RFPortCovariance) {
        Machine* mach = rf->machine();
        std::map<const RFPort*, int> rfPortToIndex;

        int rfPortCount = rf->maxReads() + rf->maxWrites();

        for (int j = 0; j < rfPortCount; ++j) {
            rfPortToIndex[rf->port(j)] = j;
        }

        std::set<RowID> applicationIDs = db().applicationIDs();
        for (std::set<RowID>::const_iterator id = applicationIDs.begin();
             id != applicationIDs.end(); id++) {
            std::vector<double> partialRFPortCovariance(
                rfPortCount * rfPortCount, 0);

            std::string applicationPath = db().applicationPath(*id);
            TestApplication testApplication(applicationPath);

            std::string applicationFile = testApplication.applicationPath();

            // test that program is found
            if (applicationFile.length() < 1) {
                delete mach;
                mach = NULL;
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    (boost::format(
                         "No program found from application dir '%s'") %
                     applicationPath)
                        .str());
            }

            std::unique_ptr<TTAProgram::Program> scheduledProgram(
                schedule(applicationFile, *mach));

            if (scheduledProgram.get() == NULL) {
                delete mach;
                mach = NULL;
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    (boost::format("Failed to schedule program '%s'") %
                     applicationPath)
                        .str());
                continue;
            }

            // simulate the scheduled program
            Program::InstructionVector instructions =
                scheduledProgram->instructionVector();

            std::vector<ClockCycleCount> executions(instructions.size(), 0);

            ClockCycleCount partialCycleCount;
            simulate(
                *scheduledProgram, *mach, testApplication, 0,
                partialCycleCount, false, false, &executions);

            for (unsigned int i = 0; i < executions.size(); ++i) {
                if (executions[i] == 0) continue;
                Instruction* instr = instructions[i];

                std::vector<bool> hasRFAccess(rfPortCount, false);

                for (int j = 0; j < instr->moveCount(); j++) {
                    Move& move = instr->move(j);

                    TerminalRegister* regwr =
                        dynamic_cast<TerminalRegister*>(&move.destination());
                    if (regwr) {
                        if (regwr->isGPR() &&
                            (&regwr->registerFile() == rf)) {
                            int index = rfPortToIndex.at(
                                dynamic_cast<const RFPort*>(&regwr->port()));
                            hasRFAccess[index] = true;
                        }
                    }

                    TerminalRegister* regrd =
                        dynamic_cast<TerminalRegister*>(&move.source());
                    if (regrd) {
                        if (regrd->isGPR() &&
                            (&regrd->registerFile() == rf)) {
                            int index = rfPortToIndex.at(
                                dynamic_cast<const RFPort*>(&regrd->port()));
                            hasRFAccess[index] = true;
                        }
                    }
                }

                for (int j = 0; j < rfPortCount; j++) {
                    if (!hasRFAccess[j]) continue;
                    partialRFPortCovariance[j + j * rfPortCount] +=
                        executions[i];
                    for (int k = j + 1; k < rfPortCount; k++) {
                        if (!hasRFAccess[k]) continue;
                        partialRFPortCovariance[k + j * rfPortCount] +=
                            executions[i];
                        partialRFPortCovariance[j + k * rfPortCount] +=
                            executions[i];
                    }
                }
            }
            /*
                        Application::errorStream()
                            << "Partial RF port covariance matrix:" <<
               std::endl; for (int j=0; j<rfPortCount; j++) { for (int k=0;
               k<rfPortCount; k++) { Application::errorStream()
                                    <<
               partialRFPortCovariance[j+k*rfPortCount] << "\t";
                            }
                            Application::errorStream() << std::endl;
                        }
            */
            for (int j = 0; j < rfPortCount * rfPortCount; ++j) {
                RFPortCovariance[j] +=
                    partialRFPortCovariance[j] / partialCycleCount;
            }

            //            Application::errorStream()
            //                << "Partial cycle count:\t" << partialCycleCount
            //                << std::endl;
        }
        /*
                Application::errorStream()
                    << "RF port covariance matrix:" << std::endl;
                for (int j=0; j<rfPortCount; ++j) {
                    Application::errorStream() << rf->port(j)->name() << "\t";
                }
                Application::errorStream() << std::endl;
                for (int j=0; j<rfPortCount; j++) {
                    for (int k=0; k<rfPortCount; k++) {
                        Application::errorStream()
                            << RFPortCovariance[j+k*rfPortCount] << "\t";
                    }
                    Application::errorStream() << std::endl;
                }
        */
    }

    virtual std::vector<RowID>
    explore(const RowID& configurationID, const unsigned int&) {
        readParameters();
        std::vector<RowID> result;
        DSDBManager::MachineConfiguration conf;
        conf.hasImplementation = false;
        Machine* mach = NULL;

        // load the adf from file or from dsdb
        try {
            conf = db().configuration(configurationID);
            mach = db().architecture(conf.architectureID);
        } catch (const Exception& e) {
            std::ostringstream msg(std::ostringstream::out);
            Application::errorStream()
                << "Error loading the adf." << std::endl;
            return result;
        }
        assert(mach != NULL);

        std::vector<TCEString> sortedRFids{};

        // specific RF name to merge is given
        if (rfToMerge_.length() > 0) {
            RegisterFile* rf = mach->registerFileNavigator().item(rfToMerge_);
            sortedRFids.push_back(rf->name());
            // Otherwise start merging RFs by decreasing width order
        } else {
            sortedRFids = ExplorationTools::sortRFsByWidth(mach);
        }

        auto rfiter = sortedRFids.begin();

        // Architecture refining loop
        while (true) {
            // Nothing to merge
            if (sortedRFids.empty()) {
                break;
            }

            // Reached end of RF names list, start over
            if (rfiter == sortedRFids.end()) {
                rfiter = sortedRFids.begin();
            }

            RegisterFile* rf = mach->registerFileNavigator().item(*rfiter);
            int rfPortCount = rf->maxReads() + rf->maxWrites();

            // Port merging for this RF is finished
            if (rfPortCount <= stopPortCount_) {
                sortedRFids.erase(rfiter);
                continue;
            }

            std::vector<double> RFPortCovariance(
                rfPortCount * rfPortCount, 0);

            // Build covariance matrix
            benchmark(rf, RFPortCovariance);

            double minCorr = 0x7fffffff;
            int minPortA = -1;
            int minPortB = -1;
            std::vector<bool> viableMerges(rfPortCount * rfPortCount, true);

            for (int j = 0; j < rfPortCount - 1; j++) {
                for (int k = j + 1; k < rfPortCount; k++) {
                    if (rf->port(j)->isInput() != rf->port(k)->isInput()) {
                        viableMerges[j + k * rfPortCount] = false;
                        viableMerges[k + j * rfPortCount] = false;
                        continue;
                    }

                    double corr = RFPortCovariance[j + k * rfPortCount];
                    if (corr < minCorr) {
                        minCorr = corr;
                        minPortA = j;
                        minPortB = k;
                    }
                }
            }
            /*
                        Application::errorStream() << "Viable merges:" <<
               std::endl; for (int j=0; j<rfPortCount; j++) { for (int k=0;
               k<rfPortCount; k++) { Application::errorStream()
                                    << viableMerges[j+k*rfPortCount] << "\t";
                            }
                            Application::errorStream() << std::endl;
                        }
            */

            RFPort* portA = rf->port(minPortA);
            RFPort* portB = rf->port(minPortB);
            if (Application::verboseLevel() > 0) {
                Application::errorStream()
                    << "Merging " << rf->name() << ", [" << rf->width()
                    << "bit] ports ..."
                    << "\n\tPort " << portB->name() << " -> " << portA->name()
                    << ", correlation " << minCorr << std::endl;
            }

            Socket* sockA = portA->isInput() ? portA->inputSocket()
                                             : portA->outputSocket();
            Socket* sockB = portB->isInput() ? portB->inputSocket()
                                             : portB->outputSocket();

            for (int i = 0; i < sockB->segmentCount(); ++i) {
                if (!sockA->isConnectedTo(*sockB->segment(i))) {
                    sockA->attachBus(*sockB->segment(i));
                }
            }

            delete sockB;
            delete portB;

            RowID confBackupID = db().configurationId(conf);
            // add machine to configuration, save new conf to db, evaluate
            conf.architectureID = db().addArchitecture(*mach);
            RowID confId = db().addConfiguration(conf);
            if (Application::verboseLevel() > 0) {
                Application::errorStream() << "\tEvaluating...";
            }
            bool evalOK = evaluate(conf);

            std::vector<ClockCycleCount> cycles = db().cycleCounts(conf);
            evalOK &= cycles.begin() != cycles.end();

            // If we didn't get any cycle counts, the configuration
            // failed to compile or simulate.
            ClockCycleCount maxCC =
                !evalOK ? (ClockCycleCount)-1
                        : *max_element(cycles.begin(), cycles.end());
            if (Application::verboseLevel() > 0) {
                Application::errorStream()
                    << " cycle count: " << maxCC << std::endl;
            }

            // Cycle count threshold was given
            if ((ccThreshold_ != 0 && maxCC > ccThreshold_) || !evalOK) {
                // revert machine to previous state before merging ports
                if (Application::verboseLevel() > 0) {
                    Application::errorStream()
                        << "\tThreshold " << ccThreshold_ << " reached "
                        << "or evaluation failed." << std::endl;
                }
                conf = db().configuration(confBackupID);
                mach = db().architecture(confBackupID);
                sortedRFids.erase(rfiter);
                if (!evalOK) return result;
                continue;
            }
            rfiter++;
            result.push_back(confId);
        }
        return result;
    }

private:
    // Clockcycle count threshold
    ClockCycleCount ccThreshold_;
    static const TCEString ccThresholdPN_;

    // The name of the Register File which ports will be merged.
    static const TCEString rfToMergePN_;
    TCEString rfToMerge_;

    // The amount of RF ports at which merging will stop
    static const TCEString stopPortCountPN_;
    int stopPortCount_;

    /**
     * Reads the parameters given to the plugin.
     */
    void
    readParameters() {
        readOptionalParameter(ccThresholdPN_, ccThreshold_);
        readOptionalParameter(rfToMergePN_, rfToMerge_);
        readOptionalParameter(stopPortCountPN_, stopPortCount_);
    }
};

const TCEString RFPortMergeMinimizer::ccThresholdPN_("cc_threshold");
const TCEString RFPortMergeMinimizer::rfToMergePN_("rf_to_merge");
const TCEString RFPortMergeMinimizer::stopPortCountPN_("stop_port_count");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(RFPortMergeMinimizer)
