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
 * @file ComponentImplementationSelector.cc
 *
 * Implementation of ComponentImplementationSelector class
 *
 * @author Jari M‰ntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @author Esa M‰‰tt‰ 2008 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#include "ComponentImplementationSelector.hh"
#include "FunctionUnit.hh"
#include "RegisterFile.hh"
#include "RFPort.hh"
#include "FUPort.hh"
#include "Program.hh"
#include "UnitImplementationLocation.hh"
#include "ExecutionTrace.hh"
#include "CostEstimates.hh"
#include "Operation.hh"
#include "HDBManager.hh"
#include "FUArchitecture.hh"
#include "RFArchitecture.hh"
#include "HWOperation.hh"
#include "StringTools.hh"
#include "FUEntry.hh"
#include "RFEntry.hh"
#include "NullUnitImplementationLocation.hh"
#include "FUImplementation.hh"
#include "ImmediateUnit.hh"

using std::map;
using std::pair;
using std::set;
using std::list;
using std::string;
using namespace HDB;
using namespace TTAMachine;


/**
 * The constructor.
 */
ComponentImplementationSelector::ComponentImplementationSelector() {
}

/**
 * The destructor
 */
ComponentImplementationSelector::~ComponentImplementationSelector() {
}

/**
 * Adds new HDB to look for components.
 *
 * @param hdb The HDB file name to be added.
 * @exception Exception in case there was a problem while opening the HDB.
 */
void
ComponentImplementationSelector::addHDB(const HDBManager& hdb) 
    throw (Exception) {

    usedHDBs_.insert(hdb.fileName());
}

/**
 * Adds new traceDB and corresponding program.
 *
 * @param program Test case program to be used.
 * @param traceDB Simulation trace of the program.
 */
void
ComponentImplementationSelector::addCase(
    const TTAProgram::Program&, const ExecutionTrace&) {

}

/** 
 * Finds set of possible function unit implementations.
 *
 * Finds out which function unit implementations fulfill the frequency and
 * area requirements. The implementations are selected by estimating costs
 * of different function unit implementations that match the given funtion unit
 * architecture. All implementations that fulfill the given frequency and area
 * requirements are returned including the estimated cost data of those
 * implementations.
 *
 * @param fu Function unit architecture.
 * @param frequencyMHz Target frequency (MHz) of the function unit.
 * @param maxArea Maximum area (in gates) of the function unit.
 * @return Set of FU implementations with cost estimation data. Returns an
 * empty set if none found.
 */
map<const IDF::FUImplementationLocation*, CostEstimates*>
ComponentImplementationSelector::fuImplementations(
    const TTAMachine::FunctionUnit& fu, double frequencyMHz, double maxArea) {

    // Get all fu entries matching the given architecture from all available
    // HDBs and create implementation locations of them.
    set<const IDF::FUImplementationLocation*> fuImplementations;
    for (set<string>::const_iterator i = usedHDBs_.begin();
         i != usedHDBs_.end(); i++) {
        
        HDBManager& hdb = HDBRegistry::instance().hdb(*i);
        set<RowID> fuEntryIDs = hdb.fuEntriesByArchitecture(fu);
        set<RowID>::const_iterator id = fuEntryIDs.begin();
        for (; id != fuEntryIDs.end(); id++) {
            IDF::FUImplementationLocation* fuIDF = 
                new IDF::FUImplementationLocation(
                    hdb.fileName(), *id,
                    fu.name());
            IDF::MachineImplementation* machIDF =
                new IDF::MachineImplementation();
            machIDF->addFUImplementation(fuIDF);
            fuImplementations.insert(fuIDF);
        }
    }
    map<const IDF::FUImplementationLocation*, CostEstimates*> results;
    set<const IDF::FUImplementationLocation*>::const_iterator iter = 
        fuImplementations.begin();
    
    // in case we are not limiting the costs, return all matching
    // implementations
    if (static_cast<int>(frequencyMHz) <= 0 && 
            static_cast<int>(maxArea) <= 0) {

        for (; iter != fuImplementations.end(); iter++) {
            results.insert(
                pair<const IDF::FUImplementationLocation*, CostEstimates*>(
                    (*iter), NULL));
        }
        return results;
    }

    // Estimate costs the the implementations
    for (; iter != fuImplementations.end(); iter++) {
        try {
            double area = estimator_.functionUnitArea(fu, *(*iter));
            if (area > maxArea && static_cast<int>(maxArea) > 0) {
                // FU area too large
                fuImplementations.erase(iter);
                continue;
            }
            double delayInNanoSeconds = 
                estimator_.functionUnitMaximumComputationDelay(fu, *(*iter));
            
            // 1000/ns = MHz
            if ((1000/delayInNanoSeconds) < frequencyMHz) {
                // FU too slow
                fuImplementations.erase(iter);
                continue;
            }
            CostEstimates* estimates = new CostEstimates();
            estimates->setArea(area);
            estimates->setLongestPathDelay(delayInNanoSeconds);
            results.insert(
                pair<const IDF::FUImplementationLocation*, CostEstimates*>(
                    (*iter), estimates));
        } catch (CannotEstimateCost& e) {
            // Couldn't estimate the fu.
            fuImplementations.erase(iter);
            continue;
        }
    }
    return results;
}

