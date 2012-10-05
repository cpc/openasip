/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file ConnectionSweeper.cc
 *
 * Explorer plugin that optimizes the interconnection network by
 * removing connections with least effect to the cycle count.
 *
 * @author Pekka J‰‰skel‰inen 2011
 * @note rating: red
 */

#include <vector>
#include <string>
#include <set>
#include <memory>

#include "DesignSpaceExplorerPlugin.hh"
#include "MachineConnectivityCheck.hh"
#include "DSDBManager.hh"
#include "Machine.hh"
#include "Connection.hh"
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
 * Explorer plugin that optimizes the interconnection network by
 * removing connections with least effect to the cycle count first.
 *
 * 1) Sweeps through the buses in order. 
 * 2) For each bus, tries to remove each connection one-by-one,
 *    and evaluates the effect to the cycle count. The one with
 *    the least effect is removed first. The process is repeated
 *    until the minimum cycle count increase crosses an user-defined
 *    threshold, or the program becomes unschedulable.
 *    Emphasize RF port connections as they tend to get to the longest
 *    path due to the register selection logic being in the same path.
 * 3) After that, the next bus is tried similarly, until either all
 *    the buses have been optimized this way.
 * 4) The DSDB can be queried with the connection count as the first measure,
 *    the cycle count the second with both as small as possible.
 *
 */
class ConnectionSweeper : public DesignSpaceExplorerPlugin {
    PLUGIN_DESCRIPTION(
        "Optimizes the IC of the given configuration by removing bus "
        "connections with least effect to the cycle count first.");

    ConnectionSweeper() : DesignSpaceExplorerPlugin() {
        // parameters that have a default value
        addParameter(ccWorseningThresholdPN_, UINT, false, "50");
    }

    virtual bool requiresStartingPointArchitecture() const { return true; }
    virtual bool producesArchitecture() const { return true; }
    virtual bool requiresHDB() const { return false; }
    virtual bool requiresSimulationData() const { return true; }
    
    virtual std::vector<RowID>
    explore(const RowID& startPointConfigurationID, const unsigned int&) {


        readParameters();
        
        DSDBManager& dsdb = db();
        DSDBManager::MachineConfiguration startConf = 
            dsdb.configuration(startPointConfigurationID);
            
        // loads starting configuration
        Machine* origMach = NULL;
        origMach = dsdb.architecture(startConf.architectureID);
        // get cycle counts for the original architecture for each app
        std::set<RowID> appIds = dsdb.applicationIDs();
        for (std::set<RowID>::const_iterator appI = appIds.begin(); 
             appI != appIds.end(); ++appI) {
            RowID appID = *appI;
            if (!dsdb.hasCycleCount(appID, startConf.architectureID)) {
                CostEstimates estimates;
                bool success = evaluate(startConf, estimates, false);
                if (!success) {
                    throw Exception(
                        __FILE__, __LINE__, __func__,
                        "Could not evaluate the starting point arch.");
                }
            }
            assert(dsdb.hasCycleCount(appID, startConf.architectureID));
            origCycles_[appID] = 
                dsdb.cycleCount(appID, startConf.architectureID);
            TCEString s;
            s << "app: " << appID << " original cycles: "; 
            s << origCycles_[appID] << " total connections: "
              << MachineConnectivityCheck::totalConnectionCount(*origMach);
            verboseLog(s);                   
        }

        std::vector<RowID> result;

        // sweep through the buses, the register file connections first
        const int thresholdStep = 
            std::max(maxCcWorseningThreshold_ / 4, (unsigned)1);
        for (ccWorseningThreshold_ = thresholdStep; 
             ccWorseningThreshold_ <= maxCcWorseningThreshold_;
             ccWorseningThreshold_ == maxCcWorseningThreshold_ ?
                 ccWorseningThreshold_++ :
                 ccWorseningThreshold_ = 
                 std::min(
                     ccWorseningThreshold_ + thresholdStep,
                     maxCcWorseningThreshold_)) {
            TCEString s;
            s << "### Testing max worsening threshold "
              << ccWorseningThreshold_ << "% of "
              << maxCcWorseningThreshold_ << "%";
            verboseLog(s);

            s = "### Reducing register file and immediate unit connections.";
            verboseLog(s);
            std::list<RowID> rfSweepResults = 
                sweepRFs(startPointConfigurationID);

            if (rfSweepResults.size() == 0) continue;

            RowID bestFromRFSweep = rfSweepResults.front();
            result.insert(result.begin(), bestFromRFSweep);

            s = "### Reducing bypass connections.";
            verboseLog(s);

            std::list<RowID> bypassSweepResults = 
                sweepBypasses(bestFromRFSweep);
            if (bypassSweepResults.size() == 0) continue;

            RowID bestFromBPSweep = rfSweepResults.front();
            result.insert(result.begin(), bestFromBPSweep);
        }
        return result;
    }

