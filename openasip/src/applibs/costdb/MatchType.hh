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
 * @file MatchType.hh
 *
 * Declaration of MatchType class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_MATCH_TYPE_HH
#define TTA_MATCH_TYPE_HH


#include "CostDBTypes.hh"
#include "CostDBEntry.hh"
#include "CostDBEntryKey.hh"
#include "EntryKeyFieldProperty.hh"

class CostDatabase;


/**
 * Represents a type of match for a certain field.
 */
class MatchType {
public:
    MatchType(
        const EntryKeyFieldProperty* field,
        CostDBTypes::TypeOfMatch match);
    virtual ~MatchType();
    MatchType(const MatchType& old);

    const EntryKeyFieldProperty* fieldType() const;
    CostDBTypes::TypeOfMatch matchingType() const;
    bool isEqual(const MatchType& m) const;

    MatchType& operator=(const MatchType&) = delete;

private:
    /// Type of field.
    const EntryKeyFieldProperty* fieldType_;
    /// Type of match.
    CostDBTypes::TypeOfMatch matchType_;
};

#include "MatchType.icc"

#endif
