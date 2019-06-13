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
 * @file Interpolation.cc
 *
 * Implementation of Interpolation class.
 *
 * @author Tommi Rantanen 2003 (tommi.rantanen-no.spam-tut.fi)
 * @author Jari Mäntyneva 2005 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */

#include <vector>

#include "Interpolation.hh"
#include "Application.hh"

using std::vector;

/**
 * Constructor.
 *
 * @param type Type of the field.
 */
Interpolation::Interpolation(const EntryKeyFieldProperty* type) :
    Matcher(type) {
}

/**
 * Destructor.
 *
 * Deallocates memory reserved for entries created during interpolation.
 */
Interpolation::~Interpolation() {

    for (CostDBTypes::EntryTable::iterator i = created_.begin();
         i != created_.end(); i++) {
        
        assert(*i != NULL);
        delete *i;
        *i = NULL;
    }
}

/**
 * Nothing to do since no entry can be removed in linear time.
 */
void
Interpolation::quickFilter(const CostDBEntryKey&, CostDBTypes::EntryTable&) {
}

/**
 * Searches for database entries.
 *
 * If a value equal to the search key in the requested field is not
 * found, interpolation of the smaller and greater field value is
 * applied. Only the best matches are returned, i.e. equal match is
 * chosen and no interpolation is done, or if two entries has only the
 * field to which search is applied different, the closer one to the
 * search key is chosen for interpolation.
 *
 * @param searchKey Search key.
 * @param components Entries from which to find. Updated to contain
 *                   entries that matched the search request.
 * @exception TypeMismatch Interpolation was requested for field that cannot
 * be interpolated.
 * @exception KeyNotFound Some CostDBEntryStats have no value caused by
 * missing cost data.
 */
void
Interpolation::filter(
    const CostDBEntryKey& searchKey, CostDBTypes::EntryTable& components) {
    vector<Pair> entries;
    EntryKeyField searchField = searchKey.keyFieldOfType(*fieldType());
    for (CostDBTypes::EntryTable::iterator i = components.begin();
         i != components.end(); i++) {

        EntryKeyField field = (*i)->keyFieldOfType(*fieldType());
        bool newPair = true;
        for (vector<Pair>::iterator p = entries.begin();
             p != entries.end(); p++) {

            if ((p->smaller != 0 && 
                 !onlyThisFieldDiffers(fieldType(), *(p->smaller), *(*i))) ||
                (p->greater != 0 &&
                 !onlyThisFieldDiffers(fieldType(), *(p->greater), *(*i)))) {
            
                continue;
            }
            if (field.isEqual(searchField)) {
                p->smaller = *i;
                p->greater = 0;
            } else if (field.isSmaller(searchField)) {
                if (p->smaller == 0 ||
                    (p->smaller != 0 &&
                     field.isGreater(
                         p->smaller->keyFieldOfType(*fieldType())))) {

                    p->smaller = *i;
                }
            } else {
                if (!(field.isGreater(searchField))) {
                    throw TypeMismatch(__FILE__, __LINE__,
                                       "Interpolation::filter");
                }
                if (p->greater == 0 ||
                    (p->greater != 0 &&
                     field.isSmaller(
                         p->greater->keyFieldOfType(*fieldType())))) {

                    p->greater = *i;
                }
            }
            newPair = false;
            break;
        }
        if (newPair) {
            Pair pair;
            if (field.isEqual(searchField)) {
                pair.smaller = *i;
                pair.greater = 0;
            } else if (field.isSmaller(searchField)) {
                pair.smaller = *i;
                pair.greater = 0;
            } else {
                if (!(field.isGreater(searchField))) {
                    throw TypeMismatch(__FILE__, __LINE__,
                                       "Interpolation::filter");
                }
                pair.greater = *i;
                pair.smaller = 0;
            }
            entries.push_back(pair);
        }
    }
    CostDBTypes::EntryTable filtered;
    for (vector<Pair>::iterator p = entries.begin(); p != entries.end(); p++) {
        if (p->smaller != 0 &&
            p->smaller->keyFieldOfType(*fieldType()).isEqual(searchField)) {
            filtered.push_back(p->smaller);
        } else if (p->smaller != 0 && p->greater != 0) {
            CostDBEntry* newEntry = new CostDBEntry(
                *p->smaller, *p->greater, searchField);
            filtered.push_back(newEntry);
            created_.push_back(newEntry);
        }
    }
    components = filtered;
}
