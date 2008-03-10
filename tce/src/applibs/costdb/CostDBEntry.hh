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
