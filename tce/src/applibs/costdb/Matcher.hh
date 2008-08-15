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
 * @file Matcher.hh
 *
 * Declaration of Matcher class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
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