/**
 * Finds the minimum set of Function units that is needed to satisfy all the
 * wanted operations.
 *
 * Function units are selected so that the number of needed units would be as
 * low as possible and smaller latency is preferred. If there are units with
 * same latency then units with smaller amount of operations is preferred. If
 * there still are units with equal operations then the unit with the widest
 * triggering port is preferred.
 *
 * @param operationSet Set of operation names the resultig set of function
 * units must include.
 * @param width Bitwidth of the funtion unit ports. Not used if left as default.
 * @return Minimal set of function units that have all the asked operations.
 * Returns an empty set if all operations cannot be supported by any set of
 * function units.
 */
list<TTAMachine::FunctionUnit*>
ComponentImplementationSelector::fuArchsByOpSetWithMinLatency(
    const std::set<std::string>& operationSet, int width)
    const {

    // Convert operation names to lower case.
    set<string> operations;
    set<string>::const_iterator oper = operationSet.begin();
    for (; oper != operationSet.end(); oper++) {
        operations.insert(StringTools::stringToLower((*oper)));
    }

    // Get all function unit architectures.
    list<pair<TTAMachine::FunctionUnit*, int> >functionUnits;
    for (set<string>::const_iterator i = usedHDBs_.begin();
         i != usedHDBs_.end(); i++) {

        set<RowID> fuArchIDs = 
            HDBRegistry::instance().hdb(*i).fuArchitectureIDsByOperationSet(
                operations);
        set<RowID>::const_iterator iter = fuArchIDs.begin();
        for (;iter != fuArchIDs.end(); iter++) {
            FUArchitecture* fuArch = 
                HDBRegistry::instance().hdb(*i).fuArchitectureByID(*iter);

            // check the fu port widths if the width is given as parameter
            if (width) {
                bool portsDiffer = false;
                for (int p = 0; p < fuArch->architecture().portCount(); p++) {
                    if (fuArch->architecture().port(p)->width() != width) {
                        portsDiffer = true;
                    }
                }
                if (portsDiffer) {
                    continue;
                }
            }
            pair<FunctionUnit*, int> fuIntPair(&fuArch->architecture(), 0);
            functionUnits.push_back(fuIntPair);
        }
    }
    
    list<TTAMachine::FunctionUnit*> results;
    
    // Find the best set of function units
    while (operations.size() != 0) {
        list<pair<TTAMachine::FunctionUnit*, int> >::iterator fu = 
            functionUnits.begin();
        for (; fu != functionUnits.end(); fu++) {
            int neededOperations = 0;
            set<string>::iterator operation = operations.begin();
            while (operation != operations.end()) {
                if ((*fu).first->hasOperation(*operation)) {
                    neededOperations++;
                }
                operation++;
            }
            (*fu).second = neededOperations;
        }
        
        list<TTAMachine::FunctionUnit*> bestMatches;
        int bestFU = 0;
        
        // Find out which FU:s implement most needed operations
        for (fu = functionUnits.begin(); fu != functionUnits.end(); fu++) {
            if ((*fu).second > bestFU) {
                bestMatches.clear();
                bestMatches.push_back((*fu).first);
                bestFU = (*fu).second;
            } else if ((*fu).second == bestFU) {
                bestMatches.push_back((*fu).first);
            }
        }

        // if found set
        if (bestMatches.size() != 0) {
            TTAMachine::FunctionUnit* bestFU = NULL;
            int minLatency = -1;
            int minOperations = -1;
            int maxPortWidth = -1;
            // Select one of the best FU:s
            list<TTAMachine::FunctionUnit*>::const_iterator bestMatchFU = 
                bestMatches.begin();
            for (; bestMatchFU != bestMatches.end(); bestMatchFU++) {
                int fuMaxLatency = (*bestMatchFU)->maxLatency();
                if (fuMaxLatency < minLatency) {
                    minLatency = fuMaxLatency;
                    minOperations = (*bestMatchFU)->operationCount();
                    bestFU = (*bestMatchFU);
                } else if (minLatency == -1) {
                    minLatency = fuMaxLatency;
                    minOperations = (*bestMatchFU)->operationCount();
                    bestFU = (*bestMatchFU);
                } else if (fuMaxLatency == minLatency) {
                    int fuOperations = (*bestMatchFU)->operationCount();
                    if (fuOperations < minOperations) {
                        minOperations = fuOperations;
                        bestFU = (*bestMatchFU);
                    } else if (minOperations == -1) {
                        minOperations = fuOperations;
                        bestFU = (*bestMatchFU);
                    } else if (fuOperations == minOperations) {
                        int bestFUPortWidth = 0;
                        for (int i = 0;
                             i < (*bestMatchFU)->operationPortCount();
                             i++) {

                            FUPort* port = (*bestMatchFU)->operationPort(i);
                            if (port->isTriggering()) {
                                if (bestFUPortWidth < port->width()) {
                                    bestFUPortWidth = port->width();
                                }
                            }
                        }
                        if (maxPortWidth < bestFUPortWidth) {
                            maxPortWidth = bestFUPortWidth;
                            bestFU = (*bestMatchFU);
                        }
                    }
                }
            }
            for (int oper = 0; oper < bestFU->operationCount(); oper++) {
                operations.erase(
                    StringTools::stringToLower(
                        bestFU->operation(oper)->name()));
            }
            results.push_back(bestFU);
            
            // remove the best suitable fu from the set of function untis.
            list<pair<FunctionUnit*, int> >::iterator fuIntIter =
                functionUnits.begin();
            for (; fuIntIter != functionUnits.end(); fuIntIter++) {
                if ((*fuIntIter).first == bestFU) {
                    functionUnits.erase(fuIntIter);
                    break;
                }
            }
        } else {
            break;
        }
    }
    if (operations.size() != 0) {
        results.clear();
    }
    return results;
}

