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
 * @file SelectSet.hh
 *
 * Declaration of SelectSet class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_SELECT_SET_HH
#define TTA_SELECT_SET_HH


#include "Matcher.hh"


/**
 * Implementation for searching database entries that satisfies
 * selection criteria.
 *
 * However, equal values are also accepted.
 *
 * The selection criteria as well as the criteria for not selecting
 * some entry are given as template parameter.
 */
template <bool (EntryKeyField::*select)(const EntryKeyField&) const,
          bool (EntryKeyField::*unSelect)(const EntryKeyField&) const>
class SelectSet: public Matcher {
public:
    SelectSet(const EntryKeyFieldProperty* type);
    virtual ~SelectSet();

    virtual void quickFilter(
        const CostDBEntryKey& searchKey,
        CostDBTypes::EntryTable& components);
    virtual void filter(
        const CostDBEntryKey&,
        CostDBTypes::EntryTable& components);

private:
    /// Copying not allowed.
    SelectSet(const SelectSet&);
    /// Assignment not allowed.
    SelectSet& operator=(const SelectSet&);
};

#include "SelectSet.icc"

#endif
