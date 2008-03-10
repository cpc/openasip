/**
 * @file EntryKeyProperty.hh
 *
 * Declaration of EntryKeyProperty class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_ENTRY_KEY_PROPERTY_HH
#define TTA_ENTRY_KEY_PROPERTY_HH

#include <vector>
#include <string>

#include "Exception.hh"

class EntryKeyFieldProperty;


/**
 * Represents a type of an CostDBEntryKey which contains types of its
 * fields, that is, EntryKeyFields.
 *
 * Using the EntryKeyProperty usually includes finding of the specific
 * EntryKeyProperty with Find(). Then the types of EntryKeyProperty's
 * fields can be accessed using FieldProperty(). Creation of the types
 * is done similarly using first Create() and then
 * CreateFieldProperty().
 *
 * For each CostDBEntryKey an EntryKeyProperty should be assigned.
 */
class EntryKeyProperty {
public:
    ~EntryKeyProperty();

    std::string name() const;
    EntryKeyFieldProperty* createFieldProperty(std::string field);
    EntryKeyFieldProperty* fieldProperty(std::string field) const
        throw (KeyNotFound);

    static EntryKeyProperty* create(std::string type);
    static EntryKeyProperty* find(std::string type) 
        throw (KeyNotFound);
    static void destroy();

protected:
    EntryKeyProperty(std::string propertyName);

private:
    /// Table of field types.
    typedef std::vector<EntryKeyFieldProperty*> FieldPropertyTable;
    /// Table of entry types.
    typedef std::vector<EntryKeyProperty*> EntryPropertyTable;

    /// Entry type.
    std::string name_;
    /// Field types of the entry type.
    FieldPropertyTable entryFields_;

    /// All known entry types.
    static EntryPropertyTable entryTypes_;

    /// Copying not allowed.
    EntryKeyProperty(const EntryKeyProperty&);
    /// Assignment not allowed.
    EntryKeyProperty& operator=(const EntryKeyProperty&);
};

#include "EntryKeyProperty.icc"

#endif