/**
 * Finds set of possible register file implementations.
 *
 * Finds out which register file implementations fulfill the frequency and
 * area requirements. The implementations are selected by estimating costs
 * of different register file implementations that match the given register file
 * architecture. All implementations that fulfill the given frequency and area
 * requirements are returned including the estimated cost data of those
 * implementations.
 *
 * @param rf Register file architecture.
 * @param guarded Flag indicating if the register file is guarded, defaults 
 * to false.
 * @param frequencyMHz Target frequency (MHz) of the register file.
 * @param maxArea Maximum area (in gates) of the register file.
 * @return Set of RF implementations with cost evaluation data. Returns an
 * empty set if none found.
 */
map<const IDF::RFImplementationLocation*, CostEstimates*>
ComponentImplementationSelector::rfImplementations(
    const TTAMachine::RegisterFile& rf, bool guarded, double frequencyMHz,
    double maxArea) {

    // Get all rf entries matching the given architecture from all available
    // HDBs and create implementation locations of them.
    set<const IDF::RFImplementationLocation*> rfImplementations;
    for (set<string>::const_iterator i = usedHDBs_.begin();
         i != usedHDBs_.end(); i++) {
        
        HDBManager& hdb = HDBRegistry::instance().hdb(*i);
        int readPorts = 0;
        int writePorts = 0;
        int bidirPorts = 0;
        for (int p = 0; p < rf.portCount(); p++) {
            const TTAMachine::RFPort* port = rf.port(p);
            if (port->inputSocket() != NULL && port->outputSocket() != NULL) {
                bidirPorts++;
            } else if (port->inputSocket() != NULL) {
                writePorts++;
            } else if (port->outputSocket() != NULL) {
                readPorts++;
            }
        }
        set<RowID> rfEntryIDs =
            hdb.rfEntriesByArchitecture(
                readPorts,
                writePorts,
                bidirPorts,
                rf.maxReads(),
                rf.maxWrites(),
                // latency always 1
                1,
                // guard support
                guarded,
                // guard latency
                rf.guardLatency(),
                rf.width(),
                rf.numberOfRegisters());

        set<RowID>::const_iterator id = rfEntryIDs.begin();
        for (; id != rfEntryIDs.end(); id++) {
            IDF::RFImplementationLocation* rfIDF = 
                new IDF::RFImplementationLocation(
                    hdb.fileName(), *id, rf.name());
            IDF::MachineImplementation* machIDF =
                new IDF::MachineImplementation();
            machIDF->addRFImplementation(rfIDF);
            rfImplementations.insert(rfIDF);
        }
    }
    map<const IDF::RFImplementationLocation*, CostEstimates*> results;
    set<const IDF::RFImplementationLocation*>::const_iterator iter = 
        rfImplementations.begin();

    // in case we are not limiting the costs, return all matching
    // implementations
    if (static_cast<int>(frequencyMHz) <= 0 && 
            static_cast<int>(maxArea) <= 0) {

        for (; iter != rfImplementations.end(); iter++) {
            results.insert(
                pair<const IDF::RFImplementationLocation*, CostEstimates*>(
                    (*iter), NULL));
        }
        return results;
    }

    for (; iter != rfImplementations.end(); iter++) {
        try {
            double area = estimator_.registerFileArea(rf, *(*iter));
            if (area > maxArea && static_cast<int>(maxArea) > 0) {
                // RF area too large
                rfImplementations.erase(iter);
                continue;
            }
            double delayInNanoSeconds = 
                estimator_.registerFileMaximumComputationDelay(rf, *(*iter));

            // 1000/ns = MHz
            if ((1000/delayInNanoSeconds) < frequencyMHz) {
                // RF too slow
                rfImplementations.erase(iter);
                continue;
            }
            CostEstimates* estimates = new CostEstimates();
            estimates->setArea(area);
            estimates->setLongestPathDelay(delayInNanoSeconds);
            results.insert(
                pair<const IDF::RFImplementationLocation*, CostEstimates*>(
                    (*iter), estimates));
        } catch (CannotEstimateCost& e) {
            // Couldn't estimate the rf.
            rfImplementations.erase(iter);
            continue;
        }
    }
    return results;
}


