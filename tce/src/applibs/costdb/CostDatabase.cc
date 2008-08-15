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
 * @file CostDatabase.cc
 *
 * Implementation of CostDatabase class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#include <vector>
#include <set>
#include <map>
#include <string>
#include <boost/regex.hpp>

#include "Application.hh"
#include "HDBManager.hh"
#include "CostDatabase.hh"
#include "CostDBEntryStats.hh"
#include "CostDBEntryStatsRF.hh"
#include "CostDBEntryStatsFU.hh"
#include "Conversion.hh"
#include "SearchStrategy.hh"
#include "CostDBEntry.hh"
#include "RFEntry.hh"
#include "RFArchitecture.hh"
#include "FUEntry.hh"
#include "FUArchitecture.hh"
#include "FunctionUnit.hh"
#include "FUPort.hh"
#include "HWOperation.hh"
#include "CostFunctionPlugin.hh"
#include "CostDatabaseRegistry.hh"

using std::pair;
using std::string;
using std::set;
using std::vector;
using namespace HDB;
using namespace TTAMachine;

CostDatabase* CostDatabase::instance_ = NULL;

/**
 * Default constructor.
 *
 * @param hdb HDBManager to be used with the cost database.
 */
CostDatabase::CostDatabase(const HDB::HDBManager& hdb) :
    searchStrategy_(NULL), hdb_(hdb), registerFilesBuilt_(false),
    functionUnitsBuilt_(false), busesBuilt_(false), socketsBuilt_(false) {

    // Creates entry and field types that the database contains.
    
    EntryKeyProperty* rfileProperty =
        EntryKeyProperty::create(CostDBTypes::EK_RFILE);
    rfileProperty->createFieldProperty(CostDBTypes::EKF_NUM_REGISTERS);
    rfileProperty->createFieldProperty(CostDBTypes::EKF_READ_PORTS);
    rfileProperty->createFieldProperty(CostDBTypes::EKF_WRITE_PORTS);
    rfileProperty->createFieldProperty(CostDBTypes::EKF_BIDIR_PORTS);
    rfileProperty->createFieldProperty(CostDBTypes::EKF_BIT_WIDTH);
    rfileProperty->createFieldProperty(CostDBTypes::EKF_LATENCY);
    rfileProperty->createFieldProperty(CostDBTypes::EKF_MAX_READS);
    rfileProperty->createFieldProperty(CostDBTypes::EKF_MAX_WRITES);
    rfileProperty->createFieldProperty(CostDBTypes::EKF_GUARD_SUPPORT);
    rfileProperty->createFieldProperty(CostDBTypes::EKF_GUARD_LATENCY);

    EntryKeyProperty* unitProperty =
        EntryKeyProperty::create(CostDBTypes::EK_UNIT);
    unitProperty->createFieldProperty(CostDBTypes::EKF_BIT_WIDTH);
    unitProperty->createFieldProperty(CostDBTypes::EKF_FUNCTION_UNIT);    

    EntryKeyProperty* mbusProperty =
        EntryKeyProperty::create(CostDBTypes::EK_MBUS);
    mbusProperty->createFieldProperty(CostDBTypes::EKF_BIT_WIDTH);
    mbusProperty->createFieldProperty(CostDBTypes::EKF_BUS_FANIN);
    mbusProperty->createFieldProperty(CostDBTypes::EKF_BUS_FANOUT);
    
    EntryKeyProperty::create(CostDBTypes::EK_SOCKET);
    
    EntryKeyProperty* inputSocketProperty =
        EntryKeyProperty::create(CostDBTypes::EK_INPUT_SOCKET);
    inputSocketProperty->createFieldProperty(
        CostDBTypes::EKF_BIT_WIDTH);
    inputSocketProperty->createFieldProperty(
        CostDBTypes::EKF_INPUT_SOCKET_FANIN);
    
    EntryKeyProperty* outputSocketProperty =
        EntryKeyProperty::create(CostDBTypes::EK_OUTPUT_SOCKET);
    outputSocketProperty->createFieldProperty(
        CostDBTypes::EKF_BIT_WIDTH);
    outputSocketProperty->createFieldProperty(
        CostDBTypes::EKF_OUTPUT_SOCKET_FANOUT);
/*
  EntryKeyProperty* controlProperty =
  EntryKeyProperty::create(CostDBTypes::EK_CONTROL);
  controlProperty->createFieldProperty(
  CostDBTypes::EKF_CONTROL_CONNECTIVITY);
*/    
    EntryKeyProperty::create(CostDBTypes::EK_INLINE_IMM_SOCKET);

    buildDefaultCostDatabase();
}

