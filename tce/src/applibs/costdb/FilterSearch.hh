/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file FilterSearch.hh
 *
 * Declaration of FilterSearch class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_FILTER_SEARCH_HH
#define TTA_FILTER_SEARCH_HH


#include <vector>

#include "CostDBTypes.hh"
#include "SearchStrategy.hh"
#include "MatchType.hh"
#include "CostDBEntry.hh"
#include "Matcher.hh"
#include "Exception.hh"


/**
 * Implementation of the queries from the cost database.
 *
 * Using the FilterSearch algorithm requires only a Search() function
 * call.
 *
 * Search is based on filtering non-matching entries out according to
 * information of one field at a time. Thus, it is very generic
 * accepting field changes to entries without touching to the searrch
 * algorithms.
 *
 * Cache and quick filtering is used for optimisation. Cache contains
 * all the results from the previous queries. Quick filtering removes
 * unneccessary entries in linear time in the beginning of the search.
 */
class FilterSearch: public SearchStrategy {
public:
    FilterSearch();
    virtual ~FilterSearch();

    SearchStrategy* copy() const;
    CostDBTypes::EntryTable search(
        const CostDBEntryKey& searchKey,
        CostDBTypes::EntryTable components,
        const CostDBTypes::MatchTypeTable& match);

private:
    /**
     * Represents a cache entry.
     */
    class Cache {
        friend class FilterSearchTest;
    public:
	Cache(
            CostDBTypes::MatchTypeTable matchingType,
            CostDBEntryKey* key,
            CostDBTypes::EntryTable& entry);
	~Cache();
        Cache* copy() const;
	bool isEqual(
            CostDBTypes::MatchTypeTable matchingType,
            const CostDBEntryKey* key) const;
	CostDBTypes::EntryTable entries() const;
    private:
        /// Type of match used for these results.
	CostDBTypes::MatchTypeTable matchType_;
        /// Search key of the query for these results.
	CostDBEntryKey* searchKey_;
        /// Resulting database entries. Not owned by this class.
	CostDBTypes::EntryTable entries_;

        /// Copying not allowed.
        Cache(const Cache&);
        /// Assignment not allowed.
        Cache& operator=(const Cache&);
    };

    /// Table of cache entries.
    typedef std::vector<Cache*> CacheTable;
    /// Table of matcher types.
    typedef std::vector<Matcher*> MatcherTable;

    CostDBTypes::EntryTable findFromCache(
        const CostDBEntryKey& searchKey,
        const CostDBTypes::MatchTypeTable& match);
    MatcherTable createMatchers(const CostDBTypes::MatchTypeTable& match)
	throw (TypeMismatch);

    /// Results of the previous queries.
    CacheTable entryCache_;
    /// Storage for all matchers. They cannot be deleted before search
    /// strategy itself is deleted. Thus, this storage exists to
    /// deallocate the memory reserved by matchers.
    MatcherTable matcherStorage_;

    /// Copying not allowed.
    FilterSearch(const FilterSearch&);
    /// Assignment not allowed.
    FilterSearch& operator=(const FilterSearch&);
};

#endif