/**
 * Finds set of possible immediate unit implementations.
 *
 * Finds out which immediate unit implementations fulfill the frequency and
 * area requirements. The implementations are selected by estimating costs
 * of different immediate unit implementations that match the given immediate
 * unit architecture. All implementations that fulfill the given frequency
 * and area requirements are returned including the estimated cost data of
 * those implementations.
 *
 * @param iu Immediate unit architecture.
 * @param frequencyMHz Target frequency (MHz) of the immediate unit.
 * @param maxArea Maximum area (in gates) of the immediate unit.
 * @return Set of IU implementations with cost evaluation data. Returns an
 * empty set if none found.
 */
map<const IDF::IUImplementationLocation*, CostEstimates*>
ComponentImplementationSelector::iuImplementations(
    const TTAMachine::ImmediateUnit& iu, double frequencyMHz, double maxArea) {

    // Get all iu entries matching the given architecture from all available
    // HDBs and create implementation locations of them.
    set<const IDF::IUImplementationLocation*> iuImplementations;
    for (set<string>::const_iterator i = usedHDBs_.begin();
         i != usedHDBs_.end(); i++) {
        
        HDBManager& hdb = HDBRegistry::instance().hdb(*i);
        int readPorts = iu.portCount();

        set<RowID> iuEntryIDs =
            hdb.rfEntriesByArchitecture(
                readPorts,
                1, // writePorts in iu is always 1, not visible in adf
                0, // bidirPorts
                0, // maxReads
                0, // maxWrites
                1, // latency always 1
                false, // guard support always false
                0, // guard latency
                iu.width(),
                iu.numberOfRegisters());

        set<RowID>::const_iterator id = iuEntryIDs.begin();
        for (; id != iuEntryIDs.end(); id++) {
            IDF::IUImplementationLocation* iuIDF = 
                new IDF::IUImplementationLocation(
                    hdb.fileName(), *id, iu.name());
            IDF::MachineImplementation* machIDF =
                new IDF::MachineImplementation();
            machIDF->addIUImplementation(iuIDF);
            iuImplementations.insert(iuIDF);
        }
    }
    map<const IDF::IUImplementationLocation*, CostEstimates*> results;
    set<const IDF::IUImplementationLocation*>::const_iterator iter = 
        iuImplementations.begin();

    // in case we are not limiting the costs, return all matching
    // implementations
    if (static_cast<int>(frequencyMHz) <= 0 && 
            static_cast<int>(maxArea) <= 0) {

        for (; iter != iuImplementations.end(); iter++) {
            results.insert(
                pair<const IDF::IUImplementationLocation*, CostEstimates*>(
                    (*iter), NULL));
        }
        return results;
    }

    for (; iter != iuImplementations.end(); iter++) {
        try {
            double area = estimator_.registerFileArea(iu, *(*iter));
            if (area > maxArea && static_cast<int>(maxArea) > 0) {
                // IU area too large
                iuImplementations.erase(iter);
                continue;
            }
            double delayInNanoSeconds = 
                estimator_.registerFileMaximumComputationDelay(iu, *(*iter));

            // 1000/ns = MHz
            if ((1000/delayInNanoSeconds) < frequencyMHz) {
                // RF too slow
                iuImplementations.erase(iter);
                continue;
            }
            CostEstimates* estimates = new CostEstimates();
            estimates->setArea(area);
            estimates->setLongestPathDelay(delayInNanoSeconds);
            results.insert(
                pair<const IDF::IUImplementationLocation*, CostEstimates*>(
                    (*iter), estimates));
        } catch (CannotEstimateCost& e) {
            // Couldn't estimate the iu.
            iuImplementations.erase(iter);
            continue;
        }
    }
    return results;
}