/**
 * Destructor.
 *
 * Deallocates memory reserved for the search strategy.
 */
CostDatabase::~CostDatabase() {

    if (searchStrategy_ != NULL) {
        delete searchStrategy_;
        searchStrategy_ = NULL;
    }

    for (EntryMap::iterator i = entries_.begin(); i != entries_.end(); i++) {

        for (CostDBTypes::EntryTable::iterator j = i->second.begin();
             j != i->second.end(); j++) {

            if (*j != NULL) {
                delete *j;
                *j = NULL;
            }
        }
    }
}

/**
 * Creates and returns an instance of cost database build in base of the HDB.
 *
 * @param hdb HDB to use in building the CostDatabase.
 * @return An instance of cost database.
 * @exception Exception in case that an error occurred while creating the
 * CostDatabase.
 */
CostDatabase&
CostDatabase::instance(const HDB::HDBManager& hdb) 
    throw (Exception) {

    CostDatabaseRegistry* registry = &CostDatabaseRegistry::instance();
    if (!registry->hasCostDatabase(hdb)) {
        registry->addCostDatabase(new CostDatabase(hdb), hdb);
    }
    return registry->costDatabase(hdb);
}

/**
 * Creates default cost database from all HDB entries.
 *
 * @exception Exception if an error occured during the cost database building.
 */
void
CostDatabase::buildDefaultCostDatabase() 
    throw (Exception) {

    if (!isRegisterFilesBuilt()) {
        buildRegisterFiles("StrictMatchRFEstimator");
    }
    if (!isFunctionUnitsBuilt()) {
        buildFunctionUnits("StrictMatchFUEstimator");
    }
    if (!isBusesBuilt()) {
        buildBuses("DefaultICEstimator");
    }
    if (!isSocketsBuilt()) {
        buildSockets("DefaultICEstimator");
    }
}

/**
 * Reads register files from the set HDB and builds register file entries in to
 * the cost database.
 *
 * @param rfEstimatorPluginName Name of the register file estimator plugin
 * which cost data are read and used in the cost database.
 * @exception Exception if an error occured during the cost database building.
 */
