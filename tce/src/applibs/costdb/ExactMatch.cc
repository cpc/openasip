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
 * @file ExactMatch.cc
 *
 * Implementation of ExactMatch class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
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
