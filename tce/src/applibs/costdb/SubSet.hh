/**
 * @file SubSet.hh
 *
 * Declaration of SubSet class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SUB_SET_HH
#define TTA_SUB_SET_HH


#include "SelectSet.hh"


/**
 * Implementation for searching database entries that have smaller
 * value or a subset of the search key in the requested field.\ In
 * addition, equal values are accepted.
 */
class SubSet: public SelectSet<&EntryKeyField::isSmaller,
                               &EntryKeyField::isGreater> {
// The implementation utilizes SelectSet template class. Template
// parameter Select is EntryKeyField::isSmaller to accept smaller
// values in the results. Furthermore, UnSelect template parameter is
// EntryKeyField::isGreater to ignore greater values.
public:
    SubSet(const EntryKeyFieldProperty* type);
    virtual ~SubSet();

private:
    /// Copying not allowed.
    SubSet(const SubSet&);
    /// Assignment not allowed.
    SubSet& operator=(const SubSet&);
};

#endif
