/**
 * @file SuperSet.hh
 *
 * Declaration of SuperSet class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SUPER_SET_HH
#define TTA_SUPER_SET_HH


#include "SelectSet.hh"


/**
 * Implementation for searching database entries that have greater
 * value or a superset of the search key in the requested field.\ In
 * addition, equal values are accepted.
 */
class SuperSet: public SelectSet<&EntryKeyField::isGreater,
                                 &EntryKeyField::isSmaller> {
// The implementation utilizes SelectSet template class. Template
// parameter Select is EntryKeyField::isGreater to accept greater
// values in the results. Furthermore, UnSelect template parameter is
// EntryKeyField::isSmaller to ignore smaller values.
public:
    SuperSet(const EntryKeyFieldProperty* type);
    virtual ~SuperSet();

private:
    /// Copying not allowed.
    SuperSet(const SuperSet&);
    /// Assignment not allowed.
    SuperSet& operator=(const SuperSet&);
};

#endif
