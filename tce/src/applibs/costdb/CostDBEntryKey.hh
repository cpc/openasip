/**
 * @file CostDBEntryKey.hh
 *
 * Declaration of CostDBEntryKey class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen@tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#ifndef TTA_COSTDB_ENTRY_KEY_HH
#define TTA_COSTDB_ENTRY_KEY_HH


#include <string>
#include <vector>

#include "CostDBTypes.hh"
#include "EntryKeyField.hh"
#include "Exception.hh"


/**
 * Represents the key of the CostDBEntry.
 *
 * Key properties are the properties of an entry that are used as a
 * search key.
 */
class CostDBEntryKey {
public:
    CostDBEntryKey(const EntryKeyProperty* entryType);
    virtual ~CostDBEntryKey();
    CostDBEntryKey* copy() const;

    const EntryKeyProperty* type() const;
    EntryKeyField keyFieldOfType(
        const EntryKeyFieldProperty& fieldType) const 
        throw (KeyNotFound);
    EntryKeyField keyFieldOfType(std::string fieldType) const;
    bool isEqual(const CostDBEntryKey& entryKey) const;

    void addField(EntryKeyField* field) 
        throw (ObjectAlreadyExists);
    void replaceField(EntryKeyField* newField) 
        throw (KeyNotFound);
    int fieldCount() const;
    const EntryKeyField& field(int index) const 
        throw (OutOfRange);

private:
    /// Table of entry fields.
    typedef std::vector<EntryKeyField*> FieldTable;

    /// Type of the entry key.
    const EntryKeyProperty* type_;
    /// Fields of the entry key.
    FieldTable fields_;

    /// Copying not allowed.
    CostDBEntryKey(const CostDBEntryKey&);
    /// Assignment not allowed.
    CostDBEntryKey& operator=(const CostDBEntryKey&);
};

#include "CostDBEntryKey.icc"

#endif
