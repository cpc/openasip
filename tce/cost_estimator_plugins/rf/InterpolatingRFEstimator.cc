/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file InterpolatingRFEstimator.cc
 *
 * Declaration of InterpolatingRFEstimator. A RF estimation plugin that 
 * estimates data generating cost database from the cost data stored in HDB.
 * Estimate is interpolated if possible if no exact data values are found.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */
#include "RFCostEstimationPlugin.hh"
#include "Application.hh"
#include "DataObject.hh"
#include "Exception.hh"
#include "HDBManager.hh"
#include "BaseRegisterFile.hh"
#include "RegisterFile.hh"
#include "ExecutionTrace.hh"
#include "CostDatabase.hh"
#include "EntryKeyProperty.hh"
#include "CostDBTypes.hh"
#include "MatchType.hh"
#include "RFArchitecture.hh"
#include "FilterSearch.hh"
#include "CostDatabaseRegistry.hh"

using namespace CostEstimator;
using namespace HDB;
using namespace TTAMachine;

class InterpolatingRFEstimator : public RFCostEstimationPlugin {
public:
    InterpolatingRFEstimator(const std::string& name) :
        RFCostEstimationPlugin(name) {
        costDatabaseRegistry_ = &CostDatabaseRegistry::instance();
    }

    virtual ~InterpolatingRFEstimator() {
    }

    DESCRIPTION(
        "RF cost estimator plugin that estimates costs of RFs by generating"
        "cost database from cost values of HDB and uses interpolation to "
        "estimate the costs. In case there's no cost data available for the "
        "given RF the plugin interpolates the estimate if possible.");

public:

    /**
     * Estimates the register file's area by fetching cost data named 'area'
     * from HDB.
     */  
    bool estimateArea(
        const TTAMachine::BaseRegisterFile& rf,
        const IDF::RFImplementationLocation& /*implementation*/,
        AreaInGates& area,
        HDB::HDBManager& hdb) {

//#define DEBUG_AREA_ESTIMATION
        try {
            initializeEstimator(hdb);
            CostDBTypes::EntryTable results = createSearch(rf);
            CostDBTypes::EntryTable::const_iterator i = results.begin();
            area = 0.0;
            // worst case area is returned
            for (;i < results.end(); i++) {
                for (int n = 0; n < (*i)->statisticsCount(); n++) {
                    if (area < (*i)->statistics(n).area()) {
                        area = (*i)->statistics(n).area();
                    }
                }
            }
        } catch (Exception& e) {
#ifdef DEBUG_AREA_ESTIMATION
            std::cout << "Exception: " << e.name() << std::endl
                      << "Error message: " << e.errorMsg() << std::endl
                      << "File: " << e.fileName() << std::endl
                      <<" Line: " e.lineNum() << std::endl;
#endif //DEBUG_AREA_ESTIMATION
            return false;
        }
#ifdef DEBUG_AREA_ESTIMATION
        Application::logStream() 
            << rf.name() << " area " << area << std::endl;
#endif //DEBUG_AREA_ESTIMATION
        return true;
    }    

    /**
     * Estimates the register file port write delay by fetching cost data 
     * named 'input_delay' from HDB.
     *
     * Assumes that all ports have the same input delay, that is, there is
     * only one 'input_delay' entry for a RF in HDB.
     */
    bool
    estimatePortWriteDelay(
        const TTAMachine::RFPort& port,
        const IDF::RFImplementationLocation& /*implementation*/,
        DelayInNanoSeconds& delay,
        HDB::HDBManager& hdb) {

//#define DEBUG_DELAY_ESTIMATION
        try {
            initializeEstimator(hdb);
            CostDBTypes::EntryTable results = createSearch(*port.parentUnit());
            
            CostDBTypes::EntryTable::const_iterator i = results.begin();
            delay = 0.0;
            // worst case delay is returned
            for (;i < results.end(); i++) {
                for (int n = 0; n < (*i)->statisticsCount(); n++) {
#ifndef UNIQUE_PORT_DELAY
                    if (delay < (*i)->statistics(n).delayPort("input_delay")) {
                        delay = (*i)->statistics(n).delayPort("input_delay");
                    }
#endif // UNIQUE_PORT_DELAY
#ifdef UNIQUE_PORT_DELAY
                    // this one is used if defferent ports of an unit 
                    // can have different delays
                    if (delay < (*i)->statistics(n).delayPort(port.name())) {
                        delay = (*i)->statistics(n).delayPort(port.name());
                    }
#endif // UNIQUE_PORT_DELAY
                }
            }
        } catch (Exception& e) {
#ifdef DEBUG_DELAY_ESTIMATION
            Application::logStream() << "No input_delay data for register "
                                     << "file " 
                                     << port.parentUnit()->name()
                                     << " found in HDB." << std::endl;
#endif // DEBUG_DELAY_ESTIMATION
            return false;
        }
#ifdef DEBUG_DELAY_ESTIMATION
        Application::logStream() 
            << port.name() << " (port) delay " << delay << std::endl;
#endif // DEBUG_DELAY_ESTIMATION
        return true;
    } 

