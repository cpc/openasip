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
 * @file ExactMatch.cc
 *
 * Implementation of ExactMatch class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#include <vector>

#include "ExactMatch.hh"

using std::vector;


/**
 * Constructor.
 *
 * @param type Type of the field.
 */
ExactMatch::ExactMatch(const EntryKeyFieldProperty* type) :
    Matcher(type) {
}

/**
 * Destructor.
 */
ExactMatch::~ExactMatch() {
}

/**
 * filters out values unequal to the search key.
 *
 * @param searchKey Search key.
 * @param components Entries from which to find. Updated to contain
 *                   entries that matched the search request.
 */
void
ExactMatch::quickFilter(
    const CostDBEntryKey& searchKey,
    CostDBTypes::EntryTable& components) {

    CostDBTypes::EntryTable filtered;
    for (CostDBTypes::EntryTable::iterator i = components.begin();
	 i != components.end(); i++) {

	if ((*i)->keyFieldOfType(*fieldType()).isEqual(
		searchKey.keyFieldOfType(*fieldType()))) {

	    filtered.push_back(*i);
	}
    }
    components = filtered;
}

/**
 * Nothing to do since quick filtering already accepted all possible
 * entries.
 */
void
ExactMatch::filter(
    const CostDBEntryKey&,
    CostDBTypes::EntryTable&) {
}
