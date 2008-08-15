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
 * @file CostDBEntry.hh
 *
 * Declaration of CostDBEntry class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
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
    const CostDBEntryStats& statistics(int index) const throw (OutOfRange);

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
