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
 * @file AllOperationMachine.cc
 *
 * Collects all operations from OSAL and creates separate FU for each
 * operation.
 *
 * @author Alex Hirvonen 2017 (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */

#include <set>
#include <vector>

#include "Conversion.hh"
#include "DSDBManager.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "Exception.hh"
#include "ExecutionPipeline.hh"
#include "ExplorationTools.hh"
#include "FUPort.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "Machine.hh"
#include "Operand.hh"
#include "Operation.hh"
#include "OperationIndex.hh"
#include "OperationModule.hh"
#include "OperationPool.hh"
#include "Segment.hh"

using namespace TTAMachine;

class AllOperationMachine : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION(
        "Collects all operations from OSAL and adds a separate "
        "FU for each operation into current machine.");

    AllOperationMachine()
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
        return false;
    }

    virtual std::vector<RowID>
    explore(const RowID& configurationID, const unsigned int&) {
        readParameters();

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

        // dont add load/store operations for wrong endianness
        std::set<TCEString> unwantedMemOps;
        if (mach->isLittleEndian()) {
            unwantedMemOps = {"ALDH", "ALDHU", "ALDQ", "ALDQU", "ALDW", "AST",
                              "ASTH", "ASTQ",  "ASTW", "LDQ",   "LDQU", "LDH",
                              "LDHU", "LDW",   "STQ",  "STH",   "STW"};
        } else {
            unwantedMemOps = {"LD8",  "LDU8", "LD16", "LDU16",
                              "LD32", "ST8",  "ST16", "ST32"};
        }
        blacklist_.insert(unwantedMemOps.begin(), unwantedMemOps.end());

        // Add Immediate Unit
        if (mach->immediateUnitNavigator().count() == 0) {
            ImmediateUnit* immu =
                new ImmediateUnit("IMM_32", 1, 32, 1, 0, Machine::ZERO);
            mach->addImmediateUnit(*immu);
            RFPort* port = new RFPort("out1", *immu);
            ExplorationTools::connectPortToBus(
                port, mach->busNavigator().item(0), Socket::OUTPUT);
        }

        std::vector<TCEString> skeletonFUs =
            StringTools::chopString(skeleton_, ";");

        TTAMachine::Machine::FunctionUnitNavigator fuNavi =
            mach->functionUnitNavigator();

        // Remove existing FUs
        for (int i = 0; i < fuNavi.count(); i++) {
            FunctionUnit* fu = fuNavi.item(i);
            TCEString fuName = fu->name();
            // skip in case of existing skeleton IO
            if (skeletonFUs.size() > 0) {
                // add skeleton FU operations to blacklist
                if (std::find(
                        skeletonFUs.begin(), skeletonFUs.end(), fuName) !=
                    skeletonFUs.end()) {
                    for (int opId = 0; opId < fu->operationCount(); opId++) {
                        HWOperation* hwOp = fu->operation(opId);
                        TCEString opName = hwOp->name();
                        verboseLogC("Skipping: " + opName.upper(), 2);
                        blacklist_.insert(opName.upper());
                    }
                    // do not remove skeleton FU
                    continue;
                }
            }
            mach->removeFunctionUnit(*fu);
            i--;
        }

        // Remove register files, except BOOL
        for (int i = 0; i < mach->registerFileNavigator().count(); i++) {
            RegisterFile* rf = mach->registerFileNavigator().item(i);
            if (rf->width() != 1) {
                mach->removeRegisterFile(
                    *mach->registerFileNavigator().item(i));
                i--;
            } else {
                rf->setGuardLatency(1);
            }
        }

        // Remove unconnected sockets
        Machine::SocketNavigator socketNavi = mach->socketNavigator();
        for (int i = 0; i < socketNavi.count(); i++) {
            if (socketNavi.item(i)->portCount() == 0) {
                mach->removeSocket(*socketNavi.item(i));
                i--;
            }
        }

        // Generate specified number of FUs with one single operation from
        // OSAL
        OperationPool pool;
        OperationIndex& index = pool.index();

        // get list of operations in OSAL
        std::vector<TCEString> osalOperations;
        for (int mId = 0; mId < index.moduleCount(); mId++) {
            OperationModule& module = index.module(mId);

            // Skip vector operations in scalar mode
            if (module.name() == "simd" && mode_ == "scalar") continue;

            verboseLog("Adding operations from module: " + module.name());
            for (int i = 0; i < index.operationCount(module); i++) {
                TCEString opName = index.operationName(i, module);

                // skip blacklisted operations
                if (blacklist_.find(opName) != blacklist_.end()) {
                    verboseLogC("Skipping: " + opName, 2);
                    continue;
                }
                osalOperations.push_back(opName);
            }
        }

        // create FUs from operations list
        for (unsigned int i = 0; i < osalOperations.size(); i++) {
            TCEString opName = osalOperations[i];
            const Operation& osalOp = pool.operation(opName.c_str());

            // create FU for each operation
            ExplorationTools::createOperationFU(mach, osalOp);
        }

        // Add distinct multi-width RFs and attach them to buses
        verboseLog("Adding Register Files");
        for (int width : {32, 64, 128, 256, 512, 1024, 2048, 4096}) {
            // Add register file
            RegisterFile* rf = new RegisterFile(
                "RF_" + Conversion::toString(width),
                512,  // size big enough to avoid register spilling
                width, 1, 1, 0, RegisterFile::NORMAL);
            mach->addRegisterFile(*rf);

            RFPort* inPort = new RFPort("in", *rf);
            RFPort* outPort = new RFPort("out", *rf);

            // connect ports to bus
            Bus* firstBus = mach->busNavigator().item(0);
            ExplorationTools::connectPortToBus(
                inPort, firstBus, Socket::INPUT);
            ExplorationTools::connectPortToBus(
                outPort, firstBus, Socket::OUTPUT);
        }
        mach->busNavigator().item(0)->setWidth(4096);

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
        result.push_back(confID);

        return result;
    }

