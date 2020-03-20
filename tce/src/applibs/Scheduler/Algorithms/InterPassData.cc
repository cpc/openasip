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
 * @file InterPassData.cc
 *
 * Definition of InterPassData class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
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
InterPassData::datum(const std::string& key) {
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

void
InterPassData::removeDatum(const std::string& key) {
    if (MapTools::containsKey(data_, key)) {
        delete data_[key];
        data_.erase(key);
    }    
}
