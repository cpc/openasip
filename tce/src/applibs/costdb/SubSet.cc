/**
 * @file SubSet.cc
 *
 * Implementation of SubSet class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#include <vector>

#include "SubSet.hh"

using std::vector;

/**
 * Constructor.
 *
 * @param type Type of the field.
 */
SubSet::SubSet(const EntryKeyFieldProperty* type) :
    SelectSet<&EntryKeyField::isSmaller, &EntryKeyField::isGreater>(type) {
}

/**
 * Destructor.
 */
SubSet::~SubSet() {
}
