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
 * @file ConfigurationFile.cc
 *
 * Definition of ConfigurationFile class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifdef _GLIBCXX_DEBUG
// boost::match produces errors when this is defined, try with Boost 1.34 or
// higher if it has been fixed
#undef _GLIBCXX_DEBUG
#endif

#include <boost/regex.hpp>
#include <ctime>
#include <fstream>

#include <string>
#include <iostream>
#include <vector>

#include "ConfigurationFile.hh"
#include "Conversion.hh"
#include "StringTools.hh"
#include "MapTools.hh"

using std::tm;
using std::mktime;
using std::ifstream;
using std::string;
using std::cout;
using std::endl;
using std::vector;

/**
 * Constructor.
 *
 * @param doChecking If true, checks the semantics of the configuration file
 *                   while reading it.
 */
ConfigurationFile::ConfigurationFile(bool doChecking) : check_(doChecking) {
}

/**
 * Destructor.
 */
ConfigurationFile::~ConfigurationFile() {
    MapTools::deleteAllKeys(keys_);
}

/**
 * Parses the configuration file and stores the results to internal structure.
 *
 * @param inputStream Input file stream.
 */
void
ConfigurationFile::load(std::istream& inputStream) {
    parse(inputStream);
}

/**
 * Returns true if there is a given key with a value in configuration.
 *
 * @param key The key.
 * @return True if there is a given key in a configuration.
 */
bool
ConfigurationFile::hasKey(const std::string& key) {
    ValueContainer::iterator iter = values_.find(key);
    return iter != values_.end();
}

/**
 * Returns the value of the given key.
 *
 * @param key The key.
 * @param index The index of the value.
 * @return The value of the given key.
 * @exception KeyNotFound If key is not found.
 * @exception OutOfRange If index is out of range.
 */
std::string
ConfigurationFile::value(const std::string& key, int index)
    throw (KeyNotFound, OutOfRange) {

    return valueOfKey(key, index);
}

/**
 * Returns the number of values a given key has.
 *
 * @param key The key.
 * @return The number of values key has.
 * @exception KeyNotFound If key is not found.
 */
int
ConfigurationFile::itemCount(const std::string& key)
    throw (KeyNotFound) {

    ValueContainer::iterator iter = values_.find(key);
    if (iter == values_.end()) {
        string msg = "Key " + key + " not found";
        throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
    }
    return (*iter).second.size();
}

/**
 * Returns the int value of the given key.
 *
 * @param key The key.
 * @param index The index of the value.
 * @return The int value of the key.
 * @exception KeyNotFound If key is not found.
 * @exception OutOfRange If index is out of range.
 * @exception InvalidData If data is wrong type.
 */
