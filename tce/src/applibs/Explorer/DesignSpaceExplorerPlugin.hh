/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file DesignSpaceExplorerPlugin.hh
 *
 * Declaration of DesignSpaceExplorerPlugin class.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_DESIGN_SPACE_EXPLORER_PLUGIN_HH
#define TTA_DESIGN_SPACE_EXPLORER_PLUGIN_HH

#include <string>
#include <map>

#include "DesignSpaceExplorer.hh"
#include "ExplorerPluginParameter.hh"

#include "Exception.hh"
#include "Conversion.hh"
#include "Application.hh"



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

    virtual ~DesignSpaceExplorerPlugin();
    
    virtual bool requiresStartingPointArchitecture() const = 0;
    virtual bool producesArchitecture() const = 0;
    virtual bool requiresHDB() const = 0;
    virtual bool requiresSimulationData() const = 0;
    virtual bool requiresApplication() const { return true; }

    typedef std::pair<std::string, 
            ExplorerPluginParameter> Parameter;
    typedef std::map<std::string, 
            ExplorerPluginParameter> ParameterMap;
    typedef std::map<std::string, 
            ExplorerPluginParameter>::iterator PMIt;
    typedef std::map<std::string, 
            ExplorerPluginParameter>::const_iterator PMCIt;

    virtual void giveParameter(
        const std::string& name, 
        const std::string& value) 
        throw (InstanceNotFound);
    
    virtual inline std::string description() const;
    virtual std::string name() const;
    virtual void setPluginName(const std::string& pluginName);

    virtual std::vector<RowID> explore(
        const RowID& startPointConfigurationID,
        const unsigned int& maxIter = 0);

    virtual bool hasParameter(const std::string& paramName) const;

    ParameterMap parameters() const;
    
    inline void addParameter(
        std::string name, 
        ExplorerPluginParameterType type, 
        bool compulsory = true, 
        std::string defaultValue = "");

    template <typename T>
    void readCompulsoryParameter(const std::string paramName, T& param) const
        throw (NotAvailable, IllegalParameters);

    template <typename T>
    void readOptionalParameter(const std::string paramName, T& param) const
        throw (NotAvailable, IllegalParameters);

    template <typename RT>
    RT parameterValue(const std::string& paramName) const
        throw (NotAvailable, IllegalParameters);

    virtual bool booleanValue(const std::string& parameter) const
        throw (IllegalParameters);

protected:
    DesignSpaceExplorerPlugin();

    void checkParameters() const throw(IllegalParameters);

    /// the name of the explorer plugin
    std::string pluginName_;
    /// Parameters for the plugin.
    ParameterMap parameters_;
};

#include "DesignSpaceExplorerPlugin.icc"


/**
 * Exports the given class as a DesignSpaceExplorer plugin.
 */
#define EXPORT_DESIGN_SPACE_EXPLORER_PLUGIN(PLUGIN_NAME__)   \
extern "C" { \
    DesignSpaceExplorerPlugin* create_explorer_plugin_##PLUGIN_NAME__() { \
        PLUGIN_NAME__* instance = new PLUGIN_NAME__(); \
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
#define PLUGIN_DESCRIPTION(TEXT__) \
public:\
    virtual std::string description() const { return TEXT__; }

#endif
