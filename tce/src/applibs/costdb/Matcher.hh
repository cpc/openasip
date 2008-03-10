/**
 * @file Matcher.hh
 *
 * Declaration of Matcher class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_MATCHER_HH
#define TTA_MATCHER_HH


#include "CostDBEntry.hh"
#include "CostDBEntryKey.hh"
#include "EntryKeyFieldProperty.hh"


/**
 * Interface for searching database entries.
 *
 * Search is based only on the information of one specific field of
 * the entries. Using the interface for queries requires at first a
 * QuickFilter() call and then a Filter() call.
 */
class Matcher {
public:
    Matcher(const EntryKeyFieldProperty* type);
    virtual ~Matcher();

    /**
     * Filters out unnecessary entries in linear time.
     *
     * @param searchKey Search key.
     * @param components Entries from which to find. Updated to contain
     *                   entries that matched the search request.
     */
    virtual void quickFilter(
        const CostDBEntryKey& searchKey,
        CostDBTypes::EntryTable& components) = 0;

    /**
     * Searches for database entries that match the search key.
     */
    virtual void filter(const CostDBEntryKey&, CostDBTypes::EntryTable&) = 0;

protected:
    const EntryKeyFieldProperty* fieldType() const;
    bool onlyThisFieldDiffers(
        const EntryKeyFieldProperty* type,
        const CostDBEntry& entry1,
        const CostDBEntry& entry2) const;

private:
    /// Copying not allowed.
    Matcher(const Matcher&);
    /// Assignment not allowed.
    Matcher& operator=(const Matcher&);

    /// Type of the field on which filtering is applied to.
    const EntryKeyFieldProperty* fieldType_;
};

#include "Matcher.icc"

#endif