/**
 * Selects the implementations for machine configuration.
 *
 * Stores created idf to the given configuration and the configuration hold
 * information in the given dsdb. If no machine is given then read the machine
 * from dsdb through given configuration.
 *
 * @param conf Machine configuration.
 * @param dsdb DSDB where implementation is added.
 * @param mach Machine for what the implementation is generated.
 * @param icDecoder The name of the ic decoder plugin for idf.
 * @param icDecoderHDB The name of the hdb used by ic decoder plugin.
 * @param frequency The minimum frequency of the implementations.
 * @param maxArea The maximum area of the implementations.
 */
void 
ComponentImplementationSelector::selectComponentsToConf(
    DSDBManager::MachineConfiguration& conf, 
    DSDBManager& dsdb, 
    TTAMachine::Machine* mach,
    const std::string& icDecoder, 
    const std::string& icDecoderHDB,
    const double& frequency,
    const double& maxArea) throw(Exception) {

    if (mach == NULL) {
        try {
            mach = dsdb.architecture(conf.architectureID);
        } catch (const Exception& e) {
            Exception error(__FILE__, __LINE__, __func__, 
                    e.errorMessage());
            error.setCause(e);
            throw error;
        }
    }
        
    IDF::MachineImplementation* idf = NULL;
    try {
        // building the idf
        idf = selectComponents(
                mach, icDecoder, icDecoderHDB, frequency, maxArea);
    } catch (const Exception& e) {
        Exception error(__FILE__, __LINE__, __func__, 
                e.errorMessage());
        error.setCause(e);
        conf.hasImplementation = false;
        throw error;
    }

    conf.implementationID = dsdb.addImplementation(*idf, 0, 0);
    conf.hasImplementation = true;
}


