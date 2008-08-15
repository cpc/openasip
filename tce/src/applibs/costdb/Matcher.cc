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
 * @file Matcher.cc
 *
 * Implementation of Matcher class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#include "Matcher.hh"


/**
 * Constructor.
 *
 * Marks the field type to which a search is aplied.
 *
 * @param type Type of the field.
 */
Matcher::Matcher(const EntryKeyFieldProperty* type) :
    fieldType_(type) {
}

/**
 * Destructor.
 */
Matcher::~Matcher() {
}

/**
 * Checks if only a specific field of two entries is different and all
 * other fields have equal values.
 *
 * @param type Type of the field.
 * @param entry1 An entry.
 * @param entry2 An entry.
 * @return True if only one field of two entries is different, otherwise false.
 */
bool
Matcher::onlyThisFieldDiffers(
    const EntryKeyFieldProperty* type,
    const CostDBEntry& entry1,
    const CostDBEntry& entry2) const {

    for (int i = 0; i < entry1.fieldCount(); i++) {
        const EntryKeyField& j = entry1.field(i);

	if (!j.isEqual(entry2.keyFieldOfType(*j.type())) &&
	    !(j.type() == type)) {

	    return false;
	}
    }
    return true;
}
