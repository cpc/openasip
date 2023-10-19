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
 * @file FUMergeMinimizer.cc
 *
 * Explorer plugin that greedily merges FUs which are rarely used
 * simultaneously.
 *
 * @author Alex Hirvonen
 * @note rating: red
 */

#include <map>
#include <string>
#include <vector>

#include "ControlUnit.hh"
#include "Conversion.hh"
#include "CostEstimates.hh"
#include "DSDBManager.hh"
#include "DesignSpaceExplorerPlugin.hh"
#include "Exception.hh"
#include "ExecutionPipeline.hh"
#include "FUPort.hh"
#include "FunctionUnit.hh"
#include "HDBRegistry.hh"
#include "HWOperation.hh"
#include "Instruction.hh"
#include "InstructionExecution.hh"
#include "Machine.hh"
#include "Move.hh"
#include "Operand.hh"
#include "Operation.hh"
#include "OperationPool.hh"
#include "Port.hh"
#include "Program.hh"
#include "Segment.hh"
#include "TemplateSlot.hh"
#include "TerminalImmediate.hh"
#include "TerminalRegister.hh"

using namespace TTAProgram;
using namespace TTAMachine;
using std::endl;

class FUMergeMinimizer : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION(
        "Explorer plugin that greedily merges FUs which are "
        "rarely used simultaneously.");

    FUMergeMinimizer()
        : DesignSpaceExplorerPlugin(), numLSUs_(1), dontMerge_("") {
        addParameter(
            numLSUsPN_, UINT, false, Conversion::toString(numLSUs_),
            "Minimum number of LSU to be created.");
        addParameter(dontMergePN_, STRING, false, dontMerge_);
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
    benchmark(
        TTAMachine::Machine* mach, DSDBManager::MachineConfiguration& conf,
        std::vector<double>& fuCovariance, ClockCycleCount& cycleCount) {
        cycleCount = 0;

        DSDBManager& dsdb = db();
        int fuCount = mach->functionUnitNavigator().count();

        // generate needed variable to easily get FU index
        std::map<const FunctionUnit*, int> fuToIndex;
        for (int i = 0; i < fuCount; i++) {
            FunctionUnit* fu = mach->functionUnitNavigator().item(i);
            fuToIndex[fu] = i;
        }

        assert(fuCount > 0);

        // benchmark all applications in dsdb
        std::set<RowID> applicationIDs = dsdb.applicationIDs();
        for (std::set<RowID>::const_iterator id = applicationIDs.begin();
             id != applicationIDs.end(); id++) {
            // calc covariance for each instruction
            std::vector<double> partialCovariance(fuCount * fuCount, 0);

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

            for (unsigned int i = 0; i < executions.size(); i++) {
                if (executions[i] == 0) continue;

                Instruction* instr = instructions[i];
                std::vector<bool> hasFUAccess(fuCount, false);

                // gather infos about FUs used on the same clock cycle
                for (int j = 0; j < instr->moveCount(); j++) {
                    Move& move = instr->move(j);

                    if (move.source().isFUPort()) {
                        const FunctionUnit* fu =
                            &move.source().functionUnit();

                        if (!dynamic_cast<const ControlUnit*>(fu)) {
                            int index = fuToIndex.at(fu);
                            hasFUAccess[index] = true;
                        }
                    }
                    if (move.destination().isFUPort()) {
                        const FunctionUnit* fu =
                            &move.destination().functionUnit();

                        if (!dynamic_cast<const ControlUnit*>(fu)) {
                            int index = fuToIndex.at(fu);
                            hasFUAccess[index] = true;
                        }
                    }
                }

                // calculate partial covariance matrix
                for (int j = 0; j < fuCount; j++) {
                    if (!hasFUAccess[j]) continue;

                    partialCovariance[j + j * fuCount] += executions[i];

                    for (int k = j + 1; k < fuCount; k++) {
                        if (!hasFUAccess[k]) continue;

                        partialCovariance[k + j * fuCount] += executions[i];
                        partialCovariance[j + k * fuCount] += executions[i];
                    }
                }
            }

            /*            verboseLog("Partial FU covariance matrix:\n");
                        for (int j = 0; j < fuCount; j++) {
                            for (int k = 0; k < fuCount; k++) {
                                Application::errorStream()
                                    << partialCovariance[j + k * fuCount] <<
               "\t";
                            }
                            verboseLog("\n");
                        }
            */
            // calculate FU covariance matrix
            for (int j = 0; j < fuCount * fuCount; j++) {
                fuCovariance[j] += partialCovariance[j] / partialCycleCount;
            }
            //            Application::errorStream()
            //                << "Partial cycle count:\t" << partialCycleCount
            //                << std::endl;
            db().addCycleCount((*id), conf.architectureID, partialCycleCount);
        }
        /*
                Application::errorStream()
                    << "FU covariance matrix:" << std::endl;
                for (int j = 0; j < fuCount; ++j) {
                    FunctionUnit* fu = mach->functionUnitNavigator().item(j);
                    Application::errorStream() << fu->name() << "\t";
                }
                Application::errorStream() << std::endl;
                for (int j = 0; j < fuCount; j++) {
                    for (int k = 0; k < fuCount; k++) {
                        Application::errorStream()
                            << fuCovariance[j + k * fuCount] << "\t";
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

        TTAMachine::Machine::FunctionUnitNavigator fuNavi =
            mach->functionUnitNavigator();

        // get information about function units in the machine
        int fuCount = fuNavi.count();
        int lsuUnits = 0;
        int nonMergeableUnits = 0;
        for (int i = 0; i < fuNavi.count(); i++) {
            FunctionUnit* fu = fuNavi.item(i);
            if (fu->hasAddressSpace()) {
                lsuUnits++;
            }
            if (!canMerge(fu->name())) {
                nonMergeableUnits++;
            }
        }

        // if 1 LSU -> no need to merge LSUs using covariance
        if (numLSUs_ == 1) {
            verboseLog("Merging LSUs without covariance calculations...");

            for (int i = 0; i < fuNavi.count(); i++) {
                FunctionUnit* lsu1 = NULL;

                if (fuNavi.item(i)->hasAddressSpace() &&
                    canMerge(fuNavi.item(i)->name())) {
                    lsu1 = fuNavi.item(i);
                } else {
                    continue;
                }
                // architecture must have atleast one LSU
                assert(lsu1 != NULL);

                for (int j = i + 1; j < fuNavi.count(); j++) {
                    if (fuNavi.item(j)->hasAddressSpace() &&
                        canMerge(fuNavi.item(j)->name())) {
                        FunctionUnit* lsu2 = fuNavi.item(j);
                        mergeFUs(mach, lsu1, lsu2);
                        j--;
                    }
                }
                lsu1->setName("LSU");
                removeUnconnectedSockets(mach);
                break;
            }
        }

        // loop while we have only 1 FU + N user defined LSUs left
        while (true) {
            fuCount = fuNavi.count();

            // check if merge is done
            if (fuCount == (1 + numLSUs_ + nonMergeableUnits)) break;

            std::vector<double> FUCovariance(fuCount * fuCount, 0);

            ClockCycleCount cycleCount = 0;
            benchmark(mach, conf, FUCovariance, cycleCount);

            double minCorr = 0x7fffffff;
            int minUnitA = -1;
            int minUnitB = -1;
            bool needsLSUMerge = false;

            // check if we have LSUs left to merge
            if (numLSUs_ > 1)
                needsLSUMerge = hasNumLSUsLeft(mach, numLSUs_ + 1);

            // find units with minimum covariance
            for (int j = 0; j < fuCount - 1; j++) {
                if (!canMerge(fuNavi.item(j)->name())) continue;

                for (int k = j + 1; k < fuCount; k++) {
                    if (!canMerge(fuNavi.item(k)->name())) continue;

                    // both LSUs or both normal FUs
                    if ((needsLSUMerge && fuNavi.item(j)->hasAddressSpace() &&
                         fuNavi.item(k)->hasAddressSpace()) ||
                        (!fuNavi.item(j)->hasAddressSpace() &&
                         !fuNavi.item(k)->hasAddressSpace())) {
                        double corr = FUCovariance[j + k * fuCount];
                        if (corr < minCorr) {
                            minCorr = corr;
                            minUnitA = j;
                            minUnitB = k;
                        }
                    }
                }
            }

            // nothing to merge
            if (minUnitA == -1 || minUnitB == -1) break;

            mergeFUs(mach, fuNavi.item(minUnitA), fuNavi.item(minUnitB));
            removeUnconnectedSockets(mach);
            removeUnusedBuses(mach);

            // add machine to configuration
            conf.architectureID = dsdb.addArchitecture(*mach);
            RowID confId = dsdb.addConfiguration(conf);

            verboseLog(
                "Created a new configuration " +
                Conversion::toString(confId));

            result.push_back(confId);
        }

        // Benchmark also the last configuration
        int unitsLeft = fuNavi.count();
        std::vector<double> fuCovariance(unitsLeft * unitsLeft, 0);
        ClockCycleCount cycleCount = 0;
        benchmark(mach, conf, fuCovariance, cycleCount);

        return result;
    }

private:
    static const TCEString numLSUsPN_;
    static const TCEString dontMergePN_;
    // Number of parallel LSUs to create
    int numLSUs_;
    // FUs that are not allowed to be merged, separated by semicolon
    TCEString dontMerge_;

    /**
     * Reads the parameters given to the plugin.
     */
    void
    readParameters() {
        readOptionalParameter(numLSUsPN_, numLSUs_);
        readOptionalParameter(dontMergePN_, dontMerge_);
    }

    /**
     * Checks if machine has specified amount of LSUs.
     *
     * @param mach Machine given.
     * @param amout Specified amount of LSUs.
     */
    bool
    hasNumLSUsLeft(Machine* mach, int amount) {
        for (int i = 0; i < mach->functionUnitNavigator().count(); i++) {
            if (mach->functionUnitNavigator().item(i)->hasAddressSpace()) {
                amount--;
            }
        }
        return (amount < 1);
    }

    /**
     * Checks if specified Function Unit is allowed to be merged.
     *
     * @param fuName Function Unit name.
     * @return True if Function Unit is allowed to be merged.
     */
    bool
    canMerge(std::string fuName) {
        std::vector<TCEString> skipList =
            StringTools::chopString(dontMerge_, ";");

        if (std::find(skipList.begin(), skipList.end(), fuName) ==
            skipList.end()) {
            return true;
        }
        return false;
    }

    /**
     * Merges two function units in given machine.
     *
     * Merges function unit B into A and removes function unit B from machine.
     *
     * @param mach given TTAMachine
     * @param fuA Function Unit A
     * @param fuB Function Unit B
     */
    void
    mergeFUs(
        TTAMachine::Machine* mach, FunctionUnit* fuA, FunctionUnit* fuB) {
        verboseLog("Merging: " + fuB->name() + " into " + fuA->name());

        OperationPool pool;
        for (int i = 0; i < fuB->operationCount(); i++) {
            std::string opName = fuB->operation(i)->name();

            // skip if A has that operation already
            if (fuA->hasOperation(opName)) {
                continue;
            }

            verboseLog(" -Merging operation: " + opName);

            // get operation information from OSAL
            const Operation& osalOp = pool.operation(opName.c_str());
            HWOperation* hwOp = new HWOperation(opName, *fuA);
            ExecutionPipeline* pipelineB = fuB->operation(i)->pipeline();

            // select B's operation operand and find suitable port in A
            for (int j = 1; j <= osalOp.operandCount(); j++) {
                // std::cout << "Operand: #" << j << std::endl;

                Operand operand = osalOp.operand(j);
                FUPort* portA = NULL;
                FUPort* portB = fuB->operation(opName)->port(j);
                Socket* socketA = NULL;
                Socket* socketB = operand.isInput() ? portB->inputSocket()
                                                    : portB->outputSocket();

                bool foundPort = false;
                // find first available port in A to bind the operand
                for (int pid = 0; pid < fuA->operationPortCount(); pid++) {
                    portA = fuA->operationPort(pid);

                    // go to next port if this one is already bound by hwOp
                    if (hwOp->isBound(*portA)) {
                        continue;
                    }
                    // find triggering port for connection
                    if (portB->isTriggering()) {
                        if (portA->isTriggering()) {
                            foundPort = true;
                            break;
                        } else {
                            // cycle untill we find triggering port in A
                            continue;
                        }
                    } else {
                        // find port for usual input/output ports
                        if ((!portA->isTriggering() && portA->isInput() &&
                             operand.isInput()) ||
                            (portA->isOutput() && operand.isOutput())) {
                            foundPort = true;
                            break;
                        }
                    }
                }

                int operandWidth =
                    operand.elementWidth() * operand.elementCount();

                // 64bit bug workaround fix
                if (portB->isTriggering()) {
                    if (opName == "ld32" || opName == "st32" ||
                        opName == "ld8" || opName == "ld16" ||
                        opName == "ldu8" || opName == "ldu16") {
                        operandWidth = 32;
                    }
                }  // workaround bugfix end

                if (foundPort) {
                    // found port in A
                    // grow A's port width if its smaller
                    if (portA->width() < operandWidth) {
                        portA->setWidth(operandWidth);
                    }
                    socketA = portA->isInput() ? portA->inputSocket()
                                               : portA->outputSocket();

                } else {
                    // input/output port not found, also trigger not found
                    portA = createFUPort(
                        fuA, operandWidth, operand.isInput(),
                        portB->isTriggering());
                    socketA = createSocket(mach);
                }

                // copy latencies
                for (int cycle = 0; cycle < pipelineB->latency(); cycle++) {
                    if (pipelineB->isPortRead(*portB, cycle)) {
                        hwOp->pipeline()->addPortRead(j, cycle, 1);
                    }
                    if (pipelineB->isPortWritten(*portB, cycle)) {
                        hwOp->pipeline()->addPortWrite(j, cycle, 1);
                    }
                }
                // bind operand to A's FUPort
                hwOp->bindPort(j, *portA);

                // connect A's port socket to B's bus connections
                if (!foundPort) {
                    for (int busId = 0; busId < socketB->segmentCount();
                         busId++) {
                        if (!socketA->isConnectedTo(
                                *socketB->segment(busId))) {
                            socketA->attachBus(*socketB->segment(busId));
                        }
                    }
                } else {
                    // TODO: bypass connections???
                    // remove bypasses from A to B
                    // check if output socket connected bus is connected to B
                    // input socket

                    // check if we already have the connection in socketA to
                    // specific RF
                    Segment* rfSegment =
                        rfConnectedBus(mach, socketA, operandWidth);
                    if (rfSegment == NULL) {
                        // no rf connection, copy connection from socketB
                        // TODO: copy bypasses
                        rfSegment =
                            rfConnectedBus(mach, socketB, operandWidth);
                        socketA->attachBus(*rfSegment);
                    }
                }

                // if we created new input/output port before
                if (portA->isConnectedTo(*socketA) == false) {
                    Socket::Direction direction =
                        operand.isInput() ? Socket::INPUT : Socket::OUTPUT;
                    socketA->setDirection(direction);
                    portA->attachSocket(*socketA);
                }
            }
        }
        mach->removeFunctionUnit(*fuB);
    }

    /**
     * Check if specific socket has connection to register file of specified
     * width.
     *
     * @param  mach Input TTA machine.
     * @param  sock Input socket
     * @param  width width of register file.
     * @return bus Pointer to the bus connected to that register file.
     */
    Segment*
    rfConnectedBus(
        TTAMachine::Machine* mach, TTAMachine::Socket* sock, int width) {
        if (width > 1 && width < 32) {
            width = 32;
        }

        // cycle through all socket's connected buses
        for (int sid = 0; sid < sock->segmentCount(); sid++) {
            Segment* seg = sock->segment(sid);

            // cycle all register files
            for (int rfid = 0; rfid < mach->registerFileNavigator().count();
                 rfid++) {
                RegisterFile* rf = mach->registerFileNavigator().item(rfid);

                // skip untill we have correct register file width
                if (rf->width() != width) {
                    continue;
                }

                // cycle all register file ports
                for (int pid = 0; pid < rf->portCount(); pid++) {
                    RFPort* rfPort = rf->port(pid);

                    if ((sock->direction() == Socket::INPUT &&
                         rfPort->isInput()) ||
                        (sock->direction() == Socket::OUTPUT &&
                         rfPort->isOutput())) {
                        continue;
                    }

                    Socket* rfSocket = sock->direction() == Socket::INPUT
                                           ? rfPort->outputSocket()
                                           : rfPort->inputSocket();

                    // if that rf port socket is connected to that bus ->
                    // return true
                    if (rfSocket->isConnectedTo(*seg)) {
                        return seg;
                    }
                }
            }
        }
        return NULL;
    }

    /**
     * Removes unused Bus which connections are not used after FU merge.
     *
     * @param mach Given TTAMachine.
     */
    void
    removeUnusedBuses(TTAMachine::Machine* mach) {
        Machine::BusNavigator busNavi = mach->busNavigator();
        Machine::SocketNavigator socketNavi = mach->socketNavigator();

        for (int bid = 0; bid < busNavi.count(); bid++) {
            bool removeBus = false;
            bool rfConnected = false;
            bool fuConnected = false;
            bool hasInputs = false;
            bool hasOutputs = false;
            Segment* segment = busNavi.item(bid)->segment(0);

            for (int cid = 0; cid < segment->connectionCount(); cid++) {
                Socket* sock = segment->connection(cid);
                Port* port = sock->port(0);

                if (port->isInput()) {
                    hasInputs = true;
                } else if (port->isOutput()) {
                    hasOutputs = true;
                }

                if (dynamic_cast<RFPort*>(port)) {
                    rfConnected = true;
                } else if (dynamic_cast<FUPort*>(port)) {
                    fuConnected = true;
                }
            }
            // simply remove bus with unused connections
            if (rfConnected && !fuConnected) {
                removeBus = true;
            }
            if ((hasInputs && !hasOutputs) || (!hasInputs && hasOutputs)) {
                removeBus = true;
            }
            if (removeBus) {
                mach->removeBus(*busNavi.item(bid));
                bid--;
            }
        }

        // TODO remove rf ports
        for (int sid = 0; sid < socketNavi.count(); sid++) {
            if (socketNavi.item(sid)->segmentCount() == 0) {
                Port* port = socketNavi.item(sid)->port(0);

                if (dynamic_cast<RFPort*>(port)) {
                    delete port;
                }
            }
        }
        removeUnconnectedSockets(mach);
    }

    /**
     * Removes unconnected sockets in the given machine.
     *
     * @ param mach Given TTAMachine.
     */
    void
    removeUnconnectedSockets(TTAMachine::Machine* mach) {
        // Remove unconnected sockets
        Machine::SocketNavigator socketNavi = mach->socketNavigator();
        for (int i = 0; i < socketNavi.count(); i++) {
            if (socketNavi.item(i)->portCount() == 0) {
                mach->removeSocket(*socketNavi.item(i));
                i--;
            }
        }
    }

    /**
     * Adds port to a given function unit.
     *
     * @param fu Function unit for that port.
     * @param width Bit width.
     * @param isInput Port is input.
     * @param isTriggering Port is triggering.
     * @return Created function unit port pointer.
     */
    FUPort*
    createFUPort(
        FunctionUnit* fu, int width, bool isInput, bool isTriggering) {
        std::string prefix = "";
        std::string postfix = "";

        if (isInput) {
            prefix = "in";
        } else {
            prefix = "out";
        }
        if (isTriggering) {
            postfix = "t";
        }

        unsigned idx = 1;
        while (fu->hasPort(prefix + Conversion::toString(idx) + postfix)) {
            idx++;
        }

        // TODO add port creation success check
        FUPort* port = new FUPort(
            prefix + Conversion::toString(idx) + postfix, width, *fu, false,
            false);

        if (isTriggering) {
            port->setTriggering(true);
        }

        return port;
    }

    /**
     * Creates socket in the given machine.
     *
     * @param mach Machine for the socket.
     * @return Created socket.
     */
    Socket*
    createSocket(Machine* mach) {
        unsigned int idx = 0;
        std::string socketPrefix = "S_";
        const Machine::SocketNavigator socketNav = mach->socketNavigator();

        while (socketNav.hasItem(socketPrefix + Conversion::toString(idx))) {
            idx++;
        }
        Socket* socket = new Socket(socketPrefix + Conversion::toString(idx));
        mach->addSocket(*socket);

        return socket;
    }
};

const TCEString FUMergeMinimizer::numLSUsPN_("num_lsu");
const TCEString FUMergeMinimizer::dontMergePN_("dont_merge");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(FUMergeMinimizer)
