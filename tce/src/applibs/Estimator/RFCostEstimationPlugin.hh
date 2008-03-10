/**
 * @file RFCostEstimationPlugin.hh
 *
 * Declaration of RFCostEstimationPlugin class and definitions of CPP macros
 * used to define and export RF cost estimation plugins.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_COST_ESTIMATION_PLUGIN_HH
#define TTA_RF_COST_ESTIMATION_PLUGIN_HH

#include <string>

#include "Exception.hh"
#include "CostEstimatorTypes.hh"
#include "CostEstimationPlugin.hh"
#include "RFImplementationLocation.hh"

namespace HDB {
    class HDBManager;
    class FUEntry;
}

namespace TTAProgram {
    class Program;
}

namespace TTAMachine {
    class BaseRegisterFile;
    class RFPort;
}

class ExecutionTrace;
class UtilizationStats;

namespace CostEstimator {

/**
 * A base interface for all classes that implement pluggable register file
 * cost estimation algorithms.
 */
class RFCostEstimationPlugin : public CostEstimationPlugin {
public:
    RFCostEstimationPlugin(const std::string& name);
    virtual ~RFCostEstimationPlugin();

    virtual bool estimateArea(
        const TTAMachine::BaseRegisterFile& architecture,
        const IDF::RFImplementationLocation& implementation,
        AreaInGates& area,
        HDB::HDBManager& hdb);

    virtual bool estimateEnergy(
        const TTAMachine::BaseRegisterFile& architecture,
        const IDF::RFImplementationLocation& implementation,
        const TTAProgram::Program& program,
        const ExecutionTrace& traceDB,
        EnergyInMilliJoules& energy,
        HDB::HDBManager& hdb);

    virtual bool estimatePortWriteDelay(
        const TTAMachine::RFPort& port,
        const IDF::RFImplementationLocation& implementation,
        DelayInNanoSeconds& inputDelay,
        HDB::HDBManager& hdb);

    virtual bool estimatePortReadDelay(
        const TTAMachine::RFPort& port,
        const IDF::RFImplementationLocation& implementation,
        DelayInNanoSeconds& outputDelay,
        HDB::HDBManager& hdb);

    virtual bool estimateMaximumComputationDelay(
        const TTAMachine::BaseRegisterFile& architecture,
        const IDF::RFImplementationLocation& implementation,
        DelayInNanoSeconds& computationDelay,
        HDB::HDBManager& hdb);
};

/**
 * Exports the given class as a RF cost estimation plugin.
 */
#define EXPORT_RF_COST_ESTIMATOR_PLUGIN(PLUGIN_NAME__) \
extern "C" { \
    RFCostEstimationPlugin* create_estimator_plugin_##PLUGIN_NAME__(\
        ) { \
        PLUGIN_NAME__* instance = new PLUGIN_NAME__(#PLUGIN_NAME__);\
        return instance;\
    }\
    void delete_estimator_plugin_##PLUGIN_NAME__(\
        RFCostEstimationPlugin* target) {\
        delete target;\
    }\
}

}

/**
 * Defines the description string for the plugin.
 *
 * Can be used only inside a plugin class definition.
 *
 * @param TEXT__ The description string. 
 */
#define DESCRIPTION(TEXT__) \
public:\
    virtual std::string description() const { return TEXT__; }\
    int* XXXXXd_escrip__tion__

#endif
