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
 * @file RemoveUnconnectedComponents.cc
 *
 * Explorer plugin that removes unconnected ports from units or creates
 * connections to these ports in case of a FUs. Also removes unconnected
 * buses. If all ports from a unit are removed, removes also the unit.
 *
 *
 * @author Esa Määttä 2007 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <vector>

#include "DesignSpaceExplorerPlugin.hh"
#include "DSDBManager.hh"
#include "Machine.hh"
#include "MachineTester.hh"
#include "Exception.hh"
#include "StringTools.hh"
#include "Conversion.hh"

#include "MachineResourceModifier.hh"

using std::endl;
using namespace TTAMachine;

/**
 * Explorer plugin that removes unconnected ports from units or creates
 * connections to these ports in case of a FUs. Also removes unconnected
 * buses. If all ports from a unit are removed, removes also the unit.
 */
class RemoveUnconnectedComponents : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION("Removes unconnected components from a configuration.");

    RemoveUnconnectedComponents(): DesignSpaceExplorerPlugin(), 
        allowRemoval_(false) {

        // compulsory parameters
        // no compulsory parameters

        // parameters that have a default value
        addParameter(allowRemovalPN_, BOOL, false, 
            Conversion::toString(allowRemoval_));
    }


    virtual bool requiresStartingPointArchitecture() const { return true; }
    virtual bool producesArchitecture() const { return true; }
    virtual bool requiresHDB() const { return false; }
    virtual bool requiresSimulationData() const { return false; }
    virtual bool requiresApplication() const { return false; }
    
    /**
     * Removes unconnected components from a configuration.
     *
     * Explorer plugin that removes unconnected ports from units or creates
     * connections to these ports in case of a FUs. Also removes unconnected
     * buses. If all ports from a unit are removed, removes also the unit.
     * Removes also unconnected FUs. First unconnected sockets are removed.
     *
     * Supported parameters:
     * - allow_remove, Allows the RFs port removal and portless RFs removal.
     *
     * @param configurationID Configuration to optimize.
     */
    virtual std::vector<RowID>
    explore(const RowID& configurationID, const unsigned int&) {

        std::vector<RowID> result;

        readParameters();

        try {
            DSDBManager& dsdb = db();
            DSDBManager::MachineConfiguration conf = 
                dsdb.configuration(configurationID);
            
            // get the machine belonging to the configuration
            Machine* mach = NULL;
            try {
                mach = dsdb.architecture(conf.architectureID);
            } catch (const Exception& e) {
                return result;
            }

            // removes sockets not connected to any bus
            removeSockets(*mach);

            // removes FUs not connected to any socket by any port
            std::vector<std::string> removedFUNames;
            removeUnconnectedFUs(*mach, removedFUNames);

            // checks that every FU port is connected to a socket
            checkFUPorts(*mach);

            // register files are removed only if allow_remove parameter is
            // given, else connections to sockets are made for RFs
            std::vector<std::string> removedRFNames;
            checkRFPorts(*mach, removedRFNames);

            // removes buses that have no connections to any socket
            checkBuses(*mach);

            DSDBManager::MachineConfiguration newConf;
            if (conf.hasImplementation) {
                newConf.hasImplementation = true;
                IDF::MachineImplementation* idf = 
                    dsdb.implementation(conf.implementationID);
                // removes removed RFs from idf
                while (!removedRFNames.empty()) {
                    idf->removeRFImplementation(removedRFNames.at(
                        removedRFNames.size()-1));
                    removedRFNames.pop_back();
                }
                // removes removed FUs from idf
                while (!removedFUNames.empty()) {
                    idf->removeFUImplementation(removedFUNames.at(
                        removedFUNames.size()-1));
                    removedFUNames.pop_back();
                }
                newConf.implementationID = 
                    dsdb.addImplementation(*idf, 0, 0);
            } else {
                newConf.hasImplementation = false;
            }
            newConf.architectureID = dsdb.addArchitecture(*mach);

            RowID confID = dsdb.addConfiguration(newConf);
            result.push_back(confID);

        } catch (const Exception& e) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "Error while using RemoveUnconnectedComponents:" 
                << endl << e.errorMessage() << endl;
            verboseLog(msg.str());
            return result;
        }
        return result;
    }