void
CostDatabase::buildRegisterFiles(const std::string& rfEstimatorPluginName) 
    throw (Exception) {

    // find out the register file estimator plugin id
    std::set<RowID> pluginIDs = hdb_.costFunctionPluginIDs();
    RowID rfEstimatorPluginID = 0;
    std::set<RowID>::const_iterator pluginID = pluginIDs.begin();
    for (; pluginID != pluginIDs.end(); pluginID++) {
        CostFunctionPlugin* plugin = hdb_.costFunctionPluginByID(*pluginID);
        if (plugin->name() == rfEstimatorPluginName) {
            //"StrictMatchRFEstimator") {
            rfEstimatorPluginID = *pluginID;
            break;
        }
    }
    
    // Registers
    std::set<RowID> rfs = hdb_.rfEntryIDs();
    std::set<RowID>::const_iterator id = rfs.begin();
    for (; id != rfs.end(); id++) {
        // fetch all data from the entry and put it to CostDB
        int size = 0;
        int reads = 0;
        int writes = 0;
        int bidirPorts = 0;
        int maxReads = 0;
        int maxWrites = 0;
        int bitWidth = 0;
        int latency = 0;
        bool guardSupport = false;
        int guardLatency = 0;
        double area = 0;
        double delay = 0;
        
        const HDB::RFEntry* entry = hdb_.rfByEntryID(*id);
        
        if (entry->hasCostFunction()) {
            if (entry->costFunction().id() != rfEstimatorPluginID) {
                // wrong type of estimation plugin
                continue;
            }
        } else {
            // no cost function plugin set.
            continue;
        }
        if (entry->hasArchitecture()) {
            try {
                size = entry->architecture().size();
            } catch (NotAvailable& e) {
                // size is parametrized
                // entry cannot be added to costDB
                continue;
            }
            try {
                bitWidth = entry->architecture().width();
            } catch (NotAvailable& e) {
                // bit width is parametrized
                // entry cannot be added to costDB
                continue;
            }
            writes = entry->architecture().writePortCount();
            reads = entry->architecture().readPortCount();
            bidirPorts = entry->architecture().bidirPortCount();
            latency = entry->architecture().latency();
            maxReads = entry->architecture().maxReads();
            maxWrites = entry->architecture().maxWrites();
            guardSupport = entry->architecture().hasGuardSupport();
            guardLatency = entry->architecture().guardLatency();
        } else {
            // entry has no architecture
            continue;
        }        
        EntryKeyProperty* rfileProperty = 
            EntryKeyProperty::find(CostDBTypes::EK_RFILE);
        CostDBEntryKey* newEntryKey = 
            new CostDBEntryKey(rfileProperty);
        
        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataInt(size),
                rfileProperty->fieldProperty(
                    CostDBTypes::EKF_NUM_REGISTERS)));
        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataInt(reads),
                rfileProperty->fieldProperty(
                    CostDBTypes::EKF_READ_PORTS)));
        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataInt(writes),
                rfileProperty->fieldProperty(
                    CostDBTypes::EKF_WRITE_PORTS)));
        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataInt(bidirPorts),
                rfileProperty->fieldProperty(
                    CostDBTypes::EKF_BIDIR_PORTS)));
        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataInt(bitWidth),
                rfileProperty->fieldProperty(
                    CostDBTypes::EKF_BIT_WIDTH)));
        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataInt(latency),
                rfileProperty->fieldProperty(
                    CostDBTypes::EKF_LATENCY)));
        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataInt(maxReads),
                rfileProperty->fieldProperty(
                    CostDBTypes::EKF_MAX_READS)));
        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataInt(maxWrites),
                rfileProperty->fieldProperty(
                    CostDBTypes::EKF_MAX_WRITES)));
        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataBool(guardSupport),
                rfileProperty->fieldProperty(
                    CostDBTypes::EKF_GUARD_SUPPORT)));
        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataInt(guardLatency),
                rfileProperty->fieldProperty(
                    CostDBTypes::EKF_GUARD_LATENCY)));

        CostEstimationData query;
        query.setRFReference(*id);
        query.setPluginID(rfEstimatorPluginID);
        std::set<RowID> dataIDs = hdb_.costEstimationDataIDs(query);
        std::set<RowID>::const_iterator dataID = dataIDs.begin();
        for (; dataID != dataIDs.end(); dataID++) {
            CostEstimationData data = hdb_.costEstimationData(*dataID);
            if (data.name() == "area") {
                area = data.value().doubleValue();
                continue;
            }
            if (data.name() == "computation_delay") {
                delay = data.value().doubleValue();
                continue;
            }
        }
        CostDBEntryStatsRF* newStatistics = 
            new CostDBEntryStatsRF(area, delay);
        dataID = dataIDs.begin();

        for (; dataID != dataIDs.end(); dataID++) {
            CostEstimationData data = hdb_.costEstimationData(*dataID);
            // input delays

            // this case is for one delay/unit case
            if (data.name() == "input_delay") {
                newStatistics->setDelay(
                    "input_delay", data.value().doubleValue());
                continue;
            }
            boost::smatch match = 
                getValues(data.name(), "input_delay[ \t]*(\\S+)");
            if (match.size() == 2) {
                // match[0] contains the whole string
                // match[1] contains the port name
                newStatistics->setDelay(
                    match[1], data.value().doubleValue());
                continue;
            }

            // output delays

            // this case is for one delay/unit case
            if (data.name() == "output_delay") {
                newStatistics->setDelay(
                    "output_delay", data.value().doubleValue());
                continue;
            }

            match = getValues(data.name(), "output_delay[ \t]*(\\S+)");
            if (match.size() == 2) {
                // match[0] contains the whole string
                // match[1] contains the port name
                newStatistics->setDelay(
                    match[1], data.value().doubleValue());
                continue;
            }
            
            if (data.name() == "output_delay") {
                // match[0] contains the whole string
                // match[1] contains the name of the port
                newStatistics->setDelay(
                    match[1], data.value().doubleValue());
                continue;
            }

            // access energies
            match = 
                getValues(
                    data.name(), "rf_access_energy ([0-9])* ([0-9])*");
            if (match.size() == 3) {
                // match[0] contains the whole string
                // match[1] contains the number of reads
                // match[2] contains the number of writes
                newStatistics->setEnergyReadWrite(
                    Conversion::toInt(
                        match[1]), Conversion::toInt(match[2]),
                    data.value().doubleValue());
                continue;
            }

            // idle energy
            if (data.name() == "rf_idle_energy") {
                newStatistics->setEnergyIdle(data.value().doubleValue());
                continue;
            }
        }

        CostDBEntry* newEntry =
            new CostDBEntry(newEntryKey);
        newEntry->addStatistics(newStatistics);
        insertEntry(newEntry);
    }
    registerFilesBuilt_ = true;
}

