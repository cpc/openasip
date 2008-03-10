/**
 * @file ExactMatch.cc
 *
 * Implementation of ExactMatch class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#include <vector>

#include "ExactMatch.hh"

using std::vector;


/**
 * Constructor.
 *
 * @param type Type of the field.
 */
ExactMatch::ExactMatch(const EntryKeyFieldProperty* type) :
    Matcher(type) {
}

/**
 * Destructor.
 */
ExactMatch::~ExactMatch() {
}

/**
 * filters out values unequal to the search key.
 *
 * @param searchKey Search key.
 * @param components Entries from which to find. Updated to contain
 *                   entries that matched the search request.
 */
void
ExactMatch::quickFilter(
    const CostDBEntryKey& searchKey,
    CostDBTypes::EntryTable& components) {

    CostDBTypes::EntryTable filtered;
    for (CostDBTypes::EntryTable::iterator i = components.begin();
	 i != components.end(); i++) {

	if ((*i)->keyFieldOfType(*fieldType()).isEqual(
		searchKey.keyFieldOfType(*fieldType()))) {

	    filtered.push_back(*i);
	}
    }
    components = filtered;
}

/**
 * Nothing to do since quick filtering already accepted all possible
 * entries.
 */
void
ExactMatch::filter(
    const CostDBEntryKey&,
    CostDBTypes::EntryTable&) {
}