private:
    static const std::string allowRemovalPN_;
    /// parameter allow removal of unused ports and RFs without ports.
    bool allowRemoval_;


    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        readOptionalParameter(allowRemovalPN_, allowRemoval_);
    }


    /**
     * Removes totally unconnected FUs.
     * 
     * @param mach Machine which unconnected FUs are removed.
     * @param removedFUNames Names of FU units that were removed from machine
     */
    void removeUnconnectedFUs(TTAMachine::Machine& mach,
        std::vector<std::string>& removedFUNames) {

        //MachineTester& tester = mach.machineTester();
        Machine::SocketNavigator socketNav = mach.socketNavigator();

        Machine::FunctionUnitNavigator FUNav = mach.functionUnitNavigator();
        for (int i = 0; i < FUNav.count(); ++i) {
            FunctionUnit* FU = FUNav.item(i);
            bool noConnections = true;
            for (int j = 0; j < FU->portCount(); ++j) {
                Port* port = FU->port(j);
                // if no connections
                if (port->socketCount() > 0) {
                    noConnections = false;
                    break;
                }
            }
            // remove FU if it did not have any connections
            if (noConnections) {
                mach.removeFunctionUnit(*FU);
                removedFUNames.push_back(FU->name());
                --i;
            }
        }
    }

    /**
     * Checks that every FU port has at least one connection.
     * 
     * Adds an new connection to some available socket if port has no
     * connections
     *
     * @param mach Machine which FU ports are checked. 
     */
    void checkFUPorts(TTAMachine::Machine& mach) {
        MachineTester& tester = mach.machineTester();
        Machine::SocketNavigator socketNav = mach.socketNavigator();

        Machine::FunctionUnitNavigator FUNav = mach.functionUnitNavigator();
        for (int i = 0; i < FUNav.count(); ++i) {
            FunctionUnit* FU = FUNav.item(i);
            for (int j = 0; j < FU->portCount(); ++j) {
                Port* port = FU->port(j);
                // if no connections, make a connection to some socket
                if (port->socketCount() < 1) {
                    Socket* socket = NULL;
                    // find a socket where FU port can be connected
                    for (int soc = 0; i < socketNav.count(); ++soc) {
                        socket = socketNav.item(soc);
                        if (tester.canConnect(*socket, *port)) {
                            port->attachSocket(*socket);
                            break;
                        }
                    }
                }
            }
        }
    }

    /**
     * Checks that every RF port has connections.
     *
     * Removes every port of a RF that has no connections to sockets if
     * allowRemoval_ is true else makes connection to available socket.
     * If RF ends up having no ports, removes the RF from machine.
     *
     * @param mach Machine which RF ports are checked. 
     * @param removedRFNames Names of RF units that were removed from machine
     * content only added not read in this function.
     */
    void checkRFPorts(TTAMachine::Machine& mach, 
        std::vector<std::string>& removedRFNames) {

        MachineTester& tester = mach.machineTester();
        Machine::SocketNavigator socketNav = mach.socketNavigator();

        Machine::RegisterFileNavigator RFNav = mach.registerFileNavigator();
        for (int i = 0; i < RFNav.count(); ++i) {
            RegisterFile* RF = RFNav.item(i);
            for (int j = 0; j < RF->portCount(); ++j) {
                Port* port = RF->port(j);
                // if no connections
                if (port->socketCount() < 1) {
                    if (allowRemoval_) {
                        delete port;
                        port = NULL;
                        --j;
                        if (RF->portCount() < 1) {
                            mach.removeRegisterFile(*RF);
                            removedRFNames.push_back(RF->name());
                            --i;
                        } 
                    } else {
                        Socket* socket = NULL;
                        // find a socket where RF port can be connected
                        for (int soc = 0; i < socketNav.count(); ++soc) {
                            socket = socketNav.item(soc);
                            if (tester.canConnect(*socket, *port)) {
                                port->attachSocket(*socket);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    /** 
     * Checks buses and removes the ones that have no connections to sockets.
     * 
     * @param mach Machine which buses are checked.
     */
    void checkBuses(TTAMachine::Machine& mach) {
        Machine::SocketNavigator socketNav = mach.socketNavigator();
        Machine::BusNavigator busNav = mach.busNavigator();
        for (int i = 0; i < busNav.count(); ++i) {
            bool isConnected = false;
            for (int j = 0; j < socketNav.count(); ++j) {
                if ((busNav.item(i))->isConnectedTo(
                            *socketNav.item(j))) {
                    isConnected = true;
                }
            }
            // if not connected to any socket remove bus
            if (!isConnected) {
                mach.removeBus(*busNav.item(i));
            }
        }
    }

    /**
     * Removes sockets that are not needed in the machine.
     * 
     * @param mach Machine which extra sockets are removed.
     */
    void removeSockets(TTAMachine::Machine& mach) {
        
        // remove not connected sockets
        MachineResourceModifier modifier;
        std::list<std::string> removedSocketNames;
        modifier.removeNotConnectedSockets(mach, removedSocketNames);
    }
};

const std::string 
    RemoveUnconnectedComponents::allowRemovalPN_("allow_remove");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(RemoveUnconnectedComponents)
