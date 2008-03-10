/**
 * @file ICDecoderCostEstimationPluginRegistry.hh
 *
 * Declaration of ICDecoderCostEstimationPluginRegistry type.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ICDECODER_ESTIMATION_PLUGIN_REGISTRY_HH
#define TTA_ICDECODER_ESTIMATION_PLUGIN_REGISTRY_HH

#include "CostEstimationPluginRegistry.hh"
#include "ICDecoderEstimatorPlugin.hh"

namespace CostEstimator {

typedef CostEstimationPluginRegistry<ICDecoderEstimatorPlugin> 
ICDecoderCostEstimationPluginRegistry;

}

#endif