    /**
     * Estimates the register file port read delay by fetching cost data 
     * named 'output_delay' from HDB.
     *
     * Assumes that all ports have the same output delay, that is, there is
     * only one 'output_delay' entry for a RF in HDB.
     */
    bool
    estimatePortReadDelay(
        const TTAMachine::RFPort& port,
        const IDF::RFImplementationLocation& /*implementation*/,
        DelayInNanoSeconds& delay,
        HDB::HDBManager& hdb) {

//#define DEBUG_DELAY_ESTIMATION
        try {
            initializeEstimator(hdb);
            CostDBTypes::EntryTable results = createSearch(*port.parentUnit());
            CostDBTypes::EntryTable::const_iterator i = results.begin();
            delay = 0.0;
            // worst case delay is returned
            for (;i < results.end(); i++) {
                for (int n = 0; n < (*i)->statisticsCount(); n++) {
#ifndef UNIQUE_PORT_DELAY
                    if (delay < (*i)->statistics(n).delayPort("output_delay")) {
                        delay = (*i)->statistics(n).delayPort("output_delay");
                    }
#endif // UNIQUE_PORT_DELAY
#ifdef UNIQUE_PORT_DELAY
                    // this one is used if defferent ports of an unit 
                    // can have different delays
                    if (delay < (*i)->statistics(n).delayPort(port.name())) {
                        delay = (*i)->statistics(n).delayPort(port.name());
                    }
#endif // UNIQUE_PORT_DELAY
                }
            }
        } catch (Exception& e) {
#ifdef DEBUG_DELAY_ESTIMATION
            Application::logStream() << "No output_delay data for register "
                                     << "file " 
                                     << port.parentUnit()->name()
                                     << " found in HDB." << std::endl;
#endif // DEBUG_DELAY_ESTIMATION
            return false;
        }
#ifdef DEBUG_DELAY_ESTIMATION
        Application::logStream() 
            << port.name() << " (port) delay " << delay << std::endl;
#endif // DEBUG_DELAY_ESTIMATION
        return true;
    }

    /**
     * Estimates the register file maximum computation delay by fetching 
     * cost data named 'computation_delay' from HDB.
     */
    bool
    estimateMaximumComputationDelay(
        const TTAMachine::BaseRegisterFile& rf,
        const IDF::RFImplementationLocation& /*implementation*/,
        DelayInNanoSeconds& delay,
        HDB::HDBManager& hdb) {
        
//#define DEBUG_DELAY_ESTIMATION
        try {
            initializeEstimator(hdb);
            CostDBTypes::EntryTable results = createSearch(rf);            
            CostDBTypes::EntryTable::const_iterator i = results.begin();
            delay = 0.0;
            // the worst case is returned if found multiple results
            for (;i < results.end(); i++) {
                for (int n = 0; n < (*i)->statisticsCount(); n++) {
                    if (delay < (*i)->statistics(n).delay()) {
                        delay = (*i)->statistics(n).delay();
                    }
                }
            }
        } catch (Exception& e) {
            return false;
        }
#ifdef DEBUG_DELAY_ESTIMATION
        Application::logStream() << rf.name() 
                                 << " computation delay " 
                                 << delay << std::endl;
#endif // DEBUG_DELAY_ESTIMATION
        return true;
    }  

