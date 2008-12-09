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
 * @file RFEntry.cc
 *
 * Implementation of RFEntry class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "RFEntry.hh"
#include "RFArchitecture.hh"
#include "RFImplementation.hh"
#include "Application.hh"

using std::string;

namespace HDB {

/**
 * The constructor.
 *
 * Creates an empty RF entry without ID, architecture, implementation and
 * cost function.
 */
RFEntry::RFEntry() : 
    HDBEntry(), architecture_(NULL), implementation_(NULL) {
}


/**
 * The destructor.
 */
RFEntry::~RFEntry() {
    if (hasArchitecture()) {
        delete architecture_;
    }
    if (hasImplementation()) {
        delete implementation_;
    }
}


/**
 * Tells whether the entry has a hardware implementation.
 *
 * @return True if the entry has a hardware implementation, otherwise false.
 */
bool
RFEntry::hasImplementation() const {
    return implementation_ != NULL;
}


/**
 * Sets implementation for the entry.
 *
 * Deletes the old implementation if one exists.
 *
 * @param implementation The new implementation.
 */
void
RFEntry::setImplementation(RFImplementation* implementation) {
    if (hasImplementation()) {
        delete implementation_;
    }
    implementation_ = implementation;
}


/**
 * Returns the implementation of the entry.
 *
 * @return The implementation.
 * @exception NotAvailable If there is no implementation.
 */
RFImplementation&
RFEntry::implementation() const
    throw (NotAvailable) {

    if (!hasImplementation()) {
        const string procName = "RFEntry::implementation";
        throw NotAvailable(__FILE__, __LINE__, procName);
    }

    return *implementation_;
}


/**
 * Tells whether the RF entry has an architecture.
 *
 * @return True if the RF entry has an architecture, otherwise false.
 */
bool
RFEntry::hasArchitecture() const {
    return architecture_ != NULL;
}


/**
 * Sets new architecture for the entry.
 *
 * Deletes the old architecture if one exists.
 *
 * @param architecture The new architecture.
 */
void
RFEntry::setArchitecture(RFArchitecture* architecture) {
    if (hasArchitecture()) {
        delete architecture_;
    }
    architecture_ = architecture;
}


/**
 * Returns the architecture of the entry.
 *
 * @return The architecture.
 * @exception NotAvailable If the entry doesn't have an architecture.
 */
RFArchitecture&
RFEntry::architecture() const 
    throw (NotAvailable) {

    if (!hasArchitecture()) {
        const string procName = "RFEntry::architecture";
        throw NotAvailable(__FILE__, __LINE__, procName);
    }
    
    return *architecture_;
}
}
