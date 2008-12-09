/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
