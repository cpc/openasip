/**
 * @file Interpolation.hh
 *
 * Declaration of Interpolation class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_INTERPOLATION_HH
#define TTA_INTERPOLATION_HH


#include "Matcher.hh"
#include "Exception.hh"


/**
 * Implementation for searching database entries.
 *
 * If a value equal to the search key in the requested field is not
 * found, interpolation of the smaller and greater field value is
 * tried.
 */
class Interpolation: public Matcher {
public:
    Interpolation(const EntryKeyFieldProperty* type);
    virtual ~Interpolation();
    
    void quickFilter(const CostDBEntryKey&, CostDBTypes::EntryTable&);
    void filter(
        const CostDBEntryKey& searchKey,
        CostDBTypes::EntryTable& components)
        throw (TypeMismatch, KeyNotFound);

private:
    /**
     * Represents an entry pair.
     */
    struct Pair {
        /// An entry.
        CostDBEntry* smaller;
        /// An entry.
        CostDBEntry* greater;
    };

    /// Entries created during interpolation.
    CostDBTypes::EntryTable created_;

    /// Copying not allowed.
    Interpolation(const Interpolation&);
    /// Assignment not allowed.
    Interpolation& operator=(const Interpolation&);
};

#endif