int
ConfigurationFile::intValue(const std::string& key, int index)
    throw (KeyNotFound, OutOfRange, InvalidData) {

    string value = valueOfKey(key, index);
    try {
        return Conversion::toInt(value);
    } catch (const NumberFormatException& n) {
        string msg = "Wrong type of value: " + value;
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Returns the float value of the given key.
 *
 * @param key The key.
 * @param index The index of the value.
 * @return The float value of the given key.
 * @exception KeyNotFound If key is not found.
 * @exception OutOfRange If index is out of range.
 * @exception InvalidData If data is wrong type.
 */
float
ConfigurationFile::floatValue(const std::string& key, int index)
    throw (KeyNotFound, OutOfRange, InvalidData) {

    string value = valueOfKey(key, index);
    try {
        return Conversion::toFloat(value);
    } catch (const NumberFormatException& n) {
        string msg = "Wrong type of value: " + value;
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Returns string value of the given key.
 *
 * @param key The key.
 * @param index The index of the value.
 * @return The string value of the given key.
 * @exception KeyNotFound If key is not found.
 * @exception OutOfRange If index is out of range.
 */
string
ConfigurationFile::stringValue(const std::string& key, int index)
    throw (KeyNotFound, OutOfRange) {

    return valueOfKey(key, index);
}

/**
 * Returns the boolean value of the given key.
 *
 * @param key The key.
 * @param index The index of the value.
 * @return The boolean value of the key.
 * @exception KeyNotFound If key is not found.
 * @exception OutOfRange If index is out of range.
 * @exception InvalidData If value type is wrong.
 */
bool
ConfigurationFile::booleanValue(const std::string& key, int index)
    throw (KeyNotFound, OutOfRange, InvalidData) {

    string value = valueOfKey(key, index);
    value = StringTools::stringToLower(value);
    if (value == "true" || value == "yes" || value == "on" ||
        value == "enabled" || value == "1") {

        return true;
    } else if (value == "false" || value == "no" || value == "off" ||
               value == "disabled" || value == "0") {

        return false;
    } else {
        string msg = "Wrong type of value: " + value;
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
        return false;
    }
}

/**
 * Returns the time stamp value of the given key.
 *
 * @param key The key.
 * @return The time stamp value.
 * @exception KeyNotFound If key is not found.
 */
unsigned int
ConfigurationFile::timeStampValue(const std::string& key)
    throw (KeyNotFound) {

    ValueContainer::iterator iter = values_.find(key);
    if (iter == values_.end()) {
        string msg = "Key " + key + " not found";
        throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
    }

    string value = "";
    for (size_t i = 0; i < (*iter).second.size(); i++) {
        value += (*iter).second[i] + " ";
    }

    value = StringTools::trim(value);

    if (!legalTime(value)) {
        return 0;
    }

    size_t pos = value.find("-");
    string year = value.substr(0, pos);
    value.replace(0, pos + 1, "");

    pos = value.find("-");
    string month = value.substr(0, pos);
    value.replace(0, pos + 1, "");

    pos = value.find(" ");
    string day = value.substr(0, pos);
    value.replace(0, pos + 1, "");

    pos = value.find(":");
    string hours = value.substr(0, pos);
    value.replace(0, pos + 1, "");

    string minutes = value;

    time_t rawtime;
    tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime (&rawtime);

    int temp = Conversion::toInt(year);
    timeinfo->tm_year = temp - 1900;
    temp = Conversion::toInt(month);
    timeinfo->tm_mon = temp - 1;
    temp = Conversion::toInt(day);
    timeinfo->tm_mday = temp;
    temp = Conversion::toInt(hours);
    timeinfo->tm_hour = temp;
    temp = Conversion::toInt(minutes);
    timeinfo->tm_min = temp;

    return mktime(timeinfo);
}

/**
 * Add supported key type.
 *
 * @param key Key to be added.
 * @param type Type of the value.
 * @param caseSensitive True if key is case sensitive.
 */
void
ConfigurationFile::addSupportedKey(
    const std::string& key,
    ConfigurationValueType type,
    bool caseSensitive) {

    Key* k = new Key();
    k->name_ = key;
    k->caseSensitive_ = caseSensitive;

    keys_[k] = type;
}

/**
 * Default implementation, all errors are handled.
 *
 * @return True.
 */
bool
ConfigurationFile::handleError(
    int,
    ConfigurationFileError,
    const std::string&) {

    return true;
}

/**
 * Parses the configuration file.
 *
 * @param inputStream Stream where file is read from.
 */
void
ConfigurationFile::parse(std::istream& inputStream) {

    string line = "";
    int lineNumber = 1;
    while (getline(inputStream, line)) {
        line = StringTools::trim(line);

        if (isComment(line) || line == "") {
            lineNumber++;
            continue;
        } else {
            const char* regExp = "[ ]*(.+)[ ]*=[ ]*(.+)[ ]*";
            boost::regex expression(regExp);
            boost::smatch what;
            if (!boost::regex_match(line, what, expression)) {
                if (!handleError(lineNumber, FE_SYNTAX, line)) {
                    return;
                } else {
                    lineNumber++;
                    continue;
                }
            }

            string key = what[1];
            string value = what[2];
            key = StringTools::trim(key);
            value = StringTools::trim(value);

            vector<string> values = StringTools::chopString(value, " ");

            if (value == "") {
                if (!handleError(lineNumber, FE_MISSING_VALUE, line)) {
                    return;
                } else {
                    lineNumber++;
                    continue;
                }
            }

            if (check_) {
                if (!checkSemantics(key, value, lineNumber, line)) {
                    return;
                }
            }
            ValueContainer::iterator iter = values_.find(key);
            if (iter == values_.end()) {
                values_[key] = values;
            } else {
                for (size_t i = 0; i < values.size(); i++) {
                    (*iter).second.push_back(values[i]);
                }
            }
        }
        lineNumber++;
    }
}

/**
 * Returns true if line is a comment.
 *
 * @param line Line to be checked.
 * @return True if line is a comment.
 */
bool
ConfigurationFile::isComment(const std::string& line) {
    if (line.substr(0, 1) == "#") {
        return true;
    } else {
        return false;
    }
}

/**
 * Check the semantics of key value pair.
 *
 * @param key The key.
 * @param value The value.
 * @param lineNumber The line number.
 * @param line The line.
 * @return True if semantics are ok, otherwise false.
 */
bool
ConfigurationFile::checkSemantics(
    const std::string& key,
    const std::string& value,
    int lineNumber,
    const std::string& line) {

    KeyContainer::iterator iter = keys_.begin();
    while (iter != keys_.end()) {
        Key* current = (*iter).first;
        if (current->caseSensitive_) {
            if (current->name_ == key) {
                break;
            }
        } else {
            if (StringTools::stringToUpper(current->name_) ==
                StringTools::stringToUpper(key)) {
                break;
            }
        }
        iter++;
    }

    if (iter == keys_.end()) {
        return handleError(lineNumber, FE_UNKNOWN_KEY, line);
    }

    ConfigurationValueType type = (*iter).second;
    switch (type) {
    case VT_INTEGER:
        try {
            Conversion::toInt(value);
        } catch (const NumberFormatException& n) {
            return handleError(lineNumber, FE_ILLEGAL_TYPE, line);
        }
        break;
    case VT_FLOAT:
        try {
            Conversion::toFloat(value);
        } catch (const NumberFormatException& n) {
            return handleError(lineNumber, FE_ILLEGAL_TYPE, line);
        }
        break;
    case VT_BOOLEAN:
        if (value != "true" && value != "yes" && value != "on" &&
            value != "enable" && value != "1" && value != "false" &&
            value != "no" && value != "off" && value != "disable" &&
            value != "0") {

            return handleError(lineNumber, FE_ILLEGAL_TYPE, line);
        }
        break;
    case VT_READABLE_TIME:
        if (!legalTime(value)) {
            return handleError(lineNumber, FE_ILLEGAL_TYPE, line);
        }
        break;
    case VT_UNIX_TIMESTAMP:
        try {
            Conversion::toInt(value);
        } catch (const NumberFormatException& n) {
            return handleError(lineNumber, FE_ILLEGAL_TYPE, line);
        }
        break;
    default:
        break;
    }

    return true;
}

/**
 * Returns true if string is a legal time representation.
 *
 * @param line Line to be checked.
 * @return True if string is legal time, false otherwise.
 */
bool
ConfigurationFile::legalTime(const std::string& line) {

    const char* regExp = "([0-9]{4})-([0-9]{1,2})-([0-9]{1,2}) ([0-9]{1,2}):([0-9]{2})";
    boost::regex expression(regExp);
    boost::smatch what;
    if (!boost::regex_match(line, what, expression)) {
        return false;
    } else {
        return true;
    }
}

/**
 * Returns the value of the given key and a given index.
 *
 * @param key The key.
 * @param index Index of the value.
 * @return The value of the key.
 * @exception KeyNotFound If key is not found.
 * @exception OutOfRange If index is out of range.
 */
string
ConfigurationFile::valueOfKey(const std::string& key, int index)
    throw (KeyNotFound, OutOfRange) {

    ValueContainer::iterator iter = values_.find(key);
    if (iter == values_.end()) {
        string msg = "Key " + key + " not found";
        throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
    }

    if (index < 0 || index > static_cast<int>((*iter).second.size()) - 1) {
        string msg = "Index out of range.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }
    return (*iter).second[index];
}
