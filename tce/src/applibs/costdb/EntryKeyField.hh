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
 * @file EntryKeyField.hh
 *
 * Declaration of EntryKeyField class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_ENTRY_KEY_FIELD_HH
#define TTA_ENTRY_KEY_FIELD_HH


#include "EntryKeyData.hh"
#include "EntryKeyFieldProperty.hh"


/**
 * Represents a field containing a value and a type.
 */
class EntryKeyField {
public:
    EntryKeyField(EntryKeyData* fieldData, const EntryKeyFieldProperty* type);
    virtual ~EntryKeyField();
    EntryKeyField(const EntryKeyField& old);
    EntryKeyField& operator=(const EntryKeyField& old);

    bool isEqual(const EntryKeyField& field) const;
    bool isGreater(const EntryKeyField& field) const;
    bool isSmaller(const EntryKeyField& field) const;
    double coefficient(const EntryKeyField& field1,
                       const EntryKeyField& field2) const;
    std::string toString() const;
    const EntryKeyFieldProperty* type() const;

private:
    /// Value of the field
    EntryKeyData* data_;
    /// Type of the field
    const EntryKeyFieldProperty* properties_;
};

#include "EntryKeyField.icc"

#endif
