/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file Parameter.cc
 *
 * Implementation of Parameter class.
 *
 * Created on: 20.4.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "Parameter.hh"

#include "BaseNetlistBlock.hh"

#include "Conversion.hh"

namespace ProGe {

/**
 * Empty parameter with undefined members.
 */
Parameter::Parameter()
    : name_(""),
      type_(""),
      value_(""),
      default_(""),
      package_("") {
}

/**
 * Constructs a parameter with both the assigned and default value having the
 * same value.
 *
 * @param name The name of the parameter
 * @param type The type of the parameter given as string
 * @param assignedAndDefaultValue The assigned and default value of the
 *                                parameter.
 */
Parameter::Parameter(
    const TCEString& name,
    const TCEString& type,
    const TCEString& assignedAndDefaultValue)
    : Parameter(name, type, assignedAndDefaultValue, assignedAndDefaultValue) {
}

/**
 * Constructs a parameter with separate assigned and default value.
 *
 * @param name The name of the parameter
 * @param type The type of the parameter given as string
 * @param assignedValue The assigned value of the parameter.
 * @param defaultValue The default value of the parameter.
 */
Parameter::Parameter(
    const TCEString& name,
    const TCEString& type,
    const TCEString& assignedValue,
    const TCEString& defaultValue)
    : name_(name),
      type_(type),
      value_(assignedValue),
      default_(defaultValue),
      package_("") {
}
Parameter::Parameter(
    const TCEString& name,
    const TCEString& type,
    int assignedValue,
    const TCEString& defaultValue)
    : name_(name),
      type_(type),
      value_(""),
      default_(defaultValue),
      package_("") {

    value_ = Conversion::toString(assignedValue);
}

/**
 * @param name The name of the parameter
 * @param type The type of the parameter given as string
 * @param nameOfConstant The value of the parameter as name to a constant.
 * @param defaultValue The default value of the parameter.
 * @param nameOfPackage The name of a package where the constant is defined.
 */
Parameter::Parameter(
    const TCEString& name,
    const TCEString& type,
    const TCEString& nameOfConstant,
    const TCEString& defaultValue,
    const TCEString& nameOfPackage)
    : name_(name),
      type_(type),
      value_(nameOfConstant),
      default_(defaultValue),
      package_(nameOfPackage) {
}

Parameter::~Parameter() {
}

void
Parameter::set(
        const TCEString& name,
        const TCEString& type,
        const TCEString& value) {
    name_ = name;
    type_ = type;
    value_ = value;
}

void
Parameter::setName(const TCEString& name) {
    name_ = name;
}

void
Parameter::setType(const TCEString& type) {
    type_ = type;
}

void
Parameter::setValue(const TCEString& value) {
    value_ = value;
}

const TCEString&
Parameter::name() const {
    return name_;
}

const TCEString&
Parameter::type() const {
    return type_;
}

const TCEString&
Parameter::value() const {
    return value_;
}

const TCEString&
Parameter::defaultValue() const {
    return default_;
}

/**
 * Return true is the value is a symbol therefore the actual value is defined
 * elsewhere. Otherwise, the value is literal.
 */
bool
Parameter::valueIsSymbol() const {
    return !package_.empty(); //note: placeholder
}

/**
 * Return true is the value is a symbol to a constant that are stored in
 * packages.
 */
bool
Parameter::valueIsConstant() const {
    return valueIsSymbol() && !package_.empty();
}

const std::string&
Parameter::packageNameOfConstant() const {
    return package_;
}

} /* namespace ProGe */
