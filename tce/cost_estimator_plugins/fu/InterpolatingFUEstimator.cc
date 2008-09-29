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
 * @file InterpolatingFUEstimator.cc
 *
 * Declaration of InterpolatingFUEstimator. A FU estimation plugin that 
 * estimates data generating cost database from the cost data stored in HDB.
 * Estimate is interpolated if possible if no exact data values are found.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */
#include "FUCostEstimationPlugin.hh"
#include "Application.hh"
#include "DataObject.hh"
#include "Exception.hh"
#include "HDBManager.hh"
#include "FunctionUnit.hh"
#include "ExecutionTrace.hh"
#include "CostDatabase.hh"
#include "EntryKeyProperty.hh"
#include "CostDBTypes.hh"
#include "MatchType.hh"
#include "FilterSearch.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "FUEntry.hh"
#include "FUImplementation.hh"
#include "FUArchitecture.hh"
#include "BaseFUPort.hh"
#include "CostDatabaseRegistry.hh"
#include "StringTools.hh"

using namespace CostEstimator;
using namespace HDB;
using namespace TTAMachine;
using std::string;
using std::set;
using std::vector;

class InterpolatingFUEstimator : public FUCostEstimationPlugin {
public:
    InterpolatingFUEstimator(const std::string& name) :
        FUCostEstimationPlugin(name) {
        costDatabaseRegistry_ = &CostDatabaseRegistry::instance();
    }

    virtual ~InterpolatingFUEstimator() {
    }

    DESCRIPTION(
        "FU cost estimator plugin that estimates costs of FUs by generating"
        "cost database from cost values of HDB and uses interpolation to "
        "estimate the costs. In case there's no cost data available for the "
        "given FU the plugin interpolates the estimate if possible.");

public:
    
    /**
     * Estimates the function unit's area by fetching cost data named 'area'
     * from HDB.
     */
    bool estimateArea(
        const TTAMachine::FunctionUnit& fu,
        const IDF::FUImplementationLocation& implementation,
        AreaInGates& area,
        HDB::HDBManager& hdb) {

//#define DEBUG_AREA_ESTIMATION
        try {
            initializeEstimator(hdb);
            CostDBTypes::EntryTable results = createSearch(fu);
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
            return false;
        }
#ifdef DEBUG_AREA_ESTIMATION
        Application::logStream() 
            << fu.name() << " area " << area << std::endl;
#endif
        return true;
    }    

