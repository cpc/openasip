/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file ShortImmediateOptimizer.cc
 *
 * Explorer plugin that creates or modifies bus immediates.
 *
 * @author Kati Tervo 2019
 * @note rating: red
 */

#include <algorithm>
#include <functional>
#include <set>
#include <string>
#include <vector>

#include "Application.hh"
#include "BEMGenerator.hh"
#include "BinaryEncoding.hh"
#include "ControlUnit.hh"
#include "Conversion.hh"
#include "CostEstimates.hh"
#include "DSDBManager.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "Exception.hh"
#include "HDBRegistry.hh"
#include "HWOperation.hh"
#include "ICDecoderEstimatorPlugin.hh"
#include "Instruction.hh"
#include "Machine.hh"
#include "MachineResourceModifier.hh"
#include "MathTools.hh"
#include "Move.hh"
#include "MoveSlot.hh"
#include "Procedure.hh"
#include "Program.hh"
#include "Segment.hh"
#include "StringTools.hh"
#include "TemplateSlot.hh"
#include "Terminal.hh"
#include "TestApplication.hh"

using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using std::endl;

/**
 * Explorer plugin that creates or modifies machine instruction template by
 * adding/removing immediates.
 */
class ShortImmediateOptimizer : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION("Optimizes short immediate distribution.");

    ShortImmediateOptimizer()
        : DesignSpaceExplorerPlugin(), totalBits_(0), bitsPerBus_(0) {
        // compulsory parameters
        // no compulsory parameters

        // parameters that have a default value
        addParameter(totalBitsPN_, UINT, false, "0");
        addParameter(bitsPerBusPN_, UINT, false, "0");
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
        return true;
    }
    virtual bool
    requiresApplication() const {
        return true;
    }

    /**
     * Explorer plugin that creates or modifies machine instruction templates.
     * TODO: add some modifying functionality.
     *
     * Supported parameters:
     * - print, boolean, print information about machines instruction
     * templates.
     * - remove_it_name, string, remove instruction template with a given
     * name.
     * - add_it_name, string, add empty instruction template with a given
     * name.
     * - modify_it_name, string, modify instruction template with a given
     * name.
     * - width, int, instruction template supported width.
     * - split, boolean, split immediate among slots.
     * - dst_imm_unit, string, destination immediate unit.
     *
     * @param startPointConfigurationID Configuration to optimize.
     */
    virtual std::vector<RowID>
    explore(const RowID& startPointConfigurationID, const unsigned int&) {
        // XXX: does this plugin have to touch short immediates.

        std::vector<RowID> result;

        readParameters();

        try {
            DSDBManager& dsdb = db();
            // loads starting configuration
            DSDBManager::MachineConfiguration conf =
                dsdb.configuration(startPointConfigurationID);

            // load machine from configuration
            // Machine* origMach = NULL;
            Machine* mach = NULL;
            try {
                // origMach = dsdb.architecture(conf.architectureID);
                mach = dsdb.architecture(conf.architectureID);
            } catch (const Exception& e) {
                std::ostringstream msg(std::ostringstream::out);
                msg << e.errorMessage() << endl;
                verboseLog(msg.str());
                return result;
            }

            if (bitsPerBus_ != 0) {
                totalBits_ = mach->busNavigator().count() * bitsPerBus_;
            }

            if (totalBits_ == 0) {
                // Default to same number of immediate bits as the starting
                // architecture, but redistributed
                auto busNav = mach->busNavigator();
                for (int busID = 0; busID < busNav.count(); ++busID) {
                    auto bus = busNav.item(busID);
                    totalBits_ += bus->immediateWidth();
                }
            }

            verboseLog(
                "Targetting a total immediate width of " +
                std::to_string(totalBits_));

            benchmark(*mach);

            // create the new configuration to be saved to dsdb
            // short imms shouldn't affect these
            DSDBManager::MachineConfiguration newConf;
            if (conf.hasImplementation) {
                newConf.hasImplementation = true;
                IDF::MachineImplementation* idf =
                    dsdb.implementation(conf.implementationID);
                CostEstimator::Estimator estimator;
                CostEstimator::AreaInGates area =
                    estimator.totalArea(*mach, *idf);
                CostEstimator::DelayInNanoSeconds longestPathDelay =
                    estimator.longestPath(*mach, *idf);
                newConf.implementationID =
                    dsdb.addImplementation(*idf, longestPathDelay, area);
            } else {
                newConf.hasImplementation = false;
            }
            newConf.architectureID = dsdb.addArchitecture(*mach);
            CostEstimates estimates;

            RowID confID = dsdb.addConfiguration(newConf);
            evaluate(newConf);
            result.push_back(confID);

        } catch (const Exception& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error while using ShortImmediateOptimizer:" << endl
                << e.errorMessage() << endl;
            verboseLog(msg.str());
            return result;
        }
        return result;
    }

