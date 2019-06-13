/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file CmdLineOptionParser.cc
 *
 * Definitions of CmdLineOptionParser classes.
 *
 * @author Jussi Nyk‰nen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 3 December 2003 by jn, kl, ao
 * @note rating: red
 */

#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>

#include "Application.hh"
#include "CmdLineOptionParser.hh"
#include "Exception.hh"
#include "OptionValue.hh"

using std::cerr;
using std::endl;
using std::string;
using std::istringstream;

//////////////////////////////////////////////////////////////////////////////
// CmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param name The name of the option.
 * @param desc The description of the option.
 * @param alias The short name of the option.
 */
CmdLineOptionParser::CmdLineOptionParser(
    std::string name,
    std::string desc,
    std::string alias) :
    longName_(name), shortName_(alias), desc_(desc), defined_(false) {
}

/**
 * Destructor.
 */
CmdLineOptionParser::~CmdLineOptionParser() {
}

/**
 * This implementation should never be called.
 *
 * @return Nothing.
 * @exception WrongSubclass Called for a wrong subclass.
 */
int
CmdLineOptionParser::integer(int) const {
    throw WrongSubclass(__FILE__, __LINE__, __func__);
    return 0;
}

unsigned
CmdLineOptionParser::unsignedInteger(int) const {
    throw WrongSubclass(__FILE__, __LINE__, __func__);
    return 0;
}

/**
 * This implementation should be never called.
 *
 * @return Nothing.
 * @exception WrongSubclass Called for a wrong subclass.
 */
