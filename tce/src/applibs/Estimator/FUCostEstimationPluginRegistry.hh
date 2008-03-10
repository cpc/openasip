/**
 * @file FUCostEstimationPluginRegistry.hh
 *
 * Declaration of FUCostEstimationPluginRegistry type.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_COST_ESTIMATION_PLUGIN_REGISTRY_HH
#define TTA_FU_COST_ESTIMATION_PLUGIN_REGISTRY_HH

#include "CostEstimationPluginRegistry.hh"
#include "FUCostEstimationPlugin.hh"

namespace CostEstimator {

typedef CostEstimationPluginRegistry<FUCostEstimationPlugin> 
FUCostEstimationPluginRegistry;

}
#endif
