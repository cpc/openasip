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
