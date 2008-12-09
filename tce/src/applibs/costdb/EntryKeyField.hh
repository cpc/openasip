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
 * @file EntryKeyField.hh
 *
 * Declaration of EntryKeyField class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
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
