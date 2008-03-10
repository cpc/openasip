/**
 * @file ICDecoderEstimatorPlugin.hh
 *
 * Declaration of ICDecoderEstimatorPlugin class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_IC_DECODER_ESTIMATOR_PLUGIN_HH
#define TTA_IC_DECODER_ESTIMATOR_PLUGIN_HH

#include "CostEstimationPlugin.hh"
#include "TransportPath.hh"
#include "SocketImplementationLocation.hh"
#include "BusImplementationLocation.hh"

namespace TTAMachine {
    class Machine;
    class ControlUnit;
}
namespace TTAProgram {
    class Program;
}

namespace HDB {
    class HDBRegistry;
}

namespace IDF {
    class MachineImplementation;
}

class ExecutionTrace;

namespace CostEstimator {
/**
 * Base class for plugins that are able to estimate costs of a IC and
 * decoder combination.
 */
class ICDecoderEstimatorPlugin : public CostEstimationPlugin {
public:

    ICDecoderEstimatorPlugin(
        const std::string& name);
    virtual ~ICDecoderEstimatorPlugin();

    virtual bool estimateICDelayOfPath(
        HDB::HDBRegistry& hdbRegistry,
        const TransportPath& path,
        const IDF::MachineImplementation& machineImplementation,
        const IDF::SocketImplementationLocation& sourceSocketImplementation,
        const IDF::BusImplementationLocation& busImplementation,
        const IDF::SocketImplementationLocation& 
        destinationSocketImplementation,
        DelayInNanoSeconds& delay);    

    virtual bool estimateICArea(
        HDB::HDBRegistry& hdbRegistry,
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation,
        AreaInGates& area);

    virtual bool estimateICEnergy(
        HDB::HDBRegistry& hdbRegistry,
        const TTAMachine::Machine& machine,
        const IDF::MachineImplementation& machineImplementation,
        const TTAProgram::Program& program,
        const ExecutionTrace& traceDB,
        EnergyInMilliJoules& energy);

    virtual TTAMachine::ControlUnit* generateControlUnit();
};

}

/**
 * Exports the given class as an IC&decoder estimator.
 *
 * @note The class name of the estimator MUST be [PLUGIN_NAME__]Estimator,
 *       thus an estimator exported with EXPORT_ICDEC_ESTIMATOR(Example)
 *       exports a class called ExampleEstimator.
 */
#define EXPORT_ICDEC_ESTIMATOR(PLUGIN_NAME__) \
extern "C" { \
    ICDecoderEstimatorPlugin*\
    create_estimator_plugin_##PLUGIN_NAME__(\
        ) { \
        PLUGIN_NAME__##Estimator* instance = \
            new PLUGIN_NAME__##Estimator(#PLUGIN_NAME__);\
        return instance;\
    }\
    void delete_estimator_plugin_##PLUGIN_NAME__(\
        ICDecoderEstimatorPlugin* target) {\
        delete target;\
    }\
}

#endif
