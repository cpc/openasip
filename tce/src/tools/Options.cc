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
 * @file Options.cc
 *
 * A generic container of option values.
 *
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#include <string>
#include <typeinfo>

#include "Options.hh"
#include "MapTools.hh"

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

	// check that new value is same type that old ones
	if (typeid(*option) == typeid(*values.at(0))) {
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
