/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file UnitImplementationLocation.cc
 *
 * Implementation of UnitImplementationLocation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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
