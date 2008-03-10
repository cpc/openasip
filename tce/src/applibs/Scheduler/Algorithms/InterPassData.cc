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
