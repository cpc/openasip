/**
 * @file CostEstimationPlugin.hh
 *
 * Declaration of CostEstimationPlugin class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_COST_ESTIMATION_PLUGIN_HH
#define TTA_COST_ESTIMATION_PLUGIN_HH

#include <string>

#include "CostEstimatorTypes.hh"

namespace CostEstimator {
/**
 * A base interface for all classes that implement pluggable cost estimation
 * algorithms.
 */
class CostEstimationPlugin {
public:
    CostEstimationPlugin(const std::string& name);
    virtual ~CostEstimationPlugin();

    virtual std::string name() const;
    virtual std::string description() const;
    
protected:
    /// the name of the plugin class in the HDB; used to identify cost data
    std::string name_;
};

}

#endif