std::string
CmdLineOptionParser::String(int) const {
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
CmdLineOptionParser::real() const {
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
CmdLineOptionParser::isFlagOn() const {
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
CmdLineOptionParser::isFlagOff() const {
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
CmdLineOptionParser::listSize() const {
    throw WrongSubclass(__FILE__, __LINE__, __func__);
    return 0;
}

/**
 * Sets the CmdLineOptionParser to be defined in the parsed command line.
 */
void
CmdLineOptionParser::setDefined() {
    defined_ = true;
}

/**
 * Returns true if the option was defined in the command line.
 *
 * @return True if the option was defined in the command line.
 */
bool
CmdLineOptionParser::isDefined() {
    return defined_;
}

//////////////////////////////////////////////////////////////////////////////
// IntegerCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param name The name of the option.
 * @param desc The description of the option.
 * @param alias The short name of the option.
 */
IntegerCmdLineOptionParser::IntegerCmdLineOptionParser(
    std::string name,
    std::string desc,
    std::string alias) : CmdLineOptionParser(name, desc, alias), value_(0) {
}

/**
 * Destructor.
 */
IntegerCmdLineOptionParser::~IntegerCmdLineOptionParser() {
}

/**
 * Copies the option value into a OptionValue object.
 *
 * Client is responsible of deallocating the memeory reserved for the
 * returned object.
 *
 * @return A copy of the option value.
 */
OptionValue*
IntegerCmdLineOptionParser::copy() const {
    IntegerOptionValue* copy = new IntegerOptionValue(value_);
    return copy;
}

/**
 * Parses the integer option value.
 *
 * @param arguments The arguments of option.
 * @param prefix The prefix of option.
 * @return True when parsing is ready.
 * @exception IllegalCommandLine If the arguments of option are erronous.
 */
bool
IntegerCmdLineOptionParser::parseValue(
    std::string arguments, std::string prefix) {
    if (prefix != "") {
        string msg = "Illegal prefix for integer option";
        string method = "IntegerCmdLineOptionParser::parseValue()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
    }

    if (arguments == "") {
        string msg = "Missing value for integer option.";
        string method = "IntegerCmdLineOptionParser::parseValue()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
    }

    istringstream istream(arguments);
    if((!(istream >> value_)) || istream.peek() != EOF) {
        string msg = "Format of " + longName() + " option value was wrong: "
            + arguments;
        string method = "IntegerCmdLineOptionParser::parseValue()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
    }

    setDefined();
    return true;
}

/**
 * Returns the value of integer option.
 *
 * @param index This must be zero for integer option.
 * @return The value of integer option.
 * @exception WrongSubclass Is never thrown by this function.
 */
int
IntegerCmdLineOptionParser::integer(int index) const {
    assert(index == 0);
    return value_;
}

//////////////////////////////////////////////////////////////////////////////
// IntegerCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param name The name of the option.
 * @param desc The description of the option.
 * @param alias The short name of the option.
 */
UnsignedIntegerCmdLineOptionParser::UnsignedIntegerCmdLineOptionParser(
    std::string name,
    std::string desc,
    std::string alias) : 
    CmdLineOptionParser(name, desc, alias), value_(0) {
}

/**
 * Destructor.
 */
UnsignedIntegerCmdLineOptionParser::~UnsignedIntegerCmdLineOptionParser() {
}

OptionValue*
UnsignedIntegerCmdLineOptionParser::copy() const {
    UnsignedIntegerOptionValue* copy = new UnsignedIntegerOptionValue(value_);
    return copy;
}

/**
 * Parses the unsigned integer option value.
 *
 * @param arguments The arguments of option.
 * @param prefix The prefix of option.
 * @return True when parsing is ready.
 * @exception IllegalCommandLine If the arguments of option are erronous.
 */
bool
UnsignedIntegerCmdLineOptionParser::parseValue(
    std::string arguments, std::string prefix) {
    if (prefix != "") {
        string msg = "Illegal prefix for unsigned integer option";
        string method = "UnsignedIntegerCmdLineOptionParser::parseValue()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
    }

    if (arguments == "") {
        string msg = "Missing value for unsigned integer option.";
        string method = "UnsignedIntegerCmdLineOptionParser::parseValue()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
    }

    istringstream istream(arguments);
    if((!(istream >> value_)) || istream.peek() != EOF) {
        string msg = "Format of " + longName() + " option value was wrong: "
            + arguments;
        string method = "unsignedIntegerCmdLineOptionParser::parseValue()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
    }

    setDefined();
    return true;
}

/**
 * Returns the value of the unsigned integer option.
 *
 * @param index This must be zero for usigned integer option.
 * @return The value of usigned integer option.
 * @exception WrongSubclass Is never thrown by this function.
 */
unsigned
UnsignedIntegerCmdLineOptionParser::unsignedInteger(int index) const {
    assert(index == 0);
    return value_;
}

//////////////////////////////////////////////////////////////////////////////
// StringCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param name The name of the option.
 * @param desc The description of the option.
 * @param alias The short name of the option.
 */
StringCmdLineOptionParser::StringCmdLineOptionParser(
    std::string name,
    std::string desc,
    std::string alias) : CmdLineOptionParser(name, desc, alias), value_("") {
}

/**
 * Destructor.
 */
StringCmdLineOptionParser::~StringCmdLineOptionParser() {
}

/**
 * Copies the option value into a OptionValue object.
 *
 * Client is responsible of deallocating the memeory reserved for the
 * returned object.
 *
 * @return A copy of the option value.
 */
OptionValue*
StringCmdLineOptionParser::copy() const {
    StringOptionValue* copy = new StringOptionValue(value_);
    return copy;
}

/**
 * Parses the value of option.
 *
 * @param arguments The arguments of option.
 * @param prefix The prefix of option.
 * @return True when parsing is ready.
 * @exception IllegalCommandLine Not thrown.
 */
bool
StringCmdLineOptionParser::parseValue(
    std::string arguments, std::string prefix) {
    if (prefix != "") {
        string msg = "Illegal prefix for string option";
        string method = "StringCmdLineOptionParser::parseValue()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
    }

    if (arguments == "") {
        string msg = "Missing value for string option.";
        string method = "StringCmdLineOptionParser::parseValue()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
    }

    value_ = arguments;

    setDefined();
    return true;
}

string
StringCmdLineOptionParser::String(int index) const {
    assert(index == 0);
    return value_;
}

//////////////////////////////////////////////////////////////////////////////
// RealCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param name The name of the option.
 * @param desc The description of the option.
 * @param alias The short name of the option.
 */
RealCmdLineOptionParser::RealCmdLineOptionParser(
    std::string name,
    std::string desc,
    std::string alias) :
    CmdLineOptionParser(name, desc, alias), value_(0) {
}

/**
 * Destructor.
 */
RealCmdLineOptionParser::~RealCmdLineOptionParser() {
}

/**
 * Copies the option value into a OptionValue object.
 *
 * Client is responsible of deallocating the memeory reserved for the
 * returned object.
 *
 * @return A copy of the option value.
 */
OptionValue*
RealCmdLineOptionParser::copy() const {
    RealOptionValue* copy = new RealOptionValue(value_);
    return copy;
}

/**
 * Parses the value of option.
 *
 * @param arguments The arguments of option.
 * @param prefix The prefix of option.
 * @return True when parsing is ready.
 * @exception IllegalCommandLine If the argument is erronous.
 */
bool
RealCmdLineOptionParser::parseValue(std::string arguments, std::string prefix) {
    if (prefix != "") {
        string msg = "Illegal prefix for real option";
        string method = "RealCmdLineOptionParser::parseValue()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
    }

    // string must not contain anything else than numbers and a dot.
    if (arguments == "") {
        string msg = "Missing value for real option.";
        string method = "RealCmdLineOptionParser::parseValue()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
    }

    istringstream istream(arguments);
    if((!(istream >> value_)) || istream.peek() != EOF) {
        string msg = "Format of " + longName() + " option value was wrong: "
            + arguments;
        string method = "RealCmdLineOptionParser::parseValue()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
    }

    setDefined();
    return true;
}

/**
 * Returns the real value.
 *
 * @return The real value.
 * @exception WrongSubclass Is never thrown by this function.
 */
double
RealCmdLineOptionParser::real() const {
    return value_;
}

//////////////////////////////////////////////////////////////////////////////
// BoolCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param name The name of the option.
 * @param desc The description of the option.
 * @param alias The short name of the option.
 */
BoolCmdLineOptionParser::BoolCmdLineOptionParser(
    std::string name,
    std::string desc,
    std::string alias) :
    CmdLineOptionParser(name, desc, alias), value_(false) {
}

/**
 * Destructor.
 */
BoolCmdLineOptionParser::~BoolCmdLineOptionParser() {
}

/**
 * Copies the option value into a OptionValue object.
 *
 * Client is responsible of deallocating the memeory reserved for the
 * returned object.
 *
 * @return A copy of the option value.
 */
OptionValue*
BoolCmdLineOptionParser::copy() const {
    BoolOptionValue* copy = new BoolOptionValue(value_);
    return copy;
}

/**
 * Parses the value of option.
 *
 * @param arguments The arguments for option.
 * @param prefix The prefix of option.
 * @return True if parsing is ready, otherwise false.
 * @exception IllegalCommandLine If the prefix or arguments are invalid.
 */
bool
BoolCmdLineOptionParser::parseValue(std::string arguments, std::string prefix) {
    if (prefix == "no-") {
        value_ = false;
    } else if (prefix == "") {
        value_ = true;
    } else {
        string msg = "Illegal prefix for boolean option " + longName() +
            ": " + prefix;
        string method = "BoolCmdLineOptionParser::parseValue()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
    }

    if (arguments != "") {
        return false;
    } else {
	setDefined();
        return true;
    }
}

/**
 * Returns true if the flag is set.
 *
 * @return The boolean flag value.
 * @exception WrongSubclass Is never thrown by this function.
 */
bool
BoolCmdLineOptionParser::isFlagOn() const {
    return value_;
}

/**
 * Returns true if the flag is not set.
 *
 * @return Inverse of the boolean flag.
 * @exception WrongSubclass Is never thrown by this function.
 */
bool
BoolCmdLineOptionParser::isFlagOff() const {
    return !value_;
}

//////////////////////////////////////////////////////////////////////////////
// IntegerListCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param name The name of the option.
 * @param desc The description of the option.
 * @param alias The short name of the option.
 */
IntegerListCmdLineOptionParser::IntegerListCmdLineOptionParser(
    std::string name,
    std::string desc,
    std::string alias) : CmdLineOptionParser(name, desc, alias) {
}

/**
 * Destructor.
 */
IntegerListCmdLineOptionParser::~IntegerListCmdLineOptionParser() {
    values_.clear();
}

/**
 * Copies the option value into a OptionValue object.
 *
 * Client is responsible of deallocating the memeory reserved for the
 * returned object.
 *
 * @return A copy of the option value.
 */
OptionValue*
IntegerListCmdLineOptionParser::copy() const {
    IntegerListOptionValue* copy = new IntegerListOptionValue(values_);
    return copy;
}

/**
 * Parses the value of integer set.
 *
 * Value should be integers separated by commas.
 *
 * @param arguments The arguments of option.
 * @param prefix The prefix of option.
 * @return True when parsing is ready.
 * @exception IllegalCommandLine If argument is invalid.
 */
bool
IntegerListCmdLineOptionParser::parseValue(
    std::string arguments, std::string prefix) {
    if (prefix != "") {
        string msg = "Illegal prefix for integer list option";
        string method = "IntegerListCmdLineOptionParser::parseValue()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
    }

    if (arguments == "") {
        string msg = "Missing value for option " + longName();
        string method = "IntegerSetCmdLineOptionParser::parseValue()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
    }

    int number;
    char comma;
    istringstream istream(arguments);
    // there must be at least one number
    if (!(istream >> number)) {
        string msg = "Format of " + longName() + "option value was wrong";
        string method = "IntegerSetCmdLineOptionParser::parseValue()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
    }
    values_.push_back(number);

    while (istream.peek() != EOF) {

        // now there must be comma
        if (!(istream >> comma) || comma != ',') {
            string msg = "Format of " + longName() + "option value was wrong";
            string method = "IntegerSetCmdLineOptionParser::parseValue()";
            throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
        }

        if (!(istream >> number)) {
            string msg = "Format of " + longName() + "option value was wrong";
            string method = "IntegerSetCmdLineOptionParser::parseValue()";
            throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
        }
        values_.push_back(number);
    }
    setDefined();
    return true;
}

/**
 * Returns a particular value of integer list option.
 *
 * @param index The index of list element that is wanted.
 * @return The value of a particular element in list.
 * @exception WrongSubclass Is never thrown by this function.
 */
int
IntegerListCmdLineOptionParser::integer(int index) const {
    assert(index > 0 && static_cast<unsigned>(index) <= values_.size());
    return values_[index - 1];
}

/**
 * Returns the number of list elements.
 *
 * @return The number of list elements.
 * @exception WrongSubclass Is never thrown by this function.
 */
int
IntegerListCmdLineOptionParser::listSize() const {
    return values_.size();
}

//////////////////////////////////////////////////////////////////////////////
// StringListCmdLineOptionParser
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param name The name of the option.
 * @param desc The description of the option.
 * @param alias The short name of the option.
 */
StringListCmdLineOptionParser::StringListCmdLineOptionParser(
    std::string name,
    std::string desc,
    std::string alias) : CmdLineOptionParser(name, desc, alias) {
}

/**
 * Destructor.
 */
StringListCmdLineOptionParser::~StringListCmdLineOptionParser() {
    values_.clear();
}

/**
 * Copies the option value into a OptionValue object.
 *
 * Client is responsible of deallocating the memeory reserved for the
 * returned object.
 *
 * @return A copy of the option value.
 */
OptionValue*
StringListCmdLineOptionParser::copy() const {
    StringListOptionValue* copy = new StringListOptionValue(values_);
    return copy;
}

/**
 * Parses the value of string set.
 *
 * Value should be strings separated by commas.
 *
 * @param arguments The arguments of option.
 * @param prefix The prefix of option.
 * @return True when parsing is ready.
 * @exception IllegalCommandLine If argument is invalid.
 */
bool
StringListCmdLineOptionParser::parseValue(
    std::string arguments, std::string prefix) {
    if (prefix != "") {
        string msg = "Illegal prefix for string list option";
        throw IllegalCommandLine(__FILE__, __LINE__, __func__, msg);
    }

    if (arguments == "") {
        string msg = "Missing value for option " + longName();
        throw IllegalCommandLine(__FILE__, __LINE__, __func__, msg);
    }

    string comma = ",";
    string::size_type commaPosition = 0;
    string::size_type lastPosition = 0;
    while (commaPosition < arguments.size()) {
        commaPosition = arguments.find(comma, lastPosition);
        if (commaPosition == string::npos) {
            values_.push_back(
                arguments.substr(lastPosition, arguments.size()));
            break;
        } else {
            values_.push_back(
                arguments.substr(lastPosition, commaPosition-lastPosition));
            lastPosition = commaPosition + 1;
        }
    }

    setDefined();
    return true;
}

/**
 * Returns a particular value of string list option.
 *
 * @param index The index of list element that is wanted.
 * @return The value of a particular element in list.
 * @exception WrongSubclass Is never thrown by this function.
 */
std::string
StringListCmdLineOptionParser::String(int index) const {
    assert(index > 0 && static_cast<unsigned>(index) <= values_.size());
    return values_[index - 1];
}

/**
 * Returns the number of list elements.
 *
 * @return The number of list elements.
 * @exception WrongSubclass Is never thrown by this function.
 */
int
StringListCmdLineOptionParser::listSize() const {
    return values_.size();
}
