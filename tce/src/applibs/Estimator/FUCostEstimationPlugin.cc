/**
 * @file FUCostEstimationPlugin.cc
 *
 * Implementation of FUCostEstimationPlugin class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 *
 * @note rating: red
 */

#include "FUCostEstimationPlugin.hh"

namespace CostEstimator {

/**
 * Constructor.
 *
 * @param name The name of the created plugin in HDB.
 * @param data The HDB the plugin's data is stored in.
 */
FUCostEstimationPlugin::FUCostEstimationPlugin(
    const std::string& name) :
    CostEstimationPlugin(name) {
}

/**
 * Destructor.
 */
FUCostEstimationPlugin::~FUCostEstimationPlugin() {
}

/**
 * Estimates the area of the given function unit.
 *
 * @param architecture The FU architecture of which area to estimate.
 * @param implementation (The location of) the implementation of FU. Can be
 *                        an instance of NullFUImplementationLocation.
 * @param area The variable to store the area estimate into.
 * @return True only if area could be estimated successfully.
 */
bool
FUCostEstimationPlugin::estimateArea(
    const TTAMachine::FunctionUnit&,
    const IDF::FUImplementationLocation&,
    AreaInGates&,
    HDB::HDBManager&) {

    return false;
}

/**
 * Estimates the energy consumed by the given function unit when running 
 * the given program.
 *
 * @param architecture The FU architecture of which area to estimate.
 * @param implementation (The location of) the implementation of FU. Can be
 *                        an instance of NullFUImplementationLocation.
 * @param program The program of which energy to calculate.
 * @param traceDB The simulation trace database of the program running in the
 *                target architecture.
 * @param energy The variable to store the energy estimate into.
 * @return True only if energy could be estimated successfully.
 */
bool
FUCostEstimationPlugin::estimateEnergy(
    const TTAMachine::FunctionUnit&,
    const IDF::FUImplementationLocation&,
    const TTAProgram::Program&,
    const ExecutionTrace&,
    EnergyInMilliJoules&,
    HDB::HDBManager&) {

    return false;
}

/**
 * Estimates the input delay of the given function unit port.
 *
 * @param port The FU port of which input delay to estimate.
 * @param implementation (The location of) the implementation of FU. Can be
 *                        an instance of NullFUImplementationLocation.
 * @param inputDelay The variable to store the delay estimate into.
 * @return True only if input delay could be estimated successfully.
 */
bool
FUCostEstimationPlugin::estimatePortWriteDelay(
    const TTAMachine::FUPort&,
    const IDF::FUImplementationLocation&,
    DelayInNanoSeconds&,
    HDB::HDBManager&) {

    return false;
}

/**
 * Estimates the output delay of the given function unit port.
 *
 * @param port The FU port of which output delay to estimate.
 * @param implementation (The location of) the implementation of FU. Can be
 *                        an instance of NullFUImplementationLocation.
 * @param outputDelay The variable to store the delay estimate into.
 * @return True only if output delay could be estimated successfully.
 */
bool
FUCostEstimationPlugin::estimatePortReadDelay(
    const TTAMachine::FUPort&,
    const IDF::FUImplementationLocation&,
    DelayInNanoSeconds&,
    HDB::HDBManager&) {

    return false;
}

/**
 * Estimates the maximum computation delay of the given function unit.
 *
 * The maximum computation delay is the longest stage in the FU unit's
 * pipeline. It's used mainly for calculating the maximum clock frequency
 * of target architecture.
 *
 * @param architecture The FU architecture of which area to estimate.
 * @param implementation (The location of) the implementation of FU. Can be
 *                        an instance of NullFUImplementationLocation.
 * @param computationDelay The variable to store the delay estimate into.
 * @return True only if computation delay could be estimated successfully.
 */
bool
FUCostEstimationPlugin::estimateMaximumComputationDelay(
    const TTAMachine::FunctionUnit&,
    const IDF::FUImplementationLocation&,
    DelayInNanoSeconds&,
    HDB::HDBManager&) {

    return false;
}

}
