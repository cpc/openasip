/**
 * @file RemoveUnconnectedComponents.cc
 *
 * Explorer plugin that removes unconnected ports from units or creates
 * connections to these ports in case of a FUs. Also removes unconnected
 * buses. If all ports from a unit are removed, removes also the unit.
 * Removes also unconnected FUs.
 *
 * @author Esa Määttä 2007 (esa.maatta@tut.fi)
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

using std::endl;
using namespace TTAMachine;

/**
 * Explorer plugin that removes unconnected ports from units or creates
 * connections to these ports in case of a FUs. Also removes unconnected
 * buses. If all ports from a unit are removed, removes also the unit.
 */
class RemoveUnconnectedComponents : public DesignSpaceExplorerPlugin {
public:
    DESCRIPTION("Removes unconnected components from a configuration.");
    
    /**
     * Removes unconnected components from a configuration.
     *
     * Explorer plugin that removes unconnected ports from units or creates
     * connections to these ports in case of a FUs. Also removes unconnected
     * buses. If all ports from a unit are removed, removes also the unit.
     * Removes also unconnected FUs.
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

        if (configurationID == 0) {
            std::ostringstream msg(std::ostringstream::out);
            msg << "No configuration nor adf defined. Use -s <confID> to "
                << "define the configuration to be optimized." << endl;
            errorOuput(msg.str());
            return result;
        }

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

            std::vector<std::string> removedFUNames;
            removeUnconnectedFUs(*mach, removedFUNames);

            checkFUPorts(*mach);

            std::vector<std::string> removedRFNames;
            checkRFPorts(*mach, removedRFNames);

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
            errorOuput(msg.str());
            return result;
        }
        return result;
    }

private:

    /// parameter allow removal of unused ports and RFs without ports.
    bool allowRemoval_;

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        const std::string allowRemoval = "allow_remove";

        if (hasParameter(allowRemoval)) {
            try {
                allowRemoval_ = booleanValue(parameterValue(allowRemoval));
            } catch (const Exception& e) {
                parameterError(allowRemoval, "Boolean");
                allowRemoval_ = false;
            }
        } else {
            // set defaut value for allow Removal parameter
            allowRemoval_ = false;
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

};

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(RemoveUnconnectedComponents)
