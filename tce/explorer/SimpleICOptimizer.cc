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
 * @file SimpleICOptimizer.cc
 *
 * Explorer plugin that optimizes the interconnection network by
 * removing extra sockets and not used connections.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @author Esa Määttä 2008 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#include <vector>
#include <string>
#include <set>

#include "DesignSpaceExplorerPlugin.hh"
#include "DSDBManager.hh"
#include "Machine.hh"
#include "TestApplication.hh"
#include "Program.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "Port.hh"
#include "FUPort.hh"
#include "Socket.hh"
#include "Terminal.hh"
#include "Terminal.hh"
#include "HDBRegistry.hh"
#include "ICDecoderEstimatorPlugin.hh"
#include "ControlUnit.hh"
#include "HWOperation.hh"
#include "StringTools.hh"
#include "Segment.hh"
#include "CostEstimates.hh"
#include "Application.hh"
#include "Exception.hh"
#include "Procedure.hh"

#include "LLVMBackend.hh"
#include "RegisterQuantityCheck.hh"

#include "MinimalOpSetCheck.hh"
#include "MachineCheckResults.hh"


using namespace TTAProgram;
using namespace TTAMachine;
using namespace HDB;
using std::endl;

/**
 * Explorer plugin that optimizes the interconnection network of the given
 * configuration by removing the connections that are not used in the
 * parallel program.
 */
class SimpleICOptimizer : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION("Optimizes the IC of the given configuration");

    SimpleICOptimizer(): DesignSpaceExplorerPlugin(), 
        tpef_(""),
        addOnly_(false),
        evaluateResult_(true),
        preserveMinimalOpset_(true) {

        // compulsory parameters
        // none

        // parameters that have a default value
        addParameter(tpefPN_, STRING, false, "");
        addParameter(addOnlyPN_, BOOL, false, 
                Conversion::toString(addOnly_));
        addParameter(evaluatePN_, BOOL, false, 
                Conversion::toString(evaluateResult_));
        addParameter(preserveMinOpsPN_, BOOL, false, 
                Conversion::toString(preserveMinimalOpset_));
    }

    virtual bool requiresStartingPointArchitecture() const { return true; }
    virtual bool producesArchitecture() const { return true; }
    virtual bool requiresHDB() const { return false; }
    virtual bool requiresSimulationData() const { return false; }
    virtual bool requiresApplication() const { return false; }
    
    /**
     * Optimizes the IC of the given configuration by removing not used 
     * connections. 
     *
     * First removes connections and then adds them by looking
     * all application domains programs instructions and their moves.
     * Needs the start point configuration to get the architecture
     * which connections are optimized.
     *
     * Supported parameters:
     * - tpef, name of the scheduled program file, no default value.
     * - add_only, if parameter is set the connections of the given
     *   configuration won't be emptied, just added new ones, not set as
     *   default.
     *
     * @param startPointConfigurationID Configuration to optimize.
     */
    virtual std::vector<RowID>
    explore(const RowID& startPointConfigurationID, const unsigned int&) {

        std::vector<RowID> result;

        readParameters();
        
        try {
            DSDBManager& dsdb = db();
            DSDBManager::MachineConfiguration conf = 
                dsdb.configuration(startPointConfigurationID);
            
            // loads starting configuration
            Machine* origMach = NULL;
            Machine* mach = NULL;
            try {
                origMach = dsdb.architecture(conf.architectureID);
                mach = dsdb.architecture(conf.architectureID);
            } catch (const Exception& e) {
                std::ostringstream msg(std::ostringstream::out);
                msg << e.errorMessage() << std::endl;
                verboseLog(msg.str());
                return result;
            }

            if (!addOnly_) {
                removeAllConnections(*mach);
            }

            // if no scheduled programs given
            Program* program = NULL;
            if (tpef_ == "") {
                //SchedulerFrontend scheduler;
                try {
                    std::set<RowID> appIDs = dsdb.applicationIDs();
                    std::set<RowID>::const_iterator iter = appIDs.begin();
                    if (iter == appIDs.end()) {
                        // no applications, nothing can be done
                        return result;
                    }
                    for (; iter != appIDs.end(); iter++) {
                        TestApplication testApp(dsdb.applicationPath(*iter));

                        program = DesignSpaceExplorer::schedule(
                                testApp.applicationPath(), *origMach);
                        if (program == NULL) {
                            std::string msg = "SimpleICOptimizer: Schedule "
                                "failed for program: " + 
                                testApp.applicationPath();
                            verboseLogC(msg, 1)
                            return result;     
                        }

                        addConnections(*mach, *program);
                    }
                } catch (const Exception& e) {
                    std::ostringstream msg(std::ostringstream::out);
                    msg << e.errorMessageStack() << std::endl;
                    verboseLog(msg.str());
                    delete mach;
                    mach = NULL;
                    return result;
                }
            } else {
                try {
                    program = Program::loadFromTPEF(tpef_, *origMach);

                    addConnections(*mach, *program);
                } catch (const Exception& e) {
                    std::ostringstream msg(std::ostringstream::out);
                    msg << "Error while loading the program in "
                        << "SimpleICOptimizer:" << endl
                        << e.errorMessage() << endl;
                    verboseLog(msg.str());
                    return result;
                }
            }
             
            DSDBManager::MachineConfiguration tempConf;
            try {
                tempConf.architectureID = dsdb.addArchitecture(*mach);
                tempConf.hasImplementation = false;
            } catch (const RelationalDBException& e) {
                std::ostringstream msg(std::ostringstream::out);
                msg << "Error while adding ADF to the dsdb. "
                    << "ADF probably too big." << endl;
                verboseLog(msg.str());
                return result;
            }

            // add config to database to pass it to
            // RemoveUnconnectedComponents plugin
            RowID tempConfID = dsdb.addConfiguration(tempConf);
            
            // remove unconnected components with RemoveUnconnectedComponents
            // plugin
            DesignSpaceExplorerPlugin* removeUnconnected =
                DesignSpaceExplorer::loadExplorerPlugin(
                    "RemoveUnconnectedComponents", &db());

            // parameters for RemoveUnconnectedComponents plugin
            removeUnconnected->giveParameter("allow_remove", "true");

            std::vector<RowID> cleanedTempConfIDs = 
                removeUnconnected->explore(tempConfID);
            delete removeUnconnected;
            removeUnconnected = NULL;
            if (cleanedTempConfIDs.size() < 1) {
                std::string errorMsg = "SimpleICOptimizer plugin: "
                    "RemoveUnconnectedComponents plugin failed to produce"
                    "result.";
                verboseLogC(errorMsg, 2)
                return result;
            }

            DSDBManager::MachineConfiguration newConf =
                dsdb.configuration(cleanedTempConfIDs.at(0));

            // if original machine had enough int registers, check and
            // possibly try to fix the new machine
            RegisterQuantityCheck RFCheck;
            if (RFCheck.checkIntRegs(*origMach)) {
                Machine* cleanedMach =
                    dsdb.architecture(newConf.architectureID);
                if (fixIntRegisters(*cleanedMach)) {
                    // replace newConf machine with the fixed one
                    newConf.architectureID =
                        dsdb.addArchitecture(*cleanedMach); 
                }
            }

            // create implementation for newConf if orginal config had one
            if (conf.hasImplementation && !newConf.hasImplementation) {
                createImplementation(newConf, newConf);
            } else {
                newConf.hasImplementation = false;
            }

            // evaluate and save new config id as result
            CostEstimates estimates;
            if (evaluateResult_) {
                // evaluate the new config
               bool estimate = newConf.hasImplementation;
                if (evaluate(newConf, estimates, estimate)) {
                    RowID confID = dsdb.addConfiguration(newConf);
                    result.push_back(confID);
                } else {
                    // cannot evaluate
                    std::ostringstream msg(std::ostringstream::out);
                    msg << "SimpleICOptimizer plugin couldn't evaluate "
                        << "generated config - nothing done." << endl;
                    verboseLog(msg.str());
                    return result;
                }
            } else {
                RowID confID = dsdb.addConfiguration(newConf);
                result.push_back(confID);
            }

        } catch (const Exception& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error while using SimpleICOptimizer:" << endl
                << e.errorMessage() << endl;
            verboseLog(msg.str());
            return result;
        }
        return result;
    }

