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
 * @file Interpolation.hh
 *
 * Declaration of Interpolation class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_INTERPOLATION_HH
#define TTA_INTERPOLATION_HH


#include "Matcher.hh"
#include "Exception.hh"


/**
 * Implementation for searching database entries.
 *
 * If a value equal to the search key in the requested field is not
 * found, interpolation of the smaller and greater field value is
 * tried.
 */
class Interpolation: public Matcher {
public:
    Interpolation(const EntryKeyFieldProperty* type);
    virtual ~Interpolation();
    
    void quickFilter(const CostDBEntryKey&, CostDBTypes::EntryTable&);
    void filter(
        const CostDBEntryKey& searchKey,
        CostDBTypes::EntryTable& components)
        throw (TypeMismatch, KeyNotFound);

private:
    /**
     * Represents an entry pair.
     */
    struct Pair {
        /// An entry.
        CostDBEntry* smaller;
        /// An entry.
        CostDBEntry* greater;
    };

    /// Entries created during interpolation.
    CostDBTypes::EntryTable created_;

    /// Copying not allowed.
    Interpolation(const Interpolation&);
    /// Assignment not allowed.
    Interpolation& operator=(const Interpolation&);
};

#endif