/**
 * Reads function units from the set HDB and builds function unit entries in to
 * the cost database.
 *
 * @param fuEstimatorPluginName Name of the function unit estimator plugin
 * which cost data are read and used in the cost database.
 * @exception Exception if an error occured during the cost database building.
 */
void
CostDatabase::buildFunctionUnits(const std::string& fuEstimatorPluginName) 
    throw (Exception) {

    // find out the function unit estimator plugin id
    std::set<RowID> pluginIDs = hdb_.costFunctionPluginIDs();    
    RowID fuEstimatorPluginID = 0;
    std::set<RowID>::const_iterator pluginID = pluginIDs.begin();
    for (; pluginID != pluginIDs.end(); pluginID++) {
        CostFunctionPlugin* plugin = hdb_.costFunctionPluginByID(*pluginID);
        if (plugin->name() == fuEstimatorPluginName) {
            fuEstimatorPluginID = *pluginID;
            break;
        }
    }

    // Function units
    std::set<RowID> fus = hdb_.fuEntryIDs();
    std::set<RowID>::const_iterator id = fus.begin();
    for (; id != fus.end(); id++) {
        // fetch all data from the entry and put it to CostDB
        set<string> operations;
        set<vector<string> > parameters;
        vector<string> portSet;
        int operationCount = 0;
        int latency = 0;
        double area = 0;
        double delay = 0;
        int width = 0;

        const HDB::FUEntry* entry = hdb_.fuByEntryID(*id);

        if (entry->hasCostFunction()) {
            if (entry->costFunction().id() != fuEstimatorPluginID) {
                // wrong type of estimation plugin
                continue;
            }
        } else {
            // no cost function plugin set.
            continue;
        }

        if (entry->hasArchitecture()) {            
            operationCount = entry->architecture().architecture().
                operationCount();                
            latency = entry->architecture().architecture().maxLatency();

            int ports = entry->architecture().architecture().portCount();
            int i;
            for (i = 0; i < ports; i++) {
                BaseFUPort* port = 
                    entry->architecture().architecture().port(i);
                if (dynamic_cast<FUPort*>(port) != NULL) {
                    FUPort* fuPort = dynamic_cast<FUPort*>(port);
                    if (entry->architecture().hasParameterizedWidth(
                            fuPort->name())) {
                        // parameterized port
                        break;
                    } else {
                        if (width == 0) {
                            width = fuPort->width();
                        } else if (width != fuPort->width()) {
                            break;
                        }
                    }
                }
            }
            if (i != ports) {
                // Some port has parameterized width or 
                // port widths are not equal and entry cannot be used.
                continue;
            }
        } else {
            continue;
        }
        EntryKeyProperty* unitProperty = 
            EntryKeyProperty::find(CostDBTypes::EK_UNIT);
        CostDBEntryKey* newEntryKey = 
            new CostDBEntryKey(unitProperty);
        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataInt(width),
                unitProperty->fieldProperty(
                    CostDBTypes::EKF_BIT_WIDTH)));
        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataFunctionUnit(
                    &(entry->architecture().architecture())),
                unitProperty->fieldProperty(
                    CostDBTypes::EKF_FUNCTION_UNIT)));
        CostEstimationData query;
        query.setFUReference(*id);
        query.setPluginID(fuEstimatorPluginID);
        std::set<RowID> dataIDs = hdb_.costEstimationDataIDs(query);
        std::set<RowID>::const_iterator dataID = dataIDs.begin();
        for (; dataID != dataIDs.end(); dataID++) {
            CostEstimationData data = hdb_.costEstimationData(*dataID);
            if (data.name() == "area") {
                area = data.value().doubleValue();
                continue;
            }
            if (data.name() == "computation_delay") {
                delay = data.value().doubleValue();
                continue;
            }
        }

        CostDBEntryStatsFU* newStatistics = 
            new CostDBEntryStatsFU(area, delay);

        dataID = dataIDs.begin();
        for (; dataID != dataIDs.end(); dataID++) {
            CostEstimationData data = hdb_.costEstimationData(*dataID);
            // input delays

            // this case is for one delay/unit case
            if (data.name() == "input_delay") {
                newStatistics->setDelay("input_delay",
                                        data.value().doubleValue());
                continue;
            }

            boost::smatch match = 
                getValues(data.name(), "input_delay[ \t]*(\\S+)");
            if (match.size() == 2) {
                // match[0] contains the whole string
                // match[1] contains the name of the port
                newStatistics->setDelay(
                    match[1], data.value().doubleValue());
                continue;
            }
            
            // output delays

            // this case is for one delay/unit case
            if (data.name() == "output_delay") {
                newStatistics->setDelay(
                    "output_delay", data.value().doubleValue());
                continue;
            }

            match = getValues(
                data.name(), "output_delay[ \t]*(\\S+)");
            if (match.size() == 2) {
                // match[0] contains the whole string
                // match[1] contains the name of the port
                newStatistics->setDelay(
                    match[1], data.value().doubleValue());
                continue;
            }
            match = getValues(
                data.name(), "(output_delay)");
            if (match.size() == 2) {
                // match[0] contains the whole string
                // match[1] contains the name of the port
                newStatistics->setDelay(
                    match[1], data.value().doubleValue());
                continue;
            }

            // operation energies
            match = getValues(
                data.name(), "operation_execution_energy (\\S+)");
            if (match.size() == 2) {
                // match[0] contains the whole string
                // match[1] contains the operation name
                newStatistics->setEnergyOperation(
                    match[1], data.value().doubleValue());
                continue;
            }

            // idle energy
            if (data.name() == "fu_idle_energy") {
                newStatistics->setEnergyIdle(data.value().doubleValue());
                continue;
            }
        }
        CostDBEntry* newEntry =
            new CostDBEntry(newEntryKey);
        newEntry->addStatistics(newStatistics);
        
        insertEntry(newEntry);    
    }
    functionUnitsBuilt_ = true;
}

