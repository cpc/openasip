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
 * @file HDBEntry.cc
 *
 * Implementation of HDBEntry class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "HDBEntry.hh"
#include "CostFunctionPlugin.hh"

namespace HDB {

/**
 * The constructor.
 */
HDBEntry::HDBEntry() :
    hasID_(false), id_(0), costFunction_(NULL), hdbFile_("") {
}


/**
 * The destructor.
 */
HDBEntry::~HDBEntry() {
    if (hasCostFunction()) {
        delete costFunction_;
    }
}


/**
 * Tells whether the entry has an ID.
 *
 * @return True if the entry has an ID, otherwise false.
 */
bool
HDBEntry::hasID() const {
    return hasID_;
}


/**
 * Sets the ID for the entry.
 *
 * @param id The ID to set.
 */
void
HDBEntry::setID(RowID id) {
    hasID_ = true;
    id_ = id;
}


/**
 * Returns the ID of the entry.
 *
 * @return ID of the entry.
 */
RowID
HDBEntry::id() const {
    if (!hasID()) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    } else {
        return id_;
    }
}

/**
 * Tells whether the entry contains a cost function.
 *
 * @return True if the entry contains a cost function.
 */
bool
HDBEntry::hasCostFunction() const {
    return costFunction_ != NULL;
}


/**
 * Returns the cost function of the entry.
 *
 * @return Cost function of the entry.
 * @exception NotAvailable If the entry doesn't have a cost function.
 */
CostFunctionPlugin&
HDBEntry::costFunction() const {
    if (!hasCostFunction()) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    }

    return *costFunction_;
}

/**
 * Sets cost function for the entry.
 *
 * Deletes the old cost function if one exists.
 *
 * @param costFunction The new cost function.
 */
void
HDBEntry::setCostFunction(CostFunctionPlugin* costFunction) {
    if (hasCostFunction()) {
        delete costFunction_;
    }
    costFunction_ = costFunction;
}


/**
 * Returns the HDB file that contains the entry.
 *
 * @return The HDB file.
 */
std::string
HDBEntry::hdbFile() const {
    return hdbFile_;
}


/**
 * Sets the HDB file that contains the entry.
 *
 * @param file The HDB file.
 */
void
HDBEntry::setHDBFile(const std::string& file) {
    hdbFile_ = file;
}
}
