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
 * @file DataDefinition.cc
 *
 * Implementation of DataDefinition class.
 *
 * @author Mikael Lepistö 2006 (mikael.lepisto-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
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
    Address start, int size, bool littleEndian,
    MinimumAddressableUnit *initData,
    bool allZeros)
    throw (OutOfRange) :
    start_(start), size_(size), allZeros_(allZeros),
    littleEndian_(littleEndian) {
    
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
    Address start, const std::vector<MinimumAddressableUnit>& initData,
    bool littleEndian) 
    throw (OutOfRange) :
    start_(start), allZeros_(false), littleEndian_(littleEndian) {    

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
    return allZeros_ || (data_ != NULL);
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
        if (littleEndian_) {
            index = size() - index -1;
        }

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

    } else if(allZeros_) {
        return 0;
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

    if (allZeros_) {
        newDef = new DataDefinition(start_, size_, littleEndian_, NULL, true);
    } else if (isInitialized()) {
        newDef = new DataDefinition(start_, *data_, littleEndian_);
    } else {
        newDef = new DataDefinition(start_, size_, littleEndian_);
    }

    return newDef;
}

}
