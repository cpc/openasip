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
 * @file SuperSet.hh
 *
 * Declaration of SuperSet class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_SUPER_SET_HH
#define TTA_SUPER_SET_HH


#include "SelectSet.hh"


/**
 * Implementation for searching database entries that have greater
 * value or a superset of the search key in the requested field.\ In
 * addition, equal values are accepted.
 */
class SuperSet: public SelectSet<&EntryKeyField::isGreater,
                                 &EntryKeyField::isSmaller> {
// The implementation utilizes SelectSet template class. Template
// parameter Select is EntryKeyField::isGreater to accept greater
// values in the results. Furthermore, UnSelect template parameter is
// EntryKeyField::isSmaller to ignore smaller values.
public:
    SuperSet(const EntryKeyFieldProperty* type);
    virtual ~SuperSet();

private:
    /// Copying not allowed.
    SuperSet(const SuperSet&);
    /// Assignment not allowed.
    SuperSet& operator=(const SuperSet&);
};

#endif
