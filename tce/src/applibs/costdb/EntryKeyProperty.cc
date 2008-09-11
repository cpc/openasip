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
EntryKeyProperty::fieldProperty(string field) const 
    throw (KeyNotFound) {
    
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
EntryKeyProperty::find(string type) 
    throw (KeyNotFound) {
    
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
