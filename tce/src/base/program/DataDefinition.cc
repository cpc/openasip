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
 * @file DataDefinition.cc
 *
 * Implementation of DataDefinition class.
 *
 * @author Mikael Lepistö 2006 (mikael.lepisto@tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include <vector> 
#include "Address.hh"
#include "AddressSpace.hh"
#include "DataDefinition.hh"
#include "MathTools.hh"

namespace TTAProgram {

/**
 * Creates data area definition.
 *
 * @param start Starting address of definition.
 * @param size Number of MAUs that area represents.
 * @param initData Table of initialization data if initialized definition.
 * @exception OutOfRange In case the initialization data contains data that
 * do not fit in a MAU.
 */

DataDefinition::DataDefinition(
    Address start, int size, MinimumAddressableUnit *initData) 
    throw (OutOfRange) :
    start_(start), size_(size) {
    
    if (initData != NULL) {
        data_ = new std::vector<MinimumAddressableUnit>(size);
        for (int i = 0 ; i < size; i++) {
            const int mauBits = start.space().width();
            const MinimumAddressableUnit mau = initData[i];
            if (MathTools::requiredBits(mau) > mauBits) {
                if (MathTools::requiredBitsSigned(mau) > mauBits) {
                    throw OutOfRange(
                        __FILE__, __LINE__, __func__,
                        "The given value does not fit to the MAU of address "
                        "space.");
                }
            }


            (*data_)[i] = mau;
        }        
    } else {
        data_ = NULL;
    }
}

/**
 * Creates initialized data area definition.
 *
 * @param start Starting address of definition.
 * @param initData Initialization data.
 * @exception OutOfRange In case the initialization data contains data that
 * do not fit in a MAU.
 */
DataDefinition::DataDefinition(
    Address start, const std::vector<MinimumAddressableUnit>& initData) 
    throw (OutOfRange) :
    start_(start) {    

    // check that the MAUs are not too large for the address space
    for (std::size_t i = 0 ; i < initData.size(); i++) {
        const int mauBits = start.space().width();
        const MinimumAddressableUnit mau = initData[i];
        
        if (MathTools::requiredBits(mau) > mauBits) {
            
            if (MathTools::requiredBitsSigned(mau) > mauBits) {
                throw OutOfRange(
                    __FILE__, __LINE__, __func__,
                    "The given value does not fit to the MAU of address "
                    "space.");
            }
        }
    }        

    data_ = new std::vector<MinimumAddressableUnit>(initData);
    size_ = data_->size();
}

/**
 * Frees all the init data if there is any.
 */
DataDefinition::~DataDefinition() {
    if (data_ != NULL) {
        delete data_;
        data_ = NULL;
    }
}

/**
 * Returns start address of data definition.
 *
 * @return Start address of data definition.
 */
Address
DataDefinition::startAddress() const {
    return start_;
}

void
DataDefinition::setStartAddress(Address start) {
    start_ = start;
}

/**
 * Returns true if data definition contains initialization data.
 * 
 * @return True if data definition contains initialization data.
 */
bool 
DataDefinition::isInitialized() const {
    return (data_ != NULL);
}

/**
 * Returns one MAU of initialization values from requested index.
 *
 * @param index Index of the MAU whic is returned.
 * @return One MAU of initialization values from requested index.
 */
MinimumAddressableUnit 
DataDefinition::MAU(int index) const {
    assert(index < size());

    if (isAddress()) {    
        MinimumAddressableUnit retVal = destinationAddress().location();
        
        // how many bits should be masked out
        int maskBits = index * startAddress().space().width();

        // how many bits is the whole area definition
        int totalAreaDefBits = size() * startAddress().space().width();
        
        // how many bits of the data definition should not be masked out
        int remainingBits = totalAreaDefBits - maskBits;
        
        // how many bits is value that is stored as a address
        int addressBits = sizeof(retVal) * BYTE_BITWIDTH;
        
        // how many bits will be masked from start of address
        int addressMaskBits = addressBits - remainingBits;
        
        // mask out bits if needed
        if (addressMaskBits >= 0) {
            retVal = retVal << addressMaskBits;
        } else {
            retVal = retVal >> (-addressMaskBits);
        }
       
        // align MAU bits to right
        retVal = retVal >> (addressBits - startAddress().space().width());
        
        // return MAU
        return retVal;

    } else if (isInitialized()) {
        return (*data_)[index];

    } else {
        throw NotAvailable(
            __FILE__, __LINE__, __func__,
            "Definition does not contain initialisation data.");
    }
}

/**
 * Returns the number of MAUs represented by definition.
 *
 * @return The number of MAUs represented by definition.
 */
int 
DataDefinition::size() const {
    return size_;
}

/**
 * Returns true if init data of definition contains an address.
 *
 * @return True if init data of definition contains an address.
 */
bool 
DataDefinition::isAddress() const {
    return false;
}

/**
 * Returns true if init data of definition contains an instruction address.
 *
 * @return True if init data of definition contains an instruction address.
 */
bool 
DataDefinition::isInstructionAddress() const {
    return false;
}

/**
 * Returns address where to initialization data refers.
 *
 * @return Address where to initialization data refers.
 */
Address
DataDefinition::destinationAddress() const {
    throw WrongSubclass(
        __FILE__, __LINE__, __func__, 
        "Data definition does not contain an address reference.");
}

/**
 * Set new destination address for the data definition.
 */
void
DataDefinition::setDestinationAddress(Address) {
    throw WrongSubclass(
        __FILE__, __LINE__, __func__,
        "Data definition does not refer to a data address.");
}

/**
 * POM style copy constructor, which supports dynamic binding.
 *
 * @return Copy of the object. 
 */
DataDefinition* DataDefinition::copy() const {
    DataDefinition* newDef = NULL;

    if (isInitialized()) {
        newDef = new DataDefinition(start_, *data_);
    } else {
        newDef = new DataDefinition(start_, size_);
    }

    return newDef;
}

}
