/**
 * @file HWBlockArchitecture.cc
 *
 * Implementation of HWBlockArchitecture class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "HWBlockArchitecture.hh"

/**
 * The constructor.
 */
HWBlockArchitecture::HWBlockArchitecture() :
    hasID_(false), id_(0) {
}


/**
 * The destructor.
 */
HWBlockArchitecture::~HWBlockArchitecture() {
}


/**
 * Tells whether the entry has an ID.
 *
 * @return True if the entry has an ID, otherwise false.
 */
bool
HWBlockArchitecture::hasID() const {
    return hasID_;
}


/**
 * Sets the ID for the entry.
 *
 * @param id The ID to set.
 */
void
HWBlockArchitecture::setID(RowID id) {
    hasID_ = true;
    id_ = id;
}


/**
 * Returns the ID of the entry.
 *
 * @return ID of the entry.
 */
RowID
HWBlockArchitecture::id() const 
    throw (NotAvailable) {

    if (!hasID()) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    } else {
        return id_;
    }
}
