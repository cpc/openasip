/**
 * @file MatchType.cc
 *
 * Implementation of MatchType class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#include "MatchType.hh"


/**
 * Constructor.
 *
 * @param field Type of field.
 * @param match Type of match.
 */
MatchType::MatchType(
    const EntryKeyFieldProperty* field,
    CostDBTypes::TypeOfMatch match):
    fieldType_(field), matchType_(match) {
}

/**
 * Destructor.
 */
MatchType::~MatchType() {
}

/**
 * Copy constructor.
 *
 * @param old Match type.
 */
MatchType::MatchType(const MatchType& old) :
    fieldType_(old.fieldType_), matchType_(old.matchType_) {
}

/**
 * Compares if two match types are equal.
 *
 * @param m A match type.
 * @return True if match types are equal.
 */
bool
MatchType::isEqual(const MatchType& m) const {
    return fieldType() == m.fieldType() && matchingType() == m.matchingType();
}
