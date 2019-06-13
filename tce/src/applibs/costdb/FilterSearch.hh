/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file FilterSearch.hh
 *
 * Declaration of FilterSearch class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
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
    MatcherTable createMatchers(const CostDBTypes::MatchTypeTable& match);

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