    /**
     * Sweeps the register file and immediate unit connections.
     */
    std::list<RowID>
    sweepRFs(RowID startPointConfigurationID) {

        DSDBManager::MachineConfiguration startConf = 
            db().configuration(startPointConfigurationID);
            
        // loads starting configuration
        Machine* mach = db().architecture(startConf.architectureID);
        DSDBManager::MachineConfiguration bestConf = startConf;


        std::list<RowID> result;
        for (int busI = 0; busI < mach->busNavigator().count();
             ++busI) {
            const TTAMachine::Bus& bus = *mach->busNavigator().item(busI);
            
            verboseLog(TCEString("considering bus ") + bus.name());

            std::vector<const TTAMachine::Connection*> connections;         
            const TTAMachine::Segment& seg = *bus.segment(0);
            for (int rfI = 0; rfI < mach->registerFileNavigator().count();
                 ++rfI) {
                const TTAMachine::RegisterFile& rf = 
                    *mach->registerFileNavigator().item(rfI);
                for (int connI = 0; connI < seg.connectionCount();
                     ++connI) {
                    const TTAMachine::Connection& conn = 
                        seg.connection(*seg.connection(connI));
                    for (int portI = 0; portI < conn.socket()->portCount(); 
                         ++portI) {
                        const TTAMachine::Port& port = 
                            *conn.socket()->port(portI);
                        if (port.parentUnit() == &rf) {
                            connections.push_back(&conn);
                        }
                    }
                }
            }
            for (int iuI = 0; iuI < mach->immediateUnitNavigator().count();
                 ++iuI) {
                const TTAMachine::ImmediateUnit& iu = 
                    *mach->immediateUnitNavigator().item(iuI);
                for (int connI = 0; connI < seg.connectionCount();
                     ++connI) {
                    const TTAMachine::Connection& conn = 
                        seg.connection(*seg.connection(connI));
                    for (int portI = 0; portI < conn.socket()->portCount(); 
                         ++portI) {
                        const TTAMachine::Port& port = 
                            *conn.socket()->port(portI);
                        if (port.parentUnit() == &iu) {
                            connections.push_back(&conn);
                        }
                    }
                }
            }

            std::list<RowID> newConfs = 
                removeLeastNecessaryConnections(connections, bestConf, true);

            if (newConfs.size() > 0) {
                bestConf = db().configuration(newConfs.front());
                for (std::list<RowID>::reverse_iterator i = newConfs.rbegin();
                     i != newConfs.rend(); ++i) {
                    result.insert(result.begin(), *i);
                }
            }
        }
        return result;
    }

