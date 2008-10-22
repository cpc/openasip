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
 * @file DesignSpaceExplorerPlugin.cc
 *
 * Implementation of DesignSpaceExplorerPlugin class
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include <map>
#include <string>

#include "DesignSpaceExplorerPlugin.hh"
#include "ExplorerPluginParameter.hh"
#include "Exception.hh"
#include "StringTools.hh"

/**
 * The constructor of the DesignExplorerPlugin class.
 *
 * @param dsdb Design space data base to be used with the plugin.
 * @name pluginName Name of the plugin.
 */
DesignSpaceExplorerPlugin::DesignSpaceExplorerPlugin():
    DesignSpaceExplorer(), verboseOut_(NULL), errorOut_(NULL) {
}


/**
 * The destructor.
 */
DesignSpaceExplorerPlugin::~DesignSpaceExplorerPlugin() {
}


/**
 * Sets the plugin name.
 *
 * @param pluginName Name of the plugin.
 */
void
DesignSpaceExplorerPlugin::setPluginName(const std::string& pluginName) {
    pluginName_ = pluginName;
}


/**
 * Returns the plugin name.
 *
 * @return The plugin name as a string.
 */
std::string 
DesignSpaceExplorerPlugin::name() const {
    return pluginName_;
}


/**
 * Gives a plugin parameter value.
 *
 * @param name The parameter name.
 * @param value The value to be set for the parameter.
 */
void
DesignSpaceExplorerPlugin::giveParameter(
    const std::string& name, 
    const std::string& value) throw (InstanceNotFound) {

    PMIt it = parameters_.find(name);
    if (it == parameters_.end()) {
        std::string msg = "Plugin has no parameter named: " + name;
        throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
    }
    it->second.setValue(value);
}


/**
 * Sets the plugin verbose out stream.
 *
 * @param verboseOut Verbose output stream.
 */
void 
DesignSpaceExplorerPlugin::setVerboseStream(std::ostream* verboseOut) {
    verboseOut_ = verboseOut;
}


/**
 * Sets the plugin error out stream.
 *
 * @param errorOut Error output output stream.
 */
void 
DesignSpaceExplorerPlugin::setErrorStream(std::ostream* errorOut) {
    errorOut_ = errorOut;
}


/**
 * Tells whether the plugin has the given parameter defined.
 *
 * @param paramName Name of the parameter.
 * @return True if the given parameter is defined, otherwise false.
 */
bool
DesignSpaceExplorerPlugin::hasParameter(const std::string& paramName) const {
    return parameters_.find(paramName)->second.isSet();
}


/**
 * Makes a copy of the parameters of the plugin and returns it.
 *
 * @return ParameterMap map of the explorer plugin parameters.
 */
DesignSpaceExplorerPlugin::ParameterMap 
DesignSpaceExplorerPlugin::parameters() const {
    return parameters_;
}


/**
 * Returns the boolean value of the given parameter if parameter can be
 * considered as a boolean.
 *
 * @param parameter The string to be converted to a boolean value
 * @return Boolean value of the parameter
 * @exception IllegalParameters Given parameter could not be considerer as
 * boolean value
 */
bool
DesignSpaceExplorerPlugin::booleanValue(const std::string& parameter) const
    throw (IllegalParameters) {
    
    std::string lowCase = StringTools::stringToLower(parameter);
    if (lowCase == "true" || lowCase == "1") {
        return true;
    } else if (lowCase == "false" || lowCase == "0") {
        return false;
    } else {
        throw IllegalParameters(__FILE__, __LINE__, __func__);
    }
}

/**
 * Checks that all compulsory parameters are set for the plugin.
 */
void 
DesignSpaceExplorerPlugin::checkParameters() const throw(IllegalParameters) {
    PMCIt it = parameters_.begin();
    while (it != parameters_.end()) {
        if (it->second.isCompulsory() && !it->second.isSet()) {
            std::string msg = it->second.name() + " parameter is needed.";
            throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
        }
    }
}

/**
 * Writes a verbose output message to a stream.
 *
 * @param message Message to be written.
 */
void
DesignSpaceExplorerPlugin::verboseOuput(
        const std::string& message) {

    if (verboseOut_ == NULL) {
       return;
    } 

    *verboseOut_ << message;
}

/**
 * Writes a error output message to a stream.
 *
 * @param message Message to be written.
 */
void
DesignSpaceExplorerPlugin::errorOuput(
        const std::string& message) {

    if (errorOut_ == NULL) {
       return;
    } 

    *errorOut_ << message;
}

/**
 * Explores the design space from the starting point machine and returns
 * best exploring results as configuration IDs.
 * 
 * Exploring creates new machine configurations (architecture, implementation)
 * that are ordered so that the best results are first in the result vector.
 * 
 * @param startPoint Starting point machine configuration for the plugin.
 * @param maxIter Maximum number of iterations that the program is allowed to
 * run. Default value for maxIter is zero when the iteration number is not
 * taken into account.
 * @return Ordered vector of IDs of the best machine configurations where the
 * target programs can be successfully run. The IDs of the best machine
 * configurations are first in the result vector. Returns an empty vector if
 * does not found any possible machine configurations.
 */
std::vector<RowID>
DesignSpaceExplorerPlugin::explore(
    const RowID&, const unsigned int&) {

    std::vector<RowID> result;
    return result;
}
