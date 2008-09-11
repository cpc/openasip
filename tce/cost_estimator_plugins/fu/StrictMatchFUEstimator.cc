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
 * @file StrictMatchFUEstimator.cc
 *
 * Declaration of StrictMatchFUEstimator. An FU estimation plugin that 
 * estimates FUs simply by fetching pre-stored cost data of the units
 * from HDB. It does not perform any kind of linearization etc. to  
 * estimate an FU that has not direct cost data in HDB.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */
#include "FUCostEstimationPlugin.hh"
#include "Application.hh"
#include "DataObject.hh"
#include "Exception.hh"
#include "HDBManager.hh"
#include "FunctionUnit.hh"
#include "ExecutionTrace.hh"
#include "StringTools.hh"

using namespace CostEstimator;

class StrictMatchFUEstimator : public FUCostEstimationPlugin {
public:
    StrictMatchFUEstimator(const std::string& name) :
        FUCostEstimationPlugin(name) {
    }

    virtual ~StrictMatchFUEstimator() {
    }

    DESCRIPTION(
        "Simple FU cost estimator plugin that estimates costs of FUs "
        "simply by looking up direct matches from HDB. In case there's "
        "no cost data available for the given FU, it's unable to estimate "
        "it by using linearization etc.");

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
            // simply fetch the area data of the FU, if any
            DataObject areaFromDB = hdb.fuCostEstimationData(
                "area", implementation.id(), name_);       
            area = areaFromDB.doubleValue();
#ifdef DEBUG_AREA_ESTIMATION
            Application::logStream() 
                << fu.name() << " area " << area << std::endl;
#endif
            return true;
        } catch (const KeyNotFound&) {
            // if no area data found, don't even try to estimate the area
            // somehow
            return false;
        } catch (const Exception& e) {
            // for example, if doubleValue() conversion failed, then it's
            // a problem with HDB contents 
            debugLog(e.errorMessage());
            return false;
        }
        return false;
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
        const TTAMachine::FUPort&,
        const IDF::FUImplementationLocation& implementation,
        DelayInNanoSeconds& delay,
        HDB::HDBManager& hdb) {

        try { 
            // simply fetch the delay data of the FU port, if any
            DataObject delayFromDB = hdb.fuCostEstimationData(
                "input_delay", implementation.id(), name_);       
            delay = delayFromDB.doubleValue();
            return true;
        } catch (const KeyNotFound&) {
            // if no data found, don't even try to estimate the area
            Application::logStream()
                << "No input_delay cost data found for FU " 
                << implementation.id() << ", plugin " << name_ << std::endl;
            return false;
        } catch (const Exception& e) {
            // for example, if doubleValue() conversion failed, then it's
            // a problem with HDB contents 
            debugLog(e.errorMessage());
            return false;
        }

        return false;
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
        const TTAMachine::FUPort&,
        const IDF::FUImplementationLocation& implementation,
        DelayInNanoSeconds& delay,
        HDB::HDBManager& hdb) {

        try { 
            // simply fetch the delay data of the FU port, if any
            DataObject delayFromDB = hdb.fuCostEstimationData(
                "output_delay", implementation.id(), name_);       
            delay = delayFromDB.doubleValue();
            return true;
        } catch (const KeyNotFound&) {
            // if no data found, don't even try to estimate the area
            Application::logStream()
                << "No output_delay cost data found for FU " 
                << implementation.id() << ", plugin " << name_ << std::endl;
            return false;
        } catch (const Exception& e) {
            // for example, if doubleValue() conversion failed, then it's
            // a problem with HDB contents 
            debugLog(e.errorMessage());
            return false;
        }

        return false;
    }

    /**
     * Estimates the function unit maximum computation delay by fetching 
     * cost data named 'computation_delay' from HDB.
     */
    bool estimateMaximumComputationDelay(
        const TTAMachine::FunctionUnit& architecture,
        const IDF::FUImplementationLocation& implementation,
        DelayInNanoSeconds& delay,
        HDB::HDBManager& hdb) {

        try { 
            // simply fetch the delay data of the FU port, if any
            DataObject delayFromDB = hdb.fuCostEstimationData(
                "computation_delay", implementation.id(), name_);       
            delay = delayFromDB.doubleValue();
            return true;
        } catch (const KeyNotFound&) {
            // if no data found, don't even try to estimate the area
            Application::logStream()
                << "No computation_delay cost data found for FU " 
                << implementation.id() << ", plugin " << name_ << std::endl;
            return false;
        } catch (const Exception& e) {
            // for example, if doubleValue() conversion failed, then it's
            // a problem with HDB contents 
            debugLog(e.errorMessage());
            return false;
        }

        return false;
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
        energy = 0.0;
        ClockCycleCount cyclesWithFUAccess = 0;
#ifdef DEBUG_ENERGY_ESTIMATION        
        Application::logStream() 
            << "## function unit " << fu.name() << ": " << std::endl;
#endif
        try {
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
                    std::string("operation_execution_energy ") +
                    operation;

                try { 
                    DataObject energyFromDB = hdb.fuCostEstimationData(
                        dataName, implementation.id(), name_);
                    EnergyInMilliJoules energyOfAccess =
                        energyFromDB.doubleValue()*count;
#ifdef DEBUG_ENERGY_ESTIMATION        
                    Application::logStream() 
                        << "## " << operation << " = " 
                        << energyFromDB.doubleValue() << " times " 
                        << count << " = " << energyOfAccess << std::endl;
#endif
                    energy += energyOfAccess;
                    cyclesWithFUAccess += count;
                } catch (const KeyNotFound&) {
                    // if no data found, don't even try to estimate the energy
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
        }  catch (const Exception& e) {
            debugLog(e.errorMessage());
            return false;
        }

        // add the cost of FU idling
        const ClockCycleCount idleCycles = 
            trace.simulatedCycleCount() - cyclesWithFUAccess;
        const std::string dataName = std::string("fu_idle_energy");
        
        try { 
            DataObject energyFromDB = hdb.fuCostEstimationData(
                dataName, implementation.id(), name_);
            EnergyInMilliJoules idleEnergy =
                energyFromDB.doubleValue()*idleCycles;
#ifdef DEBUG_ENERGY_ESTIMATION        
            Application::logStream() 
                << "## idle energy " << energyFromDB.doubleValue() << " times "
                << idleCycles << " = " << idleEnergy << std::endl;

#endif
            energy += idleEnergy;

        } catch (const KeyNotFound&) {
            // if no data found, don't even try to estimate the energy
            Application::logStream()
                        << "Cost estimation data '" << dataName 
                        << "' for FU with id " << implementation.id() 
                        << " not found in HDB." << std::endl;
            return false;
        } catch (const Exception& e) {
            debugLog(e.errorMessage());
            return false;
        }

        return true;
    }

};

EXPORT_FU_COST_ESTIMATOR_PLUGIN(StrictMatchFUEstimator)