    /**
     * Sweeps the bypass connections, that is, everything else but the
     * register file and immediate unit connections.
     */
    std::list<RowID>
    sweepBypasses(RowID startPointConfigurationID) {

        DSDBManager::MachineConfiguration startConf = 
            db().configuration(startPointConfigurationID);
            
        // loads starting configuration
        Machine* mach = db().architecture(startConf.architectureID);
        DSDBManager::MachineConfiguration bestConf = startConf;

        std::list<RowID> result;
        for (int busI = 0; busI < mach->busNavigator().count();
             ++busI) {
            const TTAMachine::Bus& bus = *mach->busNavigator().item(busI);
            
            verboseLog(TCEString("considering bus ") + bus.name());

            std::vector<const TTAMachine::Connection*> connections;         
            const TTAMachine::Segment& seg = *bus.segment(0);
            for (int connI = 0; connI < seg.connectionCount();
                 ++connI) {
                const TTAMachine::Connection& conn = 
                    seg.connection(*seg.connection(connI));
                // check if the connection is to an IU or RF
                bool rfConnection = false;
                for (int rfI = 0; rfI < mach->registerFileNavigator().count();
                     ++rfI) {
                    const TTAMachine::RegisterFile& rf = 
                        *mach->registerFileNavigator().item(rfI);
                    for (int portI = 0; portI < conn.socket()->portCount(); 
                         ++portI) {
                        const TTAMachine::Port& port = 
                            *conn.socket()->port(portI);
                        if (port.parentUnit() == &rf) {
                            rfConnection = true;
                            break;
                        }
                    }
                    
                }
                for (int iuI = 0; 
                     !rfConnection && 
                         iuI < mach->immediateUnitNavigator().count();
                     ++iuI) {
                    const TTAMachine::ImmediateUnit& iu = 
                        *mach->immediateUnitNavigator().item(iuI);
                    for (int portI = 0; portI < conn.socket()->portCount(); 
                         ++portI) {
                        const TTAMachine::Port& port = 
                            *conn.socket()->port(portI);
                        if (port.parentUnit() == &iu) {
                            rfConnection = true;
                            break;
                        }
                    }
                }
                if (!rfConnection) connections.push_back(&conn);
            }

            std::list<RowID> newConfs = 
                removeLeastNecessaryConnections(connections, bestConf, true);

            if (newConfs.size() > 0) {
                bestConf = db().configuration(newConfs.front());
                for (std::list<RowID>::reverse_iterator i = newConfs.rbegin();
                     i != newConfs.rend(); ++i) {
                    result.insert(result.begin(), *i);
                }
            }
        }
        return result;
    }


private:
    // cycle counts for the original architecture for each app
    // RowID == application ID
    std::map<RowID, ClockCycleCount> origCycles_;
    // parameter name variables
    static const std::string ccWorseningThresholdPN_;
    // the max cycle count worsening threshold in percentages in comparison to
    // the fully connected architecture
    unsigned int maxCcWorseningThreshold_;
    // the current threshold
    unsigned int ccWorseningThreshold_;

    /**
     * Reads the parameters given to the plugin.
     */
    void readParameters() {
        readOptionalParameter(
            ccWorseningThresholdPN_, maxCcWorseningThreshold_);
    }
    
    /**
     * Computes the average cycle count worsening for the given processor
     * configuration across all the explored applications.
     */
    float 
    averageWorsening(RowID confId) {
        float worsenings = 0.0f;
        std::set<RowID> appIds = db().applicationIDs();
        for (std::set<RowID>::const_iterator appI = appIds.begin(); 
             appI != appIds.end(); ++appI) {
            RowID appID = *appI;
            DSDBManager::MachineConfiguration conf = 
                db().configuration(confId);
            if (!db().hasCycleCount(appID, conf.architectureID)) {
                continue; /* the evaluation had failed */
            }
            int origCycles = origCycles_[appID];
            int newCycles = 
                db().cycleCount(appID, conf.architectureID);
            worsenings += 100.0*(newCycles - origCycles) / origCycles;
        }
        float avgWorsening = worsenings / appIds.size();
        return avgWorsening;
    }

