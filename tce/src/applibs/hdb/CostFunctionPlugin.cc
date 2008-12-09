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
 * @file CostFunctionPlugin.cc
 *
 * Implementation of CostFunctionPlugin class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pekka.jaaskelainen-no.spam-tut.fi)
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
