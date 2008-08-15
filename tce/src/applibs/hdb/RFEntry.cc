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
 * @file RFEntry.cc
 *
 * Implementation of RFEntry class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <string>

#include "RFEntry.hh"
#include "RFArchitecture.hh"
#include "RFImplementation.hh"
#include "Application.hh"

using std::string;

namespace HDB {

/**
 * The constructor.
 *
 * Creates an empty RF entry without ID, architecture, implementation and
 * cost function.
 */
RFEntry::RFEntry() : 
    HDBEntry(), architecture_(NULL), implementation_(NULL) {
}


/**
 * The destructor.
 */
RFEntry::~RFEntry() {
    if (hasArchitecture()) {
        delete architecture_;
    }
    if (hasImplementation()) {
        delete implementation_;
    }
}


/**
 * Tells whether the entry has a hardware implementation.
 *
 * @return True if the entry has a hardware implementation, otherwise false.
 */
bool
RFEntry::hasImplementation() const {
    return implementation_ != NULL;
}


/**
 * Sets implementation for the entry.
 *
 * Deletes the old implementation if one exists.
 *
 * @param implementation The new implementation.
 */
void
RFEntry::setImplementation(RFImplementation* implementation) {
    if (hasImplementation()) {
        delete implementation_;
    }
    implementation_ = implementation;
}


/**
 * Returns the implementation of the entry.
 *
 * @return The implementation.
 * @exception NotAvailable If there is no implementation.
 */
RFImplementation&
RFEntry::implementation() const
    throw (NotAvailable) {

    if (!hasImplementation()) {
        const string procName = "RFEntry::implementation";
        throw NotAvailable(__FILE__, __LINE__, procName);
    }

    return *implementation_;
}


/**
 * Tells whether the RF entry has an architecture.
 *
 * @return True if the RF entry has an architecture, otherwise false.
 */
bool
RFEntry::hasArchitecture() const {
    return architecture_ != NULL;
}


/**
 * Sets new architecture for the entry.
 *
 * Deletes the old architecture if one exists.
 *
 * @param architecture The new architecture.
 */
void
RFEntry::setArchitecture(RFArchitecture* architecture) {
    if (hasArchitecture()) {
        delete architecture_;
    }
    architecture_ = architecture;
}


/**
 * Returns the architecture of the entry.
 *
 * @return The architecture.
 * @exception NotAvailable If the entry doesn't have an architecture.
 */
RFArchitecture&
RFEntry::architecture() const 
    throw (NotAvailable) {

    if (!hasArchitecture()) {
        const string procName = "RFEntry::architecture";
        throw NotAvailable(__FILE__, __LINE__, procName);
    }
    
    return *architecture_;
}
}