private:
    // parameter name variables
    static const std::string tpefPN_;
    static const std::string tpefDefaultPN_;
    static const std::string addOnlyPN_;
    static const std::string evaluatePN_;
    static const std::string preserveMinOpsPN_;
    
    /// name of the tpef file
    std::string tpef_;
    /// can we remove connections
    bool addOnly_;
    /// evaluate the result(s)
    bool evaluateResult_;
    /// respect minimal opset when removing connections
    bool preserveMinimalOpset_;


    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        readOptionalParameter(tpefPN_, tpef_);
        readOptionalParameter(addOnlyPN_, addOnly_);
        readOptionalParameter(evaluatePN_, evaluateResult_);
        readOptionalParameter(preserveMinOpsPN_, preserveMinimalOpset_);
    }


    /**
     * Removes all socket-bus connection from the given machine.
     *
     * @param mach Machine which connections are removed.
     */
    void removeAllConnections(TTAMachine::Machine& mach) {
        Machine::SocketNavigator socketNav = mach.socketNavigator();
        Machine::BusNavigator busNav = mach.busNavigator();
        RegisterQuantityCheck RFCheck;
        MinimalOpSetCheck minimalOpSetCheck = MinimalOpSetCheck();

        if (!preserveMinimalOpset_) {
            for (int i = 0; i < socketNav.count(); i++) {
                Socket* socket = socketNav.item(i);
                for (int bus = 0; bus < busNav.count(); bus++) {
                    socket->detachBus(*busNav.item(bus));
                }
            }
        } else {
            // remove connections only if connected minimal opset preserved.
            for (int i = 0; i < socketNav.count(); i++) {
                Socket* socket = socketNav.item(i);
                std::set<std::string> ignoreFUNames;
                std::set<std::string> ignoreRFNames;
                // check all ports connected to the socket
                for (int si = 0; si < socket->portCount(); ++si) {
                    Port* port = socket->port(si); 
                    if (dynamic_cast<FUPort*>(port)) {
                        ignoreFUNames.insert(port->parentUnit()->name());
                    } else if (dynamic_cast<RFPort*>(port)) {
                        ignoreRFNames.insert(port->parentUnit()->name());
                    }
                }
                // TODO: refactor below
                // TODO: add test that same FU/RF is not checked multiple
                // times in situations where there are more than one socket
                // connected to the same FU/RF, do this by storing unit names
                // and the verdict if connections can be removed as a pair to
                // a map
                if (!ignoreFUNames.empty()) {
                    if (minimalOpSetCheck.checkWithIgnore(mach, ignoreFUNames)) {
                        if (!ignoreRFNames.empty()) {
                            if (RFCheck.checkWithIgnore(mach, ignoreRFNames)) 
                            {
                                for (int bus = 0; bus < busNav.count(); bus++) 
                                {
                                    socket->detachBus(*busNav.item(bus));
                                }
                            }
                        } else {
                            for (int bus = 0; bus < busNav.count(); bus++) {
                                socket->detachBus(*busNav.item(bus));
                            }
                        }
                    }
                } else if (!ignoreRFNames.empty()) {
                    if (RFCheck.checkWithIgnore(mach, ignoreRFNames)) {
                        for (int bus = 0; bus < busNav.count(); bus++) {
                            socket->detachBus(*busNav.item(bus));
                        }
                    }
                } else {
                    for (int bus = 0; bus < busNav.count(); bus++) {
                        socket->detachBus(*busNav.item(bus));
                    }
                } 
            }
        }
    }

    /**
     * Checks that produced machine is still usable
     *
     * @param mach Machine where the connections are added.
     * @return True if something done to the machine given as parameter.
     */
    bool fixIntRegisters(TTAMachine::Machine& mach) {
        // check that that machine has enough int registers and fix if needed
        // and can be fixed
        RegisterQuantityCheck RFCheck;
        if (!RFCheck.checkIntRegs(mach) && RFCheck.canFixIntRegs(mach)) {
            if (!RFCheck.fixIntRegs(mach)) {
                verboseLog("Failed to add missing integer registers to the"
                        "machine.");
                return false;
            } else {
                return true;
            }
        }
        return false;
    }

    /**
     * Adds connections used in the program to the given machine.
     *
     * Connections to add are determined by procedure instructions in
     * application domains programs.
     *
     * @param mach Machine where the connections are added.
     * @param prog Scheduled program where the used connections are searched.
     */
    void addConnections(TTAMachine::Machine& mach, Program& prog) {
        Machine::SocketNavigator socketNav = mach.socketNavigator();
        Machine::BusNavigator busNav = mach.busNavigator();

        // go through every procedure in the program
        for (int i = 0; i < prog.procedureCount(); i++) {
            Procedure& procedure = prog.procedure(i);

            // go through every instruction            
            for (int j = 0; j < procedure.instructionCount(); j++) {
                Instruction& instruction = procedure.instructionAtIndex(j);
                
                // check all moves and add connection from source to bus and
                // bus to destination
                for (int m = 0; m < instruction.moveCount(); m++) {
                    Move& move = instruction.move(m);
                    std::string busName = move.bus().name();
                    std::string sourceName = "";
                    try {
                        sourceName = move.sourceSocket().name();
                    } catch (const WrongSubclass&) {
                        // source was an immediate, no error
                        assert(move.source().isImmediate());
                    }
                    std::string destinationName =
                        move.destinationSocket().name();
                    Bus* bus = busNav.item(busName);

                    // connect the source socket to the bus
                    if (sourceName != "") {
                        Socket* socket = socketNav.item(sourceName);
                        for (int s = 0; s < bus->segmentCount(); s++) {
                            if (!socket->isConnectedTo(*bus->segment(s))) {
                                socket->attachBus(*bus->segment(s));
                                // socket direction is lost when the
                                // connections were removed
                                socket->setDirection(Socket::OUTPUT);
                            }
                        }
                    }
                    Socket* socket = socketNav.item(destinationName);
                    for (int s = 0; s < bus->segmentCount(); s++) {
                        if (!socket->isConnectedTo(*bus->segment(s))) {
                            socket->attachBus(*bus->segment(s));
                            // socket direction is lost when the
                            // connections were removed
                            socket->setDirection(Socket::INPUT);
                        }
                    }
                }
            }
        }
    }
};

// parameter names
const std::string SimpleICOptimizer::tpefPN_("tpef");
const std::string SimpleICOptimizer::addOnlyPN_("add_only");
const std::string SimpleICOptimizer::evaluatePN_("evaluate");
const std::string SimpleICOptimizer::preserveMinOpsPN_("preserve_min_ops");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(SimpleICOptimizer)
