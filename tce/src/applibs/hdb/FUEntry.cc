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
 * @file FUEntry.cc
 *
 * Implementation of FUEntry class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "FUEntry.hh"
#include "FUArchitecture.hh"
#include "FUImplementation.hh"
#include "Application.hh"

using std::string;

namespace HDB {

/**
 * The constructor.
 *
 * Creates an FU entry without ID, implementation, architecture and 
 * cost function.
 */
FUEntry::FUEntry() :
    HDBEntry(), architecture_(NULL), implementation_(NULL) {
}
    

/**
 * The destructor.
 */
FUEntry::~FUEntry() {
    if (hasArchitecture()) {
        delete architecture_;
    }
    if (hasImplementation()) {
        delete implementation_;
    }
}


/**
 * Tells whether the entry contains implementation data.
 *
 * @return True if the entry contains implementation data, otherwise false.
 */
bool
FUEntry::hasImplementation() const {
    return implementation_ != NULL;
}


/**
 * Returns the implementation of the entry.
 *
 * @return The implementation.
 * @exception NotAvailable If there is no implementation of the entry.
 */
FUImplementation&
FUEntry::implementation() const
    throw (NotAvailable) {

    if (!hasImplementation()) {
        const string procName = "FUEntry::implementation";
        throw NotAvailable(__FILE__, __LINE__, procName);
    }

    return *implementation_;
}


/**
 * Sets the given implementation for the entry.
 *
 * Deletes old implementation if one exists.
 *
 * @param implementation The implementation.
 */
void
FUEntry::setImplementation(FUImplementation* implementation) {
    if (hasImplementation()) {
        delete implementation_;
    }
    implementation_ = implementation;
}


/**
 * Tells whether the entry contains architecture data.
 *
 * @return True if the entry contains architecture data, otherwise false.
 */
bool
FUEntry::hasArchitecture() const {
    return architecture_ != NULL;
}


/**
 * Returns the architecture of the entry.
 *
 * @return Architecture of the entry.
 * @exception NotAvailable If the entry doesn't have architecture.
 */
FUArchitecture&
FUEntry::architecture() const 
    throw (NotAvailable) {

    if (!hasArchitecture()) {
        const string procName = "FUEntry::architecture";
        throw NotAvailable(__FILE__, __LINE__, procName);
    }

    return *architecture_;
}


/**
 * Sets architecture of the entry.
 *
 * Deletes the old architecture if one exists.
 *
 * @param architecture The new architecture.
 */
void
FUEntry::setArchitecture(FUArchitecture* architecture) {
    if (hasArchitecture()) {
        delete architecture_;
    }
    architecture_ = architecture;
}
}
