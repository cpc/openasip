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
 * @file ComponentImplementationSelector.hh
 *
 * Declaration of ComponentImplementationSelector class
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_COMPONENT_IMPLEMENTATION_SELECTOR_HH
#define TTA_COMPONENT_IMPLEMENTATION_SELECTOR_HH

#include <set>
#include <map>
#include <list>
#include <string>
#include "Application.hh"
#include "HDBRegistry.hh"
#include "HDBManager.hh"
#include "Estimator.hh"
#include "DSDBManager.hh"

namespace TTAMachine {
    class FunctionUnit;
    class RegisterFile;
    class ImmediateUnit;
}
namespace TTAProgram {
    class Program;
}
namespace IDF {
    class UnitImplementationLocation;
    typedef UnitImplementationLocation FUImplementationLocation;
    typedef UnitImplementationLocation RFImplementationLocation;
    typedef UnitImplementationLocation IUImplementationLocation;
}

class Operation;
class ExecutionTrace;
class CostEstimates;

/**
 * Purpose of the component implementation selector class is to select suitable
 * implementations to architecture components. The component implementation
 * selector uses HDB to look for implementations and returns set of suitable
 * implementations that fulfill the given cost requirements.
 * Class uses cost estimator to estimate the costs of suitable implementations.
 * Cost estimation data includes energy estimations of all program cases that
 * are set.
 */
class ComponentImplementationSelector {
public:
    ComponentImplementationSelector();
    virtual ~ComponentImplementationSelector();

    void addHDB(const HDB::HDBManager& hdb)
        throw(Exception);
    void addCase(
        const TTAProgram::Program& program, const ExecutionTrace& traceDB);

    std::map<const IDF::FUImplementationLocation*, CostEstimates*>
    fuImplementations(
        const TTAMachine::FunctionUnit& fu, double frequencyMHz = 0,
        double maxArea = 0);
    std::list<TTAMachine::FunctionUnit*>
    fuArchsByOpSetWithMinLatency(
        const std::set<std::string>& operationSet, int width = 0) const;

    std::map<const IDF::RFImplementationLocation*, CostEstimates*>
    rfImplementations(
        const TTAMachine::RegisterFile& rf, bool guarded = false,
        double frequencyMHz = 0, double maxArea = 0);

    std::map<const IDF::IUImplementationLocation*, CostEstimates*>
    iuImplementations(
        const TTAMachine::ImmediateUnit& iu, double frequencyMHz = 0,
        double maxArea = 0);

    void selectComponentsToConf(
        DSDBManager::MachineConfiguration& conf, 
        DSDBManager& dsdb, 
        TTAMachine::Machine* mach = NULL,
        const std::string& icDecoder = "ic_hdb",
        const std::string& icDecoderHDB = "asic_130nm_1.5V.hdb", 
        const double& frequency = 0,
        const double& maxArea = 0) throw(Exception);

    IDF::MachineImplementation* selectComponents(
        const TTAMachine::Machine* mach,
        const std::string& icDecoder = "ic_hdb",
        const std::string& icDecoderHDB = "asic_130nm_1.5V.hdb", 
        const double& frequency = 0,
        const double& maxArea = 0) throw(Exception);

    void selectFUs(
        const TTAMachine::Machine* mach,
        IDF::MachineImplementation* idf,
        const double& frequency = 0,
        const double& maxArea = 0,
        const bool& filterLongestPathDelay = true) throw (Exception);

    void selectRFs(
        const TTAMachine::Machine* mach,
        IDF::MachineImplementation* idf,
        const double& frequency = 0,
        const double& maxArea = 0) throw (Exception);
    
    void selectIUs(
        const TTAMachine::Machine* mach,
        IDF::MachineImplementation* idf,
        const double& frequency = 0,
        const double& maxArea = 0) throw (Exception);

private:
    /// HDBs from which implementations are serched are stored in this set
    std::set<std::string> usedHDBs_;
    /// Cost estimator to estimate the unit costs.
    CostEstimator::Estimator estimator_;
};

#endif
