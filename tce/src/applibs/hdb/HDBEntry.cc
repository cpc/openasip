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
 * @file HDBEntry.cc
 *
 * Implementation of HDBEntry class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "HDBEntry.hh"
#include "CostFunctionPlugin.hh"

namespace HDB {

/**
 * The constructor.
 */
HDBEntry::HDBEntry() :
    hasID_(false), id_(0), costFunction_(NULL), hdbFile_("") {
}


/**
 * The destructor.
 */
HDBEntry::~HDBEntry() {
    if (hasCostFunction()) {
        delete costFunction_;
    }
}


/**
 * Tells whether the entry has an ID.
 *
 * @return True if the entry has an ID, otherwise false.
 */
bool
HDBEntry::hasID() const {
    return hasID_;
}


/**
 * Sets the ID for the entry.
 *
 * @param id The ID to set.
 */
void
HDBEntry::setID(RowID id) {
    hasID_ = true;
    id_ = id;
}


/**
 * Returns the ID of the entry.
 *
 * @return ID of the entry.
 */
RowID
HDBEntry::id() const 
    throw (NotAvailable) {

    if (!hasID()) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    } else {
        return id_;
    }
}


/**
 * Tells whether the entry contains a cost function.
 *
 * @return True if the entry contains a cost function.
 */
bool
HDBEntry::hasCostFunction() const {
    return costFunction_ != NULL;
}


/**
 * Returns the cost function of the entry.
 *
 * @return Cost function of the entry.
 * @exception NotAvailable If the entry doesn't have a cost function.
 */
CostFunctionPlugin&
HDBEntry::costFunction() const 
    throw (NotAvailable) {

    if (!hasCostFunction()) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    }

    return *costFunction_;
}


/**
 * Sets cost function for the entry.
 *
 * Deletes the old cost function if one exists.
 *
 * @param costFunction The new cost function.
 */
void
HDBEntry::setCostFunction(CostFunctionPlugin* costFunction) {
    if (hasCostFunction()) {
        delete costFunction_;
    }
    costFunction_ = costFunction;
}


/**
 * Returns the HDB file that contains the entry.
 *
 * @return The HDB file.
 */
std::string
HDBEntry::hdbFile() const {
    return hdbFile_;
}


/**
 * Sets the HDB file that contains the entry.
 *
 * @param file The HDB file.
 */
void
HDBEntry::setHDBFile(const std::string& file) {
    hdbFile_ = file;
}
}
