/**
 * @file SimpleICOptimizer.cc
 *
 * Explorer plugin that optimizes the interconnection network by
 * removing extra sockets and not used connections.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
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
#include "SchedulerFrontend.hh"
#include "Procedure.hh"
#include "MachineResourceModifier.hh"


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
     * connections. Needs the start point configuration to get the architecture
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

            Program* program = NULL;
            if (tpef_ == "") {
                SchedulerFrontend scheduler;
                try {
                    std::set<RowID> appIDs = dsdb.applicationIDs();
                    std::set<RowID>::const_iterator iter = appIDs.begin();
                    for (; iter != appIDs.end(); iter++) {
                        TestApplication testApp(dsdb.applicationPath(*iter));
                        const UniversalMachine* uMach = new UniversalMachine();
                        Program* seqProg = 
                            Program::loadFromTPEF(
                                testApp.applicationPath(), *uMach);
                        SchedulingPlan* plan = 
                            SchedulingPlan::loadFromFile(
                                Environment::oldGccSchedulerConf());;

                        program = 
                            scheduler.schedule(*seqProg, *origMach, *plan);

                        delete plan;
                        plan = NULL;

                        addConnections(*mach, *program);
                    }
                } catch (const Exception& e) {
                    std::ostringstream msg(std::ostringstream::out);
                    msg << e.errorMessageStack() << std::endl;
                    errorOuput(msg.str());
                    delete mach;
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

            removeSockets(*mach);

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

            if (evaluateResult_) {
                // evaluate the new config
                if (evaluate(newConf, estimates, true)) {
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

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        // Parameter name of bus number in the machine.
        const std::string tpef = "tpef";
        const std::string tpefDefault = "";
        const std::string addOnly = "add_only";
        const std::string evaluate = "evaluate";
    
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

        for (int i = 0; i < socketNav.count(); i++) {
            Socket* socket = socketNav.item(i);
            for (int bus = 0; bus < busNav.count(); bus++) {
                socket->detachBus(*busNav.item(bus));
            }
        }
    }

    /**
     * Adds connections used in the program to the given machine.
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

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(SimpleICOptimizer);
