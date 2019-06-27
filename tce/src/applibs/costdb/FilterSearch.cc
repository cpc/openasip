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
 * @file FilterSearch.cc
 *
 * Implementation of FilterSearch class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari M�ntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include <vector>

#include "FilterSearch.hh"
#include "ExactMatch.hh"
#include "SubSet.hh"
#include "SuperSet.hh"
#include "Interpolation.hh"
#include "Application.hh"


/**
 * Default constructor.
 */
FilterSearch::FilterSearch() {
}

/**
 * Destructor.
 */
FilterSearch::~FilterSearch() {
    
    for (CacheTable::iterator i = entryCache_.begin();
         i != entryCache_.end(); i++) {
        
        assert(*i != NULL);
        delete *i;
        *i = NULL;
    }
    for (MatcherTable::iterator i = matcherStorage_.begin();
         i != matcherStorage_.end(); i++) {
        
        assert(*i != NULL);
        delete *i;
        *i = NULL;
    }
}

/**
 * Returns a copy of this search strategy.
 *
 * Client is responsible of deallocating the memory reserved for the
 * returned object.
 *
 * @return Copy of this search strategy.
 */
SearchStrategy*
FilterSearch::copy() const {
    
    CacheTable newEntryCache;
    for (CacheTable::const_iterator i = entryCache_.begin();
         i != entryCache_.end(); i++) {
        
        newEntryCache.push_back((*i)->copy());
    }
    FilterSearch* newSearch = new FilterSearch();
    newSearch->entryCache_ = newEntryCache;
    return newSearch;
}

/**
 * Searches entries that match with certain search key on a specific
 * type of match.
 *
 * Search results are valid until the strategy is deleted, after which
 * the use of the results of the queries might lead into unexpected
 * behaviour.
 *
 * Client must not deallocate the memory reserved for the results.
 *
 * @param searchKey Search key.
 * @param components Entries from which to find.
 * @param match Type of match. The fields are searched in the order
 *              they exist in this table.
 * @return Entries matching search key and type of match.
 */
CostDBTypes::EntryTable
FilterSearch::search(
    const CostDBEntryKey& searchKey,
    CostDBTypes::EntryTable components,
    const CostDBTypes::MatchTypeTable& match) {
    
    if (components.size() == 0) {
        return components;
    }
    
    // check the cache
    CostDBTypes::EntryTable cache_entries = findFromCache(searchKey, match);
    if (!cache_entries.empty()) {
        return cache_entries;
    }
    
    MatcherTable matcher = createMatchers(match);
    
    // filter quickly poor ones out
    for (MatcherTable::const_iterator i = matcher.begin();
         i != matcher.end(); i++) {
        
        (*i)->quickFilter(searchKey, components);
    }
    
    // choose correct entries from acceptable ones
    for (MatcherTable::const_iterator i = matcher.begin();
         i != matcher.end(); i++) {
        
        (*i)->filter(searchKey, components);
    }
    
    // insert found entries into cache
    entryCache_.push_back(new Cache(match, searchKey.copy(), components));
    
    return components;
}

/**
 * Finds entries matching search key and type of match from the cache.
 *
 * @param searchKey Search key.
 * @param match Type of match.
 * @return Entries matching search key and type of match.
 */
CostDBTypes::EntryTable
FilterSearch::findFromCache(
    const CostDBEntryKey& searchKey,
    const CostDBTypes::MatchTypeTable& match) {
    
    CostDBTypes::EntryTable cacheEntries;
    for (CacheTable::iterator i = entryCache_.begin();
         i != entryCache_.end(); i++) {
        
        if ((*i)->isEqual(match, &searchKey)) {
            cacheEntries = (*i)->entries();
            break;
        }
    }
    return cacheEntries;
}

/**
 * Creates sub components of this search strategy.
 *
 * @param match Type of match.
 * @return Sub components of this search strategy.
 * @exception TypeMismatch Requested type is unknown.
 */
