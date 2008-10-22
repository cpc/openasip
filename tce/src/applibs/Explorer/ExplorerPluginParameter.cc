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
 * @file ExplorerPluginParameter.hh
 *
 * Implementation of ExplorerPluginParameter class.
 *
 * @author Esa Määttä 2008 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "ExplorerPluginParameter.hh"


/**
 * The constructor.
 */
ExplorerPluginParameter::ExplorerPluginParameter(
    std::string name, 
    ExplorerPluginParameterType type,
    bool compulsory,
    std::string value): 
    name_(name), type_(type), compulsory_(compulsory), value_(value) {
}


/**
 * The destructor.
 */
ExplorerPluginParameter::~ExplorerPluginParameter() {
}


/**
 * Returns the value of the parameter as a string.
 *
 * @return The value of the parameter as a string.
 */
std::string
ExplorerPluginParameter::value() const {
    return value_;
}


/**
 * Sets the value of the parameter.
 *
 * @param value The value to be set for the parameter as a string.
 */
void 
ExplorerPluginParameter::setValue(const std::string& value) {
    value_ = value;
}



/**
 * Returns the parameter name.
 *
 * @return Name of the parameter as a string.
 */
std::string
ExplorerPluginParameter::name() const {
    return name_;
}
    

/**
 * Returns enum for parameter type.
 *
 * @return ExplorerPluginParameterType enumeration for parameter type.
 */
ExplorerPluginParameterType 
ExplorerPluginParameter::type() const {
    return type_;
}


/**
 * Returns string describing the parameter type.
 *
 * @return ExplorerPluginParameterType as a string.
 */
std::string
ExplorerPluginParameter::typeAsString() const {
    switch (type_) {
        case UINT:
            return "unsigned int";
            break;
        case INT:
            return "int";
            break;
        case STRING:
            return "string";
            break;
        case BOOL:
            return "boolean";
            break;
        default:
            return "";
    } 
}



/**
 * Returns boolean indicating if the parameter is always needed.
 *
 * @return True if parameter is compulsory, false otherwise.
 */
bool 
ExplorerPluginParameter::isCompulsory() const {
    return compulsory_;
}


/**
 * Returns boolean indicating if the parameter is always needed.
 *
 * @return True if parameter is compulsory, false otherwise.
 */
bool 
ExplorerPluginParameter::isSet() const {
    return value_.empty() ? false : true;;
}
