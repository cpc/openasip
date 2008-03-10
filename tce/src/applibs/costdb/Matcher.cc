/**
 * @file Matcher.cc
 *
 * Implementation of Matcher class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#include "Matcher.hh"


/**
 * Constructor.
 *
 * Marks the field type to which a search is aplied.
 *
 * @param type Type of the field.
 */
Matcher::Matcher(const EntryKeyFieldProperty* type) :
    fieldType_(type) {
}

/**
 * Destructor.
 */
Matcher::~Matcher() {
}

/**
 * Checks if only a specific field of two entries is different and all
 * other fields have equal values.
 *
 * @param type Type of the field.
 * @param entry1 An entry.
 * @param entry2 An entry.
 * @return True if only one field of two entries is different, otherwise false.
 */
bool
Matcher::onlyThisFieldDiffers(
    const EntryKeyFieldProperty* type,
    const CostDBEntry& entry1,
    const CostDBEntry& entry2) const {

    for (int i = 0; i < entry1.fieldCount(); i++) {
        const EntryKeyField& j = entry1.field(i);

	if (!j.isEqual(entry2.keyFieldOfType(*j.type())) &&
	    !(j.type() == type)) {

	    return false;
	}
    }
    return true;
}
