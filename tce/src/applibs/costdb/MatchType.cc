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
 * @file MatchType.cc
 *
 * Implementation of MatchType class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include "MatchType.hh"


/**
 * Constructor.
 *
 * @param field Type of field.
 * @param match Type of match.
 */
MatchType::MatchType(
    const EntryKeyFieldProperty* field,
    CostDBTypes::TypeOfMatch match):
    fieldType_(field), matchType_(match) {
}

/**
 * Destructor.
 */
MatchType::~MatchType() {
}

/**
 * Copy constructor.
 *
 * @param old Match type.
 */
MatchType::MatchType(const MatchType& old) :
    fieldType_(old.fieldType_), matchType_(old.matchType_) {
}

/**
 * Compares if two match types are equal.
 *
 * @param m A match type.
 * @return True if match types are equal.
 */
bool
MatchType::isEqual(const MatchType& m) const {
    return fieldType() == m.fieldType() && matchingType() == m.matchingType();
}