    /**
     * Estimates the energy consumed by given RF.
     *
     * Estimate is done by computing the sum of all register file access
     * type energies and RF idle energy. Register file access energies are 
     * stored with entries named 'rf_access_energy Nr Nw'. Nr is the number
     * of read accesses, Nw the number of write accesses. For example,
     * 'rf_access_energy 1 3' is the name of entry which tells how much
     * energy is consumed by the RF in case RF is accessed simultaneously
     * once for reading and trice for writing. Idle energy is stored in
     * entry 'rf_idle_energy'.
     */
    bool estimateEnergy(
        const TTAMachine::BaseRegisterFile& rf,
        const IDF::RFImplementationLocation&,
        const TTAProgram::Program&,
        const ExecutionTrace& trace,
        EnergyInMilliJoules& energy,
        HDB::HDBManager& hdb) {

//#define DEBUG_ENERGY_ESTIMATION
        energy = 0.0;
        ClockCycleCount cyclesWithRFAccess = 0;
        CostDBTypes::EntryTable results;
        try {
            initializeEstimator(hdb);
            results = createSearch(rf);

#ifdef DEBUG_ENERGY_ESTIMATION        
            Application::logStream() 
                << "## register file " << rf.name() << ": " << std::endl;
#endif
            
            ExecutionTrace::ConcurrentRFAccessCountList* accessList =
                trace.registerFileAccessCounts(rf.name());
            for (ExecutionTrace::ConcurrentRFAccessCountList::
                     const_iterator i = accessList->begin(); 
                 i != accessList->end(); ++i) {
                const ExecutionTrace::ConcurrentRFAccessCount& accessCount = 
                    *i;
                
                const std::size_t reads = accessCount.get<0>();
                const std::size_t writes = accessCount.get<1>();
                const ClockCycleCount count = accessCount.get<2>();
                
                const std::string dataName = 
                    std::string("rf_access_energy ") + 
                    Conversion::toString(reads) + " " +
                    Conversion::toString(writes);
                try {
                    CostDBTypes::EntryTable::const_iterator i = results.begin();
                    for (;i < results.end(); i++) {

                        // if there are multilpe reults for the rf access 
                        // energy, select the worst case
                        // @todo ensure that multiple result is not a bug
                        EnergyInMilliJoules energyTemp = 0.0; 
                        for (int n = 0; n < (*i)->statisticsCount(); n++) {
                            if (((*i)->statistics(n).energyReadWrite(
                                     reads, writes) * count) > energyTemp) {
                                energyTemp = 
                                    (*i)->statistics(n).energyReadWrite(
                                        reads, writes) * count;
                            }
#ifdef DEBUG_ENERGY_ESTIMATION
                            if (n > 0) {
                                Application::logStream() 
                                    << " NOTE: Multiple register access energy "
                                    << "results found!"
                                    << " " 
                                    << (*i)->statistics(n).energyReadWrite(
                                        reads, writes)
                                    << std::endl;
                            }
#endif
                        }
                        energy += energyTemp;
                    } 
                    cyclesWithRFAccess += count;
                } catch (const KeyNotFound&) {
                    // if no data found, don't even try to estimate the area
                    delete accessList;
                    accessList = NULL;
                    Application::logStream()
                        << "Cost estimation data '" << dataName
                        << "' not found in HDB." << std::endl;
                    return false;
                } catch (const Exception& e) {
                    delete accessList;
                    accessList = NULL;
                    debugLog(e.errorMessage());
                    return false;
                }
            }
            delete accessList;
            accessList = NULL;
        } catch (const Exception& e) {
            debugLog(e.errorMessage());
            return false;
        }

        // add the cost of RF idling
        const ClockCycleCount idleCycles = 
            trace.simulatedCycleCount() - cyclesWithRFAccess;
        const std::string dataName = std::string("rf_idle_energy");
        
        try {
            CostDBTypes::EntryTable::const_iterator i = results.begin();
            for (;i < results.end(); i++) {

                // if there are multilpe reults for the rf idle energy,
                // select the worst case
                // @todo Ensure that multiple results is not a bug
                EnergyInMilliJoules energyTemp = 0.0;
                for (int n = 0; n < (*i)->statisticsCount(); n++) {
                    if (((*i)->statistics(n).energyIdle() * idleCycles) >
                        energyTemp) {

                        energyTemp = 
                            (*i)->statistics(n).energyIdle() * idleCycles;
                    }
#ifdef DEBUG_ENERGY_ESTIMATION
                    if (n > 0) {
                        Application::logStream() 
                            << " NOTE: Multiple register idle energy "
                            << "results found!"
                            << " " << (*i)->statistics(n).energyIdle()
                            << std::endl;
                    }
#endif
                }
                energy += energyTemp;
            }
        } catch (const KeyNotFound&) {
            // if no data found, don't even try to estimate the area
            Application::logStream()
                << "Cost estimation data '" << dataName 
                << "' not found in HDB." << std::endl;
            return false;
        } catch (const Exception& e) {
            debugLog(e.errorMessage());
            return false;
        }
        
        return true;
    }

private:
    /// Registry of cost databases.
    CostDatabaseRegistry* costDatabaseRegistry_;
    /// Cost database being used.
    CostDatabase* costdb_;
    /// Search strategy to be used with the cost database.
    SearchStrategy* strategy_;
    /// Entry key property of register file.
    EntryKeyProperty* rfileProperty_;
    /// Search type for each entry type.
    typedef std::map<
        const EntryKeyProperty*, CostDBTypes::MatchTypeTable> MatchTypeMap;
    /// Types of matches used for searching entries from the cost database.
    MatchTypeMap searchTypes_;
    /// Table of types of match.
    CostDBTypes::MatchTypeTable interpMatchType_;

/**
 * Initializes the plugin.
 *
 * @param hdb The HDB to be used in searching entries. Cost database is created
 * in basis of this HDB.
 */
void 
initializeEstimator(const HDBManager& hdb) {

    costdb_ = &costDatabaseRegistry_->costDatabase(hdb);
    strategy_ = new FilterSearch();
    costdb_->setSearchStrategy(strategy_);
    rfileProperty_ = EntryKeyProperty::find(CostDBTypes::EK_RFILE);
    createSearchTypes();
}

/**
 * Creates a search to cost database to find entries matching the given
 * register file.
 *
 * @param rf Register file which matches are searched.
 * @return Returns entries that matched the register file.
 */
CostDBTypes::EntryTable
createSearch(const BaseRegisterFile& rf) const 
    throw (Exception) {

    CostDBTypes::EntryTable results;
    
    RFArchitecture rfArch(&rf);
    
    CostDBEntryKey* searchKey = new CostDBEntryKey(rfileProperty_);  
    searchKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(rfArch.size()),
            rfileProperty_->fieldProperty(
                CostDBTypes::EKF_NUM_REGISTERS)));
    searchKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(rfArch.readPortCount()),
            rfileProperty_->fieldProperty(
                CostDBTypes::EKF_READ_PORTS)));
    searchKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(rfArch.writePortCount()),
            rfileProperty_->fieldProperty(
                CostDBTypes::EKF_WRITE_PORTS)));
    searchKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(rfArch.width()),
            rfileProperty_->fieldProperty(CostDBTypes::EKF_BIT_WIDTH)));
    searchKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(rfArch.latency()),
            rfileProperty_->fieldProperty(CostDBTypes::EKF_LATENCY)));
    searchKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(rfArch.bidirPortCount()),
            rfileProperty_->fieldProperty(
                CostDBTypes::EKF_BIDIR_PORTS)));
    searchKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(rfArch.maxReads()),
            rfileProperty_->fieldProperty(
                CostDBTypes::EKF_MAX_READS)));
    searchKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(rfArch.maxWrites()),
            rfileProperty_->fieldProperty(
                CostDBTypes::EKF_MAX_WRITES)));
    searchKey->addField(
        new EntryKeyField(
            new EntryKeyDataBool(rfArch.hasGuardSupport()),
            rfileProperty_->fieldProperty(
                CostDBTypes::EKF_GUARD_SUPPORT)));
    searchKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(rfArch.guardLatency()),
            rfileProperty_->fieldProperty(
                CostDBTypes::EKF_GUARD_LATENCY)));
    
    // Perform a database query.
    try {
        results = costdb_->search(*searchKey, interpMatchType_);
    } catch (Exception& e) {
        delete searchKey;
        searchKey = 0;
        throw e;
    }
    delete searchKey;
    searchKey = 0;
    return results;
}
    
