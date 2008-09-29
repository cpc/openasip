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
 * @file SimpleICOptimizer.cc
 *
 * Explorer plugin that optimizes the interconnection network by
 * removing extra sockets and not used connections.
 *
 * @author Jari M‰ntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @author Esa M‰‰tt‰ 2008 (esa.maatta-no.spam-tut.fi)
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
public:
    DESCRIPTION("Optimizes the IC of the given configuration");
    
    /**
     * Optimizes the IC of the given configuration by removing not used 
     * connections. First removes connections and then adds them by looking
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

        if (startPointConfigurationID == 0) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "No configuration nor adf defined. Use -s <confID> to "
                << "define the configuration to be optimized." << endl;
            errorOuput(msg.str());
            return result;
        }

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
                errorOuput(msg.str());
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
                    errorOuput(msg.str());
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
                    errorOuput(msg.str());
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
                errorOuput(msg.str());
                return result;
            }

            // add config to database to pass it to
            // RemoveUnconnectedComponents plugin
            RowID tempConfID = dsdb.addConfiguration(tempConf);
            
            // remove unconnected components with RemoveUnconnectedComponents
            // plugin
            DesignSpaceExplorerPlugin* removeUnconnected =
                DesignSpaceExplorer::loadExplorerPlugin(
                    "RemoveUnconnectedComponents", db());

            // parameters for RemoveUnconnectedComponents plugin
            DesignSpaceExplorerPlugin::ParameterTable removeUParameters;
            DesignSpaceExplorerPlugin::Parameter allowRemovePar;
            allowRemovePar.name = "allow_remove";
            allowRemovePar.value = "true";
            removeUParameters.push_back(allowRemovePar);
            removeUnconnected->setParameters(removeUParameters);

            std::vector<RowID> cleanedTempConfIDs = 
                removeUnconnected->explore(tempConfID);
            if (cleanedTempConfIDs.size() < 1) {
                std::string errorMsg = "SimpleICOptimizer plugin: "
                    "RemoveUnconnectedComponents plugin failed to produce"
                    "result.";
                verboseLogC(errorMsg, 2)
                return result;
            }

            DSDBManager::MachineConfiguration newConf =
                dsdb.configuration(cleanedTempConfIDs.at(0));

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
                    errorOuput(msg.str());
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
            errorOuput(msg.str());
            return result;
        }
        return result;
    }

private:
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
        // Parameter name of bus number in the machine.
        const std::string tpef = "tpef";
        const std::string tpefDefault = "";
        const std::string addOnly = "add_only";
        const std::string evaluate = "evaluate";
        const std::string preserveMinOps = "preserve_min_ops";
        const bool preserveMinimalOpsetDefault = true;
        preserveMinimalOpset_ = false;
    
        if (hasParameter(tpef)) {
            try {
                tpef_ = parameterValue(tpef);
            } catch (const Exception& e) {
                parameterError(tpef, "String");
                tpef_ = tpefDefault;
            }
        } else {
            // set defaut value to tpef
            tpef_ = tpefDefault;
        }

        if (hasParameter(addOnly)) {
            try {
                addOnly_ = booleanValue(parameterValue(addOnly));
            } catch (const Exception& e) {
                parameterError(addOnly, "Boolean");
                addOnly_ = false;
            }
        } else {
            addOnly_ = false;
        }

        if (hasParameter(evaluate)) {
            try {
                evaluateResult_ = booleanValue(parameterValue(evaluate));
            } catch (const Exception& e) {
                parameterError(evaluate, "Boolean");
                evaluateResult_ = false;
            }
        } else {
            // set defaut value to tpef
            evaluateResult_ = true;
        }

        if (hasParameter(preserveMinOps)) {
            try {
                preserveMinimalOpset_ = booleanValue(parameterValue(preserveMinOps));
            } catch (const Exception& e) {
                parameterError(evaluate, "Boolean");
                preserveMinimalOpset_ = preserveMinimalOpsetDefault;
            }
        } else {
            // set default value
            preserveMinimalOpset_ = preserveMinimalOpsetDefault;
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
            Instruction* instruction = &procedure.firstInstruction();

            // go through every instruction
            bool first = true;
            while (procedure.hasNextInstruction(*instruction) || first) {
                first = false;
                // check all moves and add connection from source to bus and
                // bus to destination
                for (int m = 0; m < instruction->moveCount(); m++) {
                    Move& move = instruction->move(m);
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
                instruction = &procedure.nextInstruction(*instruction);
            }
        }
    }
};

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(SimpleICOptimizer)