/**
 * Reads buses from the set HDB and builds bus entries in to the cost database.
 *
 * @param busEstimatorPluginName Name of the bus estimator plugin
 * which cost data are read and used in the cost database.
 * @exception Exception if an error occured during the cost database building.
 */
void
CostDatabase::buildBuses(const std::string& busEstimatorPluginName) 
    throw (Exception) {
    
    // find out the plugin id that estimates buses
    std::set<RowID> pluginIDs = hdb_.costFunctionPluginIDs();    
    RowID busEstimatorPluginID = 0;
    std::set<RowID>::const_iterator pluginID = pluginIDs.begin();
    for (; pluginID != pluginIDs.end(); pluginID++) {
        CostFunctionPlugin* plugin = hdb_.costFunctionPluginByID(*pluginID);
        if (plugin->name() == busEstimatorPluginName) {
            busEstimatorPluginID = *pluginID;
            break;
        }
    } 

    // bus part
    std::set<RowID> buses = hdb_.busEntryIDs();
    std::set<RowID>::const_iterator id = buses.begin();
    for (; id != buses.end(); id++) {
        // fetch all data from the entry and put it to CostDB
        int bitWidth = 0;
        int fanin = 0;
        int fanout = 0;
        int cntrlDelay = 0;
        double area = 0.0;
        double delay = 0.0;
        double activeEnergy = 0.0;
        double idleEnergy = 0.0;

        CostEstimationData query;
        query.setBusReference(*id);
        query.setPluginID(busEstimatorPluginID);
        std::set<RowID> dataIDs = hdb_.costEstimationDataIDs(query);
        std::set<RowID>::const_iterator dataID = dataIDs.begin();
        for (; dataID != dataIDs.end(); dataID++) {
            CostEstimationData data = hdb_.costEstimationData(*dataID);
            if (data.name() == "area") {
                area = data.value().doubleValue();
                continue;
            }
            if (data.name() == "computation_delay") {
                delay = data.value().doubleValue();
                continue;
            }
            if (data.name() == "fanin") {
                fanin = data.value().integerValue();
                continue;
            }
            if (data.name() == "fanout") {
                fanout = data.value().integerValue();
                continue;
            }
            if (data.name() == "dataw") {
                bitWidth = data.value().integerValue();
                continue;
            }
            if (data.name() == "cntrl_delay") {
                cntrlDelay = data.value().integerValue();
                continue;
            }
            if (data.name() == "energy") {
                activeEnergy = data.value().doubleValue();
                continue;
            }
            if (data.name() == "idle_energy") {
                idleEnergy = data.value().doubleValue();
                continue;
            }
        }

        CostDBEntryStats* newStatistics = 
            new CostDBEntryStats(area, delay);
        newStatistics->setEnergyActive(activeEnergy);
        newStatistics->setEnergyIdle(idleEnergy);
        
        EntryKeyProperty* busProperty = 
            EntryKeyProperty::find(CostDBTypes::EK_MBUS);
        CostDBEntryKey* newEntryKey = 
            new CostDBEntryKey(busProperty);

        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataInt(fanin),
                busProperty->fieldProperty(
                    CostDBTypes::EKF_BUS_FANIN)));
        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataInt(fanout),
                busProperty->fieldProperty(
                    CostDBTypes::EKF_BUS_FANOUT)));
        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataInt(bitWidth),
                busProperty->fieldProperty(
                    CostDBTypes::EKF_BIT_WIDTH)));
        
        CostDBEntry* newEntry =
            new CostDBEntry(newEntryKey);
        newEntry->addStatistics(newStatistics);
        
        insertEntry(newEntry);
    }
    busesBuilt_ = true;
}

