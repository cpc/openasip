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
 * @file Options.hh
 *
 * Declaration of Options class.
 *
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPTIONS_HH
#define TTA_OPTIONS_HH

#include <string>
#include <map>
#include <vector>

#include "OptionValue.hh"

using std::map;
using std::string;
using std::vector;

/**
 * Generic container of option values.
 */
class Options {
public:
    Options();
    virtual ~Options();
    void addOptionValue(const string& name, OptionValue* option)
	throw (TypeMismatch);

    // index for lists of options
    OptionValue& optionValue(const string& name, int index = 0)
	throw (OutOfRange, KeyNotFound);
    int valueCount(const string& name)
	throw (KeyNotFound);

private:
    map<string, vector<OptionValue*> > options_;
};

#endif
