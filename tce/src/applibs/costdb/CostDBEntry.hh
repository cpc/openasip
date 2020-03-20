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
 * @file CostDBEntry.hh
 *
 * Declaration of CostDBEntry class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_COSTDB_ENTRY_HH
#define TTA_COSTDB_ENTRY_HH


#include <string>

#include "CostDBEntryKey.hh"
#include "CostDBEntryStats.hh"
#include "EntryKeyField.hh"
#include "EntryKeyFieldProperty.hh"
#include "Exception.hh"


/**
 * Represents an entry containing a key and statistics.
 *
 * Key means the properties of an entry that are used as a search key.
 */
class CostDBEntry {
public:
    CostDBEntry(CostDBEntryKey* key);
    CostDBEntry(
        const CostDBEntry& entry1,
        const CostDBEntry& entry2,
        const EntryKeyField& weighter);
    virtual ~CostDBEntry();
    CostDBEntry* copy() const;

    EntryKeyField keyFieldOfType(const EntryKeyFieldProperty& type) const;
    EntryKeyField keyFieldOfType(std::string type) const;
    const EntryKeyProperty* type() const;

    void replaceField(EntryKeyField* newField);
    int fieldCount() const;
    const EntryKeyField& field(int index) const;
    bool isEqualKey(const CostDBEntry& entry) const;

    void addStatistics(CostDBEntryStats* newStats);
    int statisticsCount() const;
    const CostDBEntryStats& statistics(int index) const;

private:
    /// Type definition for a table of statistics.
    typedef std::vector<CostDBEntryStats*> StatsTable;

    /// Key of the entry.
    CostDBEntryKey* entryKey_;
    /// Statistics of the entry.
    StatsTable entryStats_;

    /// Copying not allowed.
    CostDBEntry(const CostDBEntry&);
    /// Assignment not allowed.
    CostDBEntry& operator=(const CostDBEntry&);
};

#endif
