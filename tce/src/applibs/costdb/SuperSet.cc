/**
 * @file SuperSet.cc
 *
 * Implementation of SuperSet class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#include <vector>

#include "SuperSet.hh"

using std::vector;

/**
 * Constructor.
 *
 * @param type Type of the field.
 */
SuperSet::SuperSet(const EntryKeyFieldProperty* type):
    SelectSet<&EntryKeyField::isGreater, &EntryKeyField::isSmaller>(type) {
}

/**
 * Destructor.
 */
SuperSet::~SuperSet() {
}
