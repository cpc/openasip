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
 * @file Interpolation.hh
 *
 * Declaration of Interpolation class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
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
        const CostDBEntryKey& searchKey, CostDBTypes::EntryTable& components);

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
