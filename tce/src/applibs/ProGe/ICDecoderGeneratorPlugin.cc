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
 * @file ICDecoderGeneratorPlugin.cc
 *
 * Implementation of ICDecoderGeneratorPlugin class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Vinogradov Viacheslav(added Verilog generating) 2012 
 * @note rating: red
 */

#include <string>

#include "ICDecoderGeneratorPlugin.hh"
#include "MapTools.hh"

using std::string;

namespace ProGe {

/**
 * Constructor.
 *
 * @param machine The machine to generate the IC&decoder for.
 */
ICDecoderGeneratorPlugin::ICDecoderGeneratorPlugin(
    const TTAMachine::Machine& machine,
    const BinaryEncoding& bem,
    const std::string& description) :
    machine_(machine), bem_(bem), description_(description) {

}


/**
 * Destructor.
 */
ICDecoderGeneratorPlugin::~ICDecoderGeneratorPlugin() {
}


/**
 * Returns the description of the plugin.
 *
 * @return The description.
 */
std::string
ICDecoderGeneratorPlugin::pluginDescription() const {
    return description_;
}


/**
 * Returns the number of recognized parameters.
 *
 * @return The number of recognized parameters.
 */
int
ICDecoderGeneratorPlugin::recognizedParameterCount() const {
    return parameterDescriptions_.size();
}


/**
 * Returns the name of a recognized parameter by the given index.
 *
 * @param index The index.
 * @return The name of the parameter.
 * @exception OutOfRange If the given index is negative or not smaller than
 *                       the number of recognized parameters.
 */
std::string
ICDecoderGeneratorPlugin::recognizedParameter(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= recognizedParameterCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }

    int i = 0;
    for (StringMap::const_iterator iter = parameterDescriptions_.begin();
         iter != parameterDescriptions_.end(); iter++) {
        if (i == index) {
            return iter->first;
        }
        i++;
    }

    assert(false);
    return "";
}


/**
 * Returns the description of the given parameter.
 *
 * @param paramName Name of the parameter.
 * @return The description of the parameter.
 * @exception IllegalParameter If the given parameter is unrecognized.
 */
std::string
ICDecoderGeneratorPlugin::parameterDescription(
    const std::string& paramName) const 
    throw (IllegalParameters) {

    try {
        return MapTools::valueForKey<string>(
            parameterDescriptions_, paramName);
    } catch (const KeyNotFound& e) {
        throw IllegalParameters(
            __FILE__, __LINE__, __func__, e.errorMessage());
    }
}

/**
 * Sets the given parameter for the plugin.
 *
 * @param name Name of the parameter.
 * @param value Value of the parameter.
 * @exception IllegalParameter If the given parameter is unrecognized.
 */
void
ICDecoderGeneratorPlugin::setParameter(
    const std::string& name,
    const std::string& value) 
    throw (IllegalParameters) {

    if (!MapTools::containsKey(parameterDescriptions_, name)) {
        string errorMsg = "Unrecognized IC/decoder plugin parameter: " + 
            name;
        throw IllegalParameters(__FILE__, __LINE__, __func__, errorMsg);
    }

    parameterValues_.erase(name);
    parameterValues_.insert(std::pair<string, string>(name, value));
}


/**
 * Adds the given parameter to the set of recognized parameters.
 *
 * @param name Name of the parameter.
 * @param description Description of the parameter.
 */
void
ICDecoderGeneratorPlugin::addParameter(
    const std::string& name,
    const std::string& description) {

    parameterDescriptions_.erase(name);
    parameterDescriptions_.insert(
        std::pair<string, string>(name, description));
}


/**
 * Tells whether the plugin has the given parameter set.
 *
 * @param name Name of the parameter.
 * @return True if the given parameter is set, otherwise false.
 */
bool
ICDecoderGeneratorPlugin::hasParameterSet(const std::string& name) const {
    return MapTools::containsKey(parameterValues_, name);
}


/**
 * Returns the value of the given parameter.
 *
 * @param name Name of the parameter.
 * @return Value of the parameter.
 * @exception KeyNotFound If the given parameter is not set.
 */
std::string
ICDecoderGeneratorPlugin::parameterValue(const std::string& name) const
    throw (KeyNotFound) {

    if (!hasParameterSet(name)) {
        throw KeyNotFound(__FILE__, __LINE__, __func__);
    }

    return MapTools::valueForKey<string>(parameterValues_, name);
}


/**
 * Returns the machine.
 *
 * @return The machine.
 */
const TTAMachine::Machine&
ICDecoderGeneratorPlugin::machine() const {
    return machine_;
}


/**
 * Returns the binary encoding map.
 *
 * @return The binary encoding map.
 */
const BinaryEncoding&
ICDecoderGeneratorPlugin::bem() const {
    return bem_;
}
}
