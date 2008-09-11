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
 * @file CostDBEntryKey.cc
 *
 * Implementation of CostDBEntryKey class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include "CostDBEntryKey.hh"
#include "Application.hh"
#include <iostream>

/**
 * Constructor.
 *
 * @param entryType Type of the entry key.
 */
CostDBEntryKey::CostDBEntryKey(const EntryKeyProperty* entryType) :
    type_(entryType) {
}

/**
 * Destructor.
 *
 * Deallocates memory reserved for the fields of the entry key.
 */
CostDBEntryKey::~CostDBEntryKey() {
    for (FieldTable::iterator i = fields_.begin(); i != fields_.end(); i++) {
        assert(*i != NULL);
        delete *i;
        *i = NULL;
    }
}

/**
 * Copies the entry key.
 *
 * Client is responsible of deallocating the memory reserved for the
 * returned object.
 *
 * @return A copy of the entry key.
 */
CostDBEntryKey*
CostDBEntryKey::copy() const {

    CostDBEntryKey* copy = new CostDBEntryKey(type_);

    for (FieldTable::const_iterator i = fields_.begin();
         i != fields_.end(); i++) {

        copy->addField(new EntryKeyField(*(*i)));
    }
    return copy;
}

/**
 * Adds certain field to the entry key.
 *
 * @param field A field to be added.
 * @exception ObjectAlreadyExists Equal field already exists.
 */
void
CostDBEntryKey::addField(EntryKeyField* field) throw (ObjectAlreadyExists) {

    for (FieldTable::iterator i = fields_.begin(); i != fields_.end(); i++) {
        if ((*i)->type() == field->type()) {
            throw ObjectAlreadyExists(
                __FILE__, __LINE__, "CostDBEntryKey::addField");
        }
    }
    fields_.push_back(field);
}

/**
 * Replaces certain field of the entry key.
 *
 * @param newField A field.
 * @exception KeyNotFound Equal field was not found.
 */
void
CostDBEntryKey::replaceField(EntryKeyField* newField)
    throw (KeyNotFound) {
    
    for (FieldTable::iterator i = fields_.begin(); i != fields_.end(); i++) {
        if ((*i)->type() == newField->type()) {
            delete *i;
            *i = newField;
            return;
        }
    }
    throw KeyNotFound(__FILE__, __LINE__, "CostDBEntry::replaceField");
}

/**
 * Returns demanded field of the entry key.
 *
 * @param fieldType Type of the field.
 * @exception KeyNotFound Requested field type was not found.
 */
EntryKeyField
CostDBEntryKey::keyFieldOfType(const EntryKeyFieldProperty& fieldType) const
    throw (KeyNotFound) {

    for (FieldTable::const_iterator i = fields_.begin();
         i != fields_.end(); i++) {
        if ((*i)->type() == &fieldType) {
            return *(*i);
        }
    }
    throw KeyNotFound(__FILE__, __LINE__, "CostDBEntry::keyFieldOfType");
    return *fields_[0];// stupid return statement to make compiler quiet
}

/**
 * Returns demanded field of the entry key.
 *
 * @return type Type of the field.
 */
EntryKeyField
CostDBEntryKey::keyFieldOfType(std::string fieldType) const {

    return keyFieldOfType(*type_->fieldProperty(fieldType));
}

/**
 * Compares if two entry keys are equal.
 *
 * @param entryKey An entry key.
 * @return True if two entry keys are equal, otherwise false.
 */
bool
CostDBEntryKey::isEqual(const CostDBEntryKey& entryKey) const {

    if (type() == entryKey.type()) {
        bool isMatch = true;
        for (FieldTable::const_iterator f = fields_.begin();
             f != fields_.end(); f++) {
            
            if (!(*f)->isEqual(entryKey.keyFieldOfType(*(*f)->type()))) {
                isMatch = false;
                break;
            }
        }
        if (isMatch) {
            return true;
        }
    }
    return false;
}

/**
 * Returns the field found on the given index.
 *
 * The index must be between 0 and the number of fields - 1.
 *
 * @param index Index.
 * @return The field found on the given index.
 * @exception OutOfRange Index was out of range.
 */
const EntryKeyField&
CostDBEntryKey::field(int index) const
    throw (OutOfRange) {
    
    if (index >= fieldCount() || index < 0) {
        throw OutOfRange(__FILE__, __LINE__, "CostDBEntryKey::field");
    }
    return *fields_[index];
}
