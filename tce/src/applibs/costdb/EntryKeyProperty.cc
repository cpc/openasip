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
 * @file EntryKeyProperty.cc
 *
 * Implementation of EntryKeyProperty class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <vector>

#include "EntryKeyProperty.hh"
#include "EntryKeyFieldProperty.hh"
#include "Application.hh"

using std::string;
using std::vector;

EntryKeyProperty::EntryPropertyTable EntryKeyProperty::entryTypes_;

/**
 * Constructor.
 *
 * @param propertyName Type of entry.
 */
EntryKeyProperty::EntryKeyProperty(string propertyName): name_(propertyName) {
}

/**
 * Destructor.
 *
 * Deallocates memory reserved for field types.
 */
EntryKeyProperty::~EntryKeyProperty() {

    for (FieldPropertyTable::iterator i = entryFields_.begin();
         i != entryFields_.end(); i++) {
        
        assert(*i != NULL);
        delete *i;
        *i = NULL;        
    }
}

/**
 * Creates field type.
 *
 * If exists, returns the old one.
 *
 * @param field Type of field.
 * @return Field type.
 */
EntryKeyFieldProperty*
EntryKeyProperty::createFieldProperty(string field) {

    for (FieldPropertyTable::const_iterator i = entryFields_.begin();
         i != entryFields_.end(); i++) {

	if ((*i)->name() == field) {
	    return (*i);
	}
    }

    EntryKeyFieldProperty* newField = new EntryKeyFieldProperty(field, this);
    entryFields_.push_back(newField);
    return newField;
}

/**
 * Finds field type.
 *
 * If not found, exits.
 *
 * @param field Field type.
 * @return Field type.
 * @exception KeyNotFound Requested field was not found.
 */
EntryKeyFieldProperty*
EntryKeyProperty::fieldProperty(string field) const {
    for (FieldPropertyTable::const_iterator i = entryFields_.begin();
         i != entryFields_.end(); i++) {
        
        if ((*i)->name() == field) {
            return (*i);
        }
    }
    throw KeyNotFound(__FILE__, __LINE__, "EntryKeyProperty::fieldProperty");
    return NULL; // stupid return statement to make compiler quiet
}

/**
 * Creates entry type.
 *
 * If exists, returns the old one.
 *
 * @param type Entry type.
 * @return Entry type.
 */
EntryKeyProperty*
EntryKeyProperty::create(string type) {

    for (EntryPropertyTable::const_iterator i = entryTypes_.begin();
         i != entryTypes_.end(); i++) {
        
        if ((*i)->name() == type) {
            return (*i);
        }
    }

    EntryKeyProperty* newType = new EntryKeyProperty(type);
    entryTypes_.push_back(newType);
    return newType;
}

/**
 * Finds entry type.
 *
 * If not found, exits.
 *
 * @param type Entry type.
 * @return Entry type.
 * @exception KeyNotFound Requested field was not found.
 */
EntryKeyProperty*
EntryKeyProperty::find(string type) {
    for (EntryPropertyTable::const_iterator i = entryTypes_.begin();
         i != entryTypes_.end(); i++) {

        if ((*i)->name() == type) {
            return (*i);
        }
    }
    throw KeyNotFound(__FILE__, __LINE__, "EntryKeyProperty::find");
    return NULL; // stupid return statement to make compiler quiet
}

/**
 * Deallocates memory reserved for entry types.
 */
void
EntryKeyProperty::destroy() {
    
    for (EntryPropertyTable::iterator i = entryTypes_.begin();
         i != entryTypes_.end(); i++) {
        
        assert(*i != NULL);
        delete *i;
        *i = NULL;
    }
    entryTypes_.resize(0);
}