/**
 * Selects the implementations for the machine configuration.
 *
 * @param configuration MachineConfiguration of which architecture is used.
 * @param frequency The minimum frequency of the implementations.
 * @param icDecoder The name of the ic decoder plugin for idf.
 * @param icDecoderHDB The name of the hdb used by ic decoder plugin.
 * @return RowID of the new machine configuration having adf and idf.
 * @exception Exception No suitable implementations found.
 */
IDF::MachineImplementation*
ComponentImplementationSelector::selectComponents(
    const TTAMachine::Machine* mach,
    const std::string& icDecoder, 
    const std::string& icDecoderHDB,
    const double& frequency,
    const double& maxArea) throw(Exception) {


    HDBRegistry& hdbRegistry = HDBRegistry::instance();
    for (int i = 0; i < hdbRegistry.hdbCount(); i++) {
        addHDB(hdbRegistry.hdb(i));
    }

    IDF::MachineImplementation* idf = new IDF::MachineImplementation;

    // select implementations for funtion units
    selectFUs(mach, idf, frequency, maxArea);

    // select implementations for register files
    selectRFs(mach, idf);

    // select implementations for immediate units
    selectIUs(mach, idf, frequency, maxArea);

    // add the ic decoder plugin
    std::vector<std::string> icDecPaths =
        Environment::icDecoderPluginPaths();
    idf->setICDecoderPluginName(icDecoder);
    idf->setICDecoderHDB(icDecoderHDB);
    std::vector<std::string>::const_iterator iter = icDecPaths.begin();
    for (; iter != icDecPaths.end(); iter++) {
        std::string path = *iter;
        std::string file = 
            path + FileSystem::DIRECTORY_SEPARATOR + icDecoder + "Plugin.so";
        if (FileSystem::fileExists(file)) {
            idf->setICDecoderPluginFile(file);
            break;
        }
    }

    return idf;
}


/**
 * Selects the implementations for FUs in the machine configuration.
 *
 * frequency and maxArea to zero by default.
 * TODO: throw more appropriate exceptions
 */
void
ComponentImplementationSelector::selectFUs(
        const TTAMachine::Machine* mach,
        IDF::MachineImplementation* idf,
        const double& frequency,
        const double& maxArea,
        const bool& filterLongestPathDelay) throw (Exception) {

    Machine::FunctionUnitNavigator fuNav = mach->functionUnitNavigator();

    // select implementations for funtion units
    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit* fu = fuNav.item(i);

        map<const IDF::FUImplementationLocation*, CostEstimates*> fuMap =
            fuImplementations(*fu, frequency, maxArea);
        // Create an id ordered set of idf entries to ensure
        // deterministic behaviour       
        set<std::pair<const IDF::FUImplementationLocation*, CostEstimates*>, 
            implComp> fuSet;
        for (map<const IDF::FUImplementationLocation*, 
                 CostEstimates*>::const_iterator i = fuMap.begin();
             i != fuMap.end(); i++) {	    
            fuSet.insert(
                std::make_pair<const IDF::FUImplementationLocation*, 
                CostEstimates*>(i->first, i->second));
        }
       
        set<std::pair<const IDF::FUImplementationLocation*, CostEstimates*> >::
            const_iterator iter = fuSet.begin();
        if (fuMap.size() != 0) {
            set<std::pair<const IDF::FUImplementationLocation*, 
                CostEstimates*> >::const_iterator wanted = iter;
            if (filterLongestPathDelay && maxArea > 0 && frequency > 0) {
                double longestPathDelay = 0;
                double area = 0;
                bool first = true;
                while (iter != fuSet.end()) {
                    CostEstimates* estimate = iter->second;
                    if (estimate == NULL) {
                        std::string errorMsg = "When selecting FUs regarding"
                            " longest path delay, no cost estimates were"
                            " found for FU: " + fu->name();
                        Application::writeToErrorLog(
                            __FILE__, __LINE__, __func__, errorMsg, 1);
                        break;
                    }
                    if (first) {
                        area = estimate->area();
                        longestPathDelay = estimate->longestPathDelay();
                        wanted = iter;
                        first = false;
                    } else if (longestPathDelay < 
                               estimate->longestPathDelay()) {
                        longestPathDelay = estimate->longestPathDelay();
                        area = estimate->area();
                        wanted = iter;
                    } else if (longestPathDelay == 
                               estimate->longestPathDelay() && 
                               area < estimate->area()) {
                        area = estimate->area();
                        wanted = iter;
                    }
                    iter++;
                }
            }

            const IDF::FUImplementationLocation* fuImpl = (*wanted).first;
            ObjectState* state = fuImpl->saveState();
            IDF::FUImplementationLocation* newFUImpl =
                new IDF::FUImplementationLocation(state);

            try {
                idf->addFUImplementation(newFUImpl);
            } catch (const Exception& e) {
                Exception error(
                    __FILE__, __LINE__, __func__, 
                    e.errorMessage());
                error.setCause(e);
                throw error;
            }
        } else {
            mach->writeToADF("debug.adf");
            throw Exception(
                    __FILE__, __LINE__, __func__,
                    "no implementations found for FU: " + fu->name());
        }
    }
}


