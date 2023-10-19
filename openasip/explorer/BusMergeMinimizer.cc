/*
    Copyright (c) 2002-2014 Tampere University.

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
 * @file BusMergeMinimizer.cc
 *
 * Explorer plugin that greedily merges buses that are rarely used
 * simultaneously.
 *
 * @author Timo Viitanen 2014
 * @note rating: red
 */

#include <vector>

#include "Conversion.hh"
#include "CostEstimates.hh"
#include "DSDBManager.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "Exception.hh"
#include "FUPort.hh"
#include "HDBRegistry.hh"
#include "Instruction.hh"
#include "InstructionExecution.hh"
#include "Machine.hh"
#include "Move.hh"
#include "Port.hh"
#include "Program.hh"
#include "Segment.hh"
#include "TemplateSlot.hh"
#include "TerminalImmediate.hh"
#include "TerminalRegister.hh"

using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using std::endl;

const int IMM_SLOT_COUNT = 1;

class BusMergeMinimizer : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION(
        "Shrinks machine by merging busses with "
        "low activity covariance.");

    BusMergeMinimizer() : DesignSpaceExplorerPlugin() {}

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
    benchmark(
        TTAMachine::Machine* mach, DSDBManager::MachineConfiguration& conf,
        std::vector<double>& busCovariance, ClockCycleCount& cycleCount) {
        cycleCount = 0;

        DSDBManager& dsdb = db();
        int busCount = mach->busNavigator().count();

        int immBuses[IMM_SLOT_COUNT];
        findImmBuses(mach, immBuses);

        std::set<RowID> applicationIDs = dsdb.applicationIDs();
        for (std::set<RowID>::const_iterator id = applicationIDs.begin();
             id != applicationIDs.end(); id++) {
            std::vector<double> partialCovariance(busCount * busCount, 0.0);

            std::string applicationPath = dsdb.applicationPath(*id);
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
            cycleCount += partialCycleCount;

            for (unsigned int i = 0; i < executions.size(); ++i) {
                if (executions[i] == 0) continue;
                Instruction* instr = instructions[i];

                std::vector<bool> hasMove(busCount, false);

                if (instr->instructionTemplate().slotCount() > 0) {
                    for (int j = 0; j < IMM_SLOT_COUNT; ++j)
                        hasMove[immBuses[j]] = true;
                }

                for (int j = 0; j < instr->moveCount(); j++) {
                    Move& move = instr->move(j);
                    const Bus& bus = move.bus();
                    hasMove[bus.position()] = true;
                }

                for (int j = 0; j < busCount; j++) {
                    if (!hasMove[j]) continue;
                    partialCovariance[j + j * busCount] += executions[i];
                    for (int k = j + 1; k < busCount; k++) {
                        if (hasMove[k]) {
                            partialCovariance[k + j * busCount] +=
                                executions[i];
                            partialCovariance[j + k * busCount] +=
                                executions[i];
                        }
                    }
                }
            }

            /*            Application::errorStream()
                            << "Partial bus covariance matrix:" << std::endl;
                        for (int j=0; j<busCount; j++) {
                            for (int k=0; k<busCount; k++) {
                                Application::errorStream()
                                    << partialCovariance[j+k*busCount] <<
               "\t";
                            }
                            Application::errorStream() << std::endl;
                        }
            */
            for (int j = 0; j < busCount * busCount; ++j) {
                busCovariance[j] += partialCovariance[j] / partialCycleCount;
            }

            //            Application::errorStream()
            //                << "Partial cycle count:\t" << partialCycleCount
            //                << std::endl;
            db().addCycleCount((*id), conf.architectureID, partialCycleCount);
        }

        /*        Application::errorStream() << "Bus covariance matrix:" <<
           std::endl; for (int j=0; j<busCount; j++) { for (int k=0;
           k<busCount; k++) { Application::errorStream()
                            << busCovariance[j+k*busCount] << "\t";
                    }
                    Application::errorStream() << std::endl;
                }
        */
        if (Application::verboseLevel() > 0) {
            Application::errorStream()
                << "Cycle count:\t" << cycleCount << std::endl;
        }
    }

    virtual std::vector<RowID>
    explore(const RowID& configurationID, const unsigned int&) {
        std::vector<RowID> result;

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

        CostEstimates estimates;
        // bool success = evaluate(conf, estimates, false);

        while (mach->busNavigator().count() > 1) {
            int busCount = mach->busNavigator().count();

            int immBuses[2];
            findImmBuses(mach, immBuses);

            std::vector<double> busCovariance(busCount * busCount, 0);
            ClockCycleCount cycleCount = 0;

            benchmark(mach, conf, busCovariance, cycleCount);

            std::vector<bool> hasTriggerPort(busCount, false);
            std::vector<bool> hasDataPort(busCount, false);
            std::vector<bool> hasRFReadPort(busCount, false);
            std::vector<bool> hasRFWritePort(busCount, false);

            for (int busi = 0; busi < busCount; busi++) {
                Segment* segment =
                    mach->busNavigator().item(busi)->segment(0);
                for (int conni = 0; conni < segment->connectionCount();
                     conni++) {
                    Socket* socket = segment->connection(conni);
                    Port* port = socket->port(0);
                    if (dynamic_cast<RFPort*>(port)) {
                        if (socket->direction() == Socket::OUTPUT) {
                            hasRFReadPort[busi] = true;
                        } else {
                            hasRFWritePort[busi] = true;
                        }
                    } else if (dynamic_cast<FUPort*>(port)) {
                        if (socket->direction() == Socket::INPUT) {
                            if (dynamic_cast<FUPort*>(port)->isTriggering()) {
                                hasTriggerPort[busi] = true;
                            } else {
                                hasDataPort[busi] = true;
                            }
                        }
                    }
                }
            }

            double minCorr = 0x7fffffff;
            int minBusA = -1;
            int minBusB = -1;
            bool mergingImmBus = false;
            ;

            std::vector<bool> viableConnections(busCount * busCount, true);

            for (int j = 0; j < busCount - 1; j++) {
                for (int k = j + 1; k < busCount; k++) {
                    bool immediateA = false;
                    bool immediateB = false;
                    for (int l = 0; l < IMM_SLOT_COUNT; ++l) {
                        if (j == immBuses[l]) {
                            immediateA = true;
                        }
                        if (k == immBuses[l]) {
                            immediateB = true;
                        }
                    }
                    Bus* busA = mach->busNavigator().item(j);
                    Bus* busB = mach->busNavigator().item(k);
                    // bool largeSourceA = hasRFReadPort[j];
                    // bool largeSourceB = hasRFReadPort[k];
                    // bool largeDestinationA
                    //     = hasRFWritePort[j] | hasTriggerPort[j];
                    // bool largeDestinationB
                    //     = hasRFWritePort[k] | hasTriggerPort[k];

                    if (busA->width() != busB->width()) {
                        viableConnections[j + k * busCount] = false;
                        continue;
                    }

                    if (immediateA && immediateB) {
                        viableConnections[j + k * busCount] = false;
                        continue;
                    }

                    // Heuristic: do not merge large and small destination
                    // fields. This sometimes gives better code density.
                    /*if ((largeDestinationA != largeDestinationB)
                        && (largeSourceA != largeSourceB)
                        && !immediateA && !immediateB) {
                        viableConnections[j+k*busCount] = false;
                        continue;
                    }*/

                    double corr = busCovariance[j + k * busCount];
                    if (corr < minCorr) {
                        minCorr = corr;
                        minBusA = j;
                        minBusB = k;
                        mergingImmBus = immediateA || immediateB;
                    }
                }
            }

            if (minBusA == -1 || minBusB == -1) break;

            /*            Application::errorStream() << "Viable connections:"
               << std::endl; for (int j=0; j<busCount; j++) { for (int k=0;
               k<busCount; k++) { Application::errorStream()
                                    << viableConnections[j+k*busCount] <<
               "\t";
                            }
                            Application::errorStream() << std::endl;
                        }
            */
            if (Application::verboseLevel() > 0) {
                Application::errorStream()
                    << "Merging buses "
                    << mach->busNavigator().item(minBusA)->name() << " and "
                    << mach->busNavigator().item(minBusB)->name()
                    << " with correlation " << minCorr << std::endl;
            }
            std::string immBusNames[IMM_SLOT_COUNT];
            for (int i = 0; i < IMM_SLOT_COUNT; ++i) {
                immBusNames[i] =
                    mach->busNavigator().item(immBuses[i])->name();
            }

            Bus* busA = mach->busNavigator().item(minBusA);
            Bus* busB = mach->busNavigator().item(minBusB);
            assert(busA->segmentCount() == 1);
            assert(busB->segmentCount() == 1);
            Segment* segmentA = busA->segment(0);
            Segment* segmentB = busB->segment(0);
            for (int i = 0; i < segmentB->connectionCount(); ++i) {
                Socket* sock = segmentB->connection(i);
                if (!segmentA->isConnectedTo(*sock)) {
                    segmentA->attachSocket(*sock);
                }
            }

            int maxImm =
                std::max(busA->immediateWidth(), busB->immediateWidth());
            busA->setImmediateWidth(maxImm);

            //            int maxWidth = std::max(busA->width(),
            //            busB->width()); busA->setWidth(maxWidth);
            mach->removeBus(*busB);

            if (mergingImmBus) {
                ImmediateUnit* immu = mach->immediateUnitNavigator().item(0);

                while (mach->instructionTemplateNavigator().count() > 0) {
                    mach->deleteInstructionTemplate(
                        *mach->instructionTemplateNavigator().item(0));
                }

                new InstructionTemplate("no_limm", *mach);

                InstructionTemplate* limm =
                    new InstructionTemplate("limm", *mach);
                limm->addSlot(busA->name(), 32, *immu);
                // WIP code for two immediate slots:
                // if (mach->busNavigator().hasItem(immBusNames[0]))
                //     limm->addSlot(immBusNames[0], 16, *immu);
                // else
                //     limm->addSlot(immBusNames[1], 16, *immu);
            }

            busCount = mach->busNavigator().count();

            // Clean up RF
            TTAMachine::Machine::SocketNavigator socketNavi =
                mach->socketNavigator();

            std::vector<bool> connectionMask(busCount);
            for (int i = 0; i < socketNavi.count(); i++) {
                Socket* socketA = socketNavi.item(i);

                for (int b = 0; b < busCount; b++) {
                    Segment* segment =
                        mach->busNavigator().item(b)->segment(0);
                    connectionMask[b] = segment->isConnectedTo(*socketA);
                }

                /*Application::errorStream() << "!! mask " << i << ": ";
                for (int b = 0; b < busCount; b++) {
                Application::errorStream() << connectionMask[b] << " ";
                Application::errorStream() << std::endl;*/

                for (int j = i + 1; j < socketNavi.count(); j++) {
                    Socket* socketB = socketNavi.item(j);

                    if (socketA->direction() != socketB->direction())
                        continue;

                    if (socketA->port(0)->parentUnit() !=
                        socketB->port(0)->parentUnit())
                        continue;

                    if (!dynamic_cast<RFPort*>(socketA->port(0))) continue;

                    bool identical = true;

                    for (int b = 0; b < busCount; b++) {
                        Segment* segment =
                            mach->busNavigator().item(b)->segment(0);
                        if (connectionMask[b] !=
                            segment->isConnectedTo(*socketB)) {
                            identical = false;
                        }
                    }

                    if (identical) {
                        delete socketB->port(0);
                        delete socketB;
                        j--;
                    }
                }
            }

            // add machine to configuration
            conf.architectureID = dsdb.addArchitecture(*mach);
            RowID confId = dsdb.addConfiguration(conf);
            CostEstimates estimates;

            // TODO: modify code to use 'evaluate' instead of
            //  schedule and simulate?
            // bool success = evaluate(conf, estimates, false);

            result.push_back(confId);
        }

        // Benchmark also the last configuration.
        int busCount = mach->busNavigator().count();

        std::vector<double> busCovariance(busCount * busCount, 0);
        ClockCycleCount cycleCount = 0;

        benchmark(mach, conf, busCovariance, cycleCount);

        // add new configuration to dsdb
        return result;
    }

protected:
    /**
     * Finds buses with long immediate slots.
     */
    void
    findImmBuses(TTAMachine::Machine* mach, int* immBuses) {
        for (int i = 0; i < IMM_SLOT_COUNT; ++i) immBuses[i] = -1;
        for (int i = 0; i < mach->instructionTemplateNavigator().count();
             ++i) {
            InstructionTemplate* it =
                mach->instructionTemplateNavigator().item(i);
            if (it->slotCount() > 0) {
                if (immBuses[0] != -1) {
                    Application::errorStream()
                        << "Machine has multiple LIMM instruction templates."
                        << std::endl;
                }
                for (int j = 0; j < IMM_SLOT_COUNT; ++j)
                    immBuses[j] = it->slot(j)->bus()->position();
            }
        }
    }
};

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(BusMergeMinimizer)