/**
 * Creates types of matches used for searching cost database
 * entries.
 */
void createSearchTypes() {
    
    interpMatchType_.push_back(
        new MatchType(
            rfileProperty_->fieldProperty(CostDBTypes::EKF_NUM_REGISTERS),
            CostDBTypes::MATCH_INTERPOLATION));
    interpMatchType_.push_back(
        new MatchType(
            rfileProperty_->fieldProperty(CostDBTypes::EKF_READ_PORTS),
            CostDBTypes::MATCH_EXACT));
    interpMatchType_.push_back(
        new MatchType(
            rfileProperty_->fieldProperty(CostDBTypes::EKF_WRITE_PORTS),
            CostDBTypes::MATCH_EXACT));
    interpMatchType_.push_back(
        new MatchType(
            rfileProperty_->fieldProperty(CostDBTypes::EKF_BIDIR_PORTS),
            CostDBTypes::MATCH_EXACT));
    interpMatchType_.push_back(
        new MatchType(
            rfileProperty_->fieldProperty(CostDBTypes::EKF_BIT_WIDTH),
            CostDBTypes::MATCH_INTERPOLATION));
    interpMatchType_.push_back(
        new MatchType(
            rfileProperty_->fieldProperty(CostDBTypes::EKF_LATENCY),
            CostDBTypes::MATCH_EXACT));
    interpMatchType_.push_back(
        new MatchType(
            rfileProperty_->fieldProperty(CostDBTypes::EKF_MAX_READS),
            CostDBTypes::MATCH_EXACT));
    interpMatchType_.push_back(
        new MatchType(
            rfileProperty_->fieldProperty(CostDBTypes::EKF_MAX_WRITES),
            CostDBTypes::MATCH_EXACT));
    interpMatchType_.push_back(
        new MatchType(
            rfileProperty_->fieldProperty(CostDBTypes::EKF_GUARD_SUPPORT),
            CostDBTypes::MATCH_EXACT));
    interpMatchType_.push_back(
        new MatchType(
            rfileProperty_->fieldProperty(CostDBTypes::EKF_GUARD_LATENCY),
            CostDBTypes::MATCH_EXACT));
}

};

EXPORT_RF_COST_ESTIMATOR_PLUGIN(InterpolatingRFEstimator)
