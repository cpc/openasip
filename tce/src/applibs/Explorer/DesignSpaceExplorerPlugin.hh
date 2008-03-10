/**
 * @file DesignSpaceExplorerPlugin.hh
 *
 * Declaration of DesignSpaceExplorerPlugin class.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_DESIGN_SPACE_EXPLORER_PLUGIN_HH
#define TTA_DESIGN_SPACE_EXPLORER_PLUGIN_HH

#include <string>
#include <vector>
#include "DesignSpaceExplorer.hh"
#include "Machine.hh"

/**
 * A base interface for all classes that implement pluggable
 * design space explorer algorithms.
 *
 * Plugins are allowed to store state data to itself, thus, the plugin
 * instances are not allowed to be "recycled" for multiple uses, but a
 * new instance should be created always.
 */
class DesignSpaceExplorerPlugin : public DesignSpaceExplorer {
public:

    /// Parameter struct.
    struct Parameter {
        std::string name; ///< Name of the parameter.
        std::string value; ///< Value of the parameter.
    };

    /// Table for passing plugin parameters.
    typedef std::vector<Parameter> ParameterTable;

    virtual ~DesignSpaceExplorerPlugin();
    
    void setVerboseStream(std::ostream* verboseOut);
    void setErrorStream(std::ostream* errorOut);
    virtual void setParameters(ParameterTable parameters);
    virtual void setPluginName(const std::string& pluginName);

    virtual std::vector<RowID>
    explore(
        const RowID& startPointConfigurationID,
        const unsigned int& maxIter = 0);

    virtual bool hasParameter(const std::string& paramName) const;
    virtual std::string parameterValue(const std::string& paramName) const
        throw (NotAvailable);
    virtual bool booleanValue(const std::string& parameter) const
        throw (IllegalParameters);
    void verboseOuput(const std::string& message);
    void errorOuput(const std::string& message);

protected:
    DesignSpaceExplorerPlugin();

    /// the name of the explorer plugin
    std::string pluginName_;
    /// Parameters for the plugin.
    ParameterTable parameters_;
    /// plugin verbose output stream
    std::ostream* verboseOut_;
    /// plugin error output stream
    std::ostream* errorOut_;
};

/**
 * Exports the given class as a DesignSpaceExplorer plugin.
 */
#define EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(PLUGIN_NAME__)   \
extern "C" { \
    DesignSpaceExplorerPlugin* create_explorer_plugin_##PLUGIN_NAME__(\
        DSDBManager& dsdb) { \
        PLUGIN_NAME__* instance = new PLUGIN_NAME__(); \
        instance->setDSDB(dsdb);\
        instance->setPluginName(#PLUGIN_NAME__);\
        return instance;\
    }\
    void delete_explorer_plugin_##PLUGIN_NAME__(\
        DesignSpaceExplorerPlugin* target) {       \
        delete target;\
    }\
}

/**
 * Defines the description string for the plugin.
 *
 * Can be used only inside a plugin class definition.
 *
 * @param TEXT__ The description string. 
 */
#define DESCRIPTION(TEXT__) \
public:\
    virtual std::string description() const { return TEXT__; }\
    int* XXXXXd_escrip__tion__

#endif
