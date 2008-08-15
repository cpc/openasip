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
 * @file InterPassData.cc
 *
 * Definition of InterPassData class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "InterPassData.hh"
#include "InterPassDatum.hh"
#include "MapTools.hh"

/**
 * Constructor.
 */
InterPassData::InterPassData() {
}

/**
 * Destructor.
 *
 * Deletes all added InterPassDatum objects.
 */
InterPassData::~InterPassData() {
    MapTools::deleteAllValues(data_);
}

/**
 * Returns a reference to the datum associated with the given key.
 *
 * It is allowed to modify the returned object directly.
 *
 * @param key The key.
 * @returns The datum.
 * @exception KeyNotFound thrown if the key is not found.
 */
InterPassDatum& 
InterPassData::datum(const std::string& key) 
    throw (KeyNotFound) {

    if (!MapTools::containsKey(data_, key))
        throw KeyNotFound(__FILE__, __LINE__, __func__);

    return *data_[key];
}

/**
 * Returns true if there is a datum for the given key.
 *
 * @return True if there is a datum for the given key.
 */
bool
InterPassData::hasDatum(const std::string& key) const {
    return MapTools::containsKey(data_, key);
}

/**
 * Sets a datum for the given key.
 *
 * Ownership of the datum is transferred. Possible previously existing
 * datum at the given key is deleted.
 *
 * @param key The key of the datum.
 * @param datum The datum object.
 */
void
InterPassData::setDatum(const std::string& key, InterPassDatum* datum) {

    if (MapTools::containsKey(data_, key))
        delete data_[key];

    data_[key] = datum;
}
