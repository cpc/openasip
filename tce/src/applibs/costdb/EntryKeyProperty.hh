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
 * @file EntryKeyProperty.hh
 *
 * Declaration of EntryKeyProperty class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_ENTRY_KEY_PROPERTY_HH
#define TTA_ENTRY_KEY_PROPERTY_HH

#include <vector>
#include <string>

#include "Exception.hh"

class EntryKeyFieldProperty;


/**
 * Represents a type of an CostDBEntryKey which contains types of its
 * fields, that is, EntryKeyFields.
 *
 * Using the EntryKeyProperty usually includes finding of the specific
 * EntryKeyProperty with Find(). Then the types of EntryKeyProperty's
 * fields can be accessed using FieldProperty(). Creation of the types
 * is done similarly using first Create() and then
 * CreateFieldProperty().
 *
 * For each CostDBEntryKey an EntryKeyProperty should be assigned.
 */
class EntryKeyProperty {
public:
    ~EntryKeyProperty();

    std::string name() const;
    EntryKeyFieldProperty* createFieldProperty(std::string field);
    EntryKeyFieldProperty* fieldProperty(std::string field) const
        throw (KeyNotFound);

    static EntryKeyProperty* create(std::string type);
    static EntryKeyProperty* find(std::string type) 
        throw (KeyNotFound);
    static void destroy();

protected:
    EntryKeyProperty(std::string propertyName);

private:
    /// Table of field types.
    typedef std::vector<EntryKeyFieldProperty*> FieldPropertyTable;
    /// Table of entry types.
    typedef std::vector<EntryKeyProperty*> EntryPropertyTable;

    /// Entry type.
    std::string name_;
    /// Field types of the entry type.
    FieldPropertyTable entryFields_;

    /// All known entry types.
    static EntryPropertyTable entryTypes_;

    /// Copying not allowed.
    EntryKeyProperty(const EntryKeyProperty&);
    /// Assignment not allowed.
    EntryKeyProperty& operator=(const EntryKeyProperty&);
};

#include "EntryKeyProperty.icc"

#endif