    /**
     * Removes the connections in the given set that affect the cycle count
     * the least until the worsening threshold is reached or in case the
     * program becomes uncompilable.
     *
     * @param tryAllFirst Tries to remove all connections first and if it
     * the results fits in the cc threshold, does not try configuration with
     * the listed connections.
     */ 
    std::list<RowID>
    removeLeastNecessaryConnections(
        std::vector<const TTAMachine::Connection*> connections,
        const DSDBManager::MachineConfiguration& startConf,
        bool tryAllFirst) {

        std::list<RowID> results;
        DSDBManager::MachineConfiguration bestConf = startConf;

        if (tryAllFirst) {
            std::auto_ptr<TTAMachine::Machine> currentMachine(
                db().architecture(bestConf.architectureID));
            for (std::vector<const TTAMachine::Connection*>::iterator 
                     connI = connections.begin(); connI != connections.end();
                 ++connI) {
                const TTAMachine::Connection* conn = *connI;
                removeConnection(*currentMachine, *conn);
            }

            DSDBManager::MachineConfiguration conf;
            conf.architectureID = db().addArchitecture(*currentMachine);
            RowID confId = db().addConfiguration(conf);
            conf = db().configuration(confId);
            CostEstimates estimates;
            bool success = evaluate(conf, estimates, false);
            float worsening = averageWorsening(confId);
            if (success) {
                std::auto_ptr<TTAMachine::Machine> arch
                    (db().architecture(
                        db().configuration(confId).architectureID));
                if (worsening <= ccWorseningThreshold_) {
                    results.push_back(confId);
                    TCEString s;
                    s << "removing all connections was within threshold: #" 
                      << confId << " avg worsening: ";
                    s << (int)worsening << "% " << " total connections: "
                      << MachineConnectivityCheck::totalConnectionCount(*arch);
                    verboseLog(s);
                    return results;
                } else {
                    TCEString s;
                    s << "removing all connections was not within the "
                      << "threshold: #" 
                      << confId << " avg worsening: ";
                    s << (int)worsening << "% " << " total connections: "
                      << MachineConnectivityCheck::totalConnectionCount(*arch);
                    s << "\n";
                    s << "trying all connections one by one";
                    verboseLog(s);
                    
                }
            } else {
                TCEString s;
                s << "removing all connections led to unschedulable program, ";
                s << "trying all connections one by one";
                verboseLog(s);
            }
        }

        bool couldRemove = true;
        while (couldRemove) {
            couldRemove = false;
            const TTAMachine::Connection* mostUnneededConn = NULL;
            float bestAvgccWorsening = 100.0;
            TTAMachine::Machine* currentMachine = 
                db().architecture(bestConf.architectureID);
            RowID bestConfInThisIteration = -1;
            std::vector<const TTAMachine::Connection*>::iterator unneededPos = 
                connections.end();
            // find the least affecting connection removal for this stage
            for (std::vector<const TTAMachine::Connection*>::iterator 
                     connI = connections.begin(); connI != connections.end();
                 ++connI) {
                const TTAMachine::Connection* conn = *connI;

                // compute the avgccWorsening
                // check if it's the best found and if it's above the
                // threshold               
                TTAMachine::Machine mach = *currentMachine;
                removeConnection(mach, *conn);

                DSDBManager::MachineConfiguration conf;
                conf.architectureID = db().addArchitecture(mach);
                RowID confId = db().addConfiguration(conf);
                bool success = evaluate(db().configuration(confId));

                if (success) {
                    unsigned int avgWorsening = 
                        (unsigned)averageWorsening(confId);
                    if (avgWorsening <= ccWorseningThreshold_ &&
                        avgWorsening < bestAvgccWorsening) {
                        mostUnneededConn = conn;
                        bestConfInThisIteration = confId;
                        unneededPos = connI;
                        bestAvgccWorsening = avgWorsening;
                        TCEString s;
                        s << "new config: #" << bestConfInThisIteration 
                          << " avg worsening: ";
                        s << (int)bestAvgccWorsening << "% " 
                          << " total connections: "
                          << MachineConnectivityCheck::totalConnectionCount(
                              *db().architecture(
                                  db().configuration(confId).architectureID));
                        verboseLog(s);
                    }
                } else {
                    TCEString s;
                    s << "new config: #" << confId << " is unschedulable";
                    verboseLog(s);
                }
            }
            if (mostUnneededConn != NULL) {
                // more connections we remove while staying in the threshold, 
                // better the processor gets
                bestConf = db().configuration(bestConfInThisIteration);
                couldRemove = true;
                connections.erase(unneededPos);
                results.push_front(bestConfInThisIteration);
                TCEString s;
                s << "best config from this bus sweep: #" 
                  << bestConfInThisIteration << " avg worsening: ";
                s << (int)bestAvgccWorsening << "% " << " total connections: "
                  << MachineConnectivityCheck::totalConnectionCount(
                      *db().architecture(bestConf.architectureID));
                verboseLog(s);
            }
        }
        return results;
    }

    void
    removeConnection(
        TTAMachine::Machine& mach, const TTAMachine::Connection& connection) {
        TTAMachine::Segment& segment = 
            *mach.busNavigator().item(connection.bus()->parentBus()->name())->
            segment(0);

        TTAMachine::Socket& socket = 
            *mach.socketNavigator().item(connection.socket()->name());

        segment.detachSocket(socket);
    }
};

// parameter names
const std::string 
ConnectionSweeper::ccWorseningThresholdPN_("cc_worsening_threshold");

EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(ConnectionSweeper)
