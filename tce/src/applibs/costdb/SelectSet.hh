/**
 * @file SelectSet.hh
 *
 * Declaration of SelectSet class.
 *
 * @author Tommi Rantanen 2004 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SELECT_SET_HH
#define TTA_SELECT_SET_HH


#include "Matcher.hh"


/**
 * Implementation for searching database entries that satisfies
 * selection criteria.
 *
 * However, equal values are also accepted.
 *
 * The selection criteria as well as the criteria for not selecting
 * some entry are given as template parameter.
 */
template <bool (EntryKeyField::*select)(const EntryKeyField&) const,
          bool (EntryKeyField::*unSelect)(const EntryKeyField&) const>
class SelectSet: public Matcher {
public:
    SelectSet(const EntryKeyFieldProperty* type);
    virtual ~SelectSet();

    virtual void quickFilter(
        const CostDBEntryKey& searchKey,
        CostDBTypes::EntryTable& components);
    virtual void filter(
        const CostDBEntryKey&,
        CostDBTypes::EntryTable& components);

private:
    /// Copying not allowed.
    SelectSet(const SelectSet&);
    /// Assignment not allowed.
    SelectSet& operator=(const SelectSet&);
};

#include "SelectSet.icc"

#endif
