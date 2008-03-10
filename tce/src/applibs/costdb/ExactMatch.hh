/**
 * @file ExactMatch.hh
 *
 * Declaration of ExactMatch class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_EXACT_MATCH_HH
#define TTA_EXACT_MATCH_HH


#include "CostDBTypes.hh"
#include "Matcher.hh"


/**
 * Implementation for searching database entries that are equal to the
 * search key in the requested field.
 */
class ExactMatch: public Matcher {
public:
    ExactMatch(const EntryKeyFieldProperty* type);
    virtual ~ExactMatch();

    void quickFilter(
        const CostDBEntryKey& searchKey,
        CostDBTypes::EntryTable& components);
    void filter(const CostDBEntryKey&, CostDBTypes::EntryTable&);

private:
    /// Copying not allowed.
    ExactMatch(const ExactMatch&);
    /// Assignment not allowed.
    ExactMatch& operator=(const ExactMatch&);
};

#endif
