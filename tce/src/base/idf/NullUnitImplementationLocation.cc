/**
 * @file UnitImplementationLocation.cc
 *
 * Implementation of UnitImplementationLocation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <string>

#include "NullUnitImplementationLocation.hh"
#include "Application.hh"

using std::string;

namespace IDF {

NullUnitImplementationLocation NullUnitImplementationLocation::instance_;

/**
 * The constructor.
 */
NullUnitImplementationLocation::NullUnitImplementationLocation() :
    UnitImplementationLocation("", 0, "") {
}


/**
 * The destructor.
 */
NullUnitImplementationLocation::~NullUnitImplementationLocation() {
}


/**
 * Returns the only instance of NullUnitImplementationLocation.
 *
 * @return The instance.
 */
NullUnitImplementationLocation&
NullUnitImplementationLocation::instance() {
    return instance_;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
std::string
NullUnitImplementationLocation::hdbFile() const 
    throw (FileNotFound) {

    abortWithError("NullUnitImplementationLocation::hdbFile");
    return "";
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
int
NullUnitImplementationLocation::id() const {
    abortWithError("NullUnitImplementationLocation::id");
    return 0;
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
std::string
NullUnitImplementationLocation::unitName() const {
    abortWithError("NullUnitImplementationLocation::unitName");
    return "";
}


/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
void
NullUnitImplementationLocation::setParent(MachineImplementation&)
    throw (InvalidData) {

    abortWithError("NullUnitImplementationLocation::setParent");
}
}
