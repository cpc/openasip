/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file EntryKeyProperty.hh
 *
 * Declaration of EntryKeyProperty class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
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
    EntryKeyFieldProperty* fieldProperty(std::string field) const;

    static EntryKeyProperty* create(std::string type);
    static EntryKeyProperty* find(std::string type);
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
