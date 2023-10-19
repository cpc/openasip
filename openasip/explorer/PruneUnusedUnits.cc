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
 * @file PruneUnusedUnits.cc
 *
 * Compiles+simulates the program and prunes FUs which are not used in
 * scheduled program and their simulated executions are below
 * THRESHOLD_PERCENT threshold.
 *
 * @author Alex Hirvonen 2017 (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */

// 5 % of highest execution count value seems to be enough
#define THRESHOLD_PERCENT 0.05

#include <set>
#include <string>
#include <vector>

#include "ControlUnit.hh"
#include "Conversion.hh"
#include "DSDBManager.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "Exception.hh"
#include "ExecutionTrace.hh"
#include "ExplorationTools.hh"
#include "FUPort.hh"
#include "HWOperation.hh"
#include "Instruction.hh"
#include "Machine.hh"
#include "Move.hh"
#include "Operation.hh"
#include "OperationPool.hh"
#include "Program.hh"
#include "Terminal.hh"
#include "TerminalRegister.hh"

using namespace TTAMachine;
using namespace TTAProgram;

class PruneUnusedUnits : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION(
        "Compiles the program and prunes FUs which are not "
        "used.");

    PruneUnusedUnits()
        : DesignSpaceExplorerPlugin(), skeleton_(""), mode_("scalar") {
        addParameter(
            skeletonPN_, STRING, false, skeleton_,
            "Skeleton IO. List of predefined FUs in starting arch.");
        addParameter(
            modePN_, STRING, false, mode_,
            "Scalar or vector architecture mode.");
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

    virtual std::vector<RowID>
    explore(const RowID& configurationID, const unsigned int&) {
        readParameters();

        std::vector<RowID> result;
        DSDBManager& dsdb = db();
        DSDBManager::MachineConfiguration conf;
        conf.hasImplementation = false;
        Machine* mach = NULL;

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

        skeletonFUs_ = StringTools::chopString(skeleton_, ";");

        // Append operations to non-removable opset list based on endianness
        std::set<TCEString> lsOps;
        if (mach->isLittleEndian()) {
            lsOps = {"ld8",  "ldu8", "ld16", "ldu16",
                     "ld32", "st8",  "st16", "st32"};
        } else {
            lsOps = {"ldq", "ldqu", "ldh", "ldhu",
                     "ldw", "stq",  "sth", "stw"};
        }
        minimalOpset_.insert(lsOps.begin(), lsOps.end());

        std::set<RowID> applicationIDs = dsdb.applicationIDs();
        std::map<TCEString, int> simUsage;

        bool doSimulate = false;
        const ExecutionTrace* simTrace = NULL;

        while (true) {
            // Cycle all applications
            for (std::set<RowID>::const_iterator id = applicationIDs.begin();
                 id != applicationIDs.end(); id++) {
                std::string applicationPath = dsdb.applicationPath(*id);
                TestApplication testApplication(applicationPath);

                std::string applicationFile =
                    testApplication.applicationPath();

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

                verboseLog(
                    "Starting schedule for \"" + applicationPath + "\"");

                TTAProgram::Program* scheduledProgram = NULL;
                scheduledProgram = schedule(applicationFile, *mach);

                if (scheduledProgram == NULL) {
                    delete mach;
                    mach = NULL;
                    throw InvalidData(
                        __FILE__, __LINE__, __func__,
                        (boost::format("Failed to schedule program '%s'") %
                         applicationPath)
                            .str());
                    continue;
                }
                verboseLog("Schedule done");

                Program::InstructionVector instructions =
                    scheduledProgram->instructionVector();
                std::vector<ClockCycleCount> executions(
                    instructions.size(), 0);

                if (doSimulate) {
                    verboseLog("Starting simulation...");

                    ClockCycleCount cycleCount;
                    simTrace = simulate(
                        *scheduledProgram, *mach, testApplication, 0,
                        cycleCount, true, false, &executions);
                    verboseLog("Simulation done");
                    verboseLog(
                        "Clockcycles count: " +
                        Conversion::toString(cycleCount));
                }

                verboseLog("Collecting operation usage...");
                verboseLog(
                    "Total instructions: " +
                    Conversion::toString(instructions.size()));

                for (unsigned int i = 0; i < instructions.size(); i++) {
                    Instruction* instr = instructions[i];

                    for (int j = 0; j < instr->moveCount(); j++) {
                        TCEString opName;
                        Move& move = instr->move(j);
                        TTAProgram::Terminal& source = move.source();
                        TTAProgram::Terminal& dest = move.destination();

                        // collect used FU names into set
                        if (source.isFUPort()) {
                            const FunctionUnit* fu = &source.functionUnit();

                            // skip skeleton FUs
                            if (isSkeletonFU(fu->name())) {
                                continue;
                            }

                            // skip gcu
                            if (dynamic_cast<const ControlUnit*>(fu) ==
                                NULL) {
                                opName = fu->operation(0)->name();
                                usedOperations_.insert(opName);
                            }
                        }
                        if (dest.isFUPort()) {
                            const FunctionUnit* fu = &dest.functionUnit();

                            if (dynamic_cast<const ControlUnit*>(fu) ==
                                NULL) {
                                opName = fu->operation(0)->name();
                                usedOperations_.insert(opName);
                            }
                        }
                    }
                }

                // Check operation executions from simulation trace
                if (doSimulate) {
                    for (int h = 0; h < mach->functionUnitNavigator().count();
                         h++) {
                        FunctionUnit* fu =
                            mach->functionUnitNavigator().item(h);
                        ExecutionTrace::FUOperationTriggerCountList* opTrigs =
                            simTrace->functionUnitOperationTriggerCounts(
                                fu->name());

                        for (ExecutionTrace::FUOperationTriggerCountList::
                                 const_iterator i = opTrigs->begin();
                             i != opTrigs->end(); ++i) {
                            const ExecutionTrace::FUOperationTriggerCount&
                                triggerCount = *i;

                            const ExecutionTrace::OperationID operation =
                                StringTools::stringToLower(
                                    triggerCount.get<0>());

                            const ExecutionTrace::OperationTriggerCount
                                count = triggerCount.get<1>();

                            simUsage[operation] += count;
                        }
                    }
                }
            }

            if (doSimulate) {
                verboseLog("Operation executions (from simulation trace): ");
                for (auto const& op : simUsage) {
                    verboseLog(
                        op.first + ": " + Conversion::toString(op.second));
                }
            }

            // remove Function Units that are not used
            cleanupFUs(mach, doSimulate, simUsage);
            // remove Register Files that are not used
            cleanupRFs(mach);

            // set bus width to the max used bitwidth
            int maxWidth = *usedOpWidths_.rbegin();
            if (maxWidth < mach->busNavigator().item(0)->width()) {
                verboseLog(
                    "Setting bus width to: " +
                    Conversion::toString(maxWidth));
                mach->busNavigator().item(0)->setWidth(maxWidth);
            }

            // Remove unconnected sockets
            cleanupSockets(mach);

            usedOperations_.clear();
            usedOpWidths_.clear();

            if (!doSimulate) {
                doSimulate =
                    true;  // run while loop again including simulation
            } else {
                break;  // exit while loop
            }
        }

        // TODO: what is the proper number of FU copies?
        duplicateFUs(mach, 1);

        verboseLog(
            "Final machine contains " +
            Conversion::toString(mach->functionUnitNavigator().count()) +
            " FUs and " +
            Conversion::toString(mach->registerFileNavigator().count()) +
            " RFs.");

        // add machine to configuration
        conf.architectureID = dsdb.addArchitecture(*mach);
        // add new configuration to dsdb
        RowID confID = dsdb.addConfiguration(conf);
        evaluate(conf);
        result.push_back(confID);

        return result;
    }

private:
    // Initial machine has premade FU(s), list of their names
    static const TCEString skeletonPN_;
    TCEString skeleton_;
    std::vector<TCEString> skeletonFUs_;

    // Operating mode vector or scalar
    static const TCEString modePN_;
    TCEString mode_;

    // Absence of some operations result in tce compiler error even if they
    // are not used in a scheduled program. Solution -> do not prune them
    std::set<TCEString> minimalOpset_ = {"add", "and", "eq",  "gt",  "gtu",
                                         "ior", "sub", "xor", "shr", "shru"};

    std::set<TCEString> usedOperations_;
    std::set<int> usedOpWidths_;
    // Removing some operations may decrease performance, use a penalty map in
    // threshold calculations
    std::map<TCEString, int> penaltyMap{{"mul", 15}};

    /**
     * Reads the parameters given to the plugin.
     */
    void
    readParameters() {
        readOptionalParameter(skeletonPN_, skeleton_);
        readOptionalParameter(modePN_, mode_);
    }

    /**
     * Checks if specified Function Unit belongs to predefined skeleton arch.
     *
     * @param fuName Function Unit name.
     * @return True function units belongs to skeleton arch.
     */
    bool
    isSkeletonFU(TCEString fuName) {
        if (std::find(skeletonFUs_.begin(), skeletonFUs_.end(), fuName) !=
            skeletonFUs_.end()) {
            return true;
        }
        return false;
    }

    /**
     * Creates a specified amount of duplicate copies of FU in a machine.
     *
     * @param Machine.
     * @param Number of duplicate copies.
     */
    void
    duplicateFUs(Machine* mach, int numCopies) {
        const Machine::FunctionUnitNavigator fuNav =
            mach->functionUnitNavigator();
        OperationPool pool;

        int numFUs = fuNav.count();

        for (int fid = 0; fid < numFUs; fid++) {
            FunctionUnit* fu = fuNav.item(fid);

            if (isSkeletonFU(fu->name())) {
                if (Application::verboseLevel() > 1) {
                    Application::logStream()
                        << "Skipping skeleton FU" << std::endl;
                }
                continue;
            }

            int numOPs = fu->operationCount();

            for (int oid = 0; oid < numOPs; oid++) {
                HWOperation* hwop = fu->operation(oid);
                const Operation& osalOp =
                    pool.operation(hwop->name().c_str());

                // create FU copies
                for (int i = 0; i < numCopies; i++) {
                    ExplorationTools::createOperationFU(mach, osalOp);
                }
            }
        }
    }

    /*
     * Prunes Function Units that are not used based on list of operations
     * collected by schedule or simulation.
     *
     * @param mach Machine given.
     * @param simulated Cleanup using simulated operation executions data?
     * @param simUsage Simulated operation executions data.
     */
    void
    cleanupFUs(
        Machine* mach, bool simulated, std::map<TCEString, int> simUsage) {
        verboseLog("Removing unused FUs...");
        Machine::FunctionUnitNavigator fuNavi = mach->functionUnitNavigator();

        // get maximum execution count and calculate minimal threshold
        // execution
        int threshold;
        int maxExecCount = 0;
        if (simulated) {
            for (auto const& op : simUsage) {
                if (op.second > maxExecCount) {
                    maxExecCount = op.second;
                }
            }
            threshold = THRESHOLD_PERCENT * maxExecCount;
            verboseLog(" Threshold: " + Conversion::toString(threshold));
        }

        OperationPool pool;

        // delete FU if operation is not used
        for (int i = 0; i < fuNavi.count(); i++) {
            // skip skeleton FUs
            if (isSkeletonFU(fuNavi.item(i)->name())) {
                std::cout << "Skeleton: " << fuNavi.item(i)->name()
                          << std::endl;
                continue;
            }

            // assuming there is one operation in the FU
            HWOperation* op = fuNavi.item(i)->operation(0);
            const Operation& osalOp = pool.operation(op->name().c_str());

            // Prune based on simulation data
            if (simulated) {
                int opUsage = simUsage[op->name()];

                // apply penalty if operation name is in the penalty list
                if (penaltyMap.find(op->name()) != penaltyMap.end()) {
                    opUsage = opUsage * penaltyMap[op->name()];
                }

                if (mode_ == "scalar" && osalOp.isVectorOperation()) {
                    verboseLog("Removing: " + fuNavi.item(i)->name());
                    mach->removeFunctionUnit(*fuNavi.item(i));
                    i--;
                    continue;
                }

                // prune operations below the threshold level
                if ((opUsage < threshold) &&
                    (minimalOpset_.find(op->name()) == minimalOpset_.end()) &&
                    !osalOp.usesMemory()) {
                    verboseLog(
                        "Threshold removing: " + fuNavi.item(i)->name());
                    mach->removeFunctionUnit(*fuNavi.item(i));
                    i--;
                    continue;
                }
            }

            // Using skeleton, get rid of other LSUs
            if (fuNavi.item(i)->hasAddressSpace() &&
                skeletonFUs_.size() > 0) {
                verboseLog("Unnesesary LSU: " + fuNavi.item(i)->name());
                mach->removeFunctionUnit(*fuNavi.item(i));
                i--;
                continue;
            }

            // Prune based on scheduled program data
            if ((usedOperations_.find(op->name()) == usedOperations_.end() &&
                 minimalOpset_.find(op->name()) == minimalOpset_.end())) {
                mach->removeFunctionUnit(*fuNavi.item(i));
                i--;
            }
        }
    }

    /*
     * Removes Register Files that are not used based on list of operations
     * collected by schedule and simulation.
     * data.
     *
     * @param mach Machine given.
     */
    void
    cleanupRFs(Machine* mach) {
        verboseLog("Removing unused RFs...");
        Machine::FunctionUnitNavigator fuNavi = mach->functionUnitNavigator();
        Machine::RegisterFileNavigator rfNavi = mach->registerFileNavigator();

        // gather FU operation operand widths
        for (int i = 0; i < fuNavi.count(); i++) {
            FunctionUnit* fu = fuNavi.item(i);
            for (int j = 0; j < fu->operationPortCount(); j++) {
                usedOpWidths_.insert(fu->operationPort(j)->width());
            }
        }

        for (int i = 0; i < rfNavi.count(); i++) {
            RegisterFile* rf = rfNavi.item(i);

            if (usedOpWidths_.find(rf->width()) == usedOpWidths_.end()) {
                verboseLogC(" Removing RF: " + rf->name(), 2);
                mach->removeRegisterFile(*rfNavi.item(i));
                i--;
            }
        }
    }

    /*
     * Removes sockets that are not connected to anything.
     *
     * @param mach Machine given.
     */
    void
    cleanupSockets(Machine* mach) {
        Machine::SocketNavigator socketNavi = mach->socketNavigator();
        for (int i = 0; i < socketNavi.count(); i++) {
            if (socketNavi.item(i)->portCount() == 0) {
                mach->removeSocket(*socketNavi.item(i));
                i--;
            }
        }
    }
};

const TCEString PruneUnusedUnits::skeletonPN_("skeleton");
const TCEString PruneUnusedUnits::modePN_("mode");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(PruneUnusedUnits)
