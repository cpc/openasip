/**
 * @file RFCostEstimationPluginRegistry.hh
 *
 * Declaration of RFCostEstimationPluginRegistry type.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_RF_COST_ESTIMATION_PLUGIN_REGISTRY_HH
#define TTA_RF_COST_ESTIMATION_PLUGIN_REGISTRY_HH

#include "CostEstimationPluginRegistry.hh"
#include "RFCostEstimationPlugin.hh"

namespace CostEstimator {

typedef CostEstimationPluginRegistry<RFCostEstimationPlugin> 
RFCostEstimationPluginRegistry;

}

#endif