/**
 * Selects the implementations for RFs in the machine configuration.
 * 
 * Selects the the RF that has biggest longest path delay.
 *
 */
void
ComponentImplementationSelector::selectRFs(
        const TTAMachine::Machine* mach,
        IDF::MachineImplementation* idf,
        const double& frequency,
        const double& maxArea) throw (Exception) {

    Machine::RegisterFileNavigator rfNav = mach->registerFileNavigator();
    
    // selects the register that has biggest longest path delay
    for (int i = 0; i < rfNav.count(); i++) {
        RegisterFile* rf = rfNav.item(i);
        map<const IDF::RFImplementationLocation*, CostEstimates*> rfMap;

        // check if the register is boolean register.
        if (rf->isUsedAsGuard()) {
            // select from guarded registers
            rfMap = rfImplementations(*rf, true, frequency, maxArea);
        } else {
            // select from non guarded registers
            rfMap = rfImplementations(*rf, false, frequency, maxArea);
        }
	// Create an id ordered set of idf entries to ensure the deterministic behaviour
        set<std::pair<const IDF::RFImplementationLocation*, CostEstimates*>, implComp> rfSet;
	for ( map<const IDF::RFImplementationLocation*, CostEstimates*>::const_iterator i = rfMap.begin();
      	      i != rfMap.end(); i++) {
	    rfSet.insert(std::make_pair<const IDF::RFImplementationLocation*, CostEstimates*>(i->first, i->second));
        }
	set<std::pair<const IDF::RFImplementationLocation*, CostEstimates*> >::const_iterator iter = rfSet.begin();
        if (rfMap.size() != 0) {
            double longestPathDelay = 0;
	    double area = 0;
            bool first = true;
            set<std::pair<const IDF::RFImplementationLocation*,
                CostEstimates*> >::const_iterator wanted = iter;
            if (maxArea > 0 && frequency > 0) {
                while (iter != rfSet.end()) {
                    CostEstimates* estimate = iter->second;
                    if (estimate == NULL) {
                        std::string errorMsg = "When selecting RFs regarding"
                            " longest path delay, no cost estimates were"
                            " found for RF: " + rf->name();
                        Application::writeToErrorLog(
                                __FILE__, __LINE__, __func__, errorMsg, 1);
                        break;
                    }
		    if (first) {
                        area = estimate->area();
		        longestPathDelay = estimate->longestPathDelay();
		        wanted = iter;
		        first = false;
  	            } else if (longestPathDelay < estimate->longestPathDelay()) {
			longestPathDelay = estimate->longestPathDelay();
		        area = estimate->area();
		        wanted = iter;
		    } else if (longestPathDelay == estimate->longestPathDelay() && area < estimate->area()) {
			area = estimate->area();
		        wanted = iter;
		    }
		   
                    iter++;
                }
            }
            const IDF::RFImplementationLocation* rfImpl = (*wanted).first;
            ObjectState* state = rfImpl->saveState();
            IDF::RFImplementationLocation* newRFImpl = 
                new IDF::RFImplementationLocation(state);
            try {
                idf->addRFImplementation(newRFImpl);
            } catch (const Exception& e) {
                Exception error(__FILE__, __LINE__, __func__, 
                        e.errorMessage());
                error.setCause(e);
                throw error;
            }
        } else {
            throw Exception(
                    __FILE__, __LINE__, __func__,
                    "no implementations found for RF: " + rf->name());
        }
    }
}


