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
 * @file EntryKeyField.cc
 *
 * Implementation of EntryKeyField class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include "EntryKeyField.hh"
#include "Application.hh"


/**
 * Constructor.
 *
 * @param fieldData Value of the field.
 * @param type Type of the field.
 */
EntryKeyField::EntryKeyField(
    EntryKeyData* fieldData,
    const EntryKeyFieldProperty* type):
    data_(fieldData), properties_(type) {
}

/**
 * Copy constructor.
 *
 * @param old A field.
 */
EntryKeyField::EntryKeyField(const EntryKeyField& old):
    data_(old.data_->copy()), properties_(old.properties_) {
}

/**
 * Destructor.
 *
 * Deallocates memory reserved for the value of the field.
 */
EntryKeyField::~EntryKeyField() {
    assert(data_ != NULL);
    delete data_;
    data_ = NULL;
}

/**
 * Assignment operator.
 *
 * @param old A field.
 * @return A copy of the field.
 */
EntryKeyField&
EntryKeyField::operator=(const EntryKeyField& old) {
    if (this != &old) {
	data_ = old.data_->copy();
	properties_ = old.properties_;
    }
    return *this;
}

/**
 * Checks if two fields are equal.
 *
 * @param field A field.
 * @return True if two integers are equal, otherwise false.
 */
bool
EntryKeyField::isEqual(const EntryKeyField& field) const {
    return data_->isEqual(field.data_);
}

/**
 * Checks if this field is greater than another field.
 *
 * @param field A field.
 * @return True if this field is greater than another field, otherwise false.
 */
bool
EntryKeyField::isGreater(const EntryKeyField& field) const {
    return data_->isGreater(field.data_);
}

/**
 * Checks if this field is smaller than another field.
 *
 * @param field A field.
 * @return True if this field is smaller than another field, otherwise false.
 */
bool
EntryKeyField::isSmaller(const EntryKeyField& field) const {
    return data_->isSmaller(field.data_);
}

/**
 * Returns the relative position between two fields.
 *
 * Returns the field's relative position to the first field compared
 * to the second. For example, if this field has value 14, the first
 * field has 10 and the second 20, relative position would be 0.4.
 *
 * @param field1 First field.
 * @param field2 Second field.
 * @return The relative position between two fields.
 */
double
EntryKeyField::coefficient(
    const EntryKeyField& field1,
    const EntryKeyField& field2) const {

    return data_->coefficient(field1.data_, field2.data_);
}

/**
 * Returns value of the field as a string.
 *
 * @return Value of the field as a string.
 */
std::string
EntryKeyField::toString() const {
    return data_->toString();
}
