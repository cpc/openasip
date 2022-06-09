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
 * @file StrictMatchRFEstimator.cc
 *
 * Declaration of StrictMatchRFEstimator. A RF estimation plugin that 
 * estimates data only by fetching prestored cost data of the units.
 * It does not perform any kind of linearization etc. to try to estimate
 * a RF that has not direct cost data in HDB.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */
#include "RFCostEstimationPlugin.hh"
#include "Application.hh"
#include "DataObject.hh"
#include "Exception.hh"
#include "HDBManager.hh"
#include "BaseRegisterFile.hh"
#include "ExecutionTrace.hh"
#include "Conversion.hh"

using namespace CostEstimator;

class StrictMatchRFEstimator : public RFCostEstimationPlugin {
public:
    StrictMatchRFEstimator(const std::string& name) :
        RFCostEstimationPlugin(name) {
    }

    virtual ~StrictMatchRFEstimator() {
    }

    DESCRIPTION(
        "Simple RF cost estimator plugin that estimates costs of RFs "
        "simply by looking up direct matches from HDB. In case there's "
        "no cost data available for the given RF, it's unable to estimate "
        "it by using linearization etc.");

public:

    /**
     * Estimates the register file's area by fetching cost data named 'area'
     * from HDB.
     */  
    bool estimateArea(
        const TTAMachine::BaseRegisterFile& /*rf*/,
        const IDF::RFImplementationLocation& implementation,
        AreaInGates& area,
        HDB::HDBManager& hdb) {

//#define DEBUG_AREA_ESTIMATION

        try { 
            // simply fetch the area data of the FU, if any
            DataObject areaFromDB = hdb.rfCostEstimationData(
                "area", implementation.id(), name_);       
            area = areaFromDB.doubleValue();
#ifdef DEBUG_AREA_ESTIMATION
            Application::logStream() 
                << rf.name() << " area " << area << std::endl;
#endif
            return true;
        } catch (const KeyNotFound&) {
            // if no area data found, don't even try to estimate the area
            // somehow
            Application::logStream() << "No area data found in HDB.";

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
     * Estimates the register file port write delay by fetching cost data 
     * named 'input_delay' from HDB.
     *
     * Assumes that all ports have the same input delay, that is, there is
     * only one 'input_delay' entry for a RF in HDB.
     */
    bool
    estimatePortWriteDelay(
        const TTAMachine::RFPort& /*port*/,
        const IDF::RFImplementationLocation& implementation,
        DelayInNanoSeconds& delay,
        HDB::HDBManager& hdb) {

//#define DEBUG_DELAY_ESTIMATION
        try { 
            // simply fetch the delay data of the RF port, if any
            DataObject delayFromDB = hdb.rfCostEstimationData(
                "input_delay", implementation.id(), name_);       
            delay = delayFromDB.doubleValue();
#ifdef DEBUG_DELAY_ESTIMATION
            Application::logStream() 
                << port.name() << " write delay " << delay << std::endl;
#endif
            return true;
        } catch (const KeyNotFound&) {
            // if no data found, don't even try to estimate the area
            Application::logStream()
                << "No input_delay cost data found for RF " 
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
     * Estimates the register file port read delay by fetching cost data 
     * named 'output_delay' from HDB.
     *
     * Assumes that all ports have the same output delay, that is, there is
     * only one 'output_delay' entry for a RF in HDB.
     */
    bool
    estimatePortReadDelay(
        const TTAMachine::RFPort& /*port*/,
        const IDF::RFImplementationLocation& implementation,
        DelayInNanoSeconds& delay,
        HDB::HDBManager& hdb) {

        try { 
            // simply fetch the delay data of the RF port, if any
            DataObject delayFromDB = hdb.rfCostEstimationData(
                "output_delay", implementation.id(), name_);       
            delay = delayFromDB.doubleValue();
#ifdef DEBUG_DELAY_ESTIMATION
            Application::logStream() 
                << port.name() << " read delay " << delay << std::endl;
#endif
            return true;
        } catch (const KeyNotFound&) {
            // if no data found, don't even try to estimate the area
            Application::logStream()
                << "No output_delay cost data found for RF " 
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
     * Estimates the register file maximum computation delay by fetching 
     * cost data named 'computation_delay' from HDB.
     */
    bool
    estimateMaximumComputationDelay(
        const TTAMachine::BaseRegisterFile& /*rf*/,
        const IDF::RFImplementationLocation& implementation,
        DelayInNanoSeconds& delay,
        HDB::HDBManager& hdb) {

        try { 
            // simply fetch the delay data of the RF, if any
            DataObject delayFromDB = hdb.rfCostEstimationData(
                "computation_delay", implementation.id(), name_);       
            delay = delayFromDB.doubleValue();
#ifdef DEBUG_DELAY_ESTIMATION
            Application::logStream() 
                << rf.name() << " computation delay " << delay << std::endl;
#endif
            return true;
        } catch (const KeyNotFound&) {
            // if no data found, don't even try to estimate the area
            Application::logStream()
                << "No computation_delay cost data found for RF " 
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
        const IDF::RFImplementationLocation& implementation,
        const TTAProgram::Program&,
        const ExecutionTrace& trace,
        EnergyInMilliJoules& energy,
        HDB::HDBManager& hdb) {

//#define DEBUG_ENERGY_ESTIMATION

        energy = 0.0;
        ClockCycleCount cyclesWithRFAccess = 0;
#ifdef DEBUG_ENERGY_ESTIMATION        
        Application::logStream() 
            << "## register file " << rf.name() << ": " << std::endl;
#endif
        try {
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
                    DataObject energyFromDB = hdb.rfCostEstimationData(
                        dataName, implementation.id(), name_);
                    EnergyInMilliJoules energyOfAccess =
                        energyFromDB.doubleValue()*count;
#ifdef DEBUG_ENERGY_ESTIMATION        
                    Application::logStream() 
                        << "## reads " << accessCount.get<0>() << " "
                        << "writes " << accessCount.get<1>() << " = "
                        << energyFromDB.doubleValue() << " times " 
                        << count << " = " << energyOfAccess << std::endl;
#endif
                    energy += energyOfAccess;
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
            DataObject energyFromDB = hdb.rfCostEstimationData(
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


};

EXPORT_RF_COST_ESTIMATOR_PLUGIN(StrictMatchRFEstimator)
