/**
 * @file CostFunctionPlugin.cc
 *
 * Implementation of CostFunctionPlugin class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include "CostFunctionPlugin.hh"

namespace HDB {

/**
 * Constructor.
 *
 * @param pluginFilePath Path of the plugin.
 */
CostFunctionPlugin::CostFunctionPlugin(
    int id,
    const std::string& description,
    const std::string& name,
    const std::string& pluginFilePath,
    CostFunctionPluginType type) :
    id_(id), description_(description), name_(name), 
    pluginFilePath_(pluginFilePath), type_(type) {
}

/**
 * Destructor.
 */
CostFunctionPlugin::~CostFunctionPlugin() {
}

/**
 * Returns the id of the plugin.
 *
 * @return Id of the plugin.
 */
int
CostFunctionPlugin::id() const {
    return id_;
}

/**
 * Returns the description of the plugin.
 *
 * @return Description of the plugin.
 */
std::string
CostFunctionPlugin::description() const {
    return description_;
}

/**
 * Returns the (class) name of the plugin.
 *
 * @return Name of the plugin.
 */
std::string
CostFunctionPlugin::name() const {
    return name_;
}
   
/**
 * Returns the path of the plugin file.
 *
 * @return Path of the plugin.
 */
std::string
CostFunctionPlugin::pluginFilePath() const {
    return pluginFilePath_;
}

/**
 * Returns the type of the plugin.
 *
 * @return Description of the plugin.
 */
CostFunctionPlugin::CostFunctionPluginType
CostFunctionPlugin::type() const {
    return type_;
}

}
