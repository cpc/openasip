/**
 * @file EntryKeyField.hh
 *
 * Declaration of EntryKeyField class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_ENTRY_KEY_FIELD_HH
#define TTA_ENTRY_KEY_FIELD_HH


#include "EntryKeyData.hh"
#include "EntryKeyFieldProperty.hh"


/**
 * Represents a field containing a value and a type.
 */
class EntryKeyField {
public:
    EntryKeyField(EntryKeyData* fieldData, const EntryKeyFieldProperty* type);
    virtual ~EntryKeyField();
    EntryKeyField(const EntryKeyField& old);
    EntryKeyField& operator=(const EntryKeyField& old);

    bool isEqual(const EntryKeyField& field) const;
    bool isGreater(const EntryKeyField& field) const;
    bool isSmaller(const EntryKeyField& field) const;
    double coefficient(const EntryKeyField& field1,
                       const EntryKeyField& field2) const;
    std::string toString() const;
    const EntryKeyFieldProperty* type() const;

private:
    /// Value of the field
    EntryKeyData* data_;
    /// Type of the field
    const EntryKeyFieldProperty* properties_;
};

#include "EntryKeyField.icc"

#endif
