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
 * @file SelectSet.hh
 *
 * Declaration of SelectSet class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari M�ntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
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
