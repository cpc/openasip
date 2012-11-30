/*
    Copyright (c) 2002-2012 Tampere University of Technology.

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
 * @file ExplorerPluginParameter.hh
 *
 * Implementation of ExplorerPluginParameter class.
 *
 * @author Esa M‰‰tt‰ 2008 (esa.maatta-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2012
 * @note rating: red
 */

#include <string>

#include "ExplorerPluginParameter.hh"


/**
 * The constructor.
 */
ExplorerPluginParameter::
    ExplorerPluginParameter(
        const std::string& name, 
        ExplorerPluginParameterType type,
        bool compulsory,
        const TCEString value,
        const TCEString description) : 
        name_(name), type_(type), compulsory_(compulsory), value_(value), 
        description_(description) {
}


/**
 * The destructor.
 */
ExplorerPluginParameter::~ExplorerPluginParameter() {
}


TCEString
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
TCEString
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
TCEString
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
