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
 * @file Matcher.cc
 *
 * Implementation of Matcher class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
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
