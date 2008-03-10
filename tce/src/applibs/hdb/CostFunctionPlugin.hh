/**
 * @file CostFunctionPlugin.hh
 *
 * Declaration of CostFunctionPlugin class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_COST_FUNCTION_PLUGIN_HH
#define TTA_COST_FUNCTION_PLUGIN_HH

#include <string>

namespace HDB {

/**
 * Class that contains data stored in HDB of a cost function plugin.
 */
class CostFunctionPlugin {
public:
    /// all supported cost function plugin types
    enum CostFunctionPluginType {
        COST_FU,     ///< function unit cost estimator
        COST_RF,     ///< register file cost estimator
        COST_DECOMP, ///< decompressor cost estimator
        COST_ICDEC   ///< interconnection network & decoder cost estimator
    };

    CostFunctionPlugin(
        int id,
        const std::string& description,
        const std::string& name,
        const std::string& pluginFilePath,
        CostFunctionPluginType type);
    virtual ~CostFunctionPlugin();

    int id() const;
    std::string description() const;
    std::string name() const;
    std::string pluginFilePath() const;
    CostFunctionPluginType type() const;

private:
    /// id of the plugin in HDB
    int id_;
    /// description of the plugin HDB
    std::string description_;
    /// (the class) name of the plugin
    std::string name_;
    /// the path in the file system of the plugin
    std::string pluginFilePath_;
    /// the type of the plugin
    CostFunctionPluginType type_;
};

}

#endif
