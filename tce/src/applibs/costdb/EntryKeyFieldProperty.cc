/**
 * @file EntryKeyFieldProperty.cc
 *
 * Implementation of EntryKeyFieldProperty class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#include <string>
#include "EntryKeyFieldProperty.hh"

using std::string;


/**
 * Constructor.
 *
 * @param type Field type.
 * @param entry Entry type in which the field belongs to.
 */
EntryKeyFieldProperty::EntryKeyFieldProperty(
    string type,
    const EntryKeyProperty* entry):
    name_(type), parent_(entry) {
}

/**
 * Destructor.
 */
EntryKeyFieldProperty::~EntryKeyFieldProperty() {
}

/**
 * Returns entry type of the field.
 *
 * @return Entry type of the field.
 */
std::string
EntryKeyFieldProperty::entryName() const {
    return parent_->name();
}
