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
 * @file SubSet.hh
 *
 * Declaration of SubSet class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_SUB_SET_HH
#define TTA_SUB_SET_HH


#include "SelectSet.hh"


/**
 * Implementation for searching database entries that have smaller
 * value or a subset of the search key in the requested field.\ In
 * addition, equal values are accepted.
 */
class SubSet: public SelectSet<&EntryKeyField::isSmaller,
                               &EntryKeyField::isGreater> {
// The implementation utilizes SelectSet template class. Template
// parameter Select is EntryKeyField::isSmaller to accept smaller
// values in the results. Furthermore, UnSelect template parameter is
// EntryKeyField::isGreater to ignore greater values.
public:
    SubSet(const EntryKeyFieldProperty* type);
    virtual ~SubSet();

private:
    /// Copying not allowed.
    SubSet(const SubSet&);
    /// Assignment not allowed.
    SubSet& operator=(const SubSet&);
};

#endif
