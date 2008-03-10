/**
 * @file SearchStrategy.hh
 *
 * Declaration of SearchStrategy class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SEARCH_STRATEGY_HH
#define TTA_SEARCH_STRATEGY_HH


#include "MatchType.hh"
#include "CostDBEntry.hh"


/**
 * Interface for the classes implementing queries from the cost database.
 */
class SearchStrategy {
public:
    /**
     * Copies this SearchStrategy.
     *
     * @return Copy of this SearchStrategy.
     */
    virtual SearchStrategy* copy() const = 0;

    /**
     * Searches entries that match with certain search key on a specific
     * type of match.
     *
     * @param searchKey Search key.
     * @param components Entries from which to find.
     * @param match Type of match.
     * @return Entries matching search key and type of match.
     */
    virtual CostDBTypes::EntryTable search(
        const CostDBEntryKey& searchKey,
        CostDBTypes::EntryTable components,
        const CostDBTypes::MatchTypeTable& match) = 0;

    virtual ~SearchStrategy() {}
};

#endif