FilterSearch::MatcherTable
FilterSearch::createMatchers(const CostDBTypes::MatchTypeTable& match) {
    MatcherTable matcher;
    for (CostDBTypes::MatchTypeTable::const_iterator i = match.begin();
         i != match.end(); i++) {
        
        Matcher* new_matcher = NULL;
        if ((*i)->matchingType() == CostDBTypes::MATCH_EXACT) {
            new_matcher = new ExactMatch((*i)->fieldType());
        } else if ((*i)->matchingType() == CostDBTypes::MATCH_SUBSET) {
            new_matcher = new SubSet((*i)->fieldType());
        } else if ((*i)->matchingType() == CostDBTypes::MATCH_SUPERSET) {
            new_matcher = new SuperSet((*i)->fieldType());
        } else if ((*i)->matchingType() == CostDBTypes::MATCH_INTERPOLATION) {
            new_matcher = new Interpolation((*i)->fieldType());
        } else if ((*i)->matchingType() == CostDBTypes::MATCH_ALL) {
            
        } else {
            throw TypeMismatch(__FILE__, __LINE__,
                               "FilterSearch::createMatchers");
        }
        
        assert(new_matcher != NULL);
        matcherStorage_.push_back(new_matcher);
        matcher.push_back(new_matcher);
    }
    return matcher;
}

//////////////////////////////////////////////////
// FilterSearch::Cache
//////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param matchingType Type of match.
 * @param key Search key.
 * @param entry Database entries.
 */
FilterSearch::Cache::Cache(
    CostDBTypes::MatchTypeTable matchingType,
    CostDBEntryKey* key,
    CostDBTypes::EntryTable& entry):
    searchKey_(key), entries_(entry) {
    
    for (CostDBTypes::MatchTypeTable::iterator i = matchingType.begin();
         i != matchingType.end(); i++) {
        matchType_.push_back(new MatchType(*(*i)));
    }
}

/**
 * Destructor.
 *
 * Deallocates memory reserved for search key and type of match. Not
 * responsible of deleting entries.
 */
FilterSearch::Cache::~Cache() {
    
    assert(searchKey_ != NULL);
    delete searchKey_;
    searchKey_ = NULL;
    
    for (CostDBTypes::MatchTypeTable::iterator i = matchType_.begin();
         i != matchType_.end(); i++) {
        
        assert(*i != NULL);
        delete *i;
        *i = NULL;
    }
}

/**
 * Returns a copy of this cache.
 *
 * Client is responsible of deallocating the memory reserved for the
 * returned object.
 *
 * @return Copy of this cache.
 */
FilterSearch::Cache*
FilterSearch::Cache::copy() const {
    
    CostDBEntryKey* newSearchKey = searchKey_->copy();
    CostDBTypes::MatchTypeTable newMatchType;
    
    for (CostDBTypes::MatchTypeTable::const_iterator i = matchType_.begin();
         i != matchType_.end(); i++) {
        newMatchType.push_back(
            new MatchType((*i)->fieldType(), (*i)->matchingType()));
    }
    
    CostDBTypes::EntryTable newEntries;
    for (CostDBTypes::EntryTable::const_iterator i = entries_.begin();
         i != entries_.end(); i++) {
        newEntries.push_back(*i);
    }
    
    return new FilterSearch::Cache(newMatchType, newSearchKey, newEntries);
}

/**
 * Tests if cache matches to the type of match and search key.
 *
 * @param matchingType Type of match.
 * @param key Search key.
 * @return true If cache matches to the type of match and search key,
 *              false otherwise.
 */
bool
FilterSearch::Cache::isEqual(
    CostDBTypes::MatchTypeTable matchingType,
    const CostDBEntryKey* key) const {
    
    // warning: there could be different keys that yield the same result
    if (!searchKey_->isEqual(*key)) {
        return false;
    }
    
    // warning: a search may succeed if it's cached and fail if not
    // since the order of match types accepted is insignificant
    for (CostDBTypes::MatchTypeTable::const_iterator i = matchingType.begin();
         i != matchingType.end(); i++) {
        
        bool isThere = false;
        for (CostDBTypes::MatchTypeTable::const_iterator j =
                 matchType_.begin(); j != matchType_.end(); j++) {
            
            if ((*i)->isEqual(*(*j))) {
                isThere = true;
                break;
            }
        }
        if (!isThere) {
            return false;
        }
    }
    return true;
}

/**
 * Returns entries.
 *
 * @return Cached entries.
 */
CostDBTypes::EntryTable
FilterSearch::Cache::entries() const {
    return entries_;
}