/**
 * Reads sockets from the set HDB and builds socket entries in to the cost
 * database.
 *
 * @param socketEstimatorPluginName Name of the socket estimator plugin
 * which cost data are read and used in the cost database.
 * @exception Exception if an error occured during the cost database building.
 */
void
CostDatabase::buildSockets(const std::string& socketEstimatorPluginName) 
    throw (Exception) {

    // find out the plugin id that estimates sockets
    std::set<RowID> pluginIDs = hdb_.costFunctionPluginIDs();    
    RowID socketEstimatorPluginID = 0;
    std::set<RowID>::const_iterator pluginID = pluginIDs.begin();
    for (; pluginID != pluginIDs.end(); pluginID++) {
        CostFunctionPlugin* plugin = hdb_.costFunctionPluginByID(*pluginID);
        if (plugin->name() == socketEstimatorPluginName) {
            socketEstimatorPluginID = *pluginID;
            break;
        }
    }
    
    // socket part
    std::set<RowID> sockets = hdb_.socketEntryIDs();
    std::set<RowID>::const_iterator id = sockets.begin();
    for (; id != sockets.end(); id++) {
        // fetch all data from the entry and put it to CostDB
        int bitWidth = 0;
        int fanin = 0;
        int fanout = 0;
        int cntrlDelay = 0;
        double area = 0.0;
        double delay = 0.0;
        double activeEnergy = 0.0;
        double idleEnergy = 0.0;
        
        CostEstimationData query;
        query.setSocketReference(*id);
        query.setPluginID(socketEstimatorPluginID);
        std::set<RowID> dataIDs = hdb_.costEstimationDataIDs(query);
        std::set<RowID>::const_iterator dataID = dataIDs.begin();
        for (; dataID != dataIDs.end(); dataID++) {
            CostEstimationData data = hdb_.costEstimationData(*dataID);
            if (data.name() == "area") {
                area = data.value().doubleValue();
                continue;
            }
            if (data.name() == "computation_delay") {
                delay = data.value().doubleValue();
                continue;
            }
            if (data.name() == "fanin") {
                fanin = data.value().integerValue();
                continue;
            }
            if (data.name() == "fanout") {
                fanout = data.value().integerValue();
                continue;
            }
            if (data.name() == "dataw") {
                bitWidth = data.value().integerValue();
                continue;
            }
            if (data.name() == "cntrl_delay") {
                cntrlDelay = data.value().integerValue();
                continue;
            }
            if (data.name() == "energy") {
                activeEnergy = data.value().doubleValue();
                continue;
            }
            if (data.name() == "idle_energy") {
                idleEnergy = data.value().doubleValue();
                continue;
            }
        }

        CostDBEntryStats* newStatistics = 
            new CostDBEntryStats(area, delay);
        // this is contol data, not real data and not really needed
        newStatistics->setDelay("cntrl_delay", cntrlDelay);
        newStatistics->setEnergyActive(activeEnergy);
        newStatistics->setEnergyIdle(idleEnergy);

        EntryKeyProperty* socketProperty = NULL;
        CostDBEntryKey* newEntryKey = NULL;
        if (fanin != 0) {
            socketProperty = 
                EntryKeyProperty::find(CostDBTypes::EK_INPUT_SOCKET);
            newEntryKey = 
                new CostDBEntryKey(socketProperty);
            
            // fanin needed only in input sockets
            newEntryKey->addField(
                new EntryKeyField(
                    new EntryKeyDataInt(fanin),
                    socketProperty->fieldProperty(
                        CostDBTypes::EKF_INPUT_SOCKET_FANIN)));
        }
        // else socket is output socket
        else {
            socketProperty = 
                EntryKeyProperty::find(CostDBTypes::EK_OUTPUT_SOCKET);
            newEntryKey = 
                new CostDBEntryKey(socketProperty);
            
            // fanout only needed in output sockets
            newEntryKey->addField(
                new EntryKeyField(
                    new EntryKeyDataInt(fanout),
                    socketProperty->fieldProperty(
                        CostDBTypes::EKF_OUTPUT_SOCKET_FANOUT)));
        }

        newEntryKey->addField(
            new EntryKeyField(
                new EntryKeyDataInt(bitWidth),
                socketProperty->fieldProperty(
                    CostDBTypes::EKF_BIT_WIDTH)));
        
        CostDBEntry* newEntry =
            new CostDBEntry(newEntryKey);
        newEntry->addStatistics(newStatistics);
        
        insertEntry(newEntry);
    }
    socketsBuilt_ = true;
}

