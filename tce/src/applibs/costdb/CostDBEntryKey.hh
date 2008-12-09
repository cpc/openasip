/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file CostDBEntryKey.hh
 *
 * Declaration of CostDBEntryKey class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
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