/**
 * Selects the implementations for IUs in the machine configuration.
 *
 */
void
ComponentImplementationSelector::selectIUs(
        const TTAMachine::Machine* mach,
        IDF::MachineImplementation* idf,
        const double& frequency,
        const double& maxArea) throw (Exception) {

    Machine::ImmediateUnitNavigator iuNav = mach->immediateUnitNavigator();
    
    // select implementations for immediate units
    for (int index = 0; index < iuNav.count(); index++) {
        TTAMachine::ImmediateUnit* iu = iuNav.item(index);

        map<const IDF::IUImplementationLocation*, CostEstimates*> iuMap =
            iuImplementations(*iu, frequency, maxArea);

	// Create an id ordered set of idf entries to ensure the deterministic behaviour
	set<std::pair<const IDF::IUImplementationLocation*, CostEstimates*>, implComp> iuSet;
	for ( map<const IDF::IUImplementationLocation*, CostEstimates*>::const_iterator i = iuMap.begin();
	      i != iuMap.end(); i++) {
	    iuSet.insert(std::make_pair<const IDF::IUImplementationLocation*, CostEstimates*>(i->first, i->second));
        }
	set<std::pair<const IDF::IUImplementationLocation*, CostEstimates*> >::const_iterator iter = iuSet.begin();
        if (iuMap.size() != 0) {
            double longestPathDelay = 0;
            double area = 0;
            bool first = true;
            //map<const IDF::RFImplementationLocation*,
            //    CostEstimates*>::const_iterator wanted = iter;
            set<std::pair<const IDF::RFImplementationLocation*, CostEstimates*> >::const_iterator wanted = iter;

            while (iter != iuSet.end()) {
                CostEstimates* estimate = iter->second;
                if (estimate == NULL) {
                    std::string errorMsg = "When selecting IUs regarding"
                        " longest path delay, no cost estimates were"
                        " found for IU: " + iu->name();
                    Application::writeToErrorLog(
                            __FILE__, __LINE__, __func__, errorMsg, 1);
                    break;
                }
   	        if (first) {
                    area = estimate->area();
                    longestPathDelay = estimate->longestPathDelay();
                    wanted = iter;
                    first = false;
                } else if (longestPathDelay < estimate->longestPathDelay()) {
                    longestPathDelay = estimate->longestPathDelay();
                    area = estimate->area();
                    wanted = iter;
                } else if (longestPathDelay == estimate->longestPathDelay() && area < estimate->area()) {
                    area = estimate->area();
                    wanted = iter;
                }
                iter++;
            }

            const IDF::IUImplementationLocation* iuImpl = (*wanted).first;
            ObjectState* state = iuImpl->saveState();
            IDF::IUImplementationLocation* newIUImpl = 
                new IDF::IUImplementationLocation(state);
            try {
                idf->addIUImplementation(newIUImpl);
            } catch (const Exception& e) {
                Exception error(__FILE__, __LINE__, __func__, 
                        e.errorMessage());
                error.setCause(e);
                throw error;
            }
        } else {
            throw Exception(
                    __FILE__, __LINE__, __func__,
                    "no implementations found for IU: " + iu->name());
        }
    }
}

