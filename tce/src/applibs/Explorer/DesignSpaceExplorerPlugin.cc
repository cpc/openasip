/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file DesignSpaceExplorerPlugin.cc
 *
 * Implementation of DesignSpaceExplorerPlugin class
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011
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
    DesignSpaceExplorer() {
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
    const std::string& name, const std::string& value) {
    PMIt it = parameters_.find(name);
    if (it == parameters_.end()) {
        std::string msg = "Plugin has no parameter named: " + name;
        throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
    }
    it->second.setValue(value);
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
DesignSpaceExplorerPlugin::booleanValue(const std::string& parameter) const {
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
DesignSpaceExplorerPlugin::checkParameters() const {
    for (PMCIt it = parameters_.begin(); it != parameters_.end(); it++) {
        if (it->second.isCompulsory() && !it->second.isSet()) {
            std::string msg = it->second.name() + " parameter is needed.";
            throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
        }
    }
}

/**
 * Explores the design space from the starting point machine and returns
 * best exploring results as configuration IDs.
 * 
 * Exploring creates new machine configurations (architecture, implementation)
 * that are ordered so that the best results are first in the result vector.
 * 
 * @param startPoint Starting point machine configuration for the plugin.
 * @param maxIter Maximum number of design space points the plugin is allowed to
 * explore. Default value for maxIter is zero when the iteration number is not
 * taken into account. In that case the exploration runs indefinetly or stops
 * at a point defined by the algorithm.
 * @return Ordered vector of IDs of the best machine configurations where the
 * target programs can be successfully run. The IDs of the best machine
 * configurations are first in the result vector. Returns an empty vector if
 * does not find any possible machine configurations.
 */
std::vector<RowID>
DesignSpaceExplorerPlugin::explore(
    const RowID&, const unsigned int&) {

    std::vector<RowID> result;
    return result;
}
