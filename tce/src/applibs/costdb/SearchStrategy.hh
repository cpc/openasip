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
 * @file SearchStrategy.hh
 *
 * Declaration of SearchStrategy class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
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
