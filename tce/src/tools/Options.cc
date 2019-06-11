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
 * @file Options.cc
 *
 * A generic container of option values.
 *
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <typeinfo>

#include "Options.hh"
#include "MapTools.hh"
#include "OptionValue.hh"

using std::string;
using std::vector;
using std::pair;

#include <iostream>
using std::cerr;
using std::endl;

/**
 * Constructor.
 */
Options::Options() {
}

/**
 * Destructor.
 */
Options::~Options() {
    std::map<string, vector<OptionValue*> >::iterator i = options_.begin();
    vector<OptionValue*>::iterator j;
    for (; i != options_.end(); i++) {
        j = (*i).second.begin();
	for (; j != (*i).second.end(); j++) {
	    delete (*j);
	}
	(*i).second.clear();
    }
    options_.clear();
}

/**
 * Adds one option value to given option.
 *
 * @param name Name of the option.
 * @param option Value of the option.
 * @exception TypeMismatch If new value's type differ the old ones.
 */
void
Options::addOptionValue(const string& name, OptionValue* option)
    throw (TypeMismatch) {

    try {
        vector<OptionValue*> values;
        values = MapTools::valueForKey<vector<OptionValue*> >(options_, name);

        OptionValue& other = *values.at(0);
        // check that new value is same type that old ones
        if (typeid(*option) == typeid(other)) {
            values.push_back(option);
            options_.erase(name);
            options_.insert(pair<string, vector<OptionValue*> >(name, values));
        } else {
            string procName = "Options::addOptionValue";
            string errorMsg =
                "Type of new OptionValue differs from the type of old values";
            throw TypeMismatch(__FILE__, __LINE__, procName, errorMsg);
        }
    } catch (KeyNotFound& e) {
        vector<OptionValue*> values;
        values.push_back(option);
        options_.insert(pair<string, vector<OptionValue*> >(name, values));
    }
}

/**
 * Returns optionValue of the given option.
 *
 * The index must be given between 0 and the option count where 0 is the
 * latest value.
 *
 * @param name Name of the option.
 * @param index Index for lists of options.
 * @return The optionValue of an option.
 * @exception OutOfRange The index is out of range of values.
 * @exception KeyNotFound No option with given name.
 */
OptionValue&
Options::optionValue(const std::string& name, int index)
    throw (OutOfRange, KeyNotFound) {

    if (index < 0 || index > valueCount(name) - 1) {
	string procName = "OptionValue::optionValue";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }
    vector<OptionValue*> values;
    values = MapTools::valueForKey<vector<OptionValue*> >(options_,name);

    return *values[values.size() - index - 1];
}

/**
 * Returns the count of optionValues of an option.
 *
 * @param name Name of the option.
 * @return Number of values for the option.
 * @exception KeyNotFound No option with given name.
 */
int Options::valueCount(const std::string& name)
    throw (KeyNotFound) {

    vector<OptionValue*> values;
    try {
    values = MapTools::valueForKey<vector<OptionValue*> >(options_,name);
    } catch (KeyNotFound& e) {
	string procName = "OptionValue::valueCount";
	throw KeyNotFound(__FILE__, __LINE__, procName);
    }
    return values.size();
}
