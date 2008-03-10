/**
 * @file MatchType.hh
 *
 * Declaration of MatchType class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_MATCH_TYPE_HH
#define TTA_MATCH_TYPE_HH


#include "CostDBTypes.hh"
#include "CostDBEntry.hh"
#include "CostDBEntryKey.hh"
#include "EntryKeyFieldProperty.hh"

class CostDatabase;


/**
 * Represents a type of match for a certain field.
 */
class MatchType {
public:
    MatchType(
        const EntryKeyFieldProperty* field,
        CostDBTypes::TypeOfMatch match);
    virtual ~MatchType();
    MatchType(const MatchType& old);

    const EntryKeyFieldProperty* fieldType() const;
    CostDBTypes::TypeOfMatch matchingType() const;
    bool isEqual(const MatchType& m) const;

private:
    /// Type of field.
    const EntryKeyFieldProperty* fieldType_;
    /// Type of match.
    CostDBTypes::TypeOfMatch matchType_;

    /// Assignment not allowed.
    MatchType& operator=(const MatchType&);
};

#include "MatchType.icc"

#endif