/**
 * Searches entries from the database matching certain search key with
 * specific type of matches.
 *
 * @param searchKey Search key.
 * @param match Type of matches.
 * @return Entries matching the search.
 * @exception KeyNotFound If search key type is not found.
 */
CostDBTypes::EntryTable
CostDatabase::search(
    const CostDBEntryKey& searchKey,
    const CostDBTypes::MatchTypeTable& match) const
    throw (KeyNotFound) {
    
    EntryMap::const_iterator i = entries_.find(searchKey.type());
    
    // entries of searched type are in the list
    if (i == entries_.end()) {
        throw KeyNotFound(__FILE__, __LINE__, "CostDatabase::search");
    }
    
    return searchStrategy_->search(searchKey, i->second, match);
}

/**
 * Inserts an entry into the database.
 *
 * @param entry Database entry.
 * @exception ObjectAlreadyExists If inserted entry is already inserted.
 */
void
CostDatabase::insertEntry(CostDBEntry* entry) throw (ObjectAlreadyExists) {

    EntryMap::iterator i = entries_.find(entry->type());

    if (i == entries_.end()) {
        CostDBTypes::EntryTable newEntries;
        newEntries.push_back(entry);
        entries_.insert(pair<const EntryKeyProperty*, CostDBTypes::EntryTable>(
                            entry->type(), newEntries));
    } else {
        // if the database already contains an entry with same search
        // key, the statistics of the new entry will be added into the
        // existing entry
        for (CostDBTypes::EntryTable::iterator j = i->second.begin();
             j != i->second.end(); j++) {

            if ((*j)->isEqualKey(*entry)) {
		if (*j == entry) {
		    throw ObjectAlreadyExists(__FILE__, __LINE__,
					      "CostDatabase::insertEntry");
		}
                for (int k = 0; k < entry->statisticsCount(); k++) {
                    CostDBEntryStats* newStats = entry->statistics(k).copy();
                    (*j)->addStatistics(newStats);
                }
                return;
            }
        }
        i->second.push_back(entry);
    }
}

