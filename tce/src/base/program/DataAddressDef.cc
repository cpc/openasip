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
 * @file DataAddressDef.cc
 *
 * Implementation of DataAddressDef class.
 *
 * @author Mikael Lepistö 2006 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#include "DataAddressDef.hh"

namespace TTAProgram {

/**
 * Creates data area definition containing address.
 *
 * @param start Starting address of definition.
 * @param size Number of MAUs that area represents.
 * @param dest Address where to data definition refers.
 */
DataAddressDef::DataAddressDef(Address start, int size, Address dest) :
    DataDefinition(start, size), dest_(dest) {

}

/**
 * Nothing to free.
 */
DataAddressDef::~DataAddressDef() {
}

/**
 * Returns true, class defines always initialized data area.
 * 
 * @return Always true.
 */
bool 
DataAddressDef::isInitialized() const {
    return true;
}

/**
 * Returns true, class contains always address reference.
  * 
 * @return Always true.
 */
bool 
DataAddressDef::isAddress() const {
    return true;
}

/**
 * Returns destination address, where to intialization value refers.
  * 
 * @return Destination address of initialized data area.
 */
Address
DataAddressDef::destinationAddress() const {
    return dest_;
}

/**
 * Set new destination address for the data definition.
 *
 * @param dest New destination address.
 */
void
DataAddressDef::setDestinationAddress(Address dest) {
    dest_ = dest;
}

/**
 * POM style copy constructor, which supports dynamic binding.
 *
 * @return Copy of the object. 
 */
DataDefinition* 
DataAddressDef::copy() const {
    DataAddressDef* newDef = new DataAddressDef(
        startAddress(), size(), destinationAddress());
    
    return newDef;
}

}