private:
    // parameter names
    static const std::string totalBitsPN_;
    static const std::string bitsPerBusPN_;

    // parameters
    /// Minimum percentile gain in immediate coverage to include an
    // additional bit in a given short immediate
    int totalBits_;
    int bitsPerBus_;

    /**
     * Reads the parameters given to the plugin.
     */
    void
    readParameters() {
        readCompulsoryParameter(totalBitsPN_, totalBits_);
        readCompulsoryParameter(bitsPerBusPN_, bitsPerBus_);
    }

    void
    benchmark(TTAMachine::Machine& mach) {
        auto busNav = mach.busNavigator();
        std::vector<std::vector<int>> uimmUsage;
        std::vector<std::vector<int>> simmUsage;
        for (int busID = 0; busID < busNav.count(); ++busID) {
            auto bus = busNav.item(busID);
            int width = bus->width();

            if (width < 33) {
                bus->setImmediateWidth(width);
            } else {
                bus->setImmediateWidth(0);
            }
            bus->setSignExtends();
            uimmUsage.emplace_back(width, 0);
            simmUsage.emplace_back(width, 0);
        }

        DSDBManager& dsdb = db();
        std::set<RowID> applicationIDs = dsdb.applicationIDs();

        for (std::set<RowID>::const_iterator id = applicationIDs.begin();
             id != applicationIDs.end(); id++) {
            std::string applicationPath = dsdb.applicationPath(*id);
            TestApplication testApplication(applicationPath);

            std::string applicationFile = testApplication.applicationPath();

            // test that program is found
            if (applicationFile.length() < 1) {
                throw InvalidData(
                    __FILE__, __LINE__, __func__,
                    (boost::format(
                         "No program found from application dir '%s'") %
                     applicationPath)
                        .str());
            }

            std::unique_ptr<TTAProgram::Program> scheduledProgram(
                schedule(applicationFile, mach));

            if (scheduledProgram.get() == NULL) {
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
                *scheduledProgram, mach, testApplication, 0,
                partialCycleCount, false, false, &executions);

            for (unsigned int i = 0; i < executions.size(); ++i) {
                if (executions[i] == 0) continue;
                Instruction* instr = instructions[i];

                for (int j = 0; j < instr->moveCount(); j++) {
                    Terminal& srcTerminal = instr->move(j).source();
                    if (srcTerminal.isImmediate()) {
                        int busID = instr->move(j).bus().position();
                        auto bus = busNav.item(busID);
                        int immValue = srcTerminal.value().unsignedValue();
                        int lastBit = bus->width() - 1;
                        if (immValue & (1 << lastBit)) {
                            // Immediate is negative
                            int bits = MathTools::requiredBits(~immValue) + 1;
                            simmUsage[bus->position()][bits] += 1;
                        } else {
                            int bits = MathTools::requiredBits(immValue);
                            uimmUsage[bus->position()][bits] += 1;
                        }
                    }
                }
            }
        }

        std::vector<std::vector<std::pair<bool, int>>> bestImmediates;
        for (int busID = 0; busID < busNav.count(); ++busID) {
            bestImmediates.emplace_back();
            auto bus = busNav.item(busID);
            int signedImms = 0;
            int unsignedImms = 0;
            for (int i = 1; i < bus->width(); ++i) {
                unsignedImms += uimmUsage[busID][i];
                signedImms += simmUsage[busID][i] + uimmUsage[busID][i - 1];

                if (unsignedImms > signedImms) {
                    bestImmediates[busID].emplace_back(false, unsignedImms);
                } else {
                    bestImmediates[busID].emplace_back(true, signedImms);
                }
            }
        }

        int remainingBits = totalBits_;
        std::vector<int> immWidth(busNav.count(), 0);
        while (remainingBits > 0) {
            int bestBus = 0;
            float bestMetric = 0;
            int num_bits = 0;
            for (int busID = 0; busID < busNav.count(); ++busID) {
                int busWidth = busNav.item(busID)->width();
                int baseImms = 0;
                if (immWidth[busID] > 0) {
                    baseImms = bestImmediates[busID][immWidth[busID]].second;
                }

                for (int i = immWidth[busID] + 1; i < busWidth; ++i) {
                    if (i > remainingBits) {
                        break;
                    }
                    int totalImms = bestImmediates[busID][i].second;
                    int additionalImms = totalImms - baseImms;
                    int additionalBits = i - immWidth[busID];
                    float metric = float(additionalImms) / additionalBits;
                    if (metric > bestMetric) {
                        bestMetric = metric;
                        num_bits = additionalBits;
                        bestBus = busID;
                    }
                }
            }
            if (num_bits == 0) {
                verboseLog(
                    "Stopping early, cannot cover more immediates. " +
                    std::to_string(remainingBits) + " bits unsused.");
                break;
            }
            remainingBits -= num_bits;
            immWidth[bestBus] += num_bits;
        }

        // Set the short immediates to the machine
        for (int busID = 0; busID < busNav.count(); ++busID) {
            auto bus = busNav.item(busID);
            bool isSigned = bestImmediates[busID][immWidth[busID]].first;

            bus->setImmediateWidth(immWidth[busID]);
            if (isSigned) {
                bus->setSignExtends();
            } else {
                bus->setZeroExtends();
            }
        }
    }
};

// parameter names
const std::string ShortImmediateOptimizer::totalBitsPN_("total_bits");
const std::string ShortImmediateOptimizer::bitsPerBusPN_("bits_per_bus");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(ShortImmediateOptimizer)
