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
 * @file Matcher.hh
 *
 * Declaration of Matcher class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_MATCHER_HH
#define TTA_MATCHER_HH


#include "CostDBEntry.hh"
#include "CostDBEntryKey.hh"
#include "EntryKeyFieldProperty.hh"


/**
 * Interface for searching database entries.
 *
 * Search is based only on the information of one specific field of
 * the entries. Using the interface for queries requires at first a
 * QuickFilter() call and then a Filter() call.
 */
class Matcher {
public:
    Matcher(const EntryKeyFieldProperty* type);
    virtual ~Matcher();

    /**
     * Filters out unnecessary entries in linear time.
     *
     * @param searchKey Search key.
     * @param components Entries from which to find. Updated to contain
     *                   entries that matched the search request.
     */
    virtual void quickFilter(
        const CostDBEntryKey& searchKey,
        CostDBTypes::EntryTable& components) = 0;

    /**
     * Searches for database entries that match the search key.
     */
    virtual void filter(const CostDBEntryKey&, CostDBTypes::EntryTable&) = 0;

protected:
    const EntryKeyFieldProperty* fieldType() const;
    bool onlyThisFieldDiffers(
        const EntryKeyFieldProperty* type,
        const CostDBEntry& entry1,
        const CostDBEntry& entry2) const;

private:
    /// Copying not allowed.
    Matcher(const Matcher&);
    /// Assignment not allowed.
    Matcher& operator=(const Matcher&);

    /// Type of the field on which filtering is applied to.
    const EntryKeyFieldProperty* fieldType_;
};

#include "Matcher.icc"

#endif
