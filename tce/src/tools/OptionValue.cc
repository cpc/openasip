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
 * @file OptionValue.cc
 *
 * Definitions of OptionValue classes.
 *
 * @author Jari Mäntyneva (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <string>
#include <sstream>

#include "Application.hh"
#include "OptionValue.hh"

using std::string;
using std::istringstream;
using std::endl;

//////////////////////////////////////////////////////////////////////////////
// OptionValue
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 */
OptionValue::OptionValue() {
}

/**
 * Destructor.
 */
OptionValue::~OptionValue() {
}

/**
 * This implementation should never be called.
 *
 * @exception WrongSubclass Called for a wrong subclass.
 */
void
OptionValue::setStringValue(const std::string&)
    throw (WrongSubclass) {

    throw WrongSubclass(__FILE__, __LINE__, __func__);
}

/**
 * This implementation should never be called.
 *
 * @exception WrongSubclass Called for a wrong subclass.
 */
void
OptionValue::setIntegerValue(int)
    throw (WrongSubclass) {

    throw WrongSubclass(__FILE__, __LINE__, __func__);
}

/**
 * This implementation should never be called.
 *
 * @exception WrongSubclass Called for a wrong subclass.
 */
void
OptionValue::setRealValue(double)
    throw (WrongSubclass) {

    throw WrongSubclass(__FILE__, __LINE__, __func__);
}

/**
 * This implementation should never be called.
 *
 * @exception WrongSubclass Called for a wrong subclass.
 */
void
OptionValue::setBoolValue(bool)
    throw (WrongSubclass) {

    throw WrongSubclass(__FILE__, __LINE__, __func__);
}

/**
 * This implementation should never be called.
 *
 * @return Nothing.
 * @exception WrongSubclass Called for a wrong subclass.
 */
void
OptionValue::setIntegerListValue(std::vector<int>)
    throw (WrongSubclass) {

    throw WrongSubclass(__FILE__, __LINE__, __func__);
}

/**
 * This implementation should never be called.
 *
 * @return Nothing.
 * @exception WrongSubclass Called for a wrong subclass.
 * @exception OutOfRange Never thrown by this function.
 */
int
OptionValue::integerValue(int) const
    throw (WrongSubclass, OutOfRange) {

    throw WrongSubclass(__FILE__, __LINE__, __func__);
    return 0;
}

/**
 * This implementation should never be called.
 *
 * @return Nothing.
 * @exception WrongSubclass Called for a wrong subclass.
 * @exception OutOfRange Never thrown by this function.
 */
std::string
OptionValue::stringValue(int) const
    throw (WrongSubclass, OutOfRange) {

    throw WrongSubclass(__FILE__, __LINE__, __func__);
    return "";
}

/**
 * This implementation should never be called.
 *
 * @return Nothing.
 * @exception WrongSubclass Called for a wrong subclass.
 */
double
OptionValue::realValue() const
    throw (WrongSubclass) {

    throw WrongSubclass(__FILE__, __LINE__, __func__);
    return 0;
}

/**
 * This implementation should never be called.
 *
 * @return Nothing.
 * @exception WrongSubclass Called for a wrong subclass.
 */
bool
OptionValue::isFlagOn() const
    throw (WrongSubclass) {

    throw WrongSubclass(__FILE__, __LINE__, __func__);
    return false;
}

/**
 * This implementation should never be called.
 *
 * @return Nothing.
 * @exception WrongSubclass Called for a wrong subclass.
 */
bool
OptionValue::isFlagOff() const
    throw (WrongSubclass) {

    throw WrongSubclass(__FILE__, __LINE__, __func__);
    return false;
}

/**
 * This implementation should never be called.
 *
 * @return Nothing.
 * @exception WrongSubclass Called for a wrong subclass.
 */
int
OptionValue::listSize() const
    throw (WrongSubclass) {

    throw WrongSubclass(__FILE__, __LINE__, __func__);
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// IntegerOptionValue
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param value The value of integer option.
 */
IntegerOptionValue::IntegerOptionValue(int value) {
    value_ = value;
}

/**
 * Destructor.
 */
IntegerOptionValue::~IntegerOptionValue() {
}

/**
 * Sets new value for integer option.
 *
 * @param value New value for the integer option.
 * @exception WrongSubclass Is never thrown by this function.
 */
void
IntegerOptionValue::setIntegerValue(int value)
    throw (WrongSubclass) {

    value_ = value;
}

/**
 * Returns the value of integer option.
 *
 * @return The value of integer option.
 * @exception WrongSubclass Is never thrown by this function.
 * @exception OutOfRange Never thrown by this function.
 */
int
IntegerOptionValue::integerValue(int) const
    throw (WrongSubclass, OutOfRange) {

    return value_;
}

//////////////////////////////////////////////////////////////////////////////
// StringOptionValue
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param value The value of string option.
 */
StringOptionValue::StringOptionValue(const std::string value) {
    value_ = value;
}

/**
 * Destructor.
 */
StringOptionValue::~StringOptionValue() {
}

/**
 * Sets new value for string option.
 *
 * @param value New value for the string option.
 * @exception WrongSubclass Is never thrown by this function.
 */
void
StringOptionValue::setStringValue(const std::string& value)
    throw (WrongSubclass) {

    value_ = value;
}

/**
 * Returns the value of string option.
 *
 * @return The value of string option.
 * @exception WrongSubclass Is never thrown by this function.
 * @exception OutOfRange Never thrown by this function.
 */
string
StringOptionValue::stringValue(int) const
    throw (WrongSubclass, OutOfRange) {

    return value_;
}

//////////////////////////////////////////////////////////////////////////////
// RealOptionValue
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param value The value of real option.
 */
RealOptionValue::RealOptionValue(double value) {
    value_ = value;
}

/**
 * Destructor.
 */
RealOptionValue::~RealOptionValue() {
}

/**
 * Sets new value for real option.
 *
 * @param value New value for the real option.
 * @exception WrongSubclass Is never thrown by this function.
 */
void
RealOptionValue::setRealValue(double value)
    throw (WrongSubclass) {

    value_ = value;
}

/**
 * Returns the value of string option.
 *
 * @return The value of string option.
 * @exception WrongSubclass Is never thrown by this function.
 */
double
RealOptionValue::realValue() const
    throw (WrongSubclass) {

    return value_;
}

//////////////////////////////////////////////////////////////////////////////
// BoolOptionValue
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param value The value of boolean option.
 */
BoolOptionValue::BoolOptionValue(bool value) {
    value_ = value;
}

/**
 * Destructor.
 */
BoolOptionValue::~BoolOptionValue() {
}

/**
 * Sets new value for boolean option.
 *
 * @param value New value for the boolean option.
 * @exception WrongSubclass Is never thrown by this function.
 */
void
BoolOptionValue::setBoolValue(bool value)
    throw (WrongSubclass) {

    value_ = value;
}

/**
 * Returns the value of boolean option.
 *
 * @return The value of boolean option.
 * @exception WrongSubclass Is never thrown by this function.
 */
bool
BoolOptionValue::isFlagOn() const
    throw (WrongSubclass) {

    return value_;
}

/**
 * Returns the negative value of boolean option.
 *
 * @return The negative value of boolean option.
 * @exception WrongSubclass Is never thrown by this function.
 */
bool
BoolOptionValue::isFlagOff() const
    throw (WrongSubclass) {

    return !value_;
}

//////////////////////////////////////////////////////////////////////////////
// IntegerListOptionValue
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param value The values of integer list options.
 */
IntegerListOptionValue::IntegerListOptionValue(std::vector<int> values) {
    values_ = values;
}

/**
 * Destructor.
 */
IntegerListOptionValue::~IntegerListOptionValue() {
}

/**
 * Sets new value for integer list option.
 *
 * @param value New value for the integer list option.
 * @exception WrongSubclass Is never thrown by this function.
 */
void
IntegerListOptionValue::setIntegerListValue(std::vector<int> values)
    throw (WrongSubclass) {

    values_ = values;
}

/**
 * Returns the value from the given index of integer list.
 *
 * The index must be given between 0 and the size of value vector - 1
 *
 * @return The value from the given index of integer list.
 * @exception WrongSubclass Is never thrown by this function.
 * @exception OutOfRange If the index is out of the range.
 */
int
IntegerListOptionValue::integerValue(int index) const
    throw (WrongSubclass, OutOfRange) {

    if (index < 0 || static_cast<unsigned>(index) > values_.size() - 1) {
	string procName = "IntegerListOptionValue::integerValue";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }
    return values_[index];
}

/**
 * Returns the size of the integer list.
 *
 * @return Size of hte list.
 */
int IntegerListOptionValue::listSize() const
    throw (WrongSubclass) {

    return values_.size();
}


//////////////////////////////////////////////////////////////////////////////
// StringListOptionValue
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param value The values of string list options.
 */
StringListOptionValue::StringListOptionValue(std::vector<string> values) {
    values_ = values;
}

/**
 * Destructor.
 */
StringListOptionValue::~StringListOptionValue() {
}

/**
 * Sets new value for string list option.
 *
 * @param value New value for the string list option.
 * @exception WrongSubclass Is never thrown by this function.
 */
void
StringListOptionValue::setStringListValue(std::vector<string> values)
    throw (WrongSubclass) {

    values_ = values;
}

/**
 * Returns the value from the given index of string list.
 *
 * The index must be given between 0 and the size of value vector - 1
 *
 * @return The value from the given index of string list.
 * @exception WrongSubclass Is never thrown by this function.
 * @exception OutOfRange If the index is out of the range.
 */
std::string
StringListOptionValue::stringValue(int index) const
    throw (WrongSubclass, OutOfRange) {

    if (index < 0 || static_cast<unsigned>(index) > values_.size() - 1) {
	string procName = "StringListOptionValue::stringValue";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }
    return values_[index];
}

/**
 * Returns the size of the string list.
 *
 * @return Size of hte list.
 */
int StringListOptionValue::listSize() const
    throw (WrongSubclass) {

    return values_.size();
}
