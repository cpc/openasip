/**
 * @file CostEstimationPluginRegistry.hh
 *
 * Declaration of CostEstimationPluginRegistry class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_COST_ESTIMATION_PLUGIN_REGISTRY_HH
#define TTA_COST_ESTIMATION_PLUGIN_REGISTRY_HH

#include <map>
#include <string>

#include "Exception.hh"
#include "PluginTools.hh"

namespace CostEstimator {

/**
 * Templated helper class for storing, accessing and caching of different 
 * types of cost estimation plugins.
 *
 * The template parameter T is the type of the plugin to store.
 */
template <typename T>
class CostEstimationPluginRegistry {
public:
    CostEstimationPluginRegistry();
    virtual ~CostEstimationPluginRegistry();

    T& plugin(
        const std::string& pluginFileName,
        const std::string& pluginName)
        throw (IOException, FileNotFound, DynamicLibraryException, 
               WrongSubclass);
private:
    /// type of the index of accessible plugins, maps the plugin file path 
    /// to the plugin
    typedef std::map<std::string, T*> PluginRegistry;
    /// index of accessible plugins, maps the plugin file path 
    /// to the plugin    
    PluginRegistry registry_;
    /// the plugin tools instance used to access the plugin files
    PluginTools pluginTools_;
};

}

#include "CostEstimationPluginRegistry.icc"

#endif
