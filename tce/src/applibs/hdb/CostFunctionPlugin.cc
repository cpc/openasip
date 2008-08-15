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
