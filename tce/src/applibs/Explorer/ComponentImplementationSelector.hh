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
private:
    /// HDBs from which implementations are serched are stored in this set
    std::set<std::string> usedHDBs_;
    /// Cost estimator to estimate the unit costs.
    CostEstimator::Estimator estimator_;
};

#endif
