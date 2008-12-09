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
 * @file DataSection.cc
 *
 * Non-inline definitions of DataSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pjaaskel-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include <cmath>

#include "DataSection.hh"
#include "Application.hh"
#include "Swapper.hh"
#include "MathTools.hh"

namespace TPEF {

DataSection DataSection::proto_(true);

/**
 * Constructor.
 *
 * @param init true if registeration is wanted
 */
DataSection::DataSection(bool init) : UDataSection(false) {

    if (init) {
        Section::registerSection(this);
    }

    unsetFlagVLen();
    unsetFlagNoBits();
}

/**
 * Destructor.
 */
DataSection::~DataSection() {
}

/**
 * Returns section's type.
 *
 * @return Type of section.
 */
Section::SectionType
DataSection::type() const {
    return ST_DATA;
}

/**
 * Sets value of one byte of data section.
 *
 * NOTE: Byte here is tpef file byte. (8bits).
 *
 * @param offset Offset where to write the byte.
 * @param aByte Value to write.
 */
void
DataSection::setByte(Word offset, Byte aByte) {
    if (offset >= data_.size()) {
        abortWithError(
            "Can't set byte " +
            Conversion::toString(static_cast<int>(aByte)) +
            " to offset: " + Conversion::toString(offset) +
            "\t data section size: " + Conversion::toString(data_.size()));
    }
    data_[offset] = aByte;
}

/**
 * Creates an instance of DataSection.
 *
 * @return Newly created section.
 */
Section*
DataSection::clone() const {
    return new DataSection(false);
}

/**
 * Returns byte from chunk's offset.
 *
 * @param chunk Chunk containing offset of data.
 * @return Byte from chunk offset.
 */
Byte
DataSection::byte(const Chunk* chunk) const {
    return data_[chunk->offset()];
}

/**
 * Returns one MAU from section by gived MAU index.
 *
 * @param index Index of MAU to return.
 * @return One MAU of data section.
 */
MinimumAddressableUnit
DataSection::MAU(Word index) const {

    int byteOffset = MAUsToBytes(index);
    int mauInBytes = MAUsToBytes(1);

    MinimumAddressableUnit retVal = 0;
    
    // read all the bytes to retval
    for(int i = 0; i < mauInBytes; i++) {
        retVal = (retVal << BYTE_BITWIDTH) | 
            static_cast<MinimumAddressableUnit>(byte(byteOffset+i));
    }
    
    return retVal;
}

/**
 * Adds MAU to the end of section.
 *
 * @param aMAU MAU that will be copied into the section.
 * @exception OutOfRange If the given value could not fit to the bit count
 * defined by the address space's MAU width (when interpreted as unsigned or
 * signed integer).
 */
void 
DataSection::addMAU(MinimumAddressableUnit aMAU) 
    throw (OutOfRange) {

    int mauBits = aSpace()->MAU();
    
    if (MathTools::requiredBits(aMAU) > mauBits) {
        if (MathTools::requiredBitsSigned(aMAU) > mauBits) {
            throw OutOfRange(
                __FILE__, __LINE__, __func__,
                "The given value does not fit to the MAU of address space.");
        }
    }

    // how many bits are left to last byte
    int modulo = mauBits % BYTE_BITWIDTH;
    
    // write first bits
    if (modulo != 0) {
        mauBits -= modulo;            
        addByte(static_cast<Byte>(aMAU >> mauBits));
    }
    
    // move shifting to right for the next MAU
    mauBits -= BYTE_BITWIDTH;
    
    // write the rest
    for (; mauBits >= 0; 
         mauBits = mauBits - BYTE_BITWIDTH) {                
        
        addByte(static_cast<Byte>(aMAU >> mauBits));
    }
}


/**
 * Returns byte from byte offset.
 *
 * @param offset Offset of data.
 * @return Byte from offset.
 */
Byte
DataSection::byte(Word offset) const {
    return data_[offset];
}

/**
 * Adds byte of data into section.
 *
 * Method copies data that is given in parameter.
 *
 * @param aByte Byte that will be copied into the section.
 */
void
DataSection::addByte(Byte aByte) {
    data_.push_back(aByte);
}

/**
 * Returns the size of the data section in bytes.
 *
 * @return Length of data in bytes.
 */
Word
DataSection::length() const {
    return data_.size();
}

/**
 * Tries to set data length. 

 * Can be used only for making section bigger. Asserts if method is 
 * tried to use for making section smaller.
 *
 * Initializes data with zeros.
 *
 * @param length Length to which section is expanded.
 */
void
DataSection::setDataLength(Word aLength) {
    assert(length() <= aLength);
    while (length() < aLength) {
        addByte(0);
    }
}

/**
 * Writes unsigned value to data section.
 *
 * Value is aligned to field as normal big endian value
 * least significant bit is stored to last bit of last MAU.
 *
 * If we call writeValue(0, 4, 3) and MAU is 2bit
 * start of data section will be 00 00 00 11.
 *
 * However the MAUs are stored in data section like this:
 * 00000000|000000000|00000000|00000011 (one MAU per byte)
 *    MAU1 |   MAU2  |  MAU3  | MAU4
 *
 * @param index MAU offset to section where to we write value.
 * @param numOfMAUs Number of MAUs that we use for storing value.
 * @param value Value to write.
 */
void
DataSection::writeValue(Word index, Word numOfMAUs, unsigned long value) {

    int mauInBytes = MAUsToBytes(1);    
    
    MinimumAddressableUnit mauMask = 
        static_cast<unsigned int>(-1) >> 
        (sizeof(mauMask)*BYTE_BITWIDTH - aSpace()->MAU());
    
    int shiftCount = 0;

    // start writing from end of area..
    for (int i = numOfMAUs - 1; i >= 0; i--) {
        int byteOffset = MAUsToBytes(index + i);
        
        MinimumAddressableUnit currentMAU = 0;

        if (shiftCount < static_cast<int>(sizeof(value)*BYTE_BITWIDTH)) {
            // I tried math tools... system tests went broken :(
            // ssooo... if it's not broken.... 
            currentMAU = (value >> shiftCount) & mauMask;
        }
        
        shiftCount += aSpace()->MAU();
        
        // write current MAU :)
        for (int j = mauInBytes-1; j >= 0; j--) {
            Byte currentByte = static_cast<Byte>(currentMAU);
            setByte(byteOffset + j, currentByte);
            currentMAU = currentMAU >> BYTE_BITWIDTH;
        }
    }
}

/**
 * Writes signed value to data section.
 *
 * For example, when we call writeValue(0, 4, -3) and MAU is 2bit
 * start of data section will be 11 11 11 01.
 * 
 * MAUs are stored in data section like this:
 * 00000011 000000011 00000011 00000001 (one MAU per byte)
 *
 * If we call writeValue(0, 4, -3) and MAU is 10bit
 * start of data section will be 1111111111 1111111111 1111111111 1111111101.
 *
 * However the MAUs are stored in data section like this:
 *
 * 00000011 11111111|00000011 11111111|00000011 11111111|00000011 11111101|
 *       MAU1       |      MAU2       |      MAU3       |      MAU4       |
 *
 * @param index MAU offset to section where to we write value.
 * @param numOfMAUs Number of MAUs that we use for storing value.
 * @param value Value to write.
 */
void
DataSection::writeValue(Word index, Word numOfMAUs, signed long value) { 

    int mauInBytes = MAUsToBytes(1);    
    
    MinimumAddressableUnit mauMask = 
        static_cast<unsigned int>(-1) >> 
        (sizeof(mauMask)*BYTE_BITWIDTH - aSpace()->MAU());
    
    int shiftCount = 0;

    // start writing from end of area..
    for (int i = numOfMAUs - 1; i >= 0; i--) {
        int byteOffset = MAUsToBytes(index + i);
        
        MinimumAddressableUnit currentMAU = 
            static_cast<MinimumAddressableUnit>(~0);
        
        if (shiftCount < static_cast<int>(sizeof(value)*BYTE_BITWIDTH)) {
            // I tried math tools... system tests went broken :(
            // ssooo... if it's not broken.... 
            currentMAU = (value >> shiftCount) & mauMask;
            shiftCount += aSpace()->MAU();
        }
        
        // write current MAU :)
        for (int j = mauInBytes-1; j >= 0; j--) {
            Byte currentByte = static_cast<Byte>(currentMAU);
            setByte(byteOffset + j,currentByte);
            currentMAU = currentMAU >> BYTE_BITWIDTH;
        }
    }
}

}
