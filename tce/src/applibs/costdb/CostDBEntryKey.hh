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
 * @file CostDBEntryKey.hh
 *
 * Declaration of CostDBEntryKey class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_COSTDB_ENTRY_KEY_HH
#define TTA_COSTDB_ENTRY_KEY_HH


#include <string>
#include <vector>

#include "CostDBTypes.hh"
#include "EntryKeyField.hh"
#include "Exception.hh"


/**
 * Represents the key of the CostDBEntry.
 *
 * Key properties are the properties of an entry that are used as a
 * search key.
 */
class CostDBEntryKey {
public:
    CostDBEntryKey(const EntryKeyProperty* entryType);
    virtual ~CostDBEntryKey();
    CostDBEntryKey* copy() const;

    const EntryKeyProperty* type() const;
    EntryKeyField keyFieldOfType(
        const EntryKeyFieldProperty& fieldType) const 
        throw (KeyNotFound);
    EntryKeyField keyFieldOfType(std::string fieldType) const;
    bool isEqual(const CostDBEntryKey& entryKey) const;

    void addField(EntryKeyField* field) 
        throw (ObjectAlreadyExists);
    void replaceField(EntryKeyField* newField) 
        throw (KeyNotFound);
    int fieldCount() const;
    const EntryKeyField& field(int index) const 
        throw (OutOfRange);

private:
    /// Table of entry fields.
    typedef std::vector<EntryKeyField*> FieldTable;

    /// Type of the entry key.
    const EntryKeyProperty* type_;
    /// Fields of the entry key.
    FieldTable fields_;

    /// Copying not allowed.
    CostDBEntryKey(const CostDBEntryKey&);
    /// Assignment not allowed.
    CostDBEntryKey& operator=(const CostDBEntryKey&);
};

#include "CostDBEntryKey.icc"

#endif