private:
    // Initial machine has premade FU(s), list of their names
    static const TCEString skeletonPN_;
    TCEString skeleton_;

    // Operating mode vector or scalar
    static const TCEString modePN_;
    TCEString mode_;

    // list of operations that wont be added to the machine
    std::set<TCEString> blacklist_ = {
        // compiler crash
        "TRUNCHB",
        "TRUNCWB",
        "TRUNCWH",
        "CDF",
        "CFD",
        "HWLOOP",

        // these are already in GCU
        "JUMP",
        "CALL",

        // Skip unaligned memory access operations, as FUGen does not
        // support them
        "LD8X2",
        "LD8X4",
        "LD8X8",
        "LD8X16",
        "LD8X32",
        "LD8X64",
        "LD8X128",
        "LD8X256",
        "LD8X512",
        "LD16X2",
        "LD16X4",
        "LD16X8",
        "LD16X16",
        "LD16X32",
        "LD16X64",
        "LD16X128",
        "LD16X256",
        "LD32X2",
        "LD32X4",
        "LD32X8",
        "LD32X16",
        "LD32X32",
        "LD32X64",
        "LD32X128",
        "ST8X2",
        "ST8X4",
        "ST8X8",
        "ST8X16",
        "ST8X32",
        "ST8X64",
        "ST8X128",
        "ST8X256",
        "ST8X512",
        "ST16X2",
        "ST16X4",
        "ST16X8",
        "ST16X16",
        "ST16X32",
        "ST16X64",
        "ST16X128",
        "ST16X256",
        "ST32X2",
        "ST32X4",
        "ST32X8",
        "ST32X16",
        "ST32X32",
        "ST32X64",
        "ST32X128",

        // Problems with TDGen because of buggy? 64bit support
        "CHS",
        "CHSU",
        "SELECT64",
        "SXW64",
        "ZXW64",
        "EQ8",
        "EQ16",
        "GT8",
        "GTU8",
        "GT16",
        "GTU16",
    };

    /**
     * Reads the parameters given to the plugin.
     */
    void
    readParameters() {
        readOptionalParameter(skeletonPN_, skeleton_);
        readOptionalParameter(modePN_, mode_);
    }
};

const TCEString AllOperationMachine::skeletonPN_("skeleton");
const TCEString AllOperationMachine::modePN_("mode");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(AllOperationMachine)
