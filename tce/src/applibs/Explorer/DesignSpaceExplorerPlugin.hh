/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
#include "Machine.hh"

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
