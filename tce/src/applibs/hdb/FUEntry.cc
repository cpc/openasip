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
 * @file FUEntry.cc
 *
 * Implementation of FUEntry class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "FUEntry.hh"
#include "FUArchitecture.hh"
#include "FUImplementation.hh"
#include "Application.hh"

using std::string;

namespace HDB {

/**
 * The constructor.
 *
 * Creates an FU entry without ID, implementation, architecture and 
 * cost function.
 */
FUEntry::FUEntry() :
    HDBEntry(), architecture_(NULL), implementation_(NULL) {
}
    

/**
 * The destructor.
 */
FUEntry::~FUEntry() {
    if (hasArchitecture()) {
        delete architecture_;
    }
    if (hasImplementation()) {
        delete implementation_;
    }
}


/**
 * Tells whether the entry contains implementation data.
 *
 * @return True if the entry contains implementation data, otherwise false.
 */
bool
FUEntry::hasImplementation() const {
    return implementation_ != NULL;
}


/**
 * Returns the implementation of the entry.
 *
 * @return The implementation.
 * @exception NotAvailable If there is no implementation of the entry.
 */
FUImplementation&
FUEntry::implementation() const {
    if (!hasImplementation()) {
        const string procName = "FUEntry::implementation";
        throw NotAvailable(__FILE__, __LINE__, procName);
    }

    return *implementation_;
}

/**
 * Sets the given implementation for the entry.
 *
 * Deletes old implementation if one exists.
 *
 * @param implementation The implementation.
 */
void
FUEntry::setImplementation(FUImplementation* implementation) {
    if (hasImplementation()) {
        delete implementation_;
    }
    implementation_ = implementation;
}


/**
 * Tells whether the entry contains architecture data.
 *
 * @return True if the entry contains architecture data, otherwise false.
 */
bool
FUEntry::hasArchitecture() const {
    return architecture_ != NULL;
}


/**
 * Returns the architecture of the entry.
 *
 * @return Architecture of the entry.
 * @exception NotAvailable If the entry doesn't have architecture.
 */
FUArchitecture&
FUEntry::architecture() const {
    if (!hasArchitecture()) {
        const string procName = "FUEntry::architecture";
        throw NotAvailable(__FILE__, __LINE__, procName);
    }

    return *architecture_;
}

/**
 * Sets architecture of the entry.
 *
 * Deletes the old architecture if one exists.
 *
 * @param architecture The new architecture.
 */
void
FUEntry::setArchitecture(FUArchitecture* architecture) {
    if (hasArchitecture()) {
        delete architecture_;
    }
    architecture_ = architecture;
}
}
