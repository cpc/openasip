/**
 * @file CostEstimationPlugin.cc
 *
 * Implementation of CostEstimationPlugin class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 *
 * @note rating: red
 */

#include "CostEstimationPlugin.hh"

namespace CostEstimator {

/**
 * Constructor.
 *
 * @param name The name of the cost estimation plugin in the HDB.
 * @param dataSource The HDB that stores the cost estimation data of
 *                   the plugin. This reference can also be used to add new 
 *                   data, in case required by the algorithm.
 */
CostEstimationPlugin::CostEstimationPlugin(
    const std::string& name) :    
    name_(name) {
}

/**
 * Destructor.
 */
CostEstimationPlugin::~CostEstimationPlugin() {
}

/**
 * Returns the name of this cost estimation plugin.
 *
 * @return The name of the plugin.
 */
std::string
CostEstimationPlugin::name() const {
    return name_;
}

/**
 * Returns the description of this cost estimation plugin.
 *
 * @return The default description is empty.
 */
std::string
CostEstimationPlugin::description() const {
    return "";
}

}