/**
 * Returns true if register files are built in the cost database.
 *
 * @return True if register files are built in the cost database.
 */
bool
CostDatabase::isRegisterFilesBuilt() {
    return registerFilesBuilt_;
}

/**
 * Returns true if function units are built in the cost database.
 *
 * @return True if function units are built in the cost database.
 */
bool
CostDatabase::isFunctionUnitsBuilt() {    
    return functionUnitsBuilt_;
}

/**
 * Returns true if buses are built in the cost database.
 *
 * @return True if buses are built in the cost database.
 */
bool 
CostDatabase::isBusesBuilt() {
    return busesBuilt_;
}

/**
 * Returns true if sockets are built in the cost database.
 *
 * @return True if sockets are built in the cost database.
 */
bool 
CostDatabase::isSocketsBuilt() {
    return socketsBuilt_;
}

/**
 * Replaces search strategy by taking a copy of another one.
 *
 * @param strategy Search strategy.
 */
void
CostDatabase::setSearchStrategy(SearchStrategy* strategy) {
    if (searchStrategy_ != NULL) {
	delete searchStrategy_;
    }
    searchStrategy_ = strategy->copy();
}

/**
 * Returns the matching strings out of the text using regexp.
 * 
 * @param text String the regular expression is used for.
 * @param regex Regular expression to be matched with the text.
 * @return Returns the matched string patterns from the text or empty smatch
 *         if matching failed.
 **/
boost::smatch
CostDatabase::getValues(string text, string regex) {
    boost::regex regx(regex + ".*");
    boost::smatch what;
    if (boost::regex_match(text, what, regx, boost::match_extra)) {
        return what;
    }
    boost::smatch empty;
    return empty;
}
