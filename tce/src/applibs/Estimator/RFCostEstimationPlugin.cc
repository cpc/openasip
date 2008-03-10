/**
 * @file RFCostEstimationPlugin.cc
 *
 * Implementation of RFCostEstimationPlugin class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 *
 * @note rating: red
 */

#include "RFCostEstimationPlugin.hh"

namespace CostEstimator {

/**
 * Constructor.
 *
 * @param name The name of the created plugin in HDB.
 * @param data The HDB the plugin's data is stored in.
 */
RFCostEstimationPlugin::RFCostEstimationPlugin(
    const std::string& name) :
    CostEstimationPlugin(name) {
}

/**
 * Destructor.
 */
RFCostEstimationPlugin::~RFCostEstimationPlugin() {
}

/**
 * Estimates the area of the given RF.
 *
 * @param architecture The RF architecture of which area to estimate.
 * @param implementation (The location of) the implementation of RF.
 * @param area The variable to store the area estimate into.
 * @return True only if area could be estimated successfully.
 */
bool
RFCostEstimationPlugin::estimateArea(
    const TTAMachine::BaseRegisterFile&,
    const IDF::RFImplementationLocation&,
    AreaInGates&,
    HDB::HDBManager&) {

    return false;
}

/**
 * Estimates the energy consumed by the given RF when running the given
 * program.
 *
 * @param architecture The RF architecture of which area to estimate.
 * @param implementation (The location of) the implementation of RF. Can be
 *                        an instance of NullFUImplementationLocation.
 * @param program The program of which energy to calculate.
 * @param traceDB The simulation trace database of the program running in the
 *                target architecture which can be used in the estimation 
 *                algorithm.
 * @param energy The variable to store the energy estimate into.
 * @return True only if energy could be estimated successfully.
 */
bool
RFCostEstimationPlugin::estimateEnergy(
    const TTAMachine::BaseRegisterFile&,
    const IDF::RFImplementationLocation&,
    const TTAProgram::Program&,
    const ExecutionTrace&,
    EnergyInMilliJoules&,
    HDB::HDBManager&) {

    return false;
}

/**
 * Estimates the input delay of the given RF port.
 *
 * @param port The RF port of which input delay to estimate.
 * @param implementation (The location of) the implementation of RF. 
 * @param inputDelay The variable to store the delay estimate into.
 * @return True only if input delay could be estimated successfully.
 */
bool
RFCostEstimationPlugin::estimatePortWriteDelay(
    const TTAMachine::RFPort&,
    const IDF::RFImplementationLocation&,
    DelayInNanoSeconds&,
    HDB::HDBManager&) {

    return false;
}

/**
 * Estimates the output delay of the given RF port.
 *
 * @param port The RF port of which output delay to estimate.
 * @param implementation (The location of) the implementation of RF. 
 * @param outputDelay The variable to store the delay estimate into.
 * @return True only if output delay could be estimated successfully.
 */
bool
RFCostEstimationPlugin::estimatePortReadDelay(
    const TTAMachine::RFPort&,
    const IDF::RFImplementationLocation&,
    DelayInNanoSeconds&,
    HDB::HDBManager&) {

    return false;
}

/**
 * Estimates the maximum "computation delay" of the given RF.
 *
 * The maximum computation delay is the longest stage in the RF's
 * execution. It's used mainly for calculating the maximum clock frequency
 * of target architecture.
 *
 * @param architecture The RF architecture of which area to estimate.
 * @param implementation (The location of) the implementation of RF. Can be
 *                        an instance of NullRFImplementationLocation.
 * @param computationDelay The variable to store the delay estimate into.
 * @return True only if computation delay could be estimated successfully.
 */
bool
RFCostEstimationPlugin::estimateMaximumComputationDelay(
    const TTAMachine::BaseRegisterFile&,
    const IDF::RFImplementationLocation&,
    DelayInNanoSeconds&,
    HDB::HDBManager&) {

    return false;
}

}
