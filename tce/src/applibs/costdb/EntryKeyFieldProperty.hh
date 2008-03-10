/**
 * @file EntryKeyFieldProperty.hh
 *
 * Declaration of EntryKeyFieldProperty class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_ENTRY_KEY_FIELD_PROPERTY_HH
#define TTA_ENTRY_KEY_FIELD_PROPERTY_HH


#include <string>
#include "EntryKeyProperty.hh"


/**
 * Represents a type of field.
 *
 * For each EntryKeyField an EntryKeyFieldProperty should be assigned.
 */
class EntryKeyFieldProperty {
public:
    EntryKeyFieldProperty(std::string type, const EntryKeyProperty* entry);
    ~EntryKeyFieldProperty();

    std::string name() const;
    std::string entryName() const;
private:
    /// Field type.
    std::string name_;
    /// Entry type in which the field belongs to.
    const EntryKeyProperty* parent_;

    /// Copying not allowed.
    EntryKeyFieldProperty(const EntryKeyFieldProperty&);
    /// Assignment not allowed.
    EntryKeyFieldProperty& operator=(const EntryKeyFieldProperty&);
};

#include "EntryKeyFieldProperty.icc"

#endif
