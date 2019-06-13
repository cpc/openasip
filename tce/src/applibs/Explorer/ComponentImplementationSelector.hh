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
 * @file ComponentImplementationSelector.hh
 *
 * Declaration of ComponentImplementationSelector class
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
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

    void addHDB(const HDB::HDBManager& hdb);
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
        DSDBManager::MachineConfiguration& conf, DSDBManager& dsdb,
        TTAMachine::Machine* mach = NULL,
        const std::string& icDecoder = "ic_hdb",
        const std::string& icDecoderHDB = "asic_130nm_1.5V.hdb",
        const double& frequency = 0, const double& maxArea = 0);

    IDF::MachineImplementation* selectComponents(
        const TTAMachine::Machine* mach,
        const std::string& icDecoder = "ic_hdb",
        const std::string& icDecoderHDB = "asic_130nm_1.5V.hdb",
        const double& frequency = 0, const double& maxArea = 0);

    void selectFUs(
        const TTAMachine::Machine* mach, IDF::MachineImplementation* idf,
        const double& frequency = 0, const double& maxArea = 0,
        const bool& filterLongestPathDelay = true);

    void selectRFs(
        const TTAMachine::Machine* mach, IDF::MachineImplementation* idf,
        const double& frequency = 0, const double& maxArea = 0);

    void selectIUs(
        const TTAMachine::Machine* mach, IDF::MachineImplementation* idf,
        const double& frequency = 0, const double& maxArea = 0);

private:
    /// HDBs from which implementations are serched are stored in this set
    std::set<std::string> usedHDBs_;
    /// Cost estimator to estimate the unit costs.
    CostEstimator::Estimator estimator_;
   
    struct implComp {
       bool operator() (const std::pair<const IDF::UnitImplementationLocation*, CostEstimates*>& p1,
			const std::pair<const IDF::UnitImplementationLocation*, CostEstimates*>& p2) const {
	     return p1.first->id() < p2.first->id();
       }	
     };

};

#endif
