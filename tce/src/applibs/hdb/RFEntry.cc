/**
 * @file RFEntry.cc
 *
 * Implementation of RFEntry class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