    /**
     * Estimates the function unit port write delay by fetching cost data 
     * named 'input_delay' from HDB.
     *
     * Assumes that all ports have the same input delay, that is, there is
     * only one 'input_delay' entry for a FU in HDB.
     */
    bool
    estimatePortWriteDelay(
        const TTAMachine::FUPort& port,
        const IDF::FUImplementationLocation& implementation,
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
            Application::logStream() << "No input_delay data for function "
                                     << "unit " 
                                     << port.parentUnit()->name()
                                     << " found in HDB." << std::endl;
#endif // DEBUG_DELAY_ESTIMATION
            return false;
        }
#ifdef DEBUG_DELAY_ESTIMATION
        Application::logStream() 
            << port.name() << " (port) delay " << delay << std::endl;
#endif
        return true;
    }

    /**
     * Estimates the function unit port read delay by fetching cost data 
     * named 'output_delay' from HDB.
     *
     * Assumes that all ports have the same output delay, that is, there is
     * only one 'output_delay' entry for a FU in HDB.
     */
    bool
    estimatePortReadDelay(
        const TTAMachine::FUPort& port,
        const IDF::FUImplementationLocation& implementation,
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
            Application::logStream() << "No input_delay data for function "
                                     << "unit " 
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
     * Estimates the function unit maximum computation delay by fetching 
     * cost data named 'computation_delay' from HDB.
     */
    bool estimateMaximumComputationDelay(
        const TTAMachine::FunctionUnit& fu,
        const IDF::FUImplementationLocation& implementation,
        DelayInNanoSeconds& delay,
        HDB::HDBManager& hdb) {

//#define DEBUG_DELAY_ESTIMATION
        try {
            initializeEstimator(hdb);
            CostDBTypes::EntryTable results = createSearch(fu);            
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
        Application::logStream() 
            << fu.name() << " computation delay " << delay << std::endl;
#endif // DEBUG_DELAY_ESTIMATION
        return true;
    }  
    /**
     * Estimates the energy consumed by given FU.
     *
     * Estimate is done by computing the sum of all operation execution
     * energies and FU idle energy. Operation execution energies are stored
     * with entries named 'operation_execution_energy operation_name'. The
     * idle energy is in entry named 'fu_idle_energy'.
     */
    virtual bool estimateEnergy(
        const TTAMachine::FunctionUnit& fu,
        const IDF::FUImplementationLocation& implementation,
        const TTAProgram::Program& program,
        const ExecutionTrace& trace,
        EnergyInMilliJoules& energy,
        HDB::HDBManager& hdb) {

//#define DEBUG_ENERGY_ESTIMATION
        try {
            initializeEstimator(hdb);
            CostDBTypes::EntryTable results = createSearch(fu);

            energy = 0.0;
            ClockCycleCount cyclesWithFUAccess = 0;
#ifdef DEBUG_ENERGY_ESTIMATION        
            Application::logStream() 
                << "## function unit " << fu.name() << ": " << std::endl;
#endif
            ExecutionTrace::FUOperationTriggerCountList* operationTriggers =
                trace.functionUnitOperationTriggerCounts(fu.name());
            for (ExecutionTrace::FUOperationTriggerCountList::
                     const_iterator i = operationTriggers->begin(); 
                 i != operationTriggers->end(); ++i) {

                const ExecutionTrace::FUOperationTriggerCount& triggerCount = 
                    *i;

                const ExecutionTrace::OperationID operation = 
                    StringTools::stringToLower(triggerCount.get<0>());

                const ExecutionTrace::OperationTriggerCount count = 
                    triggerCount.get<1>();

                const std::string dataName = 
                    std::string("operation_execution_energy ") + operation;

                try {
                    CostDBTypes::EntryTable::const_iterator i = results.begin();
                    for (;i < results.end(); i++) {

                        // in case there are multiple operation execution
                        // energies, select the worst case in the result
                        // @todo ensure that multiple results is not a bug
                        EnergyInMilliJoules tempEnergy = 0.0;
                        for (int n = 0; n < (*i)->statisticsCount(); n++) {
                            if (((*i)->statistics(n).energyOperation(
                                     operation) * count) > tempEnergy) {

                                tempEnergy += 
                                    (*i)->statistics(n).energyOperation(
                                        operation) * count;
                            }
#ifdef DEBUG_ENERGY_ESTIMATION
                            if (n > 0) {
                                Application::logStream() 
                                    << " NOTE: Multiple fu execution energy"
                                    <<  " results found!"
                                    << " operation: " << operation
                                    << " " 
                                    << (*i)->statistics(n).energyOperation(
                                        operation)
                                    << std::endl;
                            }
#endif
                        }
                        energy += tempEnergy;
                    }
                    cyclesWithFUAccess += count;
                } catch (const KeyNotFound&) {
                    // if no data found, don't even try to estimate the area
                    delete operationTriggers;
                    operationTriggers = NULL;
                    Application::logStream()
                        << "Cost estimation data '" << dataName 
                        << "' not found in HDB." << std::endl;
                    return false;
                } catch (const Exception& e) {
                    delete operationTriggers;
                    operationTriggers = NULL;
                    debugLog(e.errorMessage());
                    return false;
                }
                
            }
            delete operationTriggers;
            operationTriggers = NULL;
            
            // add the cost of FU idling
            const ClockCycleCount idleCycles = 
                trace.simulatedCycleCount() - cyclesWithFUAccess;
            const std::string dataName = std::string("fu_idle_energy");
            
            try {
                CostDBTypes::EntryTable::const_iterator i = results.begin();
                for (;i < results.end(); i++) {

                    // in case there are multiple fu idle energies,
                    // select the worst case in the result
                    // @todo ensure that multiple results is not a bug
                    EnergyInMilliJoules tempEnergy = 0.0;
                    for (int n = 0; n < (*i)->statisticsCount(); n++) {
                        if (((*i)->statistics(n).energyIdle() * idleCycles) >
                            tempEnergy) {
                            
                            tempEnergy = 
                                (*i)->statistics(n).energyIdle() * idleCycles;
                        }
#ifdef DEBUG_ENERGY_ESTIMATION
                        if (n > 0) {
                            Application::logStream() 
                                << " NOTE: Multiple fu idle energy results found!"
                                << (*i)->statistics(n).energyIdle()
                                << std::endl;
                        }
#endif
                    }
                    energy += tempEnergy;
                } 
            } catch (const KeyNotFound&) {
                // if no data found, don't even try to estimate the area
                Application::logStream()
                    << "Cost estimation data '" << dataName 
                    << "' for FU with id " << implementation.id() 
                    << " not found in HDB." << std::endl;
                return false;
            }
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
    /// Entry key property of function unit.
    EntryKeyProperty* unitProperty_;
    /// Search type for each entry type.
    typedef std::map<
        const EntryKeyProperty*, CostDBTypes::MatchTypeTable> MatchTypeMap;
    /// Types of matches used for searching entries from the cost database.
    MatchTypeMap searchTypes_;
    /// Table of types of match.
    CostDBTypes::MatchTypeTable unitMatchType_;

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
    unitProperty_ = EntryKeyProperty::find(CostDBTypes::EK_UNIT);
    createSearchTypes();
}

/**
 * Creates types of matches used for searching cost database
 * entries.
 */
void createSearchTypes() {
    
    unitMatchType_.push_back(
        new MatchType(
            unitProperty_->fieldProperty(CostDBTypes::EKF_BIT_WIDTH),
            CostDBTypes::MATCH_INTERPOLATION));
    unitMatchType_.push_back(
        new MatchType(
            unitProperty_->fieldProperty(CostDBTypes::EKF_FUNCTION_UNIT),
            CostDBTypes::MATCH_EXACT));
}

/**
 * Creates a search to cost database to find entries matching the given
 * function unit.
 *
 * @param fu Function unit which matches are searched.
 * @return Returns entries that matched the function unit.
 */
CostDBTypes::EntryTable createSearch(const FunctionUnit& fu) const {

    CostDBTypes::EntryTable results;
    int ports = fu.operationPortCount();
    int width = 0;
    for (int n = 0; n < ports; n++) {
        BaseFUPort* port = fu.port(n);
        if (dynamic_cast<FUPort*>(port) != NULL) {
            FUPort* fuPort = dynamic_cast<FUPort*>(port);
            if (width == 0) {
                width = fuPort->width();
            } else if (width != fuPort->width()) {
                return results;
            }
        }
    }
    CostDBEntryKey* searchKey = new CostDBEntryKey(unitProperty_);  
    searchKey->addField(
        new EntryKeyField(
            new EntryKeyDataInt(width),
            unitProperty_->fieldProperty(CostDBTypes::EKF_BIT_WIDTH)));
    searchKey->addField(
        new EntryKeyField(
            new EntryKeyDataFunctionUnit(&fu),
            unitProperty_->fieldProperty(CostDBTypes::EKF_FUNCTION_UNIT)));
    // Perform a database query.
    try {
        results = costdb_->search(*searchKey, unitMatchType_);
    } catch (Exception& e) {
        // no results
        delete searchKey;
        searchKey = 0;
        throw e;
    }
    delete searchKey;
    searchKey = 0;

    return results;
}

};

EXPORT_FU_COST_ESTIMATOR_PLUGIN(InterpolatingFUEstimator)
